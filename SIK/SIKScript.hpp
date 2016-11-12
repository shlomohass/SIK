/*
* File:   SIKScript.hpp
* Author: shlomo hassid
*
*/

#ifndef SIKSCRIPT_H
#define	SIKSCRIPT_H

#include "SIKAdd.hpp"
#include <iostream>
#include <string>
#include <map>

namespace sik
{
	
	class SIKScript {

		
		std::map<std::string, std::string> ScriptMessage;

		bool script_debug_flag;

		bool validateFileExtension(std::string filename);
		void create_messages();
		void SIKScript::printIt(std::string type, std::string mesKey);

	public:

		SIKScript(bool debugMode);
		bool compile(std::string filename);

	};

}
#endif	