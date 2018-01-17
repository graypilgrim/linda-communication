//
// Created by marcin on 15.01.18.
//

#include "CommandLine.hpp"

#include <climits>

CommandLine::CommandLine(int argc, char** argv)
	: argc_(argc), argv_(argv), desc_("Allowed options") {

	desc_.add_options()
		("help,h", "print help message")
		("server,s", "server mode")
		("input,i", po::value<string>(), "print and take tuple matching to pattern from memory")
		("output,o", po::value<string>(), "place tuple in memory")
		("read,r", po::value<string>(), "print tuple matching to pattern")
		("timeout,t", po::value<int>()->default_value(INT_MAX), "value of time client waits for end of --input and --read operations")
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
			if(vm_.count("input") || vm_.count("output") || vm_.count("read")) {
				cout << "Action not available in server mode!" << endl;
				cout << desc_ << endl;
				exit(1);
			}
		}

		int action_ctr = 0;

		if(vm_.count("input"))
			++action_ctr;
		if(vm_.count("output"))
			++action_ctr;
		if(vm_.count("read"))
			++action_ctr;

		if(action_ctr > 1) {
			cerr << "You can choose only one action! Input or Output or Read" << endl;

			cout << desc_ << endl;
			exit(1);
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