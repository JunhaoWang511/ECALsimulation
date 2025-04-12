cmake_minimum_required(VERSION 3.16...3.21)

project(ECAL)

option(WITH_GEANT4_UIVIS "Build with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()
set(CMAKE_CXX_STANDARD 17 CACHE STRING "")
find_package(ROOT REQUIRED)
include(${Geant4_USE_FILE})
include(${ROOT_USE_FILE})
include_directories(${PROJECT_SOURCE_DIR}/include
  ${Geant4_INCLUDE_DIR}
  # $ENV{ROOTSYS}/include
  ${ROOT_INCLUDE_DIRS}
)
link_libraries(
  ${ROOT_LIBRARIES}
)
# We presume the existence of three directories
file(GLOB_RECURSE sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB_RECURSE headers ${PROJECT_SOURCE_DIR}/include/*.hh)
file(GLOB macros RELATIVE ${PROJECT_SOURCE_DIR} *.mac)

# Enable macros for out-of-source build
foreach(_file ${macros})
  configure_file(
    ${_file}
    ${PROJECT_BINARY_DIR}/${_file}
    COPYONLY
  )
endforeach()

add_executable(ECAL main.cc ${sources} ${headers})
target_link_libraries(ECAL ${Geant4_LIBRARIES} ${ROOT_LIBRARIES})
install(TARGETS ECAL DESTINATION bin)
