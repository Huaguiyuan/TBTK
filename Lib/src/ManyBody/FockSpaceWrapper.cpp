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

/** @file FockSpaceWrapper.cpp
 *
 *  @author Kristofer Björnson
 */

#include "FockSpaceWrapper.h"

namespace TBTK{

FockSpaceWrapper::FockSpaceWrapper(FockSpace<BitRegister> *fockSpace){
	brFockSpace = fockSpace;
	ebrFockSpace = NULL;
}

FockSpaceWrapper::FockSpaceWrapper(FockSpace<ExtensiveBitRegister> *fockSpace){
	brFockSpace = NULL;
	ebrFockSpace = fockSpace;
}

FockSpaceWrapper::~FockSpaceWrapper(){
	if(brFockSpace != NULL)
		delete brFockSpace;

	if(ebrFockSpace != NULL)
		delete ebrFockSpace;
}

};	//End of namespace TBTK