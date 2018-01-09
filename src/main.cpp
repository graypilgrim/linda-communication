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
	std::string shmName="";

	// TODO: consider using boost program options
	for(int i = 1; i<argc; ++i) {
		if (std::string(argv[i]) == "-s") {
			server = true;
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
		std::cin.get();
		std::cout << "Exiting." << std::endl;
		buffer->destroy();
	} else {
		std::cout << "Running in client mode." << std::endl;
		// TODO: implement - interactive console for testing
	}
	return 0;
}
