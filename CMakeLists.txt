cmake_minimum_required(VERSION 3.9)
project(mm)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

# -O3 is needed, otherwise the results aren't realistic when debugging
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O3")
set(CMAKE_CXX_FLAGS_PROFILING "-pg ${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")

message(STATUS "Build configuration: " ${CMAKE_BUILD_TYPE}) 
message(STATUS "CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG}")
message(STATUS "CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE}")
message(STATUS "CMAKE_CXX_FLAGS_RELWITHDEBINFO ${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")
message(STATUS "CMAKE_CXX_FLAGS_PROFILING ${CMAKE_CXX_FLAGS_PROFILING}")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -fconcepts -DIL_STD") 

# find_package(cplex REQUIRED)
# find_package(fmt REQUIRED)
# find_package(MKL REQUIRED)
# set(Boost_NO_SYSTEM_PATHS ON)
# find_package(Boost 1.60.0 COMPONENTS program_options system REQUIRED)

# include_directories(SYSTEM ${CPLEX_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${MKL_INCLUDE_DIR})
# link_directories(${Boost_LIBRARY_DIRS} $ENV{HOME}/libf77)
# link_directories(${Boost_LIBRARY_DIRS})

add_executable(mm main.cpp astar.cpp pdb.cpp)
# target_link_libraries(chaincolor ${Boost_LIBRARIES} rt dl pthread fmt::fmt)
target_link_libraries(mm rt dl)