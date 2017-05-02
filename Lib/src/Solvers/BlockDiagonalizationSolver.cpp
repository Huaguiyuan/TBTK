/* Copyright 2017 Kristofer Björnson
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/** @file BlockDiagonalizationSolver.cpp
 *
 *  @author Kristofer Björnson
 */

#include "BlockDiagonalizationSolver.h"
#include "Streams.h"
#include "TBTKMacros.h"

#include <iomanip>

using namespace std;

namespace TBTK{

BlockDiagonalizationSolver::BlockDiagonalizationSolver() : Communicator(true){
	hamiltonian = nullptr;
	eigenValues = nullptr;
	eigenVectors = nullptr;
	numBlocks = -1;

	maxIterations = 50;
	scCallback = nullptr;
}

BlockDiagonalizationSolver::~BlockDiagonalizationSolver(){
	if(hamiltonian != nullptr)
		delete [] hamiltonian;
	if(eigenValues != nullptr)
		delete [] eigenValues;
	if(eigenVectors != nullptr)
		delete eigenVectors;
}

void BlockDiagonalizationSolver::run(){
	TBTKAssert(
		getModel() != NULL,
		"DiagonalizationSolver::run()",
		"Model not set.",
		"Use DiagonalizationSolver::setModel() to set model."
	);

	int iterationCounter = 0;
	init();

	if(getGlobalVerbose() && getVerbose())
		Streams::out << "Running DiagonalizationSolver\n";
	while(iterationCounter++ < maxIterations){
		if(getGlobalVerbose() && getVerbose()){
			if(iterationCounter%10 == 1)
				Streams::out << " ";
			if(iterationCounter%50 == 1)
				Streams::out << "\n";
			Streams::out << "." << flush;
		}

		solve();

		if(scCallback){
			if(scCallback(this))
				break;
			else
				update();
		}
		else{
			break;
		}
	}
	if(getGlobalVerbose() && getVerbose())
		Streams::out << "\n";
}

void BlockDiagonalizationSolver::init(){
	if(getGlobalVerbose() && getVerbose())
		Streams::out << "Initializing BlockDiagonalizationSolver\n";

	//Find number of blocks and number of states per block.
	IndexTree blockIndices = getModel()->getHoppingAmplitudeSet()->getSubspaceIndices();
	IndexTree::Iterator blockIterator = blockIndices.begin();
	const Index *blockIndex;
	numBlocks = 0;
//	vector<int> numStatesPerBlock;
	while((blockIndex = blockIterator.getIndex())){
		numBlocks++;

		HoppingAmplitudeSet::Iterator iterator = getModel()->getHoppingAmplitudeSet()->getIterator(
			*blockIndex
		);
		numStatesPerBlock.push_back(iterator.getNumBasisIndices());

		blockIterator.searchNext();
	}

	/** Calculate block sizes and blockOffsets. */
	for(unsigned int n = 0; n < numStatesPerBlock.size(); n++){
		unsigned int numStates = numStatesPerBlock.at(n);
		blockSizes.push_back((numStates*(numStates+1))/2);
		eigenVectorSizes.push_back(numStates*numStates);
		if(n == 0){
			blockOffsets.push_back(0);
			eigenVectorOffsets.push_back(0);
		}
		else{
			blockOffsets.push_back(
				blockOffsets.at(n-1) + blockSizes.at(n-1)
			);
			eigenVectorOffsets.push_back(
				eigenVectorOffsets.at(n-1) + eigenVectorSizes.at(n-1)
			);
		}
	}

/*	for(unsigned int n = 0; n < blockSizes.size(); n++){
		Streams::out
			<< blockSizes.at(n) << "\t"
			<< blockOffsets.at(n) << "\t"
			<< eigenVectorSizes.at(n) << "\t"
			<< eigenVectorOffsets.at(n) << "\n";
	}*/

	//Calculate amount of memory required to store all the blocks of the
	//Hamiltonian.
	unsigned int hamiltonianSize = 0;
	unsigned int eigenVectorsSize = 0;
	for(unsigned int n = 0; n < numStatesPerBlock.size(); n++){
		hamiltonianSize += (numStatesPerBlock.at(n)*(numStatesPerBlock.at(n)+1))/2;
		eigenVectorsSize += numStatesPerBlock.at(n)*numStatesPerBlock.at(n);
	}

	//Setup maps that map a given state index to the correct block and vice
	//versa.
	unsigned int blockCounter = 0;
	unsigned int intraBlockCounter = 0;
	for(int n = 0; n < getModel()->getBasisSize(); n++){
		if(intraBlockCounter >= numStatesPerBlock.at(blockCounter)){
			intraBlockCounter = 0;
			blockCounter++;
		}

		if(intraBlockCounter == 0)
			blockToStateMap.push_back(n);


		stateToBlockMap.push_back(blockCounter);
		intraBlockCounter++;
	}

	if(getGlobalVerbose() && getVerbose()){
		int numBytesHamiltonian = hamiltonianSize*sizeof(
			complex<double>
		);
		int numBytesEigenVectors = eigenVectorsSize*sizeof(
			complex<double>
		);

		Streams::out << "\tBasis size: " << getModel()->getBasisSize()
			<< "\n";
		if(numBytesHamiltonian < 1024){
			Streams::out << "\tHamiltonian size: "
				<< numBytesHamiltonian*sizeof(complex<double>)
				<< "B\n";
		}
		else if(numBytesHamiltonian < 1024*1024){
			Streams::out << "\tHamiltonian size: "
				<< numBytesHamiltonian/1024 << "KB\n";
		}
		else if(numBytesHamiltonian < 1024*1024*1024){
			Streams::out << "\tHamiltonian size: "
				<< numBytesHamiltonian/1024/1024 << "MB\n";
		}
		else{
			Streams::out << "\tHamiltonian size: "
				<< numBytesHamiltonian/1024/1024/1024
				<< "GB\n";
		}
		if(numBytesEigenVectors < 1024){
			Streams::out << "\tEigenvectors size: "
				<< numBytesEigenVectors << "B\n";
		}
		else if(numBytesEigenVectors < 1024*1024){
			Streams::out << "\tEigenvectors size: "
				<< numBytesEigenVectors/1024 << "KB\n";
		}
		else if(numBytesEigenVectors < 1024*1024*1024){
			Streams::out << "\tEigenvectors size: "
				<< numBytesEigenVectors/1024/1024 << "MB\n";
		}
		else{
			Streams::out << "\tEigenvectors size: "
				<< numBytesEigenVectors/1024/1024/1024
				<< "GB\n";
		}
		Streams::out << "\tNumber of blocks: "
			<< numStatesPerBlock.size() << "\n";
	}

	hamiltonian = new complex<double>[hamiltonianSize];
	eigenValues = new double[getModel()->getBasisSize()];
	eigenVectors = new complex<double>[eigenVectorsSize];

	update();
}

void BlockDiagonalizationSolver::update(){
	Model *model = getModel();

	IndexTree blockIndices = getModel()->getHoppingAmplitudeSet()->getSubspaceIndices();
	IndexTree::Iterator blockIterator = blockIndices.begin();
	const Index *blockIndex;
	unsigned int blockCounter = 0;
	while((blockIndex = blockIterator.getIndex())){
		HoppingAmplitudeSet::Iterator iterator = getModel()->getHoppingAmplitudeSet()->getIterator(
			*blockIndex
		);
		int minBasisIndex = iterator.getMinBasisIndex();
		const HoppingAmplitude *hoppingAmplitude;
		while((hoppingAmplitude = iterator.getHA())){
			int from = model->getHoppingAmplitudeSet()->getBasisIndex(
//				hoppingAmplitude->fromIndex
				hoppingAmplitude->getFromIndex()
			) - minBasisIndex;
			int to = model->getHoppingAmplitudeSet()->getBasisIndex(
//				hoppingAmplitude->toIndex
				hoppingAmplitude->getToIndex()
			) - minBasisIndex;
			if(from >= to)
				hamiltonian[blockOffsets.at(blockCounter) + to + (from*(from+1))/2] += hoppingAmplitude->getAmplitude();

			iterator.searchNextHA();
		}

		blockCounter++;
		blockIterator.searchNext();
	}

/*	for(int b = 0; b < numBlocks; b++){
		unsigned int row = 0;
		unsigned int col = 0;
		Streams::out << "Block number:\t" << b << "\n";
		Streams::out << "Block offsets:\t" << blockOffsets.at(b) << "\n";
		Streams::out << "Block size:\t" << blockSizes.at(b) << "\n";
		for(unsigned int n = 0; n < blockSizes.at(b); n++){
			Streams::out << setw(5) << setprecision(3) << real(hamiltonian[blockOffsets.at(b) + n]);
			col++;
			if(col == row+1){
				while(col < numStatesPerBlock.at(b)){
					Streams::out << setw(5) << ".";
					col++;
				}
				Streams::out << "\n";
				row++;
				col = 0;
			}
		}
		Streams::out << "\n\n";
	}*/
}

//Lapack function for matrix diagonalization of triangular matrix.
extern "C" void zhpev_(char *jobz,		//'E' = Eigenvalues only, 'V' = Eigenvalues and eigenvectors.
			char *uplo,		//'U' = Stored as upper triangular, 'L' = Stored as lower triangular.
			int *n,			//n*n = Matrix size
			complex<double> *ap,	//Input matrix
			double *w,		//Eigenvalues, is in accending order if info = 0
			complex<double> *z,	//Eigenvectors
			int *ldz,		//
			complex<double> *work,	//Workspace, dimension = max(1, 2*N-1)
			double *rwork,		//Workspace, dimension = max(1, 3*N-2)
			int *info);		//0 = successful, <0 = -info value was illegal, >0 = info number of off-diagonal elements failed to converge.

//Lapack function for matrix diagonalization of banded triangular matrix
extern "C" void zhbeb_(
	char *jobz,		//'E' = Eigenvalues only, 'V' = Eigenvalues and eigenvectors.
	char *uplo,		//'U' = Stored as upper triangular, 'L' = Stored as lower triangular.
	int *n,			//n*n = Matrix size
	int *kd,		//Number of (sub/super)diagonal elements
	complex<double> *ab,	//Input matrix
	int *ldab,		//Leading dimension of array ab. ldab >= kd + 1
	double *w,		//Eigenvalues, is in accending order if info = 0
	complex<double> *z,	//Eigenvectors
	int *ldz,		//
	complex<double> *work,	//Workspace, dimension = max(1, 2*N-1)
	double *rwork,		//Workspace, dimension = max(1, 3*N-2)
	int *info);		//0 = successful, <0 = -info value was illegal, >0 = info number of off-diagonal elements failed to converge.

void BlockDiagonalizationSolver::solve(){
	if(true){//Currently no support for banded matrices.
		unsigned int eigenValuesOffset = 0;
		for(int b = 0; b < numBlocks; b++){
			//Setup zhpev to calculate...
			char jobz = 'V';			//...eigenvalues and eigenvectors...
			char uplo = 'U';			//...for an upper triangular...
			int n = numStatesPerBlock.at(b);	//...nxn-matrix.
			//Initialize workspaces
			complex<double> *work = new complex<double>[(2*n-1)];
			double *rwork = new double[3*n-2];
			int info;
			//Solve brop
			zhpev_(
				&jobz,
				&uplo,
				&n,
				hamiltonian + blockOffsets.at(b),
				eigenValues + eigenValuesOffset,
				eigenVectors + eigenVectorOffsets.at(b),
				&n,
				work,
				rwork,
				&info
			);

			TBTKAssert(
				info == 0,
				"DiagonalizationSolver:solve()",
				"Diagonalization routine zhpev exited with INFO=" + to_string(info) + ".",
				"See LAPACK documentation for zhpev for further information."
			);

			//Delete workspaces
			delete [] work;
			delete [] rwork;

			eigenValuesOffset += numStatesPerBlock.at(b);
		}
	}
/*	else{
		int kd;
		if(size_z != 1)
			kd = orbitals*size_x*size_y*size_z;
		else if(size_y != 1)
			kd = orbitals*size_x*size_y;
		else
			kd = orbitals*size_x;
		//Setup zhbev to calculate...
		char jobz = 'V';			//...eigenvalues and eigenvectors...
		char uplo = 'U';			//...for and upper triangluar...
		int n = orbitals*size_x*size_y*size_z;	//...nxn-matrix.
		int ldab = kd + 1;
		//Initialize workspaces
		complex<double> *work = new complex<double>[n];
		double *rwork = new double[3*n-2];
		int info;
		//Solve brop
		zhbev_(&jobz, &uplo, &n, &kd, hamiltonian, &ldab, eigenValues, eigenVectors, &n, work, rwork, &info);

		//delete workspaces
		delete [] work;
		delete [] rwork;
	}*/
}

};	//End of namespace TBTK
