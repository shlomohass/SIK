//
//  main.cpp
//  SIK
//
//  Created by Shlomo Hassid on 06/11/2016.
//  Copyright © 2016 Shlomo Hassid. All rights reserved.
//

#include <iostream>

#include "argvparser.hpp"
#include "SETSIK.hpp"
#include "SIKAdd.hpp"
#include "SIKLang.hpp"
#include "SIKScript.hpp"


#ifndef SIK_NAN
#define SIK_NAN -3231307.6790
#endif

namespace cm = CommandLineProcessing;

int main(int argc, char** argv) {


	//Define args and project settings:

	int exitCode = 0;
	std::string filepath;
	cm::ArgvParser cmd;
	bool enable_debug = SIK_DEBUG;
	int  debug_level = 1; // 1-> Errors, Warnings : 2-> Opcodes, 3-> Executions, 4-> Tokens, AST, Macros.
	bool execute_is_requested = false;

	cmd.addErrorCode(0, "Success");
	cmd.addErrorCode(1, "Error");
	cmd.setIntroductoryDescription("This is foo written by bar.");
	cmd.setHelpOption("h", "help", "Print this help page");
	cmd.defineOption("r", "Compile and run a file -r file.sik", cm::ArgvParser::OptionRequiresValue);
	cmd.defineOption("debug", "enable debug", cm::ArgvParser::NoOptionAttribute);
	cmd.defineOption("level", "set debuging level", cm::ArgvParser::OptionRequiresValue);

	int result = cmd.parse(argc, argv);

	//Parse and set Argu:
	if (result != cm::ArgvParser::NoParserError)
	{
		std::cout << cmd.parseErrorDescription(result);
		if (result == cm::ArgvParser::ParserHelpRequested) {
			exitCode = 0;
		}
		exitCode = 1;
	} else {
		if (cmd.foundOption("r")) {
			filepath = cmd.optionValue("r");
			execute_is_requested = true;
		}
		if (cmd.foundOption("debug")) {
			enable_debug = true;
		}
		if (cmd.foundOption("level")) {
			try {
				debug_level = std::stoi(cmd.optionValue("level"));
			} catch (...) {
				debug_level = 1;
			}
		}
	}

	if (result == cm::ArgvParser::ParserHelpRequested) {
		system("pause");
		exit(exitCode);
	}

	if (execute_is_requested) {

		//Create the SIKLang Definition:
		sik::SIKLang* lang = new sik::SIKLang();

		//Create a Script Container:
		sik::SIKScript script(enable_debug, debug_level);
		//Load target script:
		bool indicator = script.compile(filepath);

		//Loading success so go and do stuff:
		//if (indicator) {
			//s.run(enable_debug);
		//}

		delete lang;
	}

	if (enable_debug) {
		system("pause");
	}
	return 0;
}
