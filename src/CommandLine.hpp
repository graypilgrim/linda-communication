//
// Created by marcin on 15.01.18.
//

#ifndef LINDA_COMMUNICATION_COMMANDLINE_HPP
#define LINDA_COMMUNICATION_COMMANDLINE_HPP

#include <string>
#include <iostream>

#include <boost/program_options.hpp>

namespace po = boost::program_options;
using std::cout;
using std::cerr;
using std::endl;
using std::string;

class CommandLine {
public:
	CommandLine(int argc, char** argv);
	void Parse();
	const po::variables_map& GetVariables() const;

private:
	int argc_;
	char** argv_;

	po::options_description desc_;
	po::variables_map vm_;
	po::positional_options_description pd_;
};


#endif //LINDA_COMMUNICATION_COMMANDLINE_HPP
