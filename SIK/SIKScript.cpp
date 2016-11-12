/*
 * File:   SIKScript.cpp
 * Author : shlomo hassid
 */

#include "SIKAdd.hpp"
#include "SIKLang.hpp"
#include "SIKScript.hpp"
#include "SIKTokens.hpp"
#include "SIK_Lex.hpp"
#include <fstream>
#include <streambuf>

namespace sik
{

	SIKScript::SIKScript(bool debugMode) {
		this->script_debug_flag = debugMode;
		this->create_messages();
	};

	bool SIKScript::validateFileExtension(std::string filename) {
		std::string ext = filename.substr(filename.find_last_of(".") + 1);
		if (find(SIKLang::extensionLib.begin(), SIKLang::extensionLib.end(), ext) != SIKLang::extensionLib.end()) {
			return true;
		}
		return false;
	}

	void SIKScript::create_messages() {
		this->ScriptMessage = {
			{ "bad-ext"           , "The file you are trying to execute is not a SIK script." },
			{ "cant-read"         , "The file can't be read - may be permission error."       }
		};
	}

	void SIKScript::printIt(std::string type, std::string mesKey) {
		std::cout << this->ScriptMessage[mesKey] << std::endl;
	}

	bool SIKScript::compile(std::string filename) {

		//if Debug:
		if (this->script_debug_flag) {
			SIKLang::printHeader("TOKENIZER OUTPUT:");
		}
		//Force a file extension:
		if (!this->validateFileExtension(filename)) {
			this->printIt("ERROR", "bad-ext");
			return false;
		}

		//Open file:
		std::fstream input;
		char cbuffer;
		input.open(filename);
		if (!input) {
			this->printIt("ERROR", "cant-read");
			return false;
		}

		//The Lexer:
		sik::SIKLex lexer = sik::SIKLex();

		//Read to create token stream seperate by blocks and by end of statement:
		std::string expbuffer = "";
		int line = 1;
		char prevbuff = ' ';
		bool singleComment = false;
		bool multicomment = false;
		//Lex the lines:  
		while (input >> std::noskipws >> cbuffer) {
			//Handle new lines:
			if (cbuffer == '\n') {
				line++;
				singleComment = false;
				continue;
			}
			//Handle comments:
			if (singleComment) {
				continue;
			}
			if (multicomment) {
				if (prevbuff == '*' && cbuffer == '/') {
					multicomment = false;
					prevbuff = ' ';
					continue;
				}
				prevbuff = cbuffer;
				continue;
			}
			if (prevbuff == '/' && cbuffer == '/') {
				singleComment = true;
				prevbuff = ' ';
				expbuffer = expbuffer.substr(0, expbuffer.size() - 1);
				continue;
			}
			if (prevbuff == '/' && cbuffer == '*') {
				multicomment = true;
				expbuffer = expbuffer.substr(0, expbuffer.size() - 1);
				continue;
			}

			//Build Expression:
			expbuffer += cbuffer;

			//Check whether we are ready to pass the statement:
			if (cbuffer == ';' || cbuffer == '{' || cbuffer == '}') {
				
				//Pre compile: macros, expnsions.

				//Generate tokens:
				SIKLang::printEmpLine(1);
				lexer.parse(expbuffer, line);

				//Log tokens by debug:
				if (this->script_debug_flag) {
					lexer.outputExpressionLine(expbuffer, line);
					lexer.outputTokens();
				}

				//Parse -> AlS -> Byte code:

				lexer.truncateTokens();
				expbuffer.clear();
				continue;
			}

			//Log prev char for comment use:
			prevbuff = cbuffer;
		}
		return true;
	}
}
