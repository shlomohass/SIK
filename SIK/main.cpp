//
//  main.cpp
//  SIK
//
//  Created by Shlomo Hassid on 06/11/2016.
//  Copyright © 2016 Shlomo Hassid. All rights reserved.
//  
//  Command Line Examples:
//  -r "More\siktest.sik" --debug --level 4
//  -r "More\siktest.sik"
//  --tin "More\tests" --texec  "Release\SIK.exe" --debug
//  -h
//  In win -> x64\Release\SIK.exe --tin "More\tests" --texec  "x64\Release\SIK.exe" --debug
//

#define SIK_VERSION "0.3"
#define SIK_AUTHOR "Shlomi Hassid"
#define _CRTDBG_MAP_ALLOC

#include <stdlib.h>
#ifdef _WIN32
    #include <crtdbg.h>
#endif
#include <iostream>
#include <fstream>
#include <vector>
#include <cstdio>
#include <memory>
#include <time.h>

#include "tinydir.h"
#include "argvparser.hpp"
#include "SETSIK.hpp"
#include "SIKAdd.hpp"
#include "SIKLang.hpp"
#include "SIKScript.hpp"

#ifndef _WIN32
inline int _pipe(int fildes[2], unsigned psize, int textmode) {
    return pipe(fildes);
}
inline FILE* _popen(const char* command, const char* type) {
    return popen(command, type);
}
inline void _pclose(FILE* file) {
    pclose(file);
}
#endif

namespace cm = CommandLineProcessing;

void setMainArgs(cm::ArgvParser *cmd, std::string *filepath, std::string *execpath, bool *execute_is_requested, bool *enable_debug, int *debug_level, bool *run_tests ,std::string *testFoldeInpath, std::string *testFoldeOutpath) {
	if (cmd->foundOption("r")) {
		*filepath = cmd->optionValue("r");
		*execute_is_requested = true;
	}
	if (cmd->foundOption("debug")) {
		*enable_debug = true;
	}
	if (cmd->foundOption("level")) {
		try {
			*debug_level = std::stoi(cmd->optionValue("level"));
		}
		catch (...) {
			*debug_level = 1;
		}
	}
	if (!*execute_is_requested && cmd->foundOption("tin")) {
		*run_tests = true;
		*debug_level = 1;
		*testFoldeInpath = cmd->optionValue("tin");
		if (cmd->foundOption("tout")) {
			*testFoldeOutpath = cmd->optionValue("tout");
		}
		if (cmd->foundOption("texec")) {
			*execpath = cmd->optionValue("texec");
		}
	}
	return;
}
std::string exec(const char* cmd) {
	std::shared_ptr<FILE> pipe(_popen(cmd, "r"), _pclose);
	if (!pipe) return "ERROR";
	char buffer[128];
	std::string result = "";
	while (!feof(pipe.get())) {
		if (fgets(buffer, 128, pipe.get()) != NULL)
			result += buffer;
	}
	return result;
}
std::string unescape(const std::string& str) {
	std::string res;
	for (int i = 0; i<(int)str.length(); ++i) {
		switch (str[i]) {
		case '\r':
			res += "\\r";
			break;
		case '\n':
			res += "\\n";
			break;
		case '\\':
			res += "\\\\";
			break;
		case '\t':
			res += "\\t";
			break;
			//add other special characters if necessary...
		default:
			res += str[i];
		}
	}
	return res;
}
int parseTests(const std::string& testInPath, const std::string& testOutPath, const std::string& testExt, std::map<std::string, tinydir_file> *test_files, std::map <std::string, std::string>* expected_results_list, std::map <std::string, std::string>* computed_results_list) {
	int testcount = 0;
	tinydir_dir dir;
	tinydir_open(&dir, testInPath.c_str());

	while (dir.has_next)
	{
		tinydir_file file;
		tinydir_readfile(&dir, &file);
		if (!file.is_dir && file.extension == testExt)
		{
			++testcount;
			test_files->insert(std::pair<std::string, tinydir_file>(file.name, file));
		}
		tinydir_next(&dir);
	}
	tinydir_close(&dir);

	if (testcount > 0) {
		testcount = 0;
		typedef std::map<std::string, tinydir_file>::iterator it_type;
		for (it_type iterator = test_files->begin(); iterator != test_files->end(); iterator++) {
			std::ifstream wifs;
			std::string txtline;
			bool expectedFlag = false;
			std::string expectedBuffer = "";

			wifs.open(iterator->second.path);
			if (!wifs.is_open())
			{
				std::cerr << "Unable to open file: " << iterator->first << std::endl;
				expected_results_list->insert(std::pair<std::string, std::string>(iterator->first, "@SKIP@"));
				computed_results_list->insert(std::pair<std::string, std::string>(iterator->first, "@SKIP@"));
				continue;
			}

			while (getline(wifs, txtline)) {
				if (txtline == "@expected-output") {
					expectedFlag = true;
					continue;
				}
				if (txtline == "@end-expected-output") {
					break;
				}
				if (expectedFlag) {
					if (expectedBuffer == "") {
						expectedBuffer += txtline;
					}
					else {
						expectedBuffer += "\n" + txtline;
					}
				}
			}
			//Store results
			if (expectedFlag) {
				expected_results_list->insert(std::pair<std::string, std::string>(iterator->first, expectedBuffer));
				computed_results_list->insert(std::pair<std::string, std::string>(iterator->first, ""));
				testcount++;
			}
			else {
				expected_results_list->insert(std::pair<std::string, std::string>(iterator->first, "@SKIP@"));
				computed_results_list->insert(std::pair<std::string, std::string>(iterator->first, "@SKIP@"));
			}
		}
		if (testcount <= 0) {
			return 0;
		}
	} else {
		return 0;
	}
	return 1;
}
int executeTests(const std::string& execpath,std::map<std::string, tinydir_file> *test_files,std::map <std::string, std::string>* expected_results_list,std::map <std::string, std::string>* computed_results_list,std::vector<std::string>* failedTests,int *actualTestsPerform) {

	*actualTestsPerform = 0;

	//Execute tests and save results:
	typedef std::map<std::string, std::string>::iterator it_type;
	for (it_type iterator = expected_results_list->begin(); iterator != expected_results_list->end(); iterator++) {
		std::string unescaped = unescape(iterator->second);
		if (unescaped.length() > 48) {
			std::cout << " File: " << iterator->first << " - Expected: " << unescaped.substr(0, 45) << "...  -  Length[ " << iterator->second.length() << " ]" << std::endl;
		} else {
			std::cout << " File: " << iterator->first << " - Expected: " << unescaped << "  -  Length[ " << iterator->second.length() << " ]" << std::endl;
		}
		if (iterator->second != "@SKIP@") {
			clock_t tStart = clock();
			std::string resultbuf = exec((execpath + " -r \"" + std::string(test_files->at(iterator->first).path) + "\"").c_str());
			//std::string resultbuf = exec(("Debug\\SIK -r \"" + std::string(iterator->first.begin(), iterator->first.end()) + "\"").c_str());
			std::string un_escaped_resultbuf = unescape(resultbuf);
			if (un_escaped_resultbuf.length() > 63) {
				std::cout << "       Result: " << ((resultbuf == iterator->second) ? "Test PASSED!" : ("Test FAILLED!  actual result ----> " + un_escaped_resultbuf.substr(0, 60) + "...")) << std::endl;
			}
			else {
				std::cout << "       Result: " << ((resultbuf == iterator->second) ? "Test PASSED!" : ("Test FAILLED!  actual result ----> " + un_escaped_resultbuf)) << std::endl;
			}
			computed_results_list->at(iterator->first) = resultbuf;
			if (resultbuf != iterator->second) {
				failedTests->push_back(iterator->first);
			}
			clock_t tEnd = clock();
			printf("       Execution: %.2fs, %dms \n\n", (double)(tEnd - tStart) / CLOCKS_PER_SEC, (tEnd - tStart) / (CLOCKS_PER_SEC / 1000));

			(*actualTestsPerform)++;
		} else {
			std::cout << std::endl;
		}
	}
	return 0;
}
int main(int argc, char** argv) {


	//Define args and project settings:

	int exitCode = 0;
	std::string filepath = "";
	cm::ArgvParser cmd;
	bool enable_debug = SIK_DEBUG;
	int  debug_level = 1; // 1-> Errors, Warnings : 2-> Opcodes, 3-> Executions, 4-> Tokens, AST, Macros.
	bool execute_is_requested = false;
	bool run_tests = false;
	std::string testInPath = "";
	std::string testOutPath = "";
	std::string testExt = "tsik";
	std::string execpath = "Debug\\SIK";
	int  execution_result = 0;

	cmd.addErrorCode(0, "Success");
	cmd.addErrorCode(1, "Error");
	cmd.setIntroductoryDescription("sik compiler version: " + std::string(SIK_VERSION) + " - By: " + std::string(SIK_AUTHOR));
	cmd.setHelpOption("h", "help", "Print this help page");
	cmd.defineOption("r", "Compile and run a file -r file.sik.", cm::ArgvParser::OptionRequiresValue);
	cmd.defineOption("debug", "enable debug.", cm::ArgvParser::NoOptionAttribute);
	cmd.defineOption("level", "set debuging level.", cm::ArgvParser::OptionRequiresValue);
	cmd.defineOption("tin", "run all tests in a folder.", cm::ArgvParser::OptionRequiresValue);
	cmd.defineOption("tout", "all the test results will be saved here.", cm::ArgvParser::OptionRequiresValue);
	cmd.defineOption("texec", "the executable path to use for tests.", cm::ArgvParser::OptionRequiresValue);

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
		setMainArgs(&cmd, &filepath, &execpath, &execute_is_requested, &enable_debug, &debug_level, &run_tests, &testInPath, &testOutPath);
	}

	if (result == cm::ArgvParser::ParserHelpRequested) {
		system("pause");
		exit(exitCode);
	}

	//Execute a file:
	if (execute_is_requested) {
		
		//Create the SIKLang Definition:
		sik::SIKLang* lang = new sik::SIKLang();

		//Create a Script Container:
		sik::SIKScript script(enable_debug, debug_level);

		//Load Libs and Plugins:
		/* TODO: will be removed later -> just for laying the ground for later.
		 * The new approach of the extenssion stuff -> will be implemented by usin SIK code.
		 */

		//Load target script:
		bool indicator = script.compile(filepath);

		//Loading success so go and do stuff:
		if (indicator) {
			execution_result = script.runVm();
		}

		//Clear:
		delete lang;
		script.release();
	}
	//Execute Tests:
	else if (run_tests) {
		std::map <std::string, tinydir_file> test_files;
		std::map <std::string, std::string> expected_results_list;
		std::map <std::string, std::string> computed_results_list;
		std::vector<std::string> failedTests;
		int actualTestsPerform = 0;
		sik::SIKLang::printHeader("RUN TESTS OUTPUT:");
		if (parseTests(testInPath, testOutPath, testExt, &test_files, &expected_results_list, &computed_results_list) == 0) {
			std::cout << std::endl << "No tests Found in the path: " << testInPath << " - Aborting! " << std::endl;
		} else {
			executeTests(execpath, &test_files, &expected_results_list, &computed_results_list, &failedTests, &actualTestsPerform);
			std::cout << "  TOTAL TESTS FOUND  : " << test_files.size() << std::endl;
			std::cout << "  TOTAL TESTS DONE   : " << actualTestsPerform << std::endl;
			std::cout << "  TOTAL TESTS PASSED : " << (actualTestsPerform - failedTests.size()) << std::endl;
			std::cout << "  TOTAL TESTS FAILED : " << failedTests.size() << std::endl;
			if (failedTests.size() > 0) {
				std::cout << "  FAILED TESTS LIST  : " << std::endl;
			}
			for (std::vector<std::string>::iterator it = failedTests.begin(); it != failedTests.end(); ++it) {
				std::cout << "    -> " << *it << std::endl;
			}

		}
	}

	if (enable_debug) {
		//_CrtDumpMemoryLeaks();
		system("pause");
	}

	return execution_result;
}
