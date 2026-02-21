cmake_minimum_required(VERSION 3.22)
project(pimento)

set(${PROJECT_NAME}_MAJOR_VERSION 0)
set(${PROJECT_NAME}_MINOR_VERSION 1)
set(${PROJECT_NAME}_PATCH_VERSION 0)
set(${PROJECT_NAME}_VERSION
    ${${PROJECT_NAME}_MAJOR_VERSION}.${${PROJECT_NAME}_MINOR_VERSION}.${${PROJECT_NAME}_PATCH_VERSION})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(${PROJECT_NAME}_CMAKE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Disable warnings for dependencies
set(CMAKE_CXX_FLAGS "-w")

# Fetch argparse
include(FetchContent)
FetchContent_Declare(
    argparse
    GIT_REPOSITORY https://github.com/p-ranav/argparse.git
    GIT_TAG        v3.2
    SYSTEM)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.17.0
    SYSTEM)

FetchContent_Declare(
    magic_enum
    GIT_REPOSITORY https://github.com/Neargye/magic_enum.git
    GIT_TAG v0.9.7
    SYSTEM)

FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG        v1.17.0
    SYSTEM)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(argparse spdlog magic_enum googletest)

# Re-enable warnings
set(CMAKE_CXX_FLAGS
    "-Wall -Wextra -pedantic -Wdouble-promotion -Wconversion -Wfatal-errors -Wno-unused-function -fno-fast-math")
if(NOT ${CMAKE_BUILD_TYPE} MATCHES Debug)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
endif()

# Utils library target
add_library(${PROJECT_NAME}_utils STATIC
    src/utils.cpp)
target_include_directories(${PROJECT_NAME}_utils PUBLIC
$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
$<INSTALL_INTERFACE:include>)
target_link_libraries(${PROJECT_NAME}_utils PUBLIC spdlog::spdlog_header_only)

# Lexer library target
add_library(${PROJECT_NAME}_lexer STATIC
    src/input_buffer.cpp
    src/lexer.cpp)
target_include_directories(${PROJECT_NAME}_lexer PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)
target_link_libraries(${PROJECT_NAME}_lexer PUBLIC
    ${PROJECT_NAME}_utils magic_enum::magic_enum)

# Parser library target
add_library(${PROJECT_NAME}_parser STATIC
    src/parser.cpp)
target_include_directories(${PROJECT_NAME}_parser PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)
target_link_libraries(${PROJECT_NAME}_parser PUBLIC
    ${PROJECT_NAME}_lexer
    ${PROJECT_NAME}_utils magic_enum::magic_enum)

# Generator library target
add_library(${PROJECT_NAME}_generator STATIC
    src/generator.cpp)
target_include_directories(${PROJECT_NAME}_generator PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)
target_link_libraries(${PROJECT_NAME}_generator PUBLIC
    ${PROJECT_NAME}_parser
    ${PROJECT_NAME}_utils magic_enum::magic_enum)

# Pimento executable target
add_executable(${PROJECT_NAME}
    src/pimento.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)
target_link_libraries(${PROJECT_NAME} PUBLIC
    argparse
    ${PROJECT_NAME}_generator
    ${PROJECT_NAME}_utils)
target_compile_definitions(${PROJECT_NAME} PUBLIC
    PROJECT_VERSION="${${PROJECT_NAME}_VERSION}")

if (BUILD_TESTING)
    enable_testing()

    # Test Settings
    get_filename_component(${PROJECT_NAME}_TESTS_DIR
        ${CMAKE_CURRENT_SOURCE_DIR}/tests ABSOLUTE)
    get_filename_component(${PROJECT_NAME}_TEST_RESOURCES_DIR
        ${${PROJECT_NAME}_TESTS_DIR}/resources ABSOLUTE)
    get_filename_component(
        FLOAT_PASS_PATH
        ${${PROJECT_NAME}_TEST_RESOURCES_DIR}/float_pass.csv
        ABSOLUTE)
    get_filename_component(
        INTEGER_PASS_PATH
        ${${PROJECT_NAME}_TEST_RESOURCES_DIR}/integer_pass.csv
        ABSOLUTE)
    get_filename_component(
        SCIENTIFIC_PASS_PATH
        ${${PROJECT_NAME}_TEST_RESOURCES_DIR}/scientific_pass.csv
        ABSOLUTE)
    get_filename_component(
        FLOAT_FAIL_PATH
        ${${PROJECT_NAME}_TEST_RESOURCES_DIR}/float_fail.txt
        ABSOLUTE)
    get_filename_component(
        INTEGER_FAIL_PATH
        ${${PROJECT_NAME}_TEST_RESOURCES_DIR}/integer_fail.txt
        ABSOLUTE)
    get_filename_component(
        SCIENTIFIC_FAIL_PATH
        ${${PROJECT_NAME}_TEST_RESOURCES_DIR}/scientific_fail.txt
        ABSOLUTE)
    configure_file(
        ${${PROJECT_NAME}_TESTS_DIR}/cmake/test_settings.hpp.in
        ${${PROJECT_NAME}_TESTS_DIR}/test_settings.hpp
        @ONLY)

    # GTest
    # Lexer
    include(GoogleTest)
    # add_executable(lexer_test EXCLUDE_FROM_ALL
    #     tests/lexer_test.cpp)
    # target_include_directories(lexer_test PRIVATE
    #     $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    #     $<INSTALL_INTERFACE:include>)
    # target_link_libraries(lexer_test PRIVATE
    #     ${PROJECT_NAME}_lexer GTest::gtest_main)
    # gtest_discover_tests(lexer_test)

    # Parser
    # add_executable(parser_test EXCLUDE_FROM_ALL
    #     tests/parser_test.cpp)
    # target_include_directories(parser_test PRIVATE
    #     $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    #     $<INSTALL_INTERFACE:include>)
    # target_link_libraries(parser_test PRIVATE
    #     ${PROJECT_NAME}_parser GTest::gtest_main)
    # gtest_discover_tests(parser_test)

    # Linters
    set(${PROJECT_NAME}_LINTER_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/Linters)

    ## CMake Lint
    find_program(CMAKE_LINT_PATH cmake-lint)
    if(NOT CMAKE_LINT_PATH)
        message(FATAL_ERROR "cmake-lint not found! Aborting...")
    endif() # NOT CMAKE_LINT_PATH
    add_test(NAME ${PROJECT_NAME}_cmake_linter
        COMMAND ${CMAKE_LINT_PATH}
        ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt
        --tab-size 4
        --line-width 120
        --disabled-codes C0103
        -o ${${PROJECT_NAME}_LINTER_OUTPUT_DIR}/${PROJECT_NAME}_cmake_linter.txt
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

    ## Clang Tidy
    include(ProcessorCount)
    ProcessorCount(N_PROC)
    find_program(
        CLANG_TIDY_PATH
        NAMES run-clang-tidy)
    if(NOT CLANG_TIDY_PATH)
        message(FATAL_ERROR "clang-tidy not found! Aborting...")
    endif() # NOT CLANG_TIDY_PATH
    add_test(NAME ${PROJECT_NAME}_clang_tidy_linter
        COMMAND ${CLANG_TIDY_PATH}
        -config-file ${CMAKE_CURRENT_SOURCE_DIR}/.clang-tidy
        -p ${CMAKE_CURRENT_BINARY_DIR}
        -j ${N_PROC}
        -use-color
        -warnings-as-errors=*
        -export-fixes
        ${${PROJECT_NAME}_LINTER_OUTPUT_DIR}/${PROJECT_NAME}_clang_tidy_linter.yaml
        ${CMAKE_CURRENT_SOURCE_DIR}/src/pimento.cpp
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

    find_package(Doxygen)
    if(Doxygen_FOUND)
        set(DOXYGEN_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/docs)
        set(DOXYGEN_WARN_LOGFILE ${${PROJECT_NAME}_LINTER_OUTPUT_DIR}/${PROJECT_NAME}_doxygen_warnings.txt)

        configure_file(
            ${${PROJECT_NAME}_CMAKE_DIR}/Doxyfile.in
            ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
            @ONLY)

        doxygen_add_docs(
            doxygen_docs
            ${CMAKE_CURRENT_SOURCE_DIR}/include/pimento}
            COMMENT "Generating API documentation"
            CONFIG_FILE ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        )

        add_test(NAME pimento_doxygen_linter
                COMMAND ${CMAKE_COMMAND} --build . --target doxygen_docs
                WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    else()
        message(WARNING "Doxygen not found! API documentation will not be generated.")
    endif() # Doxygen_FOUND

    # Valgrind Memcheck Tests
    # TODO(lthomas): Refactor this into a function
    # add_test(NAME ${PROJECT_NAME}_valgrind_file
    #     COMMAND valgrind
    #     --error-exitcode=1
    #     --tool=memcheck
    #     --leak-check=full
    #     --errors-for-leak-kinds=definite
    #     --show-leak-kinds=definite
    #     $<TARGET_FILE:${PROJECT_NAME}>
    #         ${${PROJECT_NAME}_TEST_RESOURCES_DIR}/valgrind_test.oil
    #     WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})

    # add_test(NAME ${PROJECT_NAME}_valgrind_stdout
    #     COMMAND valgrind
    #     --error-exitcode=1
    #     --tool=memcheck
    #     --leak-check=full
    #     --errors-for-leak-kinds=definite
    #     --show-leak-kinds=definite
    #     $<TARGET_FILE:${PROJECT_NAME}>
    #         ${${PROJECT_NAME}_TEST_RESOURCES_DIR}/valgrind_test.oil
    #         --stdout
    #     WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})

    # add_test(NAME ${PROJECT_NAME}_valgrind_file_trace
    #     COMMAND valgrind
    #     --error-exitcode=1
    #     --tool=memcheck
    #     --leak-check=full
    #     --errors-for-leak-kinds=definite
    #     --show-leak-kinds=definite
    #     $<TARGET_FILE:${PROJECT_NAME}>
    #         ${${PROJECT_NAME}_TEST_RESOURCES_DIR}/valgrind_test.oil
    #         --trace
    #     WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})

    # add_test(NAME ${PROJECT_NAME}_valgrind_stdout_trace
    #     COMMAND valgrind
    #     --error-exitcode=1
    #     --tool=memcheck
    #     --leak-check=full
    #     --errors-for-leak-kinds=definite
    #     --show-leak-kinds=definite
    #     $<TARGET_FILE:${PROJECT_NAME}>
    #         ${${PROJECT_NAME}_TEST_RESOURCES_DIR}/valgrind_test.oil
    #         --stdout --trace
    #     WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})

    # add_test(NAME ${PROJECT_NAME}_valgrind_file_debug
    #     COMMAND valgrind
    #     --error-exitcode=1
    #     --tool=memcheck
    #     --leak-check=full
    #     --errors-for-leak-kinds=definite
    #     --show-leak-kinds=definite
    #     $<TARGET_FILE:${PROJECT_NAME}>
    #         ${${PROJECT_NAME}_TEST_RESOURCES_DIR}/valgrind_test.oil
    #         --debug
    #     WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})

    # add_test(NAME ${PROJECT_NAME}_valgrind_stdout_debug
    #     COMMAND valgrind
    #     --error-exitcode=1
    #     --tool=memcheck
    #     --leak-check=full
    #     --errors-for-leak-kinds=definite
    #     --show-leak-kinds=definite
    #     $<TARGET_FILE:${PROJECT_NAME}>
    #         ${${PROJECT_NAME}_TEST_RESOURCES_DIR}/valgrind_test.oil
    #         --stdout --debug
    #     WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})

    # add_test(NAME ${PROJECT_NAME}_valgrind_file_verbose
    #     COMMAND valgrind
    #     --error-exitcode=1
    #     --tool=memcheck
    #     --leak-check=full
    #     --errors-for-leak-kinds=definite
    #     --show-leak-kinds=definite
    #     $<TARGET_FILE:${PROJECT_NAME}>
    #         ${${PROJECT_NAME}_TEST_RESOURCES_DIR}/valgrind_test.oil
    #         --verbose
    #     WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})

    # add_test(NAME ${PROJECT_NAME}_valgrind_stdout_verbose
    #     COMMAND valgrind
    #     --error-exitcode=1
    #     --tool=memcheck
    #     --leak-check=full
    #     --errors-for-leak-kinds=definite
    #     --show-leak-kinds=definite
    #     $<TARGET_FILE:${PROJECT_NAME}>
    #         ${${PROJECT_NAME}_TEST_RESOURCES_DIR}/valgrind_test.oil
    #         --stdout --verbose
    #     WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})

    # # Gcovr Coverage Report Generation
    # list(APPEND CMAKE_MODULE_PATH ${${PROJECT_NAME}_CMAKE_DIR}/modules)
    # include(CodeCoverage)
    # append_coverage_compiler_flags()
    # set(COVERAGE_GCOVR_EXCLUDES ${FETCHCONTENT_BASE_DIR})
    # # Use llvm-cov gcov for Clang
    # if(CMAKE_C_COMPILER_ID MATCHES "Clang")
    #     find_program(
    #         LLVM_COV_PATH
    #         NAMES llvm-cov)
    #     if(NOT LLVM_COV_PATH)
    #         message(FATAL_ERROR "llvm-cov not found! Aborting...")
    #     endif() # NOT LLVM_COV_PATH
    #     list(APPEND GCOVR_ADDITIONAL_ARGS
    #         --gcov-executable "${LLVM_COV_PATH} gcov")
    # endif()
    # set(Coverage_DEPENDENCIES run_tests)
    # setup_target_for_coverage_gcovr_html(NAME coverage)

    # Custom Testing Targets
    add_custom_target(build_tests
        # DEPENDS lexer_test parser_test
        COMMENT "Build unit tests")

    find_program(CTEST_PATH ctest)
    if(NOT CTEST_PATH)
        message(FATAL_ERROR "ctest not found! Aborting...")
    endif() # NOT CTEST_PATH
    file(MAKE_DIRECTORY ${${PROJECT_NAME}_LINTER_OUTPUT_DIR})
    add_custom_target(run_linters
        COMMAND ${CTEST_PATH}
        --output-on-failure
        -R "${PROJECT_NAME}_.*_linter"
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Run linters")

    add_custom_target(run_tests
        COMMAND ${CTEST_PATH}
        --output-on-failure
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        DEPENDS build_tests ${PROJECT_NAME}
        COMMENT "Run unit tests")
endif()

find_program(COMPDB compdb)
if(COMPDB)
    add_custom_target(
        compdb ALL
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_SOURCE_DIR}/build
        COMMAND ${CMAKE_COMMAND} -E echo "Running compdb to generate compile_commands.json..."
        COMMAND ${COMPDB} -p ${CMAKE_BINARY_DIR} list > ${CMAKE_SOURCE_DIR}/build/compile_commands.json
        COMMAND ${CMAKE_COMMAND} -E echo "✓ compile_commands.json updated. Restart clangd language server to update."
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Generating enhanced compile_commands.json with compdb"
    )
else()
    message(WARN "\"compdb\" not found! Compile commands may not be generated properly!")
endif()

install(
    TARGETS ${PROJECT_NAME}
    EXPORT export_${PROJECT_NAME}
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    COMPONENT binaries)

set(export_targets ${export_targets};${PROJECT_NAME})
export(EXPORT export_${PROJECT_NAME}
    FILE "${PROJECT_BINARY_DIR}/export_${PROJECT_NAME}.cmake")

set(CONF_INCLUDE_DIRS "${CMAKE_INSTALL_PREFIX}/include")
configure_file(
    ${${PROJECT_NAME}_CMAKE_DIR}/${PROJECT_NAME}Config.cmake.in
    "${PROJECT_BINARY_DIR}/${CMAKE_FILES_DIRECTORY}/${PROJECT_NAME}Config.cmake"
    @ONLY)

configure_file(
    ${${PROJECT_NAME}_CMAKE_DIR}/${PROJECT_NAME}ConfigVersion.cmake.in
    "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
    @ONLY)

install(
    FILES
    "${PROJECT_BINARY_DIR}/${CMAKE_FILES_DIRECTORY}/${PROJECT_NAME}Config.cmake"
    "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
    DESTINATION "share/${PROJECT_NAME}/cmake"
    COMPONENT dev)

install(
    EXPORT export_${PROJECT_NAME}
    DESTINATION "share/${PROJECT_NAME}/cmake"
    FILE export_${PROJECT_NAME}.cmake
    COMPONENT dev)

install(
    DIRECTORY include/
    DESTINATION include
    COMPONENT dev)

# generate a DEB when cpack is run
set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VERSION ${${PROJECT_NAME}_VERSION})
set(CPACK_PACKAGE_VENDOR "Thomas Digital, LLC")
set(CPACK_GENERATOR "DEB;TGZ")
set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")

set(CPACK_DEB_COMPONENT_INSTALL "ON")
set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Logan Thomas")

set(CPACK_TGZ_COMPONENT_INSTALL "ON")

cpack_add_component(binaries
  DISPLAY_NAME "Pimento"
  DESCRIPTION "Compiler for Olive lang *.oil files.")

cpack_add_component(dev
  DISPLAY_NAME "Olive lang headers"
  DESCRIPTION "Headers for olive lang compiler.")

include(CPack)
