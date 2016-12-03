/*
* File:   SIKScript.hpp
* Author: shlomo hassid
*
*/

#ifndef SIKSCRIPT_H
#define	SIKSCRIPT_H

#include "SIKAdd.hpp"
#include "SIKInstruct.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <map>

namespace sik
{
	
	class SIKScript {

		
		std::map<std::string, std::string> ScriptMessage;
		std::vector<sik::SIKInstruct> Instructions;
		std::map<sik::InstructType, std::string> InstructionName;

		bool script_debug_flag;
		int script_debug_level;

		bool validateFileExtension(std::string filename);
		void create_messages();
		void printIt(std::string type, std::string mesKey);
		void printInstructions();

	public:

		SIKScript(bool debugMode, int debugLevel);
		bool compile(std::string filename);
		std::string truncateString(const std::string& str, int max);

	};

}
#endif	