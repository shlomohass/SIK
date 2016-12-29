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
		this->ObjectDefinitions.reserve(20);
		this->FunctionInstructions.reserve(20);
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
			{ INS_CHILDASSIGN, "CHASN" },
			{ INS_DEFINE,	 "DEF"   },
			{ INS_OBJCREATE, "OBJCR" },
			{ INS_OBJDONE,	 "OBJDN" },
			{ INS_IF,		 "IF"   },
			{ INS_ELSEIF,	 "ELIF" },
			{ INS_ELSE,		 "ELSE" },
			{ INS_OSBLOCK,	 "OSBLK" },
			{ INS_CSBLOCK,	 "CSBLK" },

			{ INS_CGRT,		 "CGRT" },
			{ INS_CLST,		 "CLST" },
			{ INS_CGRTE,	 "CGRTE" },
			{ INS_CLSTE,	 "CLSTE" },
			{ INS_CTEQUAL,	 "CTEQ" },
			{ INS_CNTEQUAL,	 "CNTEQ" },
			{ INS_CEQUAL,	 "CEQ" },
			{ INS_CNEQUAL,	 "CNEQ" },
			{ INS_CAND,		 "CAND" },
			{ INS_COR,		 "COR" }
		};
	}

	void SIKScript::printIt(std::string type, std::string mesKey) {
		std::cout << this->ScriptMessage[mesKey] << std::endl;
	}

	void SIKScript::printInstructions(std::vector<sik::SIKInstruct>* _instruct) {
		int getSize = (int)_instruct->size();
		for (int i = 0; i < getSize; i++) {
			sik::SIKInstruct* toPrint = &_instruct->at(i);
			std::cout
				<< "INS "
				<< (i + 1)
				<< ": "
				<< this->InstructionName[toPrint->Type]
				<< " \t -> V: "
				<< this->truncateString(toPrint->Value, 8)
				<< "\t , SUB: "
				<< toPrint->SubType
				<< " , BLOCK: "
				<< toPrint->Block
				<< " , INSPO: "
				<< toPrint->pointToInstruct
				<< " , INJMP: "
				<< this->Instructions[i].InternalJumper
				<< " , INNUM: "
				<< this->Instructions[i].MyInternalNumber
				<< " , LINE: "
				<< toPrint->lineOrigin
				<< std::endl;
		}
	}
	void SIKScript::printObjectDefinitions() {
		int getSize = (int)this->ObjectDefinitions.size();
		for (int i = 0; i < getSize; i++) {
			std::cout
				<< "INSPO : "
				<< i
				<< std::endl;
			this->printInstructions(&this->ObjectDefinitions.at(i));
		}
	}
	void SIKScript::printFunctionDefinitions() {
		int getSize = (int)this->FunctionInstructions.size();
		for (int i = 0; i < getSize; i++) {
			std::cout
				<< "INS "
				<< (i + 1)
				<< ": "
				<< this->InstructionName[this->Instructions[i].Type]
				<< " \t -> V: "
				<< this->truncateString(this->Instructions[i].Value, 8)
				<< "\t , SUB: "
				<< this->Instructions[i].SubType
				<< " , BLOCK: "
				<< this->Instructions[i].Block
				<< " , INSPO: "
				<< this->Instructions[i].pointToInstruct
				<< " , INJMP: "
				<< this->Instructions[i].InternalJumper
				<< " , INNUM: "
				<< this->Instructions[i].MyInternalNumber
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
		sik::SIKParser parser = sik::SIKParser(&this->Instructions, &this->ObjectDefinitions, &this->FunctionInstructions);

		//Read to create token stream seperate by blocks and by end of statement:
		std::map<int,std::string> expressionContainer;
		std::string expbuffer = "";
		int line = 1;
		char prevbuff = ' ';
		bool singleComment = false;
		bool multicomment = false;
		bool inString = false;
		int block = 0;
		//Lex the lines:  
		while (input >> std::noskipws >> cbuffer) {
			//Handle new lines:
			if (cbuffer == '\n') {
				expressionContainer[line] = expbuffer;
				expbuffer = "";
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
			if (cbuffer == '"' && prevbuff != '\\') {
				inString = inString ? false : true;
			}
			if (cbuffer == '{' && !inString) {
				block++;
			}
			if (cbuffer == '}' && !inString) {
				block--;
			}
			//Build Expression:
			expbuffer += cbuffer;

			//Check whether we are ready to pass the statement:
			//if (cbuffer == sik::SIKLang::LangOperationEnd || cbuffer == sik::SIKLang::LangBlockOpenChar || cbuffer == sik::SIKLang::LangBlockCloseChar) {
			if (cbuffer == sik::SIKLang::LangOperationEnd && block == 0) {
				expressionContainer[line] = expbuffer;
				if (this->compile(&lexer, &parser, expressionContainer)) {
					expbuffer.clear();
					expressionContainer.clear();
					continue;
				} else {
					return false;
				}
			}

			//Log prev char for comment use:
			prevbuff = cbuffer;
		}

		//Final parse if needed:
		if (expressionContainer.size() > 0 || !expbuffer.empty()) {
			if (!expbuffer.empty()) {
				expressionContainer[line] = expbuffer;
			}
			if (!this->compile(&lexer, &parser, expressionContainer)) {
				return false;
			}
		}

		//Print Instructions:
		if (this->script_debug_flag && this->script_debug_level > 1) {
			sik::SIKLang::printHeader("CODE GENERATED:");
			this->printInstructions(&this->Instructions);

			sik::SIKLang::printHeader("OBJECT DEFINITIONS:");
			this->printObjectDefinitions();
		}

		return true;
	}
	bool SIKScript::compile(sik::SIKLex* lexer, sik::SIKParser* parser, std::map<int, std::string> exp) {
		//Pre compile: macros, expnsions.

		//Generate tokens:
		sik::SIKLang::printEmpLine(1);
		typedef std::map<int,std::string>::iterator it_type;
		for (it_type iterator = exp.begin(); iterator != exp.end(); iterator++) {
			lexer->parse(iterator->second, iterator->first);
		}
		lexer->tokens.addIndexes();

		//Log tokens by debug:
		if (this->script_debug_flag && this->script_debug_level > 3) {
			std::string singleLineExpression = "";
			int smallestLine = 10000;
			int biggestLine = 0;
			for (it_type iterator = exp.begin(); iterator != exp.end(); iterator++) {
				singleLineExpression += iterator->second;
				if (iterator->first > biggestLine) biggestLine = iterator->first;
				if (iterator->first < smallestLine) smallestLine = iterator->first;
			}
			lexer->outputExpressionLine(singleLineExpression, smallestLine, biggestLine);
			lexer->outputTokens();
			SIKLang::printEmpLine(1);
		}

		//Parse -> AlS -> Byte code:
		sik::SIKAst* ParseTree = nullptr;
		try {
			ParseTree = parser->BuildAst(lexer->GetTokensPoint());
		}
		catch (sik::SIKException& ex)
		{
			ex.render(this->script_debug_level);
			return false;
		}

		//Log trees by debug:
		if (this->script_debug_flag && this->script_debug_level > 3) {
			for (int i = 0; i < (int)ParseTree->bulk.size(); i++) {
				parser->printTree(ParseTree->bulk[i], 1, 0, std::cout);
				SIKLang::printEmpLine(1);
			}
		}

		//Walk tree and evaluate:
		/**/
		try {
			parser->WalkAst(ParseTree);
		}
		catch (sik::SIKException& ex)
		{
			ex.render(this->script_debug_level);
			return false;
		}

		//Release memmory:
		delete ParseTree;

		lexer->truncateTokens();

		return true;
	}
	std::string SIKScript::truncateString(const std::string& str, int max) {
		if ((int)str.length() > max) {
			return std::string(str.begin(), str.begin() + (max - 1)) + ">";
		}
		return str;
	}
}
