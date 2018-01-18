#include <iostream>
#include <csignal>

#include "QueryLexer.hpp"
#include "QueryParser.hpp"
#include "Buffer.hpp"
#include "CommandLine.hpp"
#include "TupleParser.hpp"

Buffer* server_buffer = nullptr;

void signalHandler(int signum) {
	std:: cout << "Interrupt signal (" << signum << ") received." << std::endl;

	if(server_buffer) {
			std::cout << "Destroying buffer." << std::endl;
			server_buffer->destroy();
	}

	std::cout << "Exiting." << std::endl;
	exit(signum);
}

void server(const po::variables_map& vm) {
	signal(SIGINT, signalHandler);

	std::string shm_name = vm["name"].as<string>();
	std::cout << "Running in server mode." << std::endl
						<< "Shared memory name: " << shm_name << std::endl;

	server_buffer = new Buffer(shm_name, false);
	server_buffer->init();

	while (true) {
		// std::cout << "DUMP============================================" << std::endl;
		// server_buffer->print();
		// server_buffer->printList();
		std::cin.get();
	}
	// buffer->print();
	// std::cout << "Exiting." << std::endl;
	// buffer->destroy();
}

void client(const po::variables_map& vm) {
	std::string shm_name = vm["name"].as<string>();
	std::cout << "Running in client mode." << std::endl
						<< "Shared memory name: " << shm_name << std::endl;

	Buffer buffer(shm_name);

	if (vm.count("output")) {
		auto pattern = vm["output"].as<string>();
		TupleParser tuple_parser(pattern);
		Tuple tuple = tuple_parser.parse();

		if (!tuple.empty()) {
			if (buffer.output(tuple) == Buffer::OutputResult::success) {
                std:: cout << "success"  << std::endl;
            } else {
                std:: cout << "out of memory"  << std::endl;
            }
        } else {
            std:: cout << "bad tuple"  << std::endl;
			exit(1);
        }
	}
	else if (vm.count("input")) {
		auto pattern = vm["input"].as<string>();
		auto timeout = vm["timeout"].as<int>();

		if (auto t = buffer.input(pattern, timeout)) {
            std:: cout << "ok"  << std::endl;
            t.value().print();
        } else {
            std:: cout << "timeout"  << std::endl;
        }
	}
	else if (vm.count("read")) {
		auto pattern = vm["read"].as<string>();
		auto timeout = vm["timeout"].as<int>();
		if (auto t = buffer.read(pattern, timeout)) {
            std:: cout << "ok"  << std::endl;
            t.value().print();
        } else {
            std:: cout << "timeout"  << std::endl;
        }
	}
}

int main(int argc, char** argv) {
	CommandLine command_line(argc, argv);
	command_line.Parse();

	auto variables = command_line.GetVariables();
	bool is_server = variables.count("server") != 0;

	if (is_server)
		server(variables);
	else
		client(variables);

	return 0;
}
