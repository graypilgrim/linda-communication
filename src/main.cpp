#include <iostream>
#include <csignal>

#include "QueryLexer.hpp"
#include "QueryParser.hpp"
#include "Buffer.hpp"
#include "CommandLine.hpp"

Buffer* buffer = nullptr;

void signalHandler( int signum ) {
	std:: cout << "Interrupt signal (" << signum << ") received." << std::endl;
	buffer->destroy();
	std::cout << "Exiting." << std::endl;
	exit(signum);
}

int main(int argc, char* argv[]) {
	bool server;
	bool addElements;
	bool wait;
	std::string shmName;

	CommandLine command_line(argc, argv);
	command_line.Parse();

	server = command_line.GetVariables().count("server") != 0;
	addElements = command_line.GetVariables().count("add") != 0;
	wait = command_line.GetVariables().count("wait") != 0;
	shmName = command_line.GetVariables()["name"].as<string>();

	std::cout << "Using shared memory: " << shmName << std::endl;

	if (server) {
		std::cout << "Running in server mode." << std::endl;
		buffer = new Buffer(shmName, false);
		signal(SIGINT, signalHandler);
		buffer->init();
        while (true) {
            std::cout << "DUMP============================================" << std::endl;
            buffer->print();
            buffer->printList();
            std::cin.get();
        }
		// buffer->print();
		// std::cout << "Exiting." << std::endl;
		// buffer->destroy();
	} else {
		std::cout << "Running in client mode." << std::endl;
		Buffer buffer(shmName);

		// TODO: implement interactive console for testing
		// following is a simple test
		buffer.print();

		if (addElements) {
			Tuple t1{{1, "ala", "ma", "kota", 3}};
			if (buffer.output(t1) == Buffer::OutputResult::out_of_memory)
                std::cout << "out of memory" << std::endl;
			buffer.print();

			Tuple t2{{1, 2, 3}};
			if (buffer.output(t2) == Buffer::OutputResult::out_of_memory)
                std::cout << "out of memory" << std::endl;
			buffer.print();
		}

		if (wait) {
			std::string query="(integer:1, integer:*, integer:3)";
			if (auto result = buffer.input(query, 4)) {
				std::cout << "Got:" << std::endl;
				result.value().print();
			} else {
				std::cout << "Cannot get previously inserted tuple." << std::endl;
			}
		}
	}
	return 0;
}
