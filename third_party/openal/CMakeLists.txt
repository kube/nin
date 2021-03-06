# BSD 2 - Clause License
#
# Copyright(c) 2019, Maxime Bacoux
# All rights reserved.
#
# Redistributionand use in sourceand binary forms, with or without
# modification, are permitted provided that the following conditions are met :
#
# 1. Redistributions of source code must retain the above copyright notice, this
# list of conditionsand the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice,
# this list of conditionsand the following disclaimer in the documentation
# and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED.IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

include(ExternalProject)
set(OPENAL_VERSION 1.20.1)

set(OPENAL_INSTALL_DIR "${CMAKE_BINARY_DIR}/third_party")
set(OPENAL_INCLUDE_DIRECTORY "${OPENAL_INSTALL_DIR}/include")
set(OPENAL_LIBRARY_DIRECTORY "${OPENAL_INSTALL_DIR}/lib")

file(MAKE_DIRECTORY "${OPENAL_INCLUDE_DIRECTORY}")
file(MAKE_DIRECTORY "${OPENAL_LIBRARY_DIRECTORY}")

if (WIN32)
    set(OPENAL_IMPORTED_LOCATION    "${OPENAL_INSTALL_DIR}/bin/${CMAKE_SHARED_LIBRARY_PREFIX}OpenAL32${CMAKE_SHARED_LIBRARY_SUFFIX}")
    set(OPENAL_IMPORTED_IMPLIB      "${OPENAL_INSTALL_DIR}/lib/${CMAKE_IMPORT_LIBRARY_PREFIX}OpenAL32${CMAKE_IMPORT_LIBRARY_SUFFIX}")
else()
    set(OPENAL_IMPORTED_LOCATION    "${OPENAL_INSTALL_DIR}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}openal${CMAKE_SHARED_LIBRARY_SUFFIX}")
endif()

ExternalProject_Add(
    ep_openal
    URL                 "https://github.com/kcat/openal-soft/archive/openal-soft-${OPENAL_VERSION}.tar.gz"
    URL_HASH            "SHA256=C32D10473457A8B545AAB50070FE84BE2B5B041E1F2099012777EE6BE0057C13"
    INSTALL_DIR			"${CMAKE_BINARY_DIR}/third_party"
    CMAKE_ARGS
        "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
        "-DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>"
        "-DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}"
        "-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}"
        "-DALSOFT_UTILS=OFF"
        "-DALSOFT_EXAMPLES=OFF"
        "-DALSOFT_TESTS=OFF"
        "-DALSOFT_CONFIG=OFF"
        "-DALSOFT_HRTF_DEFS=OFF"
        "-DALSOFT_AMBDEC_PRESETS=OFF"
    BUILD_BYPRODUCTS
        ${OPENAL_IMPORTED_LOCATION}
        ${OPENAL_IMPORTED_IMPLIB}
)
if (APPLE)
    ExternalProject_Add_Step(
        ep_openal
        install_name
        DEPENDEES install
        COMMAND install_name_tool -id "@rpath/libopenal.dylib" "${OPENAL_IMPORTED_LOCATION}"
    )
endif()

add_library(OpenAL::AL SHARED IMPORTED GLOBAL)
set_target_properties(
    OpenAL::AL
    PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES   "${OPENAL_INCLUDE_DIRECTORY}"
    IMPORTED_LOCATION               "${OPENAL_IMPORTED_LOCATION}"
    IMPORTED_IMPLIB                 "${OPENAL_IMPORTED_IMPLIB}"
)
add_dependencies(OpenAL::AL ep_openal)
