cmake_minimum_required(VERSION 3.9)
project(clidict VERSION 0.0.1 DESCRIPTION "commands storage container for CLI")

option(BuildTests          "Build the unit tests when BUILD_TESTING is enabled." ${MAIN_PROJECT})

set(DEFAULT_BUILD_TYPE "Release")
set (CMAKE_CXX_COMPILER_VERSION 11)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_COMPILER ${CXX_COMPILER})
set(CMAKE_C_COMPILER ${C_COMPILER})

include(GNUInstallDirs)


add_library(clidict INTERFACE)
target_compile_features(clidict INTERFACE cxx_std_20)
#target_sources(clidict INTERFACE ${CMAKE_CURRENT_LIST_DIR}/src/clidict.hpp)

install(TARGETS clidict
        EXPORT ${PROJECT_NAME}_Targets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})


target_include_directories(clidict INTERFACE 
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file("clidictConfigVersion.cmake"
                                 VERSION ${PROJECT_VERSION}
                                 COMPATIBILITY SameMajorVersion)
configure_package_config_file(
  "${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in"
  "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
  INSTALL_DESTINATION
  ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake)



install(EXPORT ${PROJECT_NAME}_Targets
        FILE ${PROJECT_NAME}Targets.cmake
        NAMESPACE ${PROJECT_NAME}::
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake)

install(FILES "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
              "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/src/ DESTINATION include)


if (BuildTests )
  include(CTest)
  enable_testing()
  find_package(GTest CONFIG REQUIRED)
  add_subdirectory(tests)
  target_include_directories(clio_test PRIVATE tests/include)
  add_test(NAME Test COMMAND clidict_test)
  set(CPACK_PROJECT_NAME ${PROJECT_NAME})
  set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
  include(CPack)
endif ()
