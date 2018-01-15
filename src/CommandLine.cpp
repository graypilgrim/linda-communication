//
// Created by marcin on 15.01.18.
//

#include "CommandLine.hpp"

CommandLine::CommandLine(int argc, char** argv)
  : argc_(argc), argv_(argv) {
  po::options_description desc("Allowed options");
  desc.add_options()
      ("help,h", "print help message")
      //("interactive,i", "interactive mode")
      ("server,s", "server mode")
      ("add,a", "client: add tuple")
      //("delete,d", "client: delete tuple")
      //("list,l", "client: list tuples")
      ("wait,w", "client: wait")
      ("timeout,t", po::value<int>()->default_value(4), "client: wait timeout")
      ("name,n", "share name");

  pd_.add("name", 1);
}

void CommandLine::Parse() {
  if (vm_.count("help")) {
    cout << desc << endl;
    exit(0);
  }

  if (!vm_.count("name")) {
    cout << "Not given name of the share!" << endl;
    cout << desc << endl;
    exit(1);
  }

  if (vm_.count("server")) {
    if(vm_.count("add") || vm_.count("delete") || vm_.count("list")
       || vm_.count("wait") || vm_.count("timeout")) {
      cout << "Action not available in server mode!" << endl;
      cout << desc << endl;
      exit(1);
    }
  }

  if (vm_.count("timeout") && !vm_.count("wait")) {
    cout << "Cannot set timeout while not wait action!" << endl;
    cout << desc << endl;
    exit(1);
  }
}

const po::variables_map& CommandLine::GetVariables() const {
  return vm_;
}