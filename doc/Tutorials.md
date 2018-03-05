Tutorials {#Tutorials}
======

These tutorials assume that TBTK has already been installed as described in the [Installation instructions](@ref InstallationInstructions).
The tutorials are written for physicisists that not necessarily are familiar with C++ and therefore introduces both TBTK as well as C++ in the first couple of tutorials.

# Getting started
- @subpage CreatingANewApplication

# Building applications
- @subpage BuildingAFirstApplicationTwoLevelSystem

@page CreatingANewApplication Creating a new application (Unix like operating systems such as Linux and Mac OS)
# Creating, building, and executing a first application {#CreatingBuildingAndExecutingAFirstApplication}
## Project creation using TBTKCreateApplication
One of the main issues when writing code in a language such as C++ is the need to manage both source files and dependencies in terms of other libraries.
In addition to the main source code, a full project therefore requires one or several supporting files and folders.
TBTK therefore comes with an executable called TBTKCreateApplication that helps setup the relevant support structure, and which in combination with CMake allows the developer to focus on developing the actuall application.
Before creating a project, it is recommended to have a folder called for example *TBTKApplications* in which different applications are created.
This can be created on the command line using
```bash
	mkdir TBTKApplications
```
Then, to enter this folder and create a new application, simply type
```bash
	cd TBTKApplications
	TBTKCreateApplication ApplicationName
```
where *ApplicationName* can be any valid folder name (no spaces).
This will create a new folder named *ApplicationName* that contains all the relevant files for building and running a TBTK application.
## Build executable
To enter the project folder and build the application, type
```bash
	cd ApplicationName
	cmake .
	make
```
Here the second line creates the relevant files required to build the application, while the third line builds the actuall executable.

It can also be useful to build the application in a separate folder from source code.
This can be achieved by instead typing
```bash
	mkdir ApplicationNameBuild
	cd ApplicationNameBuild
	cmake ../ApplicationName
	make
```

## Run the executable
The program can now be run by typing
```bash
	./build/Application
```

# Default application folder structure {#DefaultApplicationFolderStructure}
When creating an application project using TBTKCreateApplication, a number of files and folders are created.
The purpose of the folders are
| Folder name | Description                                |
|-------------|--------------------------------------------|
| build       | Contains the final executable application. |
| figures     | Intended for generated figures.            |
| include     | Contains header files (.h).                |
| src         | Contains source files (.cpp).              |

At creation the project also contains a number of files.
The files and their purpose are listed below.
Application developers can largly ignore files in gray text since these are meant to be unedited.
However, descriptions are provided to aid developers interested in customizing the build procedure.
| File name                                      | Description                                                                                                      |
|------------------------------------------------|------------------------------------------------------------------------------------------------------------------|
| <span style="color:gray">CMakeLists.txt</span> | <span style="color:gray">File used by CMake to setup the build system. The file used by the *cmake* call.</span> |
| plot.sh                                        | File intended for python plot commandin to be listed in (see TBTK/Templates for exampels).                       |
| src/main.cpp                                   | The file in which the code for the actuall application is written.                                               |

Once *cmake* is executed, a number of additional files are created
| File name | Description |
|-----------|-------------|
| <span style="color:gray">CMakeCache.txt</span>      | <span style="color:gray">See the [CMake](https://cmake.org/) documentation.</span> |
| <span style="color:gray">CMakeFiles (folder)</span> | <span style="color:gray">See the [CMake](https://cmake.org/) documentation.</span> |
| <span style="color:gray">cmake_install.cmake</span> | <span style="color:gray">See the [CMake](https://cmake.org/) documentation.</span> |
| <span style="color:gray">Makefile</span>            | <span style="color:gray">The file used by the *make* call.</span>                  |

# Template applications {#TemplateApplications}
## More complex templates
While the usage of TBTKCreateApplication as presented above is useful for starting projects from scratch, it is also possible to start from more complex projects.
```bash
	TBTKCreateApplication ApplicationName TemplateName
```
where *TemplateName* can be any of
| TemplateName                    | Description                                                                                                              |
|---------------------------------|--------------------------------------------------------------------------------------------------------------------------|
| BasicArnoldi                    | Demonstrates how to use the ArnoldiIterator.                                                                             |
| BasicChebyshev                  | Demonstrates how to use the ChebyshevExapnder.                                                                           |
| BasicDiagonalization            | Demonstrates how to use the Diagonalizer.                                                                                |
| BasicFourierTransform           | Demonstrates how to use the FourierTransform.                                                                            |
| CarbonNanotube                  | Demonstrates how to set up a carbon nanotube.                                                                            |
| HexagonalLattice                | Demonstrates how to set up a hexagonal lattice.                                                                          |
| PartialBilayer                  | Demonstrates how to set up a partial bilayer.                                                                            |
| SelfConsistentSuperconductivity | Demonstrates how to set up a self-consistent calculation for the superconducting order parameter using the Diagonalizer. |
| WireOnSuperconductor            | Demonstrates how to set up a magnetic wire on top of a two-dimensional superconducting layer.                            |

## Example: BasicDiagonalization
To demonstrate this, lets build and execute BasicDiagonalization.
Before beginning we note that this template uses the FileWriter and therefore requires that HDF5 is installed and detected by TBTK.
Therefore make sure the HDF5 box is checked in the output from *cmake* below.
Starting from the folder TBTKApplication, we type
```bash
	TBTKCreateApplication MyDiagonalizationApplication BasicDiagonalization
	cd MyDiagonalizationApplication
	cmake .
	make
	./build/Application
```
The application should run and output
```bash
	Constructing system
		Basis size: 800
	Initializing Diagonalizer
		Basis size: 800
	Running Diagonalizer

	.
```
Taking a look at the code in *src/main.cpp*, we see several lines for example reading
```cpp
	FileWriter::writeDensity(density);
```
These lines writes the results to a file called *TBTKResults.h5* (set by *FileWriter::setFileName("TBTKResults.h5")*) which will contain the results of the calculation.
Next, taking a look in the file *plot.sh*, we see several corresponding lines for example reading
```bash
	TBTKPlotDensity.py TBTKResults.h5
```
These reads the results from *TBTKResults.h5* and plots the result in files stored in the folder *figures*.
Therefore type
```bash
	bash plot.sh
```
and view the results in the *figures* folder.

@page BuildingAFirstApplicationTwoLevelSystem Building a first application (two level system)
# Problem description {#ProblemDescription}
As a first example we will create an application that models a simple two level system.
Consider therefore a single electron spin in a magnetic field described by the Hamiltonian
<center>\f$ H = -\mu_B B\sigma_z\f$,</center>
where \f$\mu_B\f$ is the Bohr magneton, B is the magnetic field strength, and \f$\sigma_z\f$ is the third Pauli matrix.
The energy split induced by a magnetic field acting on an electron spin is known as the Zeeman split.
In this tutorial we therefore calculate this energy split as a function of the magnetic field.
Moreover, at zero temperature and non-zero magnetic field strength, the system will be in its ground state and be maximally magnetized parallel to the magnetic field.
However, at finite temperatures both the ground state and the excited state will have a finite occupation determined by the Fermi-Dirac distribution.
We therefore also invstigate how the magnetization varies with magnetic field strength and temperature.

# Rewriting the Hamiltonian on canonical form {#RewritingTheHamiltonianOnCanonicalForm}
Before implementing the problem we need to rewrite the Hamiltonian on the canonical second quantized form
<center>\f$H = \sum_{\mathbf{i}\mathbf{j}}a_{\mathbf{i}\mathbf{j}}c_{\mathbf{i}}^{\dagger}c_{\mathbf{j}}\f$.</center>
For the Hamiltonian above this is particularly simple.
First of all the indices \f$\mathbf{i}\f$ and \f$\mathbf{j}\f$ are simply \f$\uparrow\f$ and \f$\downarrow\f$ for up and down spins, respectively.
Second, the Pauli matrix \f$\sigma_z\f$ only have the non-zero entries \f$1\f$ and \f$-1\f$ on the diagonal.
The Hamiltonian can therefore be written as
<center>\f$H = -\mu_B Bc_{\uparrow}^{\dagger}c_{\uparrow} + \mu_B Bc_{\downarrow}^{\dagger}c_{\downarrow}\f$.</center>
Identifying up spin with \f$0\f$ and down spin with \f$1\f$, the hopping amplitudes \f$a_{\mathbf{i}\mathbf{j}}\f$ can then be tabulated as
| Hopping amplitude              | Value          | To Index | From Index |
|--------------------------------|----------------|----------|------------|
| \f$a_{\uparrow\uparrow}\f$     | \f$-\mu_B B\f$ | {0}      | {0}        |
| \f$a_{\downarrow\downarrow}\f$ | \f$\mu_B B\f$  | {1}      | {1}        |
Here the first column is an analytical symbold for the hopping amplitude, the second is its actual value, while the third and forth column corresponds to the numerical representation of the first and second index of \f$a_{\mathbf{i}\mathbf{j}}\f$.
The three last columns is therefore a sufficient representation of the hopping amplitude and is what enters into the numerical model.
This particular way of representing the hopping amplitudes reoccurs throughout TBTK and its documentation.

# Implementing the calculation {#Implementing the calculation}
## Understanding *src/main.cpp*
We are now ready to create the application and assume that an empty project has been created as described in the "Creating a new application" tutorial.
Once created, we open the file *src/main.cpp*, which should contain the following code
```cpp
	#include "TBTK/Streams.h"

	#include <complex>

	using namespace std;
	using namespace TBTK;

	int main(int argc, char **argv){
		Streams::out << "Hello quantum world!\n";

		return 0;
	}
```
Here the first two lines are include statements that includes other C++ library components into our code.
The first include statement includes a component called *Streams* from the TBTK library that can be used to output information to the screen at runtime.
The second include statement includes *complex* numbers from the C++ standard library (STL).
In this empty project nu actual complex numbers are present by default, but complex numbers are so common in TBTK applications that they are included by default in the template projects.
Each component of TBTK is available in a separate file and as we build our applications we will include more files.

To understand the next two lines we note that in C++, functions, classes, etc can be organized into namespaces to avoid name clashes between different libraries that defines components with the same names.
In particular, every component of STL is in the namespace *std*, while every component of TBTK is in the namespace *TBTK*.
For example, the Stream class included above is by default accessed as TBTK::Streams.
However, often there are no actual name clashes and it is convenient to not have to prepend every function call etc with *std::* or *TBTK::* and it is therefore possible to tell the compiler to "use" particular namespaces.
Possible name clashes can then be handled individually by prepending only those components for which ambiguity arise by their corresponding namespaces.

The actuall entry point for the program is
```cpp
	int main(int argc, char **argv){
```
However, we note that global variable assigmnets occur ahead of this time, meaning that it in principle is possible for some execution of code to occur already before the main routine begins.
The arguments *argc* and *argv* can be used to accept arguments from the command line, and the interested reader is referred to make a web search for further information on how to use these.

Inside the main function two things occur.
First, "Hello quantum world!\n" is written to the output stream, where '<<' can be read as send the right hand side to the output stream Streams::out.
Writing to Streams::out will by default result in information being printed to the screen.
The "\n" at the end of the string is a line break character and results in whatever follows to be printed on a new line.
Finally, the main function exits by returning *0*, which is a message indicating that the application finished as expected.

## Initializing units
Having understood the structure of *src/main.cpp*, we are now ready to start implementing the actuall application.
The first step is to specify the units that we will be using.
In particular, we will measure temperature in terms of Kelvin and energies in terms of *meV*.
To achieve this we remove the print statement in the code above and replace it by
```cpp
	#include "TBTK/UnitHandler.h"

	#include <complex>

	using namespace std;
	using namespace TBTK;

	int main(int argc, char **argv){
		//Initialize the UnitHandler.
		UnitHandler::setTemperatureUnit(UnitHandler::TemperatureUnit::K);
		UnitHandler::setTemperatureScale(1);
		UnitHandler::setEnergyUnit(UnitHandler::EnergyUnit::meV);
		UnitHandler::setEnergyScale(1);

		return 0;
	}
```
We here note the use of two functions of the form
```cpp
	UnitHandler::setEnergyUnit(UnitHandler::EnergyUnit::meV);
	UnitHandler::setEnergyScale(1);
```
Together these two calls tells TBTK that any other value corresponding to an energy that is entered into the code should be understood to be in units of 1meV.
That is, these two lines defines the natural energy scale for the problem to be 1meV.
Had we instead passed for example *13606* to the second function call, then every number E passed to a TBTK function that corresponds to an energy would have ben interpretted to mean *13606*E meV*.

## Specifying parameters
The next step in our calculation is to setup variables containing the relevant parameters for the problem.
We begin by specifying some default parameter values *temperature = 300K* and *magneticField = 1T*, as well as requesting the Bohr magneton from the UnitHandler.
```cpp
	#include "TBTK/UnitHandler.h"

	#include <complex>

	using namespace std;
	using namespace TBTK;

	int main(int argc, char **argv){
		//Initialize the UnitHandler.
		UnitHandler::setTemperatureUnit(UnitHandler::TemperatureUnit::K);
		UnitHandler::setTemperatureScale(1);
		UnitHandler::setEnergyUnit(UnitHandler::EnergyUnit::eV);
		UnitHandler::setEnergyScale(1);

		//Setup input parameters.
		double temperature = 300;
		double magneticField = 1;

		//Setup parameters in natural units.
		double magneticFieldNaturalUnits = UnitHandler::convertMagneticFieldDtB(
			magneticField,
			UnitHandler::MagneticFieldUnit::T
		);
		double mu_B = UnitHandler::getMu_BN();

		return 0;
	}
```
While the two first new lines may seem simple to understad, we note that numerical numbers are unit less.
The numbers are actually only physically meaning full either implicitly through the developers interpretation of the values or explicitly by convention.
What the UnitHandler calls above does is to explicitly specify that temperatures are meassured in terms of one Kelvin.
This convention allows us to unambiguosly pass the *temperature* variable to any TBTK function.
However, the variable *magneticField* is so far only implicitly understod to mean 1T.
Moreover, there is in fact no function for specifying the convention for magnetic fields directly in TBTK since its natural unit is set indirectly by specifying the natural units for six other so called *base quantities* (see [UnitHandler](@ref UnitHandler) for detailed information).
Instead we use the following function
```cpp
	double magneticFieldNaturalUnits = UnitHandler::convertMagneticFieldDtN(
		magneticField,
		UnitHandler::MagneticFieldUnit::T
	);
```
to convert the input parameter *magneticField* from units of Tesla to current natural units.
The resulting variable *magneticFieldNaturalUnits* can the be safetly passed to other TBTK functions that expects a magnetic field or be multiplied with other parameters specified in natural units.
The letters *DtN* at the end of the function name should be read 'derived to natural'.
The last new line similarly requests the Bohr magneton on natural units, where the *N* at the end of the function name stands for *natural*.

## Setting up the model
Having specified the parameters for the problem, the Model can be setup as follows.
```cpp
	#include "TBTK/UnitHandler.h"
	#include "TBTK/Model.h"

	#include <complex>

	using namespace std;
	using namespace TBTK;

	int main(int argc, char **argv){
		//Initialize the UnitHandler.
		UnitHandler::setTemperatureUnit(UnitHandler::TemperatureUnit::K);
		UnitHandler::setTemperatureScale(1);
		UnitHandler::setEnergyUnit(UnitHandler::EnergyUnit::eV);
		UnitHandler::setEnergyScale(1);

		//Setup input parameters.
		double temperature = 300;
		double magneticField = 1;

		//Setup parameters in natural units.
		double magneticFieldNaturalUnits = UnitHandler::convertMagneticFieldDtB(
			magneticField,
			UnitHandler::MagneticFieldUnit::T
		);
		double mu_B = UnitHandler::getMu_BN();

		//Create model object.
		Model model;

		//Set the temperature.
		model.setTemperature(temperature);

		//Add HoppingAmplitudes to the Model
		model << HoppingAmplitude(-mu_B*magneticFieldNaturalUnits, {0}, {0});
		model << HoppingAmplitude( mu_B*magneticFieldNaturalUnits, {1}, {1});

		//Construct a Hilbert space for the Model.
		model.construct();

		//Set the temperature.
		model.setTemperature(temperature);

		return 0;
	}
```
Here the first new line creates the actual Model object called *model*, and in the two following lines the HoppingAmplitudes are added to the Model.
Note that the syntax for feeding HoppingAmplitudes to the Model is similar to how the string "Hello quantum world!\n" was fed to Streams::out.
Also note how the HoppingAmplitudes are constructed in complete analogy with the three last columns in the hopping amplitude table at the beginning of this tutorial.
Next, the second last new line sets up the actual Hilbert space for the problem, which mainly is used internally by the various Solvers that can be applied to the problem.
Finally, the temperature is set in the last new line.

## Choosing Solver
For a small problem like the one considered here, the best solution method often is diagonalization, which solves the problem and gives direct access to the eigenvalues and eigenstates.
We therefore her setup and run a Solver::Diagonalizer as follows.
```cpp
	#include "TBTK/UnitHandler.h"
	#include "TBTK/Model.h"
	#include "TBTK/Solver/Diagonalizer.h"

	#include <complex>

	using namespace std;
	using namespace TBTK;

	int main(int argc, char **argv){
		//Initialize the UnitHandler.
		UnitHandler::setTemperatureUnit(UnitHandler::TemperatureUnit::K);
		UnitHandler::setTemperatureScale(1);
		UnitHandler::setEnergyUnit(UnitHandler::EnergyUnit::eV);
		UnitHandler::setEnergyScale(1);

		//Setup input parameters.
		double temperature = 300;
		double magneticField = 1;

		//Setup parameters in natural units.
		double magneticFieldNaturalUnits = UnitHandler::convertMagneticFieldDtB(
			magneticField,
			UnitHandler::MagneticFieldUnit::T
		);
		double mu_B = UnitHandler::getMu_BN();

		//Create model object.
		Model model;

		//Add HoppingAmplitudes to the Model
		model << HoppingAmplitude(-mu_B*magneticFieldNaturalUnits, {0}, {0});
		model << HoppingAmplitude( mu_B*magneticFieldNaturalUnits, {1}, {1});

		//Construct a Hilbert space for the Model.
		model.construct();

		//Set the temperature.
		model.setTemperature(temperature);

		//Create Solver.
		Solver::Diagonalizer solver;
		solver.setModel(model);
		solver.run();

		return 0;
	}
```
In the second new line the Solver is told to work on the Model *model*, while the last new line runs the diagonalization procedure.

## Creating a PropertyExtractor
Because different Solvers can present themself very differently to the outside world, direct extraction of properties from the Solver is discouraged in TBTK.
Instead Solvers comes with corresponding PropertyExtractors that abstracts away some of the irrelevant numerical details of the particular Solvers and allows for focus to be put on the actual physics of the problem.
The next step is therefore to wrap the Solver in a PropertyExtractor, which is done as follows.
```cpp
	#include "TBTK/UnitHandler.h"
	#include "TBTK/Model.h"
	#include "TBTK/Solver/Diagonalizer.h"
	#include "TBTK/PropertyExtractor/Diagonalizer.h"

	#include <complex>

	using namespace std;
	using namespace TBTK;

	int main(int argc, char **argv){
		//Initialize the UnitHandler.
		UnitHandler::setTemperatureUnit(UnitHandler::TemperatureUnit::K);
		UnitHandler::setTemperatureScale(1);
		UnitHandler::setEnergyUnit(UnitHandler::EnergyUnit::eV);
		UnitHandler::setEnergyScale(1);

		//Setup input parameters.
		double temperature = 300;
		double magneticField = 1;

		//Setup parameters in natural units.
		double magneticFieldNaturalUnits = UnitHandler::convertMagneticFieldDtB(
			magneticField,
			UnitHandler::MagneticFieldUnit::T
		);
		double mu_B = UnitHandler::getMu_BN();

		//Create model object.
		Model model;

		//Add HoppingAmplitudes to the Model
		model << HoppingAmplitude(-mu_B*magneticFieldNaturalUnits, {0}, {0});
		model << HoppingAmplitude( mu_B*magneticFieldNaturalUnits, {1}, {1});

		//Construct a Hilbert space for the Model.
		model.construct();

		//Set the temperature.
		model.setTemperature(temperature);

		//Create Solver.
		Solver::Diagonalizer solver;
		solver.setModel(model);
		solver.run();

		//Create PropertyExtractor.
		PropertyExtractor::Diagonalizer propertyExtractor(solver);

		return 0;
	}
```

## Calculating the Zeeman split
To calculate the Zeeman split, we need to know the energy of the individual eigenstates of the system.
In our case we have two eigenstates with eigenvalues \f$E_1\f$ and \f$E_2\f$ and the Zeeman split is calculated as \f$E_Z = E_1 - E_2\f$.
We implement these calculations as follows.
```cpp
	#include "TBTK/UnitHandler.h"
	#include "TBTK/Model.h"
	#include "TBTK/Solver/Diagonalizer.h"
	#include "TBTK/PropertyExtractor/Diagonalizer.h"
	#include "TBTK/Property/EigenValues.h"

	#include <complex>

	using namespace std;
	using namespace TBTK;

	int main(int argc, char **argv){
		//Initialize the UnitHandler.
		UnitHandler::setTemperatureUnit(UnitHandler::TemperatureUnit::K);
		UnitHandler::setTemperatureScale(1);
		UnitHandler::setEnergyUnit(UnitHandler::EnergyUnit::eV);
		UnitHandler::setEnergyScale(1);

		//Setup input parameters.
		double temperature = 300;
		double magneticField = 1;

		//Setup parameters in natural units.
		double magneticFieldNaturalUnits = UnitHandler::convertMagneticFieldDtB(
			magneticField,
			UnitHandler::MagneticFieldUnit::T
		);
		double mu_B = UnitHandler::getMu_BN();

		//Create model object.
		Model model;

		//Add HoppingAmplitudes to the Model
		model << HoppingAmplitude(-mu_B*magneticFieldNaturalUnits, {0}, {0});
		model << HoppingAmplitude( mu_B*magneticFieldNaturalUnits, {1}, {1});

		//Construct a Hilbert space for the Model.
		model.construct();

		//Set the temperature.
		model.setTemperature(temperature);

		//Create Solver.
		Solver::Diagonalizer solver;
		solver.setModel(model);
		solver.run();

		//Create PropertyExtractor.
		PropertyExtractor::Diagonalizer propertyExtractor(solver);

		//Calculate eigen values
		Property::EigenValues eigenValues = propertyExtractor.getEigenValues();

		//Print the energies of the individual eigen states.
		Streams::out << "Energies for the individual eigenstates:\n";
		for(unsigned int n = 0; n < eigenValues.getSize(); n++)
			Streams::out << eigenValues(n) << UnitHandler::getEnergyUnitString() << "\n";

		//Print the Zeeman split.
		Streams::out << "\nZeeman split: " << eigenValues(1) - eigenValues(0)
			<< UnitHandler::getEnergyUnitString() << "\n";

		return 0;
	}
```
Here we first request the Property::EigenValues from the PropertyExtractor.
This object can seen as a function of the eigenstate indices and we can obtain the individual energies using *eigenValues(0)* and *eigenValues(1)*.
The Property::EigenValues object also contains information about the number of actual eigenvalues, which is obtained using *eigenValues.getSize()*.
This is used in the newly added for-loop to loop over all eigenvalues and print them.
We also note the call to *UnitHandler::getEnergyUnitString()*, which returns a string representation of the current base units, resulting in energies being printed together with their units.
In the two last lines, the Zeeman split is calculated and similarly printed.

<b>Note</b> UnitHandler::getEnergyUnitString() prints the unit string for the base unit.
However, eigenValues are in natural units, which here happens to be the same as the base units since we used *1* in the call to
```cpp
	UnitHandler::setEnergyScale(1);
```
In general, the natural units will need to be converted back to base units before being printed as above.
This can be achieved as follows
```cpp
	UnitHandler::convertEnergyNtB(eigenValues(n));
```
where *NtB* should be read as "natural to base".

## Calculating the magnetization
To be continued...