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

#include <iostream>
#include <fstream>
#include <streambuf>

namespace sik
{

	SIKScript::SIKScript(bool debugMode, int debugLevel) {
		this->script_debug_flag = debugMode;
		this->script_debug_level = debugLevel;
		this->create_messages();
		this->Instructions.reserve(150);
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
		this->InstructionName = {
			{ INS_NONE,		 "NONE"},
			{ INS_PUSH,		 "PUSH" },

			{ INS_ADD,		 "ADD" },
			{ INS_SUBTRACT,	 "SUB" },
			{ INS_MULTI,	 "MUL" },
			{ INS_DEVIDE,	 "DEV" },
			{ INS_POW,		 "POW" },
			{ INS_INCREMENT, "INC" },
			{ INS_DECREMENT, "DEC" },

			{ INS_ASSIGN,	 "ASGN" },
			{ INS_ASSIGNADD, "ASNAD" },
			{ INS_ASSIGNSUB, "ASNSB" },

			{ INS_DEFINE,	 "DEF" },
			{ INS_IF,		 "IF" },
			{ INS_ELSEIF,	 "ELIF" },
			{ INS_ELSE,		 "ELSE" },
			{ INS_OSBLOCK,	 "OSBLK" },
			{ INS_CSBLOCK,	 "CSBLK" }
		};
	}

	void SIKScript::printIt(std::string type, std::string mesKey) {
		std::cout << this->ScriptMessage[mesKey] << std::endl;
	}

	void SIKScript::printInstructions() {
		int getSize = (int)this->Instructions.size();
		for (int i = 0; i < getSize; i++) {
			std::cout 
				<< "INS "
				<< (i+1) 
				<< ": "
				<< this->InstructionName[this->Instructions[i].Type] 
				<< " \t -> V: "
				<< this->truncateString(this->Instructions[i].Value, 8)
				<< "\t , SUB: "
				<< this->Instructions[i].SubType
				<< " , BLOCK: "
				<< this->Instructions[i].Block
				<< " , LINE: "
				<< this->Instructions[i].lineOrigin
				<< std::endl;
		}
	}
	bool SIKScript::compile(std::string filename) {

		//if Debug:
		if (this->script_debug_flag && this->script_debug_level > 3) {
			sik::SIKLang::printHeader("TOKENIZER OUTPUT:");
		}
		//Force a file extension:
		if (!this->validateFileExtension(filename)) {
			this->printIt("ERROR", "bad-ext");
			sik::SIKLang::printEmpLine(1);
			return false;
		}

		//Open file:
		std::fstream input;
		char cbuffer;
		input.open(filename);
		if (!input) {
			this->printIt("ERROR", "cant-read");
			sik::SIKLang::printEmpLine(1);
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
			if (cbuffer == sik::SIKLang::LangOperationEnd || cbuffer == sik::SIKLang::LangBlockOpenChar || cbuffer == sik::SIKLang::LangBlockCloseChar) {
				
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
					parser.WalkAst(ParseTree, &this->Instructions);
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

		//Print Instructions:
		if (this->script_debug_flag && this->script_debug_level > 1) {
			sik::SIKLang::printHeader("CODE GENERATED:");
			this->printInstructions();
		}

		return true;
	}
	std::string SIKScript::truncateString(const std::string& str, int max) {
		if ((int)str.length() > max) {
			return std::string(str.begin(), str.begin() + (max - 1)) + ">";
		}
		return str;
	}
}
