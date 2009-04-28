# - Find CxxTest
# Find the CxxTest suite and declare a helper macro for creating unit tests
# and integrating them with CTest.
# For more details on CxxTest see http://cxxtest.tigris.org
#
# INPUT Variables
#
#   CXXTEST_USE_PYTHON
#       If true, the CXXTEST_ADD_TEST macro will use
#       the Python test generator instead of Perl.
#
# OUTPUT Variables
#
#   CXXTEST_FOUND
#       True if the CxxTest framework was found
#   CXXTEST_INCLUDE_DIR
#       Where to find the CxxTest include directory
#   CXXTEST_PERL_TESTGEN_EXECUTABLE
#       The perl-based test generator.
#   CXXTEST_PYTHON_TESTGEN_EXECUTABLE
#       The python-based test generator.
#
# MACROS for optional use by CMake users:
#
#    CXXTEST_ADD_TEST(<test_name> <gen_source_file> <input_files_to_testgen...>)
#       Creates a CxxTest runner and adds it to the CTest testing suite
#       Parameters:
#           test_name               The name of the test
#           gen_source_file         The generated source filename to be generated by CxxTest
#           input_files_to_testgen  The list of header files containing the
#                                   CxxTest::TestSuite's to be included in this runner
#
#       #==============
#       Example Usage:
#
#           find_package(CxxTest)
#           if(CXXTEST_FOUND)
#               include_directories(${CXXTEST_INCLUDE_DIR})
#               enable_testing()
#
#               CXXTEST_ADD_TEST(unittest_foo foo_test.cc
#                                 ${CMAKE_CURRENT_SOURCE_DIR}/foo_test.h)
#               target_link_libraries(unittest_foo foo) # as needed
#           endif()
#
#              This will (if CxxTest is found):
#              1. Invoke the testgen executable to autogenerate foo_test.cc in the
#                 binary tree from "foo_test.h" in the current source directory.
#              2. Create an executable and test called unittest_foo.
#
#      #=============
#      Example foo_test.h:
#
#          #include <cxxtest/TestSuite.h>
#
#          class MyTestSuite : public CxxTest::TestSuite
#          {
#          public:
#             void testAddition( void )
#             {
#                TS_ASSERT( 1 + 1 > 1 );
#                TS_ASSERT_EQUALS( 1 + 1, 2 );
#             }
#          };
#
#
# Version 1.2 (3/2/08)
#     Included patch from Tyler Roscoe to have the perl & python binaries
#     detected based on CXXTEST_INCLUDE_DIR
# Version 1.1 (2/9/08)
#     Clarified example to illustrate need to call target_link_libraries()
#     Changed commands to lowercase
#     Added licensing info
# Version 1.0 (1/8/08)
#     Fixed CXXTEST_INCLUDE_DIRS so it will work properly
#     Eliminated superfluous CXXTEST_FOUND assignment
#     Cleaned up and added more documentation
#
# FindCxxTest.cmake
# Copyright (c) 2008-2009
#     Philip Lowman <philip@yhbt.com>
#
#  Redistribution AND use is allowed according to the terms of the New
#  BSD license.
#  For details see the accompanying COPYING-CMAKE-SCRIPTS file.

#=============================================================
# CXXTEST_ADD_TEST (public macro)
#=============================================================
macro(CXXTEST_ADD_TEST _cxxtest_testname _cxxtest_outfname)
    set(_cxxtest_real_outfname ${CMAKE_CURRENT_BINARY_DIR}/${_cxxtest_outfname})
    if(CXXTEST_USE_PYTHON)
        set(_cxxtest_executable ${CXXTEST_PYTHON_TESTGEN_EXECUTABLE})
    else()
        set(_cxxtest_executable ${CXXTEST_PERL_TESTGEN_EXECUTABLE})
    endif()

    add_custom_command(
        OUTPUT  ${_cxxtest_real_outfname}
        DEPENDS ${ARGN}
        COMMAND ${_cxxtest_executable}
        --error-printer -o ${_cxxtest_real_outfname} ${ARGN}
    )

    set_source_files_properties(${_cxxtest_real_outfname} PROPERTIES GENERATED true)
    add_executable(${_cxxtest_testname} ${_cxxtest_real_outfname})

    if(CMAKE_RUNTIME_OUTPUT_DIRECTORY)
        add_test(${_cxxtest_testname} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${_cxxtest_testname})
    elseif(EXECUTABLE_OUTPUT_PATH)
        add_test(${_cxxtest_testname} ${EXECUTABLE_OUTPUT_PATH}/${_cxxtest_testname})
    else()
        add_test(${_cxxtest_testname} ${CMAKE_CURRENT_BINARY_DIR}/${_cxxtest_testname})
    endif()

endmacro(CXXTEST_ADD_TEST)

#=============================================================
# main()
#=============================================================

find_path(CXXTEST_INCLUDE_DIR cxxtest/TestSuite.h)
find_program(CXXTEST_PERL_TESTGEN_EXECUTABLE cxxtestgen.pl
    PATHS ${CXXTEST_INCLUDE_DIR})
find_program(CXXTEST_PYTHON_TESTGEN_EXECUTABLE cxxtestgen.py
    PATHS ${CXXTEST_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(CxxTest DEFAULT_MSG CXXTEST_INCLUDE_DIR)

set(CXXTEST_INCLUDE_DIRS ${CXXTEST_INCLUDE_DIR})
