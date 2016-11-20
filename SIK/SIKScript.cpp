/*
 * File:   SIKScript.cpp
 * Author : shlomo hassid
 */

#include "SIKAdd.hpp"
#include "SIKLang.hpp"
#include "SIKException.hpp"
#include "SIKScript.hpp"
#include "SIKTokens.hpp"
#include "SIK_Lex.hpp"
#include "SIKAst.hpp"
#include "SIKParser.hpp"

#include <fstream>
#include <streambuf>

namespace sik
{

	SIKScript::SIKScript(bool debugMode, int debugLevel) {
		this->script_debug_flag = debugMode;
		this->script_debug_level = debugLevel;
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
		if (this->script_debug_flag && this->script_debug_level > 3) {
			SIKLang::printHeader("TOKENIZER OUTPUT:");
		}
		//Force a file extension:
		if (!this->validateFileExtension(filename)) {
			this->printIt("ERROR", "bad-ext");
			SIKLang::printEmpLine(1);
			return false;
		}

		//Open file:
		std::fstream input;
		char cbuffer;
		input.open(filename);
		if (!input) {
			this->printIt("ERROR", "cant-read");
			SIKLang::printEmpLine(1);
			return false;
		}

		//The Lexer:
		sik::SIKLex lexer = sik::SIKLex();

		//The parser:
		sik::SIKParser parser = sik::SIKParser();

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
				sik::SIKLang::printEmpLine(1);
				lexer.parse(expbuffer, line);
				lexer.tokens.addIndexes();

				//Log tokens by debug:
				if (this->script_debug_flag && this->script_debug_level > 3) {
					lexer.outputExpressionLine(expbuffer, line);
					lexer.outputTokens();
					SIKLang::printEmpLine(1);
				}

				//Parse -> AlS -> Byte code:
				sik::SIKAst* ParseTree = nullptr;
				try {
					ParseTree = parser.BuildAst(lexer.GetTokensPoint());
				}
				catch (sik::SIKException& ex)
				{
					ex.render(this->script_debug_level);
					return false;
				}

				//Log trees by debug:
				if (this->script_debug_flag && this->script_debug_level > 3) {
					for (int i = 0; i < (int)ParseTree->bulk.size(); i++) {
						parser.printTree(ParseTree->bulk[i], 1, 0, std::cout);
						SIKLang::printEmpLine(1);
					}
				}

				//Walk tree and evaluate:
				/**/
				try {
					parser.WalkAst(ParseTree);
				}
				catch (sik::SIKException& ex)
				{
					ex.render(this->script_debug_level);
					return false;
				}
				
				//Release memmory:
				delete ParseTree;

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
