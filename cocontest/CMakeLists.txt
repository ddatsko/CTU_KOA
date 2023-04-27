
project(cocontest_heuristics CXX)

set(CMAKE_MINIMUM_REQUIRED_VERSION 3.15)

# Using 11 as -std=c++11 flag is used in BRUTE
set(CMAKE_CXX_STANDARD 11)

#set(CMAKE_CXX_FLAGS "${CMAKECXX_FLAGS} -DDEBUG")

add_executable(${PROJECT_NAME} main.cpp Problem.cpp Problem.h common_types.h tabu_search.cpp tabu_search.h heuristics.cpp heuristics.h)