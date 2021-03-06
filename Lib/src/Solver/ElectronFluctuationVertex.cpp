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

/** @file ElectronFluctuationVertex.cpp
 *
 *  @author Kristofer Björnson
 */

#include "TBTK/Functions.h"
#include "TBTK/InteractionAmplitude.h"
#include "TBTK/Solver/ElectronFluctuationVertex.h"
#include "TBTK/UnitHandler.h"

#include <complex>
#include <iomanip>

using namespace std;

namespace TBTK{
namespace Solver{

ElectronFluctuationVertex::ElectronFluctuationVertex(
	const MomentumSpaceContext &momentumSpaceContext,
	const Property::Susceptibility &chargeSusceptibility,
	const Property::Susceptibility &spinSusceptibility
) :
	momentumSpaceContext(momentumSpaceContext),
	chargeSusceptibility(chargeSusceptibility),
	spinSusceptibility(spinSusceptibility)
{
	U = 0.;
	Up = 0.;
	J = 0.;
	Jp = 0.;

/*	rpaSusceptibilityCalculator = new RPASusceptibilityCalculator(
		momentumSpaceContext,
		SusceptibilityCalculator::Algorithm::Lindhard
	);*/

	interactionAmplitudesAreGenerated = false;

	isInitialized = true;
}

/*ElectronFluctuationVertex::ElectronFluctuationVertex(
	RPASusceptibilityCalculator &rpaSusceptibilityCalculator
){
	U = 0.;
	Up = 0.;
	J = 0.;
	Jp = 0.;

	this->rpaSusceptibilityCalculator = rpaSusceptibilityCalculator.createSlave();

	interactionAmplitudesAreGenerated = false;

	isInitialized = true;
}

ElectronFluctuationVertexCalculator::~ElectronFluctuationVertexCalculator(){
	if(rpaSusceptibilityCalculator != nullptr)
		delete rpaSusceptibilityCalculator;
}

ElectronFluctuationVertexCalculator* ElectronFluctuationVertexCalculator::createSlave(){
	return new ElectronFluctuationVertexCalculator(
		*rpaSusceptibilityCalculator
	);
}*/

void ElectronFluctuationVertex::generateInteractionAmplitudes(){
	if(interactionAmplitudesAreGenerated)
		return;

	u1.clear();
	u2.clear();
	u3.clear();

	unsigned int numOrbitals = momentumSpaceContext.getNumOrbitals();

	for(int a = 0; a < (int)numOrbitals; a++){
		u2.push_back(
			InteractionAmplitude(
				U,
				{{a},	{a}},
				{{a},	{a}}
			)
		);
		u3.push_back(
			InteractionAmplitude(
				-U,
				{{a},	{a}},
				{{a},	{a}}
			)
		);

		for(int b = 0; b < (int)numOrbitals; b++){
			if(a == b)
				continue;

			u1.push_back(
				InteractionAmplitude(
					Up - J,
					{{a},	{b}},
					{{b},	{a}}
				)
			);
			u2.push_back(
				InteractionAmplitude(
					Up,
					{{a},	{b}},
					{{b},	{a}}
				)
			);
			u3.push_back(
				InteractionAmplitude(
					-J,
					{{a},	{b}},
					{{b},	{a}}
				)
			);

			u1.push_back(
				InteractionAmplitude(
					J - Up,
					{{a},	{b}},
					{{a},	{b}}
				)
			);
			u2.push_back(
				InteractionAmplitude(
					J,
					{{a},	{b}},
					{{a},	{b}}
				)
			);
			u3.push_back(
				InteractionAmplitude(
					-Up,
					{{a},	{b}},
					{{a},	{b}}
				)
			);

			u2.push_back(
				InteractionAmplitude(
					Jp,
					{{a},	{a}},
					{{b},	{b}}
				)
			);
			u3.push_back(
				InteractionAmplitude(
					-Jp,
					{{a},	{a}},
					{{b},	{b}}
				)
			);
		}
	}

	interactionAmplitudesAreGenerated = true;
}

vector<complex<double>> ElectronFluctuationVertex::calculateSelfEnergyVertex(
	const Index &index
){
/*	TBTKAssert(
		isInitialized,
		"ElectronFLuctuationVertex::calculateSelfEnergyVertex()",
		"ElectronFluctuationVertex not yet initialized.",
		"Use ElectronFluctuationVertex::init() to initialize"
		<< " the SelfEnergyCalculator."
	);
	TBTKAssert(
		orbitalIndices.size() == 4,
		"ElectronFluctuationVertex::calculateSelfEnergyVertex()",
		"Two orbital indices required but " << orbitalIndices.size()
		<< " supplied.",
		""
	);*/

	TBTKAssert(
		spinSusceptibility.getEnergyType()
		== chargeSusceptibility.getEnergyType(),
		"Solver::ElectronFluctuationVertex::calculateSelfEnergyVertex()",
		"The spin and charge susceptibilities must have the same"
		<< " energy type.",
		""
	);
	unsigned int numEnergies;
	switch(spinSusceptibility.getEnergyType()){
	case Property::EnergyResolvedProperty<complex<double>>::EnergyType::Real:
		TBTKAssert(
			spinSusceptibility.getResolution()
			== chargeSusceptibility.getResolution(),
			"Solver::ElectronFluctuationVertex::calculateSelfEnergyVertex()",
			"The spin and charge susceptibilities must have the"
			<< " same energy resolution. The spin susceptibility"
			<< " energy resolution is '"
			<< spinSusceptibility.getResolution() << "' while the"
			<< " charge susceptibility energy resolution is '"
			<< chargeSusceptibility.getResolution() << "'.",
			""
		);
		numEnergies = spinSusceptibility.getResolution();

		break;
	case Property::EnergyResolvedProperty<complex<double>>::EnergyType::BosonicMatsubara:
		TBTKAssert(
			spinSusceptibility.getNumMatsubaraEnergies()
			== chargeSusceptibility.getNumMatsubaraEnergies(),
			"Solver::ElectronFluctuationVertex::calculateSelfEnergyVertex()",
			"The spin and charge susceptibilities must have the"
			<< " same number of Matsubara energies. The spin"
			<< " susceptibility has '"
			<< spinSusceptibility.getNumMatsubaraEnergies() << "'"
			<< " Matsubara energies while the charge"
			<< " susceptibility has '"
			<< chargeSusceptibility.getNumMatsubaraEnergies() << "'"
			<< " Matsubara energies.",
			""
		);

		numEnergies = spinSusceptibility.getNumMatsubaraEnergies();
		break;
	default:
		TBTKExit(
			"Solver::ElectronFluctuationVertex::calculateSelfEnergyVertex()",
			"Unknow EnergyType.",
			""
		);
	}

	vector<Index> components = index.split();
	TBTKAssert(
		components.size() == 5,
		"Solver::ElectronFluctuationVertex::calculateSelfEnergyVertex()",
		"The Index must be a compound Index with 5 component Indices,"
		<< " but '" << components.size() << "' components supplied.",
		""
	);
	const Index kIndex = components[0];
	const Index intraBlockIndices[4] = {
		components[1],
		components[2],
		components[3],
		components[4]
	};
	for(unsigned int n = 0; n < 4; n++){
		TBTKAssert(
			intraBlockIndices[n].getSize() == 1,
			"Solver::ElectronFluctuationVertex::calculateSelfEnergyVertex()",
			"The four last components of the compound Index"
			<< " currently is restricted to have a single"
			<< " subindex, but component '" << n+1 << "' has '"
			<< intraBlockIndices[n].getSize() << "' subindices.",
			"Contact the developer if support for more general"
			<< " Indices is required."
		);
		//TODO
		//Rewrite code below to not depend on the intraBlockIndices
		//having a single subindex each. The remove this assert
		//statement.
	}

	generateInteractionAmplitudes();

//	const BrillouinZone &brillouinZone
//		= momentumSpaceContext.getBrillouinZone();
//	const vector<unsigned int> &numMeshPoints
//		= momentumSpaceContext.getNumMeshPoints();

	//Calculate offset
/*	Index kIndex = brillouinZone.getMinorCellIndex(
		k,
		numMeshPoints
	);

	Index resultIndex = Index(
		kIndex,
		{
			orbitalIndices.at(0),
			orbitalIndices.at(1),
			orbitalIndices.at(2),
			orbitalIndices.at(3)
		}
	);

	SerializableVector<complex<double>> result;
	if(vertexTree.get(result, resultIndex))
		return result;

	DualIndex kDual(kIndex, k);*/

	vector<complex<double>> selfEnergyVertex;
//	selfEnergyVertex.reserve(energies.size());
	selfEnergyVertex.reserve(numEnergies);
	for(
		unsigned int n = 0;
		n < numEnergies;
		n++
	){
		selfEnergyVertex.push_back(0.);
	}

	//U_1*\chi_1*U_1
	for(unsigned int in = 0; in < u1.size(); in++){
		const InteractionAmplitude &incommingAmplitude = u1.at(in);
		complex<double> amplitude_i = incommingAmplitude.getAmplitude();
		int c0_i = incommingAmplitude.getCreationOperatorIndex(0).at(0);
		int c1_i = incommingAmplitude.getCreationOperatorIndex(1).at(0);
		int a0_i = incommingAmplitude.getAnnihilationOperatorIndex(0).at(0);
		int a1_i = incommingAmplitude.getAnnihilationOperatorIndex(1).at(0);

/*		if(
			a1_i != orbitalIndices.at(3)
			|| c0_i != orbitalIndices.at(2)
			|| abs(amplitude_i) < 1e-10
		){
			continue;
		}*/
		if(
			a1_i != intraBlockIndices[3][0]
			|| c0_i != intraBlockIndices[2][0]
			|| abs(amplitude_i) < 1e-10
		){
			continue;
		}

		for(unsigned int out = 0; out < u1.size(); out++){
			const InteractionAmplitude &outgoingAmplitude = u1.at(out);
			complex<double> amplitude_o = outgoingAmplitude.getAmplitude();
			int c0_o = outgoingAmplitude.getCreationOperatorIndex(0).at(0);
			int c1_o = outgoingAmplitude.getCreationOperatorIndex(1).at(0);
			int a0_o = outgoingAmplitude.getAnnihilationOperatorIndex(0).at(0);
			int a1_o = outgoingAmplitude.getAnnihilationOperatorIndex(1).at(0);

/*			if(
				a0_o != orbitalIndices.at(0)
				|| c1_o != orbitalIndices.at(1)
				|| abs(amplitude_o) < 1e-10
			){
				continue;
			}*/

			if(
				a0_o != intraBlockIndices[0][0]
				|| c1_o != intraBlockIndices[1][0]
				|| abs(amplitude_o) < 1e-10
			){
				continue;
			}

/*			vector<complex<double>> chargeSusceptibility = rpaSusceptibilityCalculator->calculateChargeRPASusceptibility(
				kDual,
				{c0_o, a1_o, c1_i, a0_i}
			);
			vector<complex<double>> spinSusceptibility = rpaSusceptibilityCalculator->calculateSpinRPASusceptibility(
				kDual,
				{c0_o, a1_o, c1_i, a0_i}
			);*/

			const vector<complex<double>> &chargeSusceptibilityData
				= chargeSusceptibility.getData();
			unsigned int offsetChargeSusceptibility
				= chargeSusceptibility.getOffset({
					kIndex,
					{c0_o},
					{a1_o},
					{c1_i},
					{a0_i}
				});

			const vector<complex<double>> &spinSusceptibilityData
				= spinSusceptibility.getData();
			unsigned int offsetSpinSusceptibility
				= spinSusceptibility.getOffset({
					kIndex,
					{c0_o},
					{a1_o},
					{c1_i},
					{a0_i}
			});

			for(
				unsigned int n = 0;
				n < numEnergies;
				n++
			){
				selfEnergyVertex.at(n) += amplitude_i*amplitude_o*(
					chargeSusceptibilityData[
						offsetChargeSusceptibility + n
					]
					+ spinSusceptibilityData[
						offsetSpinSusceptibility + n
					]
				)/2.;
			}
		}
	}

	//U_2*\chi_1*U_2
	for(unsigned int in = 0; in < u2.size(); in++){
		const InteractionAmplitude &incommingAmplitude = u2.at(in);
		complex<double> amplitude_i = incommingAmplitude.getAmplitude();
		int c0_i = incommingAmplitude.getCreationOperatorIndex(0).at(0);
		int c1_i = incommingAmplitude.getCreationOperatorIndex(1).at(0);
		int a0_i = incommingAmplitude.getAnnihilationOperatorIndex(0).at(0);
		int a1_i = incommingAmplitude.getAnnihilationOperatorIndex(1).at(0);

/*		if(
			a1_i != orbitalIndices.at(3)
			|| c0_i != orbitalIndices.at(2)
			|| abs(amplitude_i) < 1e-10
		){
			continue;
		}*/
		if(
			a1_i != intraBlockIndices[3][0]
			|| c0_i != intraBlockIndices[2][0]
			|| abs(amplitude_i) < 1e-10
		){
			continue;
		}

		for(unsigned int out = 0; out < u2.size(); out++){
			const InteractionAmplitude &outgoingAmplitude = u2.at(out);
			complex<double> amplitude_o = outgoingAmplitude.getAmplitude();
			int c0_o = outgoingAmplitude.getCreationOperatorIndex(0).at(0);
			int c1_o = outgoingAmplitude.getCreationOperatorIndex(1).at(0);
			int a0_o = outgoingAmplitude.getAnnihilationOperatorIndex(0).at(0);
			int a1_o = outgoingAmplitude.getAnnihilationOperatorIndex(1).at(0);

/*			if(
				a0_o != orbitalIndices.at(0)
				|| c1_o != orbitalIndices.at(1)
				|| abs(amplitude_o) < 1e-10
			){
				continue;
			}*/
			if(
				a0_o != intraBlockIndices[0][0]
				|| c1_o != intraBlockIndices[1][0]
				|| abs(amplitude_o) < 1e-10
			){
				continue;
			}

/*			vector<complex<double>> chargeSusceptibility = rpaSusceptibilityCalculator->calculateChargeRPASusceptibility(
				kDual,
				{c0_o, a1_o, c1_i, a0_i}
			);
			vector<complex<double>> spinSusceptibility = rpaSusceptibilityCalculator->calculateSpinRPASusceptibility(
				kDual,
				{c0_o, a1_o, c1_i, a0_i}
			);*/
			const vector<complex<double>> &chargeSusceptibilityData
				= chargeSusceptibility.getData();
			unsigned int offsetChargeSusceptibility
				= chargeSusceptibility.getOffset({
					kIndex,
					{c0_o},
					{a1_o},
					{c1_i},
					{a0_i}
				});
			const vector<complex<double>> &spinSusceptibilityData
				= spinSusceptibility.getData();
			unsigned int offsetSpinSusceptibility
				= spinSusceptibility.getOffset({
					kIndex,
					{c0_o},
					{a1_o},
					{c1_i},
					{a0_i}
				});

			for(
				unsigned int n = 0;
				n < numEnergies;
				n++
			){
				selfEnergyVertex.at(n) += amplitude_i*amplitude_o*(
					chargeSusceptibilityData[
						offsetChargeSusceptibility + n
					]
					+ spinSusceptibilityData[
						offsetSpinSusceptibility + n
					]
				)/2.;
			}
		}
	}

	//U_1*\chi_2*U_2
	for(unsigned int in = 0; in < u1.size(); in++){
		const InteractionAmplitude &incommingAmplitude = u1.at(in);
		complex<double> amplitude_i = incommingAmplitude.getAmplitude();
		int c0_i = incommingAmplitude.getCreationOperatorIndex(0).at(0);
		int c1_i = incommingAmplitude.getCreationOperatorIndex(1).at(0);
		int a0_i = incommingAmplitude.getAnnihilationOperatorIndex(0).at(0);
		int a1_i = incommingAmplitude.getAnnihilationOperatorIndex(1).at(0);

/*		if(
			a1_i != orbitalIndices.at(3)
			|| c0_i != orbitalIndices.at(2)
			|| abs(amplitude_i) < 1e-10
		){
			continue;
		}*/
		if(
			a1_i != intraBlockIndices[3][0]
			|| c0_i != intraBlockIndices[2][0]
			|| abs(amplitude_i) < 1e-10
		){
			continue;
		}

		for(unsigned int out = 0; out < u2.size(); out++){
			const InteractionAmplitude &outgoingAmplitude = u2.at(out);
			complex<double> amplitude_o = outgoingAmplitude.getAmplitude();
			int c0_o = outgoingAmplitude.getCreationOperatorIndex(0).at(0);
			int c1_o = outgoingAmplitude.getCreationOperatorIndex(1).at(0);
			int a0_o = outgoingAmplitude.getAnnihilationOperatorIndex(0).at(0);
			int a1_o = outgoingAmplitude.getAnnihilationOperatorIndex(1).at(0);

/*			if(
				a0_o != orbitalIndices.at(0)
				|| c1_o != orbitalIndices.at(1)
				|| abs(amplitude_o) < 1e-10
			){
				continue;
			}*/
			if(
				a0_o != intraBlockIndices[0][0]
				|| c1_o != intraBlockIndices[1][0]
				|| abs(amplitude_o) < 1e-10
			){
				continue;
			}

/*			vector<complex<double>> chargeSusceptibility = rpaSusceptibilityCalculator->calculateChargeRPASusceptibility(
				kDual,
				{c0_o, a1_o, c1_i, a0_i}
			);
			vector<complex<double>> spinSusceptibility = rpaSusceptibilityCalculator->calculateSpinRPASusceptibility(
				kDual,
				{c0_o, a1_o, c1_i, a0_i}
			);*/
			const vector<complex<double>> &chargeSusceptibilityData
				= chargeSusceptibility.getData();
			unsigned int offsetChargeSusceptibility
				= chargeSusceptibility.getOffset({
					kIndex,
					{c0_o},
					{a1_o},
					{c1_i},
					{a0_i}
				});

			const vector<complex<double>> &spinSusceptibilityData
				= spinSusceptibility.getData();
			unsigned int offsetSpinSusceptibility
				= spinSusceptibility.getOffset({
					kIndex,
					{c0_o},
					{a1_o},
					{c1_i},
					{a0_i}
				});

			for(
				unsigned int n = 0;
				n < numEnergies;
				n++
			){
				selfEnergyVertex.at(n) += amplitude_i*amplitude_o*(
					chargeSusceptibilityData[
						offsetChargeSusceptibility + n
					]
					- spinSusceptibilityData[
						offsetSpinSusceptibility + n
					]
				)/2.;
			}
		}
	}

	//U_2*\chi_2*U_1
	for(unsigned int in = 0; in < u2.size(); in++){
		const InteractionAmplitude &incommingAmplitude = u2.at(in);
		complex<double> amplitude_i = incommingAmplitude.getAmplitude();
		int c0_i = incommingAmplitude.getCreationOperatorIndex(0).at(0);
		int c1_i = incommingAmplitude.getCreationOperatorIndex(1).at(0);
		int a0_i = incommingAmplitude.getAnnihilationOperatorIndex(0).at(0);
		int a1_i = incommingAmplitude.getAnnihilationOperatorIndex(1).at(0);

/*		if(
			a1_i != orbitalIndices.at(3)
			|| c0_i != orbitalIndices.at(2)
			|| abs(amplitude_i) < 1e-10
		){
			continue;
		}*/
		if(
			a1_i != intraBlockIndices[3][0]
			|| c0_i != intraBlockIndices[2][0]
			|| abs(amplitude_i) < 1e-10
		){
			continue;
		}

		for(unsigned int out = 0; out < u1.size(); out++){
			const InteractionAmplitude &outgoingAmplitude = u1.at(out);
			complex<double> amplitude_o = outgoingAmplitude.getAmplitude();
			int c0_o = outgoingAmplitude.getCreationOperatorIndex(0).at(0);
			int c1_o = outgoingAmplitude.getCreationOperatorIndex(1).at(0);
			int a0_o = outgoingAmplitude.getAnnihilationOperatorIndex(0).at(0);
			int a1_o = outgoingAmplitude.getAnnihilationOperatorIndex(1).at(0);

/*			if(
				a0_o != orbitalIndices.at(0)
				|| c1_o != orbitalIndices.at(1)
				|| abs(amplitude_o) < 1e-10
			){
				continue;
			}*/
			if(
				a0_o != intraBlockIndices[0][0]
				|| c1_o != intraBlockIndices[1][0]
				|| abs(amplitude_o) < 1e-10
			){
				continue;
			}

/*			vector<complex<double>> chargeSusceptibility = rpaSusceptibilityCalculator->calculatechargeRPASusceptibility(
				kDual,
				{c0_o, a1_o, c1_i, a0_i}
			);
			vector<complex<double>> spinSusceptibility = rpaSusceptibilityCalculator->calculateSpinRPASusceptibility(
				kDual,
				{c0_o, a1_o, c1_i, a0_i}
			);*/
			const vector<complex<double>> &chargeSusceptibilityData
				= chargeSusceptibility.getData();
			unsigned int offsetChargeSusceptibility
				= chargeSusceptibility.getOffset({
					kIndex,
					{c0_o},
					{a1_o},
					{c1_i},
					{a0_i}
				});

			const vector<complex<double>> &spinSusceptibilityData
				= spinSusceptibility.getData();
			unsigned int offsetSpinSusceptibility
				= spinSusceptibility.getOffset({
					kIndex,
					{c0_o},
					{a1_o},
					{c1_i},
					{a0_i}
				});

			for(
				unsigned int n = 0;
				n < numEnergies;
				n++
			){
				selfEnergyVertex.at(n) += amplitude_i*amplitude_o*(
					chargeSusceptibilityData[
						offsetChargeSusceptibility + n
					]
					- spinSusceptibilityData[
						offsetSpinSusceptibility + n
					]
				)/2.;
			}
		}
	}

	//U_3*\chi_3*U_3
	for(unsigned int in = 0; in < u3.size(); in++){
		const InteractionAmplitude &incommingAmplitude = u3.at(in);
		complex<double> amplitude_i = incommingAmplitude.getAmplitude();
		int c0_i = incommingAmplitude.getCreationOperatorIndex(0).at(0);
		int c1_i = incommingAmplitude.getCreationOperatorIndex(1).at(0);
		int a0_i = incommingAmplitude.getAnnihilationOperatorIndex(0).at(0);
		int a1_i = incommingAmplitude.getAnnihilationOperatorIndex(1).at(0);

/*		if(
			a1_i != orbitalIndices.at(3)
			|| c0_i != orbitalIndices.at(2)
			|| abs(amplitude_i) < 1e-10
		){
			continue;
		}*/
		if(
			a1_i != intraBlockIndices[3][0]
			|| c0_i != intraBlockIndices[2][0]
			|| abs(amplitude_i) < 1e-10
		){
			continue;
		}

		for(unsigned int out = 0; out < u3.size(); out++){
			const InteractionAmplitude &outgoingAmplitude = u3.at(out);
			complex<double> amplitude_o = outgoingAmplitude.getAmplitude();
			int c0_o = outgoingAmplitude.getCreationOperatorIndex(0).at(0);
			int c1_o = outgoingAmplitude.getCreationOperatorIndex(1).at(0);
			int a0_o = outgoingAmplitude.getAnnihilationOperatorIndex(0).at(0);
			int a1_o = outgoingAmplitude.getAnnihilationOperatorIndex(1).at(0);

/*			if(
				a0_o != orbitalIndices.at(0)
				|| c1_o != orbitalIndices.at(1)
				|| abs(amplitude_o) < 1e-10
			){
				continue;
			}*/
			if(
				a0_o != intraBlockIndices[0][0]
				|| c1_o != intraBlockIndices[1][0]
				|| abs(amplitude_o) < 1e-10
			){
				continue;
			}

/*			vector<complex<double>> spinSusceptibility = rpaSusceptibilityCalculator->calculateSpinRPASusceptibility(
				kDual,
				{c0_o, a1_o, c1_i, a0_i}
			);*/
			const vector<complex<double>> &spinSusceptibilityData
				= spinSusceptibility.getData();
			unsigned int offsetSpinSusceptibility
				= spinSusceptibility.getOffset({
				kIndex,
				{c0_o},
				{a1_o},
				{c1_i},
				{a0_i}
			});
			for(
				unsigned int n = 0;
				n < numEnergies;
				n++
			){
				selfEnergyVertex.at(n) += amplitude_i
					*amplitude_o*spinSusceptibilityData[
						offsetSpinSusceptibility + n
					];
			}
		}
	}

//	vertexTree.add(selfEnergyVertex, resultIndex);

	return selfEnergyVertex;
}

}	//End of namespace Solver
}	//End of namesapce TBTK
