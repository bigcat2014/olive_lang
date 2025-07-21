cmake_minimum_required(VERSION 3.22)
project(pimento)

set(${PROJECT_NAME}_MAJOR_VERSION 0)
set(${PROJECT_NAME}_MINOR_VERSION 1)
set(${PROJECT_NAME}_PATCH_VERSION 0)
set(${PROJECT_NAME}_VERSION
    ${${PROJECT_NAME}_MAJOR_VERSION}.${${PROJECT_NAME}_MINOR_VERSION}.${${PROJECT_NAME}_PATCH_VERSION})  # cmake-lint: disable=C0301

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS
    "-Wall -Wextra -pedantic -Wdouble-promotion -Wconversion -Wfatal-errors")
if(NOT ${CMAKE_BUILD_TYPE} MATCHES Debug)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
endif()

set(${PROJECT_NAME}_CMAKE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Fetch argparse
include(FetchContent)
FetchContent_Declare(
  argparse
  GIT_REPOSITORY https://github.com/p-ranav/argparse.git
  GIT_TAG        v3.2)
FetchContent_MakeAvailable(argparse)

set(BUILD_DEPS
    spdlog)

foreach(package ${BUILD_DEPS})
    find_package(${package} REQUIRED)
endforeach()

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME ON)
find_package(Boost 1.74.0 REQUIRED)

# Utils library target
add_library(${PROJECT_NAME}_utils STATIC
    src/utils.cpp)
target_include_directories(${PROJECT_NAME}_utils PRIVATE
$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
$<INSTALL_INTERFACE:include>)
target_link_libraries(${PROJECT_NAME}_utils PRIVATE spdlog::spdlog_header_only)

# Lexer library target
add_library(${PROJECT_NAME}_lexer STATIC
    src/lexer.cpp)
target_include_directories(${PROJECT_NAME}_lexer PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    ${Boost_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME}_lexer PRIVATE
    ${PROJECT_NAME}_utils ${Boost_LIBRARIES})

# Parser library target
add_library(${PROJECT_NAME}_parser STATIC
    src/parser.cpp)
target_include_directories(${PROJECT_NAME}_parser PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)
target_link_libraries(${PROJECT_NAME}_parser PRIVATE ${PROJECT_NAME}_lexer)

# Generator library target
add_library(${PROJECT_NAME}_generator STATIC
    src/generator.cpp)
target_include_directories(${PROJECT_NAME}_generator PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)
target_link_libraries(${PROJECT_NAME}_generator PRIVATE ${PROJECT_NAME}_parser)

# Pimento executable target
add_executable(${PROJECT_NAME}
    src/pimento.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    ${Boost_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE
    argparse ${PROJECT_NAME}_generator)
target_compile_definitions(${PROJECT_NAME} PRIVATE
    PROJECT_VERSION="${${PROJECT_NAME}_VERSION}")

if (BUILD_TESTING)
    enable_testing()

    # GTest
    find_package(GTest REQUIRED)
    add_executable(lexer_test EXCLUDE_FROM_ALL
        tests/lexer_test.cpp)
    target_include_directories(lexer_test PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        ${Boost_INCLUDE_DIRS})
    target_link_libraries(lexer_test PRIVATE
        ${PROJECT_NAME}_lexer GTest::gtest_main)
    gtest_discover_tests(lexer_test)

    add_executable(parser_test EXCLUDE_FROM_ALL
        tests/parser_test.cpp)
    target_include_directories(parser_test PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        ${Boost_INCLUDE_DIRS})
    target_link_libraries(parser_test PRIVATE
        ${PROJECT_NAME}_parser GTest::gtest_main)
    gtest_discover_tests(parser_test)

    # Gcovr
    list(APPEND CMAKE_MODULE_PATH ${${PROJECT_NAME}_CMAKE_DIR}/modules)
    include(CodeCoverage)
    append_coverage_compiler_flags()
    set(COVERAGE_GCOVR_EXCLUDES ${FETCHCONTENT_BASE_DIR})
    # Use llvm-cov gcov for Clang
    if(CMAKE_C_COMPILER_ID MATCHES "Clang")
        find_program(LLVM_COV_PATH llvm-cov)
        if(NOT LLVM_COV_PATH)
            message(FATAL_ERROR "llvm-cov not found! Aborting...")
        endif() # NOT LLVM_COV_PATH
        list(APPEND GCOVR_ADDITIONAL_ARGS
            --gcov-executable "${LLVM_COV_PATH} gcov")
    endif()
    set(Coverage_DEPENDENCIES run_tests)
    setup_target_for_coverage_gcovr_html(NAME coverage)

    # Linters
    set(${PROJECT_NAME}_LINTER_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/Linters)
    add_test(NAME ${PROJECT_NAME}_cmake_linter
        COMMAND cmake-lint
        ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt
        --tab-size 4
        --disabled-codes C0103
        -o ${${PROJECT_NAME}_LINTER_OUTPUT_DIR}/${PROJECT_NAME}_cmake_linter.txt
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

    set(${PROJECT_NAME}_TEST_RESOURCES_DIR
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/resources)
    add_test(NAME ${PROJECT_NAME}_valgrind_file
        COMMAND valgrind
        --error-exitcode=1
        --tool=memcheck
        --leak-check=full
        --errors-for-leak-kinds=definite
        --show-leak-kinds=definite
        $<TARGET_FILE:${PROJECT_NAME}>
            ${${PROJECT_NAME}_TEST_RESOURCES_DIR}/valgrind_test.oil
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})

    add_test(NAME ${PROJECT_NAME}_valgrind_stdout
        COMMAND valgrind
        --error-exitcode=1
        --tool=memcheck
        --leak-check=full
        --errors-for-leak-kinds=definite
        --show-leak-kinds=definite
        $<TARGET_FILE:${PROJECT_NAME}>
            ${${PROJECT_NAME}_TEST_RESOURCES_DIR}/valgrind_test.oil
            --stdout
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})

    add_test(NAME ${PROJECT_NAME}_valgrind_file_trace
        COMMAND valgrind
        --error-exitcode=1
        --tool=memcheck
        --leak-check=full
        --errors-for-leak-kinds=definite
        --show-leak-kinds=definite
        $<TARGET_FILE:${PROJECT_NAME}>
            ${${PROJECT_NAME}_TEST_RESOURCES_DIR}/valgrind_test.oil
            --trace
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})

    add_test(NAME ${PROJECT_NAME}_valgrind_stdout_trace
        COMMAND valgrind
        --error-exitcode=1
        --tool=memcheck
        --leak-check=full
        --errors-for-leak-kinds=definite
        --show-leak-kinds=definite
        $<TARGET_FILE:${PROJECT_NAME}>
            ${${PROJECT_NAME}_TEST_RESOURCES_DIR}/valgrind_test.oil
            --stdout --trace
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})

    add_test(NAME ${PROJECT_NAME}_valgrind_file_debug
        COMMAND valgrind
        --error-exitcode=1
        --tool=memcheck
        --leak-check=full
        --errors-for-leak-kinds=definite
        --show-leak-kinds=definite
        $<TARGET_FILE:${PROJECT_NAME}>
            ${${PROJECT_NAME}_TEST_RESOURCES_DIR}/valgrind_test.oil
            --debug
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})

    add_test(NAME ${PROJECT_NAME}_valgrind_stdout_debug
        COMMAND valgrind
        --error-exitcode=1
        --tool=memcheck
        --leak-check=full
        --errors-for-leak-kinds=definite
        --show-leak-kinds=definite
        $<TARGET_FILE:${PROJECT_NAME}>
            ${${PROJECT_NAME}_TEST_RESOURCES_DIR}/valgrind_test.oil
            --stdout --debug
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})

    add_test(NAME ${PROJECT_NAME}_valgrind_file_verbose
        COMMAND valgrind
        --error-exitcode=1
        --tool=memcheck
        --leak-check=full
        --errors-for-leak-kinds=definite
        --show-leak-kinds=definite
        $<TARGET_FILE:${PROJECT_NAME}>
            ${${PROJECT_NAME}_TEST_RESOURCES_DIR}/valgrind_test.oil
            --verbose
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})

    add_test(NAME ${PROJECT_NAME}_valgrind_stdout_verbose
        COMMAND valgrind
        --error-exitcode=1
        --tool=memcheck
        --leak-check=full
        --errors-for-leak-kinds=definite
        --show-leak-kinds=definite
        $<TARGET_FILE:${PROJECT_NAME}>
            ${${PROJECT_NAME}_TEST_RESOURCES_DIR}/valgrind_test.oil
            --stdout --verbose
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})

    # Custom Testing Targets
    add_custom_target(build_tests
        DEPENDS lexer_test parser_test
        COMMENT "Build unit tests")

    file(MAKE_DIRECTORY ${${PROJECT_NAME}_LINTER_OUTPUT_DIR} LINTER_DIR_CREATED)
    if(LINTER_DIR_CREATED)
        message(FATAL_ERROR ${LINTER_DIR_CREATED})
    endif() # LINTER_DIR_CREATED
    add_custom_target(run_linters
        COMMAND ${CTEST_PATH}
        --output-on-failure
        -R "${PROJECT_NAME}_.*_linter"
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Run unit tests")

    find_program(CTEST_PATH ctest)
    if(NOT CTEST_PATH)
        message(FATAL_ERROR "ctest not found! Aborting...")
    endif() # NOT CTEST_PATH
    add_custom_target(run_tests
        COMMAND ${CTEST_PATH}
        --output-on-failure
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        DEPENDS build_tests ${PROJECT_NAME}
        COMMENT "Run unit tests")
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
SET(CPACK_DEBIAN_PACKAGE_DEPENDS "libspdlog-dev, libboost-dev")

set(CPACK_TGZ_COMPONENT_INSTALL "ON")

cpack_add_component(binaries
DISPLAY_NAME "Pimento"
DESCRIPTION "Compiler for Olive lang *.oil files."
)
cpack_add_component(dev
DISPLAY_NAME "Olive lang headers"
DESCRIPTION "Headers for olive lang compiler."
)

include(CPack)
