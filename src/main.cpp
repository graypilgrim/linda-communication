#include <iostream>
#include <csignal>

#include "QueryLexer.hpp"
#include "QueryParser.hpp"
#include "Buffer.hpp"

Buffer* buffer = nullptr;

void signalHandler( int signum ) {
	std:: cout << "Interrupt signal (" << signum << ") received." << std::endl;
	buffer->destroy();
	std::cout << "Exiting." << std::endl;
	exit(signum);
}

int main(int argc, char* argv[]) {
	bool server = false;
	bool addElements = false;
	bool wait = false;
	std::string shmName="";

	// TODO: consider using boost program options
	for(int i = 1; i<argc; ++i) {
		if (std::string(argv[i]) == "-s") {
			server = true;
        } else if (std::string(argv[i]) == "-a") {
			addElements = true;
        } else if (std::string(argv[i]) == "-w") {
			wait = true;
		} else {
			if (shmName != "") {
				std::cout << "Invalid arguments" << std::endl;
				return 1;
			}
			shmName = argv[i];
		}
	}
	if (shmName == "") {
		std::cout << "Shared memory name required." << std::endl;
		return 1;
	}
	std::cout << "Using shared memory: " << shmName << std::endl;

	if (server) {
		std::cout << "Running in server mode." << std::endl;
		buffer = new Buffer(shmName, false);
		signal(SIGINT, signalHandler);
		buffer->init();
        buffer->print();
		std::cin.get();
        buffer->print();
		std::cout << "Exiting." << std::endl;
		buffer->destroy();
	} else {
		std::cout << "Running in client mode." << std::endl;
        Buffer buffer(shmName);

		// TODO: implement interactive console for testing
        // following is a simple test
        buffer.print();

        if (addElements) {
            Tuple t1{{1, "ala", "ma", "kota", 3}};
            buffer.output(t1);
            buffer.print();
            Tuple t2{{1, 2, 3}};
            buffer.output(t2);
            buffer.print();
        }

        if (wait) {
            std::string query="(integer:1, integer:*, integer:3)";
            if (auto result = buffer.read(query, 4)) {
                std::cout << "Got:" << std::endl;
                result.value().print();
            } else {
                std::cout << "Cannot get previously inserted tuple." << std::endl;
            }
        }

	}
	return 0;
}
