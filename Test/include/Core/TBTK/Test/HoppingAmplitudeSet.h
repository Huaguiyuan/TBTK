#include "TBTK/HoppingAmplitudeSet.h"

#include "gtest/gtest.h"

namespace TBTK{

//TODO
//Sparse matrix format stored through the variables numMatrixElements,
//cooRowIndices, cooColIndices, and cooValues are not tested since the
//HoppingAmplitudeSet should not be responsible for storing these in the
//future.

TEST(HoppingAmplitudeSet, Constructor){
	//Not testable on its own
}

TEST(HoppingAmplitudeSet, ConstructorCapacity){
	//Not testable on its own
}

//TODO
//This test remains unimplemented as long as the HoppingAmplitudeSet stores the
//sparse matrices. Once sparse matrices are no longer stored, the
//copy-constructor itself can be removed.
TEST(HoppingAmplitudeSet, CopyConstructor){
}

//TODO
//This test remains unimplemented as long as the HoppingAmplitudeSet stores the
//sparse matrices. Once sparse matrices are no longer stored, the
//move-constructor itself can be removed.
TEST(HoppingAmplitudeSet, MoveConstructor){
}

TEST(HoppingAmplitudeSet, SerializeToJSON){
	HoppingAmplitudeSet hoppingAmplitudeSet0;
	hoppingAmplitudeSet0.add(HoppingAmplitude(1, {0, 0, 0}, {0, 0, 0}));
	hoppingAmplitudeSet0.add(HoppingAmplitude(1, {0, 0, 1}, {0, 0, 1}));
	hoppingAmplitudeSet0.add(HoppingAmplitude(1, {0, 0, 1}, {0, 0, 2}));
	hoppingAmplitudeSet0.add(HoppingAmplitude(1, {0, 0, 2}, {0, 0, 1}));
	hoppingAmplitudeSet0.add(HoppingAmplitude(1, {1, 1, 0}, {1, 1, 0}));
	hoppingAmplitudeSet0.add(HoppingAmplitude(1, {1, 1, 0}, {1, 1, 1}));
	hoppingAmplitudeSet0.add(HoppingAmplitude(1, {1, 1, 1}, {1, 1, 0}));
	hoppingAmplitudeSet0.construct();

	HoppingAmplitudeSet hoppingAmplitudeSet1(
		hoppingAmplitudeSet0.serialize(Serializable::Mode::JSON),
		Serializable::Mode::JSON
	);

	EXPECT_EQ(hoppingAmplitudeSet1.getBasisSize(), 5);

	std::vector<Index> indices = hoppingAmplitudeSet1.getIndexList({0, 0, IDX_ALL});
	EXPECT_TRUE(indices[0].equals({0, 0, 0}));
	EXPECT_TRUE(indices[1].equals({0, 0, 1}));
	EXPECT_TRUE(indices[2].equals({0, 0, 2}));

	indices = hoppingAmplitudeSet1.getIndexList({1, 1, IDX_ALL});
	EXPECT_TRUE(indices[0].equals({1, 1, 0}));
	EXPECT_TRUE(indices[1].equals({1, 1, 1}));

	indices = hoppingAmplitudeSet1.getIndexList({IDX_ALL, IDX_ALL, 1});
	EXPECT_TRUE(indices[0].equals({0, 0, 1}));
	EXPECT_TRUE(indices[1].equals({1, 1, 1}));
}

//TODO
//This test remains unimplemented as long as the HoppingAmplitudeSet stores the
//sparse matrices. Once sparse matrices are no longer stored, the
//destructor itself can be removed.
TEST(HoppingAmplitudeSet, destructor){
}

//TODO
//This test remains unimplemented as long as the HoppingAmplitudeSet stores the
//sparse matrices. Once sparse matrices are no longer stored, the
//assignment operator itself can be removed.
TEST(HoppingAmplitudeSet, operatorAssignment){
}

//TODO
//This test remains unimplemented as long as the HoppingAmplitudeSet stores the
//sparse matrices. Once sparse matrices are no longer stored, the
//move-assignment operator itself can be removed.
TEST(HoppingAmplitudeSet, operatorMoveAssignment){
}

TEST(HoppingAmplitudeSet, add){
	//Test normal function
	EXPECT_EXIT(
		{
			HoppingAmplitudeSet hoppingAmplitudeSet;
			hoppingAmplitudeSet.add(
				HoppingAmplitude(1, {1, 2}, {3, 4})
			);
			hoppingAmplitudeSet.add(
				HoppingAmplitude(1, {1, 2}, {3, 5})
			);
			std::cerr << "Test completed.";
			exit(0);
		},
		::testing::ExitedWithCode(0),
		"Test completed."
	);

	//Fail to add HoppingAmplitude with conflicting Index structures (short first).
	EXPECT_EXIT(
		{
			Streams::setStdMuteErr();
			HoppingAmplitudeSet hoppingAmplitudeSet;
			hoppingAmplitudeSet.add(
				HoppingAmplitude(1, {1, 2}, {3, 4})
			);
			hoppingAmplitudeSet.add(
				HoppingAmplitude(1, {1, 2}, {3, 4, 5})
			);
		},
		::testing::ExitedWithCode(1),
		""
	);

	//Fail to add HoppingAmplitude with conflicting Index structures (long first).
	EXPECT_EXIT(
		{
			Streams::setStdMuteErr();
			HoppingAmplitudeSet hoppingAmplitudeSet;
			hoppingAmplitudeSet.add(
				HoppingAmplitude(1, {1, 2}, {3, 4, 5})
			);
			hoppingAmplitudeSet.add(
				HoppingAmplitude(1, {1, 2}, {3, 4})
			);
		},
		::testing::ExitedWithCode(1),
		""
	);

	//Fail to add HoppingAmplitude with negative subindex.
	EXPECT_EXIT(
		{
			Streams::setStdMuteErr();
			HoppingAmplitudeSet hoppingAmplitudeSet;
			hoppingAmplitudeSet.add(
				HoppingAmplitude(1, {1, 2}, {3, -1, 5})
			);
		},
		::testing::ExitedWithCode(1),
		""
	);
}

/*TEST(HoppingAmplitudeSet, addHoppingAmplitudeAndHermitianConjugate){
	HoppingAmplitudeSet hoppingAmplitudeSet;
	hoppingAmplitudeSet.addHoppingAmplitudeAndHermitianConjugate(HoppingAmplitude(1, {1, 2}, {3, 4}));
	hoppingAmplitudeSet.construct();

	//Check that the HoppingAmplitude itself was added.
	const std::vector<HoppingAmplitude> *hoppingAmplitudes0 = hoppingAmplitudeSet.getHAs({3, 4});
	EXPECT_EQ(hoppingAmplitudes0->size(), 1);

	//Check that the Hermitian conjugate was added.
	const std::vector<HoppingAmplitude> *hoppingAmplitudes1 = hoppingAmplitudeSet.getHAs({1, 2});
	EXPECT_EQ(hoppingAmplitudes1->size(), 1);
}*/

TEST(HoppingAmplitudeSet, getHoppingAmplitudes){
	HoppingAmplitudeSet hoppingAmplitudeSet;
	hoppingAmplitudeSet.add(HoppingAmplitude(1, {0, 0, 0}, {0, 0, 0}));
	hoppingAmplitudeSet.add(HoppingAmplitude(1, {0, 0, 1}, {0, 0, 1}));
	hoppingAmplitudeSet.add(HoppingAmplitude(1, {0, 0, 1}, {0, 0, 2}));
	hoppingAmplitudeSet.add(HoppingAmplitude(1, {0, 0, 2}, {0, 0, 1}));
	hoppingAmplitudeSet.add(HoppingAmplitude(1, {1, 1, 0}, {1, 1, 0}));
	hoppingAmplitudeSet.add(HoppingAmplitude(1, {1, 1, 0}, {1, 1, 1}));
	hoppingAmplitudeSet.add(HoppingAmplitude(1, {1, 1, 1}, {1, 1, 0}));
	hoppingAmplitudeSet.construct();

	const std::vector<HoppingAmplitude> &hoppingAmplitudes0
		= hoppingAmplitudeSet.getHoppingAmplitudes({0, 0, 1});
	EXPECT_EQ(hoppingAmplitudes0.size(), 2);
	for(unsigned int n = 0; n < hoppingAmplitudes0.size(); n++){
		const HoppingAmplitude &hoppingAmplitude
			= hoppingAmplitudes0.at(n);

		EXPECT_TRUE(hoppingAmplitude.getFromIndex().equals({0, 0, 1}));
		EXPECT_TRUE(
			hoppingAmplitude.getToIndex().equals({0, 0, 1})
			|| hoppingAmplitude.getToIndex().equals({0, 0, 2})
		);
	}

	const std::vector<HoppingAmplitude> &hoppingAmplitudes1
		= hoppingAmplitudeSet.getHoppingAmplitudes({0, 0, 2});
	EXPECT_EQ(hoppingAmplitudes1.size(), 1);
	for(unsigned int n = 0; n < hoppingAmplitudes1.size(); n++){
		const HoppingAmplitude &hoppingAmplitude
			= hoppingAmplitudes1.at(n);

		EXPECT_TRUE(hoppingAmplitude.getFromIndex().equals({0, 0, 2}));
		EXPECT_TRUE(hoppingAmplitude.getToIndex().equals({0, 0, 1}));
	}
}

TEST(HoppingAmplitudeSet, getBasisIndex){
	HoppingAmplitudeSet hoppingAmplitudeSet;
	hoppingAmplitudeSet.add(HoppingAmplitude(1, {0, 0, 0}, {0, 0, 0}));
	hoppingAmplitudeSet.add(HoppingAmplitude(1, {0, 0, 1}, {0, 0, 1}));
	hoppingAmplitudeSet.add(HoppingAmplitude(1, {0, 0, 1}, {0, 0, 2}));
	hoppingAmplitudeSet.add(HoppingAmplitude(1, {0, 0, 2}, {0, 0, 1}));
	hoppingAmplitudeSet.add(HoppingAmplitude(1, {1, 1, 0}, {1, 1, 0}));
	hoppingAmplitudeSet.add(HoppingAmplitude(1, {1, 1, 0}, {1, 1, 1}));
	hoppingAmplitudeSet.add(HoppingAmplitude(1, {1, 1, 1}, {1, 1, 0}));

	EXPECT_EQ(hoppingAmplitudeSet.getBasisIndex({0, 0, 0}), -1);
	EXPECT_EQ(hoppingAmplitudeSet.getBasisIndex({0, 0, 1}), -1);
	EXPECT_EQ(hoppingAmplitudeSet.getBasisIndex({0, 0, 2}), -1);
	EXPECT_EQ(hoppingAmplitudeSet.getBasisIndex({1, 1, 0}), -1);
	EXPECT_EQ(hoppingAmplitudeSet.getBasisIndex({1, 1, 1}), -1);

	hoppingAmplitudeSet.construct();

	EXPECT_EQ(hoppingAmplitudeSet.getBasisIndex({0, 0, 0}), 0);
	EXPECT_EQ(hoppingAmplitudeSet.getBasisIndex({0, 0, 1}), 1);
	EXPECT_EQ(hoppingAmplitudeSet.getBasisIndex({0, 0, 2}), 2);
	EXPECT_EQ(hoppingAmplitudeSet.getBasisIndex({1, 1, 0}), 3);
	EXPECT_EQ(hoppingAmplitudeSet.getBasisIndex({1, 1, 1}), 4);
}

TEST(HoppingAmplitudeSet, getPhysicsIndex){
	HoppingAmplitudeSet hoppingAmplitudeSet;
	hoppingAmplitudeSet.add(HoppingAmplitude(1, {0, 0, 0}, {0, 0, 0}));
	hoppingAmplitudeSet.add(HoppingAmplitude(1, {0, 0, 1}, {0, 0, 1}));
	hoppingAmplitudeSet.add(HoppingAmplitude(1, {0, 0, 1}, {0, 0, 2}));
	hoppingAmplitudeSet.add(HoppingAmplitude(1, {0, 0, 2}, {0, 0, 1}));
	hoppingAmplitudeSet.add(HoppingAmplitude(1, {1, 1, 0}, {1, 1, 0}));
	hoppingAmplitudeSet.add(HoppingAmplitude(1, {1, 1, 0}, {1, 1, 1}));
	hoppingAmplitudeSet.add(HoppingAmplitude(1, {1, 1, 1}, {1, 1, 0}));

	EXPECT_EXIT(
		{
			Streams::setStdMuteErr();
			hoppingAmplitudeSet.getPhysicalIndex(0);
		},
		::testing::ExitedWithCode(1),
		""
	);

	hoppingAmplitudeSet.construct();

	EXPECT_TRUE(hoppingAmplitudeSet.getPhysicalIndex(0).equals({0, 0, 0}));
	EXPECT_TRUE(hoppingAmplitudeSet.getPhysicalIndex(1).equals({0, 0, 1}));
	EXPECT_TRUE(hoppingAmplitudeSet.getPhysicalIndex(2).equals({0, 0, 2}));
	EXPECT_TRUE(hoppingAmplitudeSet.getPhysicalIndex(3).equals({1, 1, 0}));
	EXPECT_TRUE(hoppingAmplitudeSet.getPhysicalIndex(4).equals({1, 1, 1}));
	EXPECT_EXIT(
		{
			Streams::setStdMuteErr();
			hoppingAmplitudeSet.getPhysicalIndex(-1);
		},
		::testing::ExitedWithCode(1),
		""
	);
	EXPECT_EXIT(
		{
			Streams::setStdMuteErr();
			hoppingAmplitudeSet.getPhysicalIndex(5);
		},
		::testing::ExitedWithCode(1),
		""
	);
}

TEST(HoppingAmplitudeSet, getBasisSize){
	HoppingAmplitudeSet hoppingAmplitudeSet;
	EXPECT_EQ(hoppingAmplitudeSet.getBasisSize(), -1);
	hoppingAmplitudeSet.add(HoppingAmplitude(1, {1, 2}, {3, 4}));
	hoppingAmplitudeSet.add(HoppingAmplitude(1, {1, 2}, {3, 2}));
	hoppingAmplitudeSet.add(HoppingAmplitude(1, {3, 4}, {1, 2}));
	hoppingAmplitudeSet.add(HoppingAmplitude(1, {3, 2}, {1, 2}));
	EXPECT_EQ(hoppingAmplitudeSet.getBasisSize(), -1);
	hoppingAmplitudeSet.construct();
	EXPECT_EQ(hoppingAmplitudeSet.getBasisSize(), 3);
}

TEST(HoppingAmplitudeSet, isProperSubspace){
	HoppingAmplitudeSet hoppingAmplitudeSet;

	hoppingAmplitudeSet.add(
		HoppingAmplitude(1, {1, 2, 3, 4, 5}, {1, 2, 3, 4, 5})
	);
	hoppingAmplitudeSet.add(
		HoppingAmplitude(1, {1, 2, 3, 4, 5}, {1, 2, 3, 5, 5})
	);
	hoppingAmplitudeSet.add(
		HoppingAmplitude(1, {1, 2, 3, 5, 5}, {1, 2, 3, 4, 5})
	);
	hoppingAmplitudeSet.add(
		HoppingAmplitude(1, {1, 2, 3, 6, 5}, {1, 2, 3, 7, 5})
	);
	hoppingAmplitudeSet.add(
		HoppingAmplitude(1, {1, 2, 3, 7, 5}, {1, 2, 3, 6, 5})
	);

	hoppingAmplitudeSet.add(
		HoppingAmplitude(1, {2, 2, 2, 4, 5}, {2, 2, 2, 4, 5})
	);
	hoppingAmplitudeSet.add(
		HoppingAmplitude(1, {2, 2, 2, 4, 5}, {2, 2, 2, 5, 5})
	);
	hoppingAmplitudeSet.add(
		HoppingAmplitude(1, {2, 2, 2, 5, 5}, {2, 2, 2, 4, 5})
	);
	hoppingAmplitudeSet.add(
		HoppingAmplitude(1, {2, 2, 2, 6, 5}, {2, 2, 2, 7, 5})
	);
	hoppingAmplitudeSet.add(
		HoppingAmplitude(1, {2, 2, 2, 7, 5}, {2, 2, 2, 6, 5})
	);

	EXPECT_EXIT(
		{
			Streams::setStdMuteErr();
			hoppingAmplitudeSet.isProperSubspace({1, -1});
		},
		::testing::ExitedWithCode(1),
		""
	);
	EXPECT_TRUE(hoppingAmplitudeSet.isProperSubspace({1, 2}));
	EXPECT_TRUE(hoppingAmplitudeSet.isProperSubspace({2, 2}));
	EXPECT_TRUE(hoppingAmplitudeSet.isProperSubspace({1, 1}));
	EXPECT_TRUE(hoppingAmplitudeSet.isProperSubspace({1, 2, 3}));
	EXPECT_FALSE(hoppingAmplitudeSet.isProperSubspace({1, 2, 3, 4}));
	EXPECT_FALSE(hoppingAmplitudeSet.isProperSubspace({1, 2, 3, 4, 5}));
	EXPECT_TRUE(hoppingAmplitudeSet.isProperSubspace({1, 3}));
}

TEST(HoppingAmplitudeSet, getSubspaceIndices){
	HoppingAmplitudeSet hoppingAmplitudeSet;
	hoppingAmplitudeSet.add(HoppingAmplitude(1, {0, 0, 0}, {0, 0, 0}));
	hoppingAmplitudeSet.add(HoppingAmplitude(1, {0, 0, 1}, {0, 0, 1}));
	hoppingAmplitudeSet.add(HoppingAmplitude(1, {0, 0, 1}, {0, 0, 2}));
	hoppingAmplitudeSet.add(HoppingAmplitude(1, {0, 0, 2}, {0, 0, 1}));
	hoppingAmplitudeSet.add(HoppingAmplitude(1, {1, 1, 0}, {1, 1, 0}));
	hoppingAmplitudeSet.add(HoppingAmplitude(1, {1, 1, 0}, {1, 1, 1}));
	hoppingAmplitudeSet.add(HoppingAmplitude(1, {1, 1, 1}, {1, 1, 0}));
	hoppingAmplitudeSet.construct();

	IndexTree subspaceIndices = hoppingAmplitudeSet.getSubspaceIndices();

	EXPECT_EQ(subspaceIndices.getSize(), 2);

	IndexTree::ConstIterator iterator = subspaceIndices.cbegin();
	EXPECT_TRUE((*iterator).equals({0, 0}));
	++iterator;
	EXPECT_TRUE((*iterator).equals({1, 1}));
}

TEST(HoppingAmplitudeSet, construct){
	//Already tested through
	//HoppingAmplitudeSet::getBasisSize()
	//HoppingAmplitudeSet::getBasisIndex()
	//HoppingAmplitudeSet::getPhysicalIndex()
}

TEST(HoppingAmplitudeSet, getIsConstructed){
	HoppingAmplitudeSet hoppingAmplitudeSet;
	EXPECT_FALSE(hoppingAmplitudeSet.getIsConstructed());
	hoppingAmplitudeSet.construct();
	EXPECT_TRUE(hoppingAmplitudeSet.getIsConstructed());
}

TEST(HoppingAmplitudeSet, getIndexList){
	HoppingAmplitudeSet hoppingAmplitudeSet;
	hoppingAmplitudeSet.add(HoppingAmplitude(1, {0, 0, 0}, {0, 0, 0}));
	hoppingAmplitudeSet.add(HoppingAmplitude(1, {0, 0, 1}, {0, 0, 1}));
	hoppingAmplitudeSet.add(HoppingAmplitude(1, {0, 0, 1}, {0, 0, 2}));
	hoppingAmplitudeSet.add(HoppingAmplitude(1, {0, 0, 2}, {0, 0, 1}));
	hoppingAmplitudeSet.add(HoppingAmplitude(1, {1, 1, 0}, {1, 1, 0}));
	hoppingAmplitudeSet.add(HoppingAmplitude(1, {1, 1, 0}, {1, 1, 1}));
	hoppingAmplitudeSet.add(HoppingAmplitude(1, {1, 1, 1}, {1, 1, 0}));
	hoppingAmplitudeSet.construct();

	std::vector<Index> indices = hoppingAmplitudeSet.getIndexList({0, 0, IDX_ALL});
	EXPECT_TRUE(indices[0].equals({0, 0, 0}));
	EXPECT_TRUE(indices[1].equals({0, 0, 1}));
	EXPECT_TRUE(indices[2].equals({0, 0, 2}));

	indices = hoppingAmplitudeSet.getIndexList({1, 1, IDX_ALL});
	EXPECT_TRUE(indices[0].equals({1, 1, 0}));
	EXPECT_TRUE(indices[1].equals({1, 1, 1}));

	indices = hoppingAmplitudeSet.getIndexList({IDX_ALL, IDX_ALL, 1});
	EXPECT_TRUE(indices[0].equals({0, 0, 1}));
	EXPECT_TRUE(indices[1].equals({1, 1, 1}));
}

TEST(HoppingAmplitudeSet, getFirstIndexInBlock){
	HoppingAmplitudeSet hoppingAmplitudeSet;
	hoppingAmplitudeSet.add(HoppingAmplitude(1, {0, 0, 0}, {0, 0, 0}));
	hoppingAmplitudeSet.add(HoppingAmplitude(1, {0, 0, 1}, {0, 0, 1}));
	hoppingAmplitudeSet.add(HoppingAmplitude(1, {0, 0, 1}, {0, 0, 2}));
	hoppingAmplitudeSet.add(HoppingAmplitude(1, {0, 0, 2}, {0, 0, 1}));
	hoppingAmplitudeSet.add(HoppingAmplitude(1, {1, 1, 0}, {1, 1, 0}));
	hoppingAmplitudeSet.add(HoppingAmplitude(1, {1, 1, 0}, {1, 1, 1}));
	hoppingAmplitudeSet.add(HoppingAmplitude(1, {1, 1, 1}, {1, 1, 0}));
	hoppingAmplitudeSet.construct();

	EXPECT_EXIT(
		{
			Streams::setStdMuteErr();
			hoppingAmplitudeSet.getFirstIndexInBlock({0, 0, 1});
		},
		::testing::ExitedWithCode(1),
		""
	);

	EXPECT_EQ(hoppingAmplitudeSet.getFirstIndexInBlock({0, 1}), -1);
	EXPECT_EQ(hoppingAmplitudeSet.getFirstIndexInBlock({0, 0}), 0);
	EXPECT_EQ(hoppingAmplitudeSet.getFirstIndexInBlock({1, 1}), 3);
}

TEST(HoppingAmplitudeSet, getLastIndexInBlock){
	HoppingAmplitudeSet hoppingAmplitudeSet;
	hoppingAmplitudeSet.add(HoppingAmplitude(1, {0, 0, 0}, {0, 0, 0}));
	hoppingAmplitudeSet.add(HoppingAmplitude(1, {0, 0, 1}, {0, 0, 1}));
	hoppingAmplitudeSet.add(HoppingAmplitude(1, {0, 0, 1}, {0, 0, 2}));
	hoppingAmplitudeSet.add(HoppingAmplitude(1, {0, 0, 2}, {0, 0, 1}));
	hoppingAmplitudeSet.add(HoppingAmplitude(1, {1, 1, 0}, {1, 1, 0}));
	hoppingAmplitudeSet.add(HoppingAmplitude(1, {1, 1, 0}, {1, 1, 1}));
	hoppingAmplitudeSet.add(HoppingAmplitude(1, {1, 1, 1}, {1, 1, 0}));
	hoppingAmplitudeSet.construct();

	EXPECT_EXIT(
		{
			Streams::setStdMuteErr();
			hoppingAmplitudeSet.getLastIndexInBlock({0, 0, 1});
		},
		::testing::ExitedWithCode(1),
		""
	);

	EXPECT_EQ(hoppingAmplitudeSet.getLastIndexInBlock({0, 1}), -1);
	EXPECT_EQ(hoppingAmplitudeSet.getLastIndexInBlock({0, 0}), 2);
	EXPECT_EQ(hoppingAmplitudeSet.getLastIndexInBlock({1, 1}), 4);
}

TEST(HoppingAmplitudeSet, getSparseMatrix){
	HoppingAmplitudeSet hoppingAmplitudeSet;
	hoppingAmplitudeSet.add(HoppingAmplitude(1, {1}, {0}));
	hoppingAmplitudeSet.add(HoppingAmplitude(2, {0}, {1}));
	hoppingAmplitudeSet.add(HoppingAmplitude(3, {2}, {2}));
	hoppingAmplitudeSet.add(HoppingAmplitude(4, {4}, {3}));
	hoppingAmplitudeSet.add(HoppingAmplitude(5, {3}, {4}));
	hoppingAmplitudeSet.construct();

	SparseMatrix<std::complex<double>> sparseMatrix
		= hoppingAmplitudeSet.getSparseMatrix();

	ASSERT_EQ(sparseMatrix.getNumRows(), 5);
	ASSERT_EQ(sparseMatrix.getNumColumns(), 5);

	//Check column pointers.
	const unsigned int *columnPointers
		= sparseMatrix.getCSCColumnPointers();
	EXPECT_EQ(columnPointers[0], 0);
	EXPECT_EQ(columnPointers[1], 1);
	EXPECT_EQ(columnPointers[2], 2);
	EXPECT_EQ(columnPointers[3], 3);
	EXPECT_EQ(columnPointers[4], 4);
	EXPECT_EQ(columnPointers[5], 5);

	//Check rows.
	const unsigned int *rows
		= sparseMatrix.getCSCRows();
	EXPECT_EQ(rows[0], 1);
	EXPECT_EQ(rows[1], 0);
	EXPECT_EQ(rows[2], 2);
	EXPECT_EQ(rows[3], 4);
	EXPECT_EQ(rows[4], 3);

	//Check values.
	const std::complex<double> *values = sparseMatrix.getCSCValues();
	EXPECT_DOUBLE_EQ(real(values[0]), 1);
	EXPECT_DOUBLE_EQ(imag(values[0]), 0);
	EXPECT_DOUBLE_EQ(real(values[1]), 2);
	EXPECT_DOUBLE_EQ(imag(values[1]), 0);
	EXPECT_DOUBLE_EQ(real(values[2]), 3);
	EXPECT_DOUBLE_EQ(imag(values[2]), 0);
	EXPECT_DOUBLE_EQ(real(values[3]), 4);
	EXPECT_DOUBLE_EQ(imag(values[3]), 0);
	EXPECT_DOUBLE_EQ(real(values[4]), 5);
	EXPECT_DOUBLE_EQ(imag(values[4]), 0);
}

TEST(HoppingAmplitudeSet, serialize){
	//Already tested through serializeToJSON
}

TEST(HoppingAmplitudeSet, getSizeInBytes){
	HoppingAmplitudeSet hoppingAmplitudeSet;
	EXPECT_TRUE(hoppingAmplitudeSet.getSizeInBytes() > 0);
}

//TODO
//...
TEST(HoppingAmplitudeSet, Iterator){
}

};
