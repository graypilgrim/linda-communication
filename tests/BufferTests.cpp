#include <boost/test/unit_test.hpp>

#include <iostream>

#include "../src/Buffer.hpp"
#include "../src/utils.hpp"

namespace BufferTests
{

    BOOST_AUTO_TEST_CASE(testOutOfMemory) {
		Buffer serv("boostTest", false);
		serv.init();

		Buffer buf("boostTest");
		for (int i = 0; i < MAX_TUPLES_COUNT; ++i) {
			Tuple t1{{1, "ala", "ma", "kota", 3}};
			BOOST_CHECK(buf.output(t1) == Buffer::OutputResult::success);
		}
		for (int i = 0; i < 10; ++i) {
			std::cout << "dupa" << std::endl;
			Tuple t1{{1, "ala", "ma", "kota", 3}};
			BOOST_CHECK(buf.output(t1) == Buffer::OutputResult::out_of_memory);
		}

		serv.destroy();
    }

    BOOST_AUTO_TEST_CASE(testAddDelete) {
		Buffer serv("boostTest", false);
		serv.init();

		Buffer buf("boostTest");
		for (int i = 0; i < MAX_TUPLES_COUNT * 2; ++i) {
			Tuple t1{{1, 2, 3}};
			BOOST_CHECK(buf.output(t1) == Buffer::OutputResult::success);

			std::string query="(integer:100, integer:*, integer:*)";
			if(auto result = buf.input(query, 0.01)) {
				BOOST_CHECK(0);
			} else {
			}

			std::string query2="(integer:1, integer:*, integer:3)";
			if(auto result = buf.input(query2, 0.01)) {
			} else {
				BOOST_CHECK(0);
			}

			if(auto result = buf.input(query2, 0.01)) {
				BOOST_CHECK(0);
			} else {
			}
		}

		serv.destroy();
	}

	// TODO: multithreaded tests
}
