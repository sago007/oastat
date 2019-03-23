cmake_minimum_required(VERSION 2.8.9)
project (oastat)
find_package(Boost COMPONENTS program_options REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -std=c++11 -DUSECPPDB=1")

file(GLOB SOURCES "src/*/*.cpp")

add_library(oastatlib ${SOURCES})

add_executable(oastat src/oastat.cpp)
TARGET_LINK_LIBRARIES( oastat oastatlib ${Boost_LIBRARIES} )
target_link_libraries( oastat cppdb gcrypt gpg-error)

add_executable(oastat_delete_games src/oastat_delete_games.cpp)
TARGET_LINK_LIBRARIES( oastat_delete_games oastatlib ${Boost_LIBRARIES} )
target_link_libraries( oastat_delete_games cppdb gcrypt gpg-error)