cmake_minimum_required(VERSION 2.8.9)
project (oastat)
find_package(Boost COMPONENTS program_options REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -std=c++11")

file(GLOB SOURCES "src/*.cpp" "src/*/*.cpp")

add_executable(oastat ${SOURCES})
TARGET_LINK_LIBRARIES( oastat ${Boost_LIBRARIES} )
target_link_libraries( oastat cppdb gcrypt gpg-error)
