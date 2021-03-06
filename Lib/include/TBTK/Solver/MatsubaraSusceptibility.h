/* Copyright 2018 Kristofer Björnson
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

/** @package TBTKcalc
 *  @file MatsubaraSuscesptibility.h
 *  @brief Calculates the Susceptibility using a Matsubara summation.
 *
 *  @author Kristofer Björnson
 */

#ifndef COM_DAFER45_TBTK_SOLVER_MATSUBARA_SUSCEPTIBILITY
#define COM_DAFER45_TBTK_SOLVER_MATSUBARA_SUSCEPTIBILITY

#include "TBTK/Property/GreensFunction2.h"
#include "TBTK/RPA/MomentumSpaceContext.h"
#include "TBTK/Solver/Susceptibility.h"

#include <complex>

namespace TBTK{
namespace Solver{

class MatsubaraSusceptibility : public Susceptibility{
public:
	/** Constructor. */
	MatsubaraSusceptibility(
		const MomentumSpaceContext &momentumSpaceContext,
		const Property::GreensFunction2 &greensFunction
	);

	/** Destructor. */
//	virtual ~MatsubaraSusceptibility();

	/** Create slave SusceptibilityCalcuator. Not used. */
	virtual MatsubaraSusceptibility* createSlave();

	/** Calculate the susceptibility (not supported, but prints error
	 *  message). */
	virtual std::vector<std::complex<double>> calculateSusceptibility(
		const Index &index,
		const std::vector<std::complex<double>> &energies
	);

	/** Calculate the susceptibility. */
	std::vector<std::complex<double>> calculateSusceptibility(
		const Index &index
	);
private:
	/** The Green's function to calculate the Susceptibility form. */
	const Property::GreensFunction2 &greensFunction;

	/** Slave constructor. */
/*	MatsubaraSusceptibility(
		const MomentumSpaceContext &momentumSpaceContext,
		int *kPlusQLookupTable,
		double *fermiDiracLookupTable
	);*/

	/** Calculate the susceptibility using the Lindhard function. */
/*	template<bool useKPlusQLookupTable>
	std::vector<std::complex<double>> calculateSusceptibilityMatsubara(
		const Index &index,
		const std::vector<std::complex<double>> &energies
	);*/
};

};	//End of namespace Solver
};	//End of namespace TBTK

#endif
