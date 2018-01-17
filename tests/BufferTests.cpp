#include <boost/test/unit_test.hpp>

#include <iostream>
#include <thread>
#include <mutex>

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

    BOOST_AUTO_TEST_CASE(testAddDeleteMultithreaded) {
		Buffer serv("boostTest", false);
		serv.init();

		std::mutex m;
		std::thread t1([&m] {
			Buffer buf("boostTest");
			for (int i = 0; i < MAX_TUPLES_COUNT ; ++i) {
				Tuple t1{{1, 2, 3}};
				bool b = buf.output(t1) == Buffer::OutputResult::success;
				m.lock();
				BOOST_CHECK(b);
				m.unlock();
			}
		});


		std::thread t2([&m] {
			Buffer buf("boostTest");
			std::string query="(integer:*, integer:*, integer:*)";
			for (int i = 0; i < MAX_TUPLES_COUNT ; ++i) {
				if(auto result = buf.input(query, 4)) {
				} else {
					// buf.print();
					m.lock();
					BOOST_CHECK(0);
					m.unlock();
				}
			}
		});

		t1.join();
		t2.join();

		serv.destroy();
	}

    BOOST_AUTO_TEST_CASE(testAddDeleteMultithreaded2) {
		Buffer serv("boostTest", false);
		serv.init();

		// boost is not thread safe
		std::mutex m;

		auto fun = [&] {
			Buffer buf("boostTest");
			std::string query="(integer:*, integer:*, integer:*)";
			for (int i = 0; i < MAX_TUPLES_COUNT-2 ; ++i) {
				Tuple t1{{1, 2, 3}};

				bool b = buf.output(t1) == Buffer::OutputResult::success;
				m.lock();
				BOOST_CHECK(b);
				m.unlock();

				if(auto result = buf.input(query, 1)) {
				} else {
					serv.print();
					m.lock();
					BOOST_CHECK(0);
					m.unlock();
				}
			}
		};

		std::thread t1(fun);
		std::thread t2(fun);
		std::thread t3(fun);
		std::thread t4(fun);

		t1.join();
		t2.join();
		t3.join();
		t4.join();

		serv.destroy();
	}

}
