cmake_minimum_required (VERSION 3.7)
project (linda-communication)

# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++17")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -g")

# Executables
add_executable (linda-communication "${PROJECT_SOURCE_DIR}/src/main.cpp")

target_link_libraries (linda-communication Buffer)
target_link_libraries (linda-communication Elem)
target_link_libraries (linda-communication QueryLexer)
target_link_libraries (linda-communication QueryParser)
target_link_libraries (linda-communication StringOrNumber)
target_link_libraries (linda-communication sync)
target_link_libraries (linda-communication Tuple)
target_link_libraries (linda-communication TupleParser)
target_link_libraries (linda-communication CommandLine)

target_link_libraries (linda-communication boost_program_options)
target_link_libraries (linda-communication rt)



add_executable (linda-tests
	"${PROJECT_SOURCE_DIR}/tests/main.cpp"
	"${PROJECT_SOURCE_DIR}/tests/BufferTests.cpp"
	"${PROJECT_SOURCE_DIR}/tests/QueryParserTests.cpp")
target_link_libraries (linda-tests Buffer)
target_link_libraries (linda-tests Elem)
target_link_libraries (linda-tests QueryLexer)
target_link_libraries (linda-tests QueryParser)
target_link_libraries (linda-tests StringOrNumber)
target_link_libraries (linda-tests sync)
target_link_libraries (linda-tests Tuple)
target_link_libraries (linda-tests CommandLine)
target_link_libraries (linda-tests rt)
target_link_libraries (linda-tests boost_unit_test_framework)

add_subdirectory(src)
