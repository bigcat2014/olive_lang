cmake_minimum_required(VERSION 3.22)
project(pimento)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(${PROJECT_NAME}_MAJOR_VERSION 0)
set(${PROJECT_NAME}_MINOR_VERSION 1)
set(${PROJECT_NAME}_PATCH_VERSION 0)
set(${PROJECT_NAME}_VERSION
    ${${PROJECT_NAME}_MAJOR_VERSION}.${${PROJECT_NAME}_MINOR_VERSION}.${${PROJECT_NAME}_PATCH_VERSION})

include(FetchContent)

# Fetch argparse
FetchContent_Declare(
  argparse
  GIT_REPOSITORY https://github.com/p-ranav/argparse.git
  GIT_TAG        v3.2
)
FetchContent_MakeAvailable(argparse)

set(BUILD_DEPS
    spdlog
)

foreach(package ${BUILD_DEPS})
    find_package(${package} REQUIRED)
endforeach()

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME ON)
find_package(Boost 1.74.0 REQUIRED)

add_executable(${PROJECT_NAME}
    src/pimento.cpp
)
target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    ${Boost_INCLUDE_DIRS}
)
target_link_libraries(${PROJECT_NAME} PRIVATE argparse spdlog::spdlog ${Boost_LIBRARIES})
target_compile_definitions(${PROJECT_NAME} PRIVATE PROJECT_VERSION="${${PROJECT_NAME}_VERSION}")
set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "-Wall -Wextra -Werror -Wdouble-promotion -Wconversion")

install(
    DIRECTORY include/
    DESTINATION include
)

include(CTest)
enable_testing()
if (BUILD_TESTING)
    find_package(GTest REQUIRED)
endif()

install(
    TARGETS ${PROJECT_NAME}
    EXPORT export_${PROJECT_NAME}
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin)

set(export_targets ${export_targets};${PROJECT_NAME})
export(EXPORT export_${PROJECT_NAME}
    FILE "${PROJECT_BINARY_DIR}/export_${PROJECT_NAME}.cmake")

set(CONF_INCLUDE_DIRS "${CMAKE_INSTALL_PREFIX}/include")
configure_file(cmake/${PROJECT_NAME}Config.cmake.in
    "${PROJECT_BINARY_DIR}/${CMAKE_FILES_DIRECTORY}/${PROJECT_NAME}Config.cmake" @ONLY)

configure_file(cmake/${PROJECT_NAME}ConfigVersion.cmake.in
    "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake" @ONLY)

install(
    FILES
    "${PROJECT_BINARY_DIR}/${CMAKE_FILES_DIRECTORY}/${PROJECT_NAME}Config.cmake"
    "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
    DESTINATION "share/${PROJECT_NAME}/cmake" COMPONENT dev)

install(EXPORT export_${PROJECT_NAME}
    DESTINATION "share/${PROJECT_NAME}/cmake"
    FILE export_${PROEJCT_NAME}.cmake
    COMPONENT dev)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${${PROJECT_NAME}_VERSION})
include(CPack)