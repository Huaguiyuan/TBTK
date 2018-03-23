FUNCTION(TBTK_MESSAGE msg)
	IF(NOT TBTK_QUIET)
		MESSAGE(${msg})
	ENDIF(NOT TBTK_QUIET)
ENDFUNCTION(TBTK_MESSAGE)

#Print the path to the config file (Useful for finding which path that is
#actually used when debugging)
TBTK_MESSAGE("Using config file: ${CMAKE_CURRENT_LIST_DIR}/TBTKConfig.cmake")

#Include TBTKTargets.cmake
INCLUDE("${CMAKE_CURRENT_LIST_DIR}/TBTKTargets.cmake")

#Add custom Find-modules
LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/Modules/")

#Include TBTKVersion.cmake
INCLUDE(TBTKVersion)

#Add link directory for global install
LINK_DIRECTORIES(${CMAKE_CURRENT_LIST_DIR}/..)
#Add link directory for build directory
#LINK_DIRECTORIES(${CMAKE_CURRENT_LIST_DIR}/../Lib/src/)

#Detect packages
FIND_PACKAGE(BLAS REQUIRED)
FIND_PACKAGE(LAPACK REQUIRED)
FIND_PACKAGE(HDF5 COMPONENTS C CXX QUIET)
FIND_PACKAGE(ARPACK QUIET)
FIND_PACKAGE(CUDA QUIET)
FIND_PACKAGE(CURL QUIET)
FIND_PACKAGE(FFTW3 QUIET)
FIND_PACKAGE(OpenCV QUIET)
FIND_PACKAGE(OpenBLAS QUIET)
FIND_PACKAGE(OpenMP QUIET)
FIND_PACKAGE(SuperLU QUIET)
FIND_PACKAGE(wxWidgets QUIET)

#Add libraries
#LIST(APPEND TBTK_LIBRARIES "-lTBTK")
LIST(APPEND TBTK_LIBRARIES "TBTK")

IF(ARPACK_FOUND)
	TBTK_MESSAGE("[X] ARPACK")
#	LIST(APPEND TBTK_LIBRARIES " -larpack")
	LIST(APPEND TBTK_LIBRARIES "arpack")
ELSE(ARPACK_FOUND)
	TBTK_MESSAGE("[ ] ARPACK")
ENDIF(ARPACK_FOUND)

IF(CUDA_FOUND)
	TBTK_MESSAGE("[X] CUDA")
#	SET(TBTK_LIBRARIES "${TBTK_LIBRARIES} -lcudart -lcusparse")
	LIST(APPEND TBTK_LIBRARIES "cudart")
	LIST(APPEND TBTK_LIBRARIES "cusparse")
ELSE(CUDA_FOUND)
	TBTK_MESSAGE("[ ] CUDA")
ENDIF(CUDA_FOUND)

IF(CURL_FOUND)
	TBTK_MESSAGE("[X] cURL")
#	SET(TBTK_LIBRARIES "${TBTK_LIBRARIES} -lcurl")
	LIST(APPEND TBTK_LIBRARIES "curl")
ELSE(CURL_FOUND)
	TBTK_MESSAGE("[ ] cURL")
ENDIF(CURL_FOUND)

IF(FFTW3_FOUND)
	TBTK_MESSAGE("[X] FFTW3")
#	SET(TBTK_LIBRARIES "${TBTK_LIBRARIES} -lfftw3")
	LIST(APPEND TBTK_LIBRARIES "fftw3")
ELSE(FFTW3_FOUND)
	TBTK_MESSAGE("[ ] FFTW3")
ENDIF(FFTW3_FOUND)

IF(HDF5_FOUND)
	TBTK_MESSAGE("[X] HDF5")
#	LIST(APPEND TBTK_LIBRARIES "hdf5")
#	LIST(APPEND TBTK_LIBRARIES "hdf5_cpp")
	INCLUDE_DIRECTORIES(${HDF5_INCLUDE_DIRS})
#	LINK_DIRECTORIES(${HDF5_LIBRARY_DIRS})
	ADD_DEFINITIONS(${HDF_DEFINITIONS})
	LIST(APPEND TBTK_LIBRARIES ${HDF5_LIBRARIES})
ELSE(HDF5_FOUND)
	TBTK_MESSAGE("[ ] HDF5")
ENDIF(HDF5_FOUND)

IF(OpenCV_FOUND)
	TBTK_MESSAGE("[X] OpenCV")
#	SET(TBTK_LIBRARIES "${TBTK_LIBRARIES} ${OpenCV_LIBRARIES}")
	LIST(APPEND TBTK_LIBRARIES ${OpenCV_LIBRARIES})
ELSE(OpenCV_FOUND)
	TBTK_MESSAGE("[ ] OpenCV")
ENDIF(OpenCV_FOUND)

IF(OpenCV_FOUND)
	TBTK_MESSAGE("[X] OpenBLAS")
ELSE(OpenCV_FOUND)
	TBTK_MESSAGE("[ ] OpenBLAS")
ENDIF(OpenCV_FOUND)

IF(OpenMP_FOUND OR OPENMP_FOUND)
	TBTK_MESSAGE("[X] OpenMP")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
ELSE(OpenMP_FOUND OR OPENMP_FOUND)
	TBTK_MESSAGE("[ ] OpenMP")
ENDIF(OpenMP_FOUND OR OPENMP_FOUND)

IF(SuperLU_FOUND)
	TBTK_MESSAGE("[X] SuperLU")
#	SET(TBTK_LIBRARIES "${TBTK_LIBRARIES} -lsuperlu")
	LIST(APPEND TBTK_LIBRARIES "superlu")
	INCLUDE_DIRECTORIES(${SUPER_LU_INCLUDES})
ELSE(SuperLU_FOUND)
	TBTK_MESSAGE("[ ] SuperLU")
ENDIF(SuperLU_FOUND)

IF(wxWidgets_FOUND)
	TBTK_MESSAGE("[X] wxWidgets")
	INCLUDE(${wxWidgets_USE_FILE})
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${wxWidgets_CXX_FLAGS}")
#	SET(TBTK_LIBRARIES "${TBTK_LIBRARIES} ${wxWidgets_LIBRARIES}")
	LIST(APPEND TBTK_LIBRARIES ${wxWidgets_LIBRARIES})
	LINK_DIRECTORIES(${wxWidgets_LIBRARY_DIRS})
ELSE(wxWidgets_FOUND)
	TBTK_MESSAGE("[ ] wxWidgets")
ENDIF(wxWidgets_FOUND)

#LIST(APPEND TBTK_LIBRARIES "-lhdf5 -lhdf5_cpp -llapack -lblas")
LIST(APPEND TBTK_LIBRARIES "lapack")
IF(OpenBLAS_FOUND)
	LIST(APPEND TBTK_LIBRARIES "openblas")
ELSE(OpenBLAS_FOUND)
	LIST(APPEND TBTK_LIBRARIES "blas")
ENDIF()
