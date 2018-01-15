//
// Created by marcin on 15.01.18.
//

#include "CommandLine.hpp"

CommandLine::CommandLine(int argc, char** argv)
	: argc_(argc), argv_(argv), desc_("Allowed options") {

	desc_.add_options()
		("help,h", "print help message")
		//("interactive,i", "interactive mode")
		("server,s", "server mode")
		("add,a", "client: add tuple")
		//("delete,d", "client: delete tuple")
		//("list,l", "client: list tuples")
		("wait,w", "client: wait")
		("timeout,t", po::value<int>()->default_value(4), "client: wait timeout in seconds")
		("name,n", po::value<string>(), "share name");

	pd_.add("name", 1);
}

void CommandLine::Parse() {
	try {
		po::store(po::command_line_parser(argc_, argv_).
				options(desc_).positional(pd_).run(), vm_);
		po::notify(vm_);

		if (vm_.count("help")) {
			cout << desc_ << endl;
			exit(0);
		}

		if (!vm_.count("name")) {
			cout << "Not given name of the share!" << endl;
			cout << desc_ << endl;
			exit(1);
		}

		if (vm_.count("server")) {
			if(vm_.count("add") || vm_.count("delete") || vm_.count("list")
				 || vm_.count("wait")) {
				cout << "Action not available in server mode!" << endl;
				cout << desc_ << endl;
				exit(1);
			}
		}
	}
	catch(po::error& e) {
		cerr << "ERROR: " << e.what() << endl;
		cout << desc_ << endl;
		exit(1);
	}
}

const po::variables_map& CommandLine::GetVariables() const {
	return vm_;
}