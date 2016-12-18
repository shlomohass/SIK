//
//  SID_Lex.cpp
//  SIK
//
//  Created by Shlomo Hassid on 06/11/2016.
//  Copyright © 2016 Shlomo Hassid. All rights reserved.
//

#include "SIKLang.hpp"
#include "SIK_Lex.hpp"

#include <iostream>

namespace sik
{
    /*
    * Constructors:
    */
    SIKLex::SIKLex() {
        this->operateMode = ONORMAL;
        this->inString = false;
        this->hadEscape = false;
		this->currentToken = "";
    }

    SIKLex::SIKLex(sik::OperatingModes debug) {
        this->operateMode = debug;
        this->inString = false;
        this->hadEscape = false;
		this->currentToken = "";
    }

	/** Get a pointer to the tokens collection
	 * @return SIKTokens* tokenSet
	*/
	sik::SIKTokens* SIKLex::GetTokensPoint()
	{
		return &this->tokens;
	}

    void SIKLex::SIKLex::parse(std::string code, int line) {
        
		int expIndex = 0;
        this->hadEscape = false;

		while (expIndex < (int)code.length()) {

			//check for endl of expression
			if (expIndex == (int)code.length()) {
				continue;
			}
			if (expIndex > (int)code.length()) {
				continue;
			}
			if (!this->inString) {
				//skip over white spaces and ;
				while (expIndex < (int)code.length() && (this->isSpace(code[expIndex]) || code[expIndex] == '\t')) {
					++expIndex;
				}
				//check for endl of expression
				if (expIndex == (int)code.length()) {
					continue;
				}
			}
			//Store correct token:
			//First watch for double delimiters:
			if (!this->inString && this->isDelimiter(code[expIndex]) && expIndex + 1 < (int)code.length()) {
				std::string lookInfront;
				lookInfront += code[expIndex];
				lookInfront += code[expIndex + 1];
				if (this->isDelimiter(lookInfront)) {
					this->currentToken += lookInfront;
					expIndex += 2;
					//Add the token found:
					this->addToken(this->currentToken, line);
					this->currentToken = "";
					continue;
				}
			}
			if (!this->inString && this->isDelimiter(code[expIndex])) {
				this->currentToken.insert(this->currentToken.end(), 1, code[expIndex]);
				expIndex++;
			}
			else if (!this->inString && this->isLetter(code[expIndex])) {
				//Grab entire word:
				while (expIndex < (int)code.length() && this->isNaming(code[expIndex])) {
					this->currentToken.insert(this->currentToken.end(), 1, code[expIndex]);
					expIndex++;
					if (expIndex >= (int)code.length()) {
						break;
					}
				}
				//Set all to lower that make sure we can check if its a keyword:
				std::string temp_currentToken = this->toLowerString(&this->currentToken);

				//Set this current token:
				this->currentToken = temp_currentToken;
			}
			else if (this->inString || this->isQstring(code[expIndex])) {
				//Grab entire string:
				char addChar = code[expIndex];
				if (addChar == '\n' || addChar == '\t' || addChar == '\r' || addChar == '\0') {
					addChar = ' ';
				}
				this->currentToken.insert(this->currentToken.end(), 1, addChar);
				if (this->inString && addChar == '"') {
					this->inString = false;
				} else {
					expIndex++;
				}
				bool avoid = false;
				while (!isQstring(code[expIndex])) {
					this->inString = false;
					if (code[expIndex] == '\\') {
						this->currentToken.insert(this->currentToken.end(), 1, code[expIndex]);
						expIndex++;
						if (expIndex >= (int)code.length()) {
							//Throw error -> string is not encapsulated.
							this->currentToken = "";
							break;
						}
						this->currentToken.insert(this->currentToken.end(), 1, code[expIndex]);
						expIndex++;
					} else {
						char addCharMore = code[expIndex];
						if (addCharMore == '\n' || addCharMore == '\t' || addCharMore == '\r' || addCharMore == '\0') {
							addCharMore = ' ';
						}
						this->currentToken.insert(this->currentToken.end(), 1, addCharMore);
						expIndex++;
					}
					if (expIndex >= (int)code.length()) {
						avoid = true;
						break;
					}
				}
				if (!avoid) {
					this->inString = false;
					this->currentToken.insert(this->currentToken.end(), 1, code[expIndex]);
					expIndex++;
				} else {
					this->inString = true;
				}
			} else if (!this->inString && this->isDigit(code[expIndex])) {
				//Grab entire number
				while (this->isDigit(code[expIndex])) { // `.` is a number too removed from the delimiter list
					this->currentToken.insert(this->currentToken.end(), 1, code[expIndex]);
					expIndex++;
					if (expIndex > (int)code.length()) break;
				}
			} else {
				this->currentToken = "";
				expIndex++;
				continue;
			}
			//Add the token found:
			if (!this->inString) {
				this->addToken(this->currentToken, line);
				this->currentToken = "";
			}
		}
    }
    void SIKLex::addToken(std::string tok, int line) {
        
		sik::Token token;
		token.index = -1;
        token.fromLine = line;
        token.obj = tok;
        token.type = this->evalTokenType(tok);
		token.priority = this->evalTokenPriority(&token);
		token.node = nullptr;

		if	(token.type == STRING) {
            this->stripStringQ(&token.obj);
        }
		if (token.type == NUMBER) {
			// Run check for value negativity and only dot prefix:
			int curSize = this->tokens.size();
			if (curSize > 0) {
				sik::Token* prevOne = this->tokens.getAtPointer(this->tokens.size() - 1);
				sik::Token* prevTwo = this->tokens.getAtPointer(this->tokens.size() - 2);
				bool check = false;
				if (
					prevTwo != nullptr &&
					this->isDelimiter(prevTwo->obj) &&
					prevTwo->type != sik::DELI_BRKCLOSE && 
					prevTwo->type != sik::DELI_SBRKCLOSE && 
					prevOne != nullptr &&
					(prevOne->obj == SIKLang::dicLang_minus || prevOne->obj == SIKLang::dicLang_objcall)
					) {
					check = true;
				} else if (
					prevTwo == nullptr &&
					prevOne != nullptr &&
					(prevOne->obj == SIKLang::dicLang_minus || prevOne->obj == SIKLang::dicLang_objcall)
					) {
					check = true;
				}
				if (check && prevOne->obj == SIKLang::dicLang_minus) {
					this->tokens.pop(1);
					token.obj = SIKLang::dicLang_minus + token.obj;
				}
				else if (check && prevOne->obj == SIKLang::dicLang_objcall) {
					this->tokens.pop(1);
					token.obj = '0' + SIKLang::dicLang_objcall + token.obj;
				}
			}
		}
        this->tokens.insert(token);
    }
    sik::TokenTypes SIKLex::evalTokenType(const std::string& token) {
        if (token.at(0) == '"' && token.at(token.size() - 1) == '"') {
            return sik::STRING;
        }
		if (SIKLang::LangIsKeyword(token)) {
			return sik::KEYWORD;
		}
		if (SIKLang::LangIsDelimiter(token)) {
			return this->evalDelimType(token);
		}
		if (SIKLang::LangIsNaming(token)) {
			if (SIKLang::LangIsBoolean(token) > -1) {
				return sik::BOOLEAN;
			}
			if (SIKLang::LangIsNull(token)) {
				return sik::NULLTYPE;
			}
			return sik::NAMING;
		}
		if (SIKLang::LangIsNumber(token)) {
			return sik::NUMBER;
		}
        return sik::NOPE;
    }
	sik::TokenTypes SIKLex::evalDelimType(const std::string& value) {
		char test = value[0];
		if (test == SIKLang::dicLang_plus[0]			&& value == SIKLang::dicLang_plus)			return sik::DELI_PLUS;
		if (test == SIKLang::dicLang_minus[0]			&& value == SIKLang::dicLang_minus)			return sik::DELI_MINUS;
		if (test == SIKLang::dicLang_inc[0]				&& value == SIKLang::dicLang_inc)			return sik::DELI_INCR;
		if (test == SIKLang::dicLang_dec[0]				&& value == SIKLang::dicLang_dec)			return sik::DELI_DECR;
		if (test == SIKLang::dicLang_multi[0]			&& value == SIKLang::dicLang_multi)			return sik::DELI_MULTI;
		if (test == SIKLang::dicLang_divide[0]			&& value == SIKLang::dicLang_divide)		return sik::DELI_DIVIDE;
		if (test == SIKLang::dicLang_equal[0]			&& value == SIKLang::dicLang_equal)			return sik::DELI_EQUAL;
		if (test == SIKLang::dicLang_childSet[0]		&& value == SIKLang::dicLang_childSet)		return sik::DELI_CHILDSET;
		if (test == SIKLang::dicLang_equalAdd[0]		&& value == SIKLang::dicLang_equalAdd)		return sik::DELI_EQUALADD;
		if (test == SIKLang::dicLang_equalSub[0]		&& value == SIKLang::dicLang_equalSub)		return sik::DELI_EQUALSUB;
		if (test == SIKLang::dicLang_pointer[0]			&& value == SIKLang::dicLang_pointer)		return sik::DELI_POINT;
		if (test == SIKLang::dicLang_braketOpen[0]		&& value == SIKLang::dicLang_braketOpen)	return sik::DELI_BRKOPEN;
		if (test == SIKLang::dicLang_braketClose[0]		&& value == SIKLang::dicLang_braketClose)	return sik::DELI_BRKCLOSE;
		if (test == SIKLang::dicLang_bracesOpen[0]		&& value == SIKLang::dicLang_bracesOpen)	return sik::DELI_BRCOPEN;
		if (test == SIKLang::dicLang_bracesClose[0]		&& value == SIKLang::dicLang_bracesClose)	return sik::DELI_BRCCLOSE;
		if (test == SIKLang::dicLang_sBraketOpen[0]		&& value == SIKLang::dicLang_sBraketOpen)	return sik::DELI_SBRKOPEN;
		if (test == SIKLang::dicLang_sBraketClose[0]	&& value == SIKLang::dicLang_sBraketClose)	return sik::DELI_SBRKCLOSE;
		if (test == SIKLang::dicLang_power[0]			&& value == SIKLang::dicLang_power)			return sik::DELI_POW;
		if (test == SIKLang::dicLang_exclamation[0]		&& value == SIKLang::dicLang_exclamation)	return sik::DELI_EXCL;
		if (test == SIKLang::dicLang_greater[0]			&& value == SIKLang::dicLang_greater)		return sik::DELI_GRT;
		if (test == SIKLang::dicLang_smaller[0]			&& value == SIKLang::dicLang_smaller)		return sik::DELI_LST;
		if (test == SIKLang::dicLang_greater_equal[0]	&& value == SIKLang::dicLang_greater_equal) return sik::DELI_GRTE;
		if (test == SIKLang::dicLang_smaller_equal[0]	&& value == SIKLang::dicLang_smaller_equal) return sik::DELI_LSTE;
		if (test == SIKLang::dicLang_comma[0]			&& value == SIKLang::dicLang_comma)			return sik::DELI_COMMA;
		if (test == SIKLang::dicLang_c_tequal[0]		&& value == SIKLang::dicLang_c_tequal)		return sik::DELI_CTEQUAL;
		if (test == SIKLang::dicLang_c_ntequal[0]		&& value == SIKLang::dicLang_c_ntequal)		return sik::DELI_CTNEQUAL;
		if (test == SIKLang::dicLang_c_equal[0]			&& value == SIKLang::dicLang_c_equal)		return sik::DELI_CEQUAL;
		if (test == SIKLang::dicLang_c_nequal[0]		&& value == SIKLang::dicLang_c_nequal)		return sik::DELI_CNEQUAL;
		if (test == SIKLang::dicLang_and[0]				&& value == SIKLang::dicLang_and)			return sik::DELI_CAND;
		if (test == SIKLang::dicLang_or[0]				&& value == SIKLang::dicLang_or)			return sik::DELI_COR;
		return sik::NOPE;
	}
	int SIKLex::evalTokenPriority(sik::Token* token) {
		switch (token->type) {
		case sik::KEYWORD:          // let, print ...
			return 100;
		case sik::DELI_BRKOPEN:		// (
		case sik::DELI_BRKCLOSE:	// )
			return 90;
		case sik::DELI_SBRKOPEN:	// [
		case sik::DELI_SBRKCLOSE:	// ]
		case sik::DELI_OBJCALL:
			return 80;
		case sik::DELI_INCR:		// ++
		case sik::DELI_DECR:		// --
			return 70;
		case sik::DELI_POW:			// ^
			return 51;
		case sik::DELI_MULTI:		// *
		case sik::DELI_DIVIDE:		// /
			return 50;
		case sik::DELI_PLUS:		// +
		case sik::DELI_MINUS:		// -
			return 40;
		case sik::DELI_GRT:			// >
		case sik::DELI_LST:			// <
		case sik::DELI_GRTE:		// >=
		case sik::DELI_LSTE:		// <=
			return 30;
		case sik::DELI_CTEQUAL:		// =~
		case sik::DELI_CTNEQUAL:	// !~
		case sik::DELI_CEQUAL:		// ==
		case sik::DELI_CNEQUAL:     // !=
			return 20;
		case sik::DELI_CAND:        // &&
			return 19;
		case sik::DELI_COR:         // ||
			return 18;
		case sik::DELI_BRCOPEN:		// {
			return 17;
		case sik::DELI_POINT:		// ->
		case sik::DELI_EQUAL:       // =
		case sik::DELI_CHILDSET:    // :
		case sik::DELI_EQUALADD:    // +=
		case sik::DELI_EQUALSUB:    // -=
			return 16;
		case sik::DELIMITER:		// ...
		case sik::DELI_COMMA:       // ,
			return 11;
		
		case sik::DELI_BRCCLOSE:    // }
			return 10;

		case sik::NAMING:
		case sik::STRING:
		case sik::NUMBER:
			return 0;
		default:
			return 0;
		}
	}
	/* Truncates all the tokens container:
	 */
	void SIKLex::truncateTokens() {
		this->tokens.clear();
	}
    void SIKLex::stripStringQ(std::string* token) {
        token->erase(0, 1);
        token->erase(token->size() - 1);
    }
	/** Indicate whether or not a char constant c is one of our defined delimiters
	*  Note: delimiters are used to separate individual tokens
	*
	* @param string | char c
	* @return return true if it is a delimiter otherwise false
	*/
	bool SIKLex::isDelimiter(const std::string& c) {
		return SIKLang::LangIsDelimiter(c);
	}
	bool SIKLex::isDelimiter(const char& c) {
		return SIKLang::LangIsDelimiter(c);
	}
	/** Check whether a char is space or not
	*
	* @param char|string c
	* @return boolean
	*/
	bool SIKLex::isSpace(const char& c) {
		return (c == SIKLang::LangGeneralSpace) ? true : false;
	}
	bool SIKLex::isSpace(const std::string& c) {
		return (c == SIKLang::dicLang_space) ? true : false;
	}
	/** Check whether we entering using a string:
	*
	* @param char|string c
	* @return boolean
	*/
	bool SIKLex::isQstring(const char& c) {
		return c == SIKLang::LangStringIndicator;
	}
	bool SIKLex::isQstring(const std::string& c) {
		return c[0] == SIKLang::LangStringIndicator;
	}
	/** Validate naming chars:
	*
	* @param char|string c
	* @return boolean
	*/
	bool SIKLex::isLetter(const char& c) {
		return SIKLang::LangIsNamingAllowed(c);
	}
	bool SIKLex::isLetter(const std::string& c) {
		return SIKLang::LangIsNamingAllowed(c);
	}
	/** Indicate true of false if a specific character constant, c, is a digit or not
	*
	* @param char|string c
	* @return boolean
	*/
	bool SIKLex::isDigit(const std::string& c) {
		return isDigit(c[0]);
	}
	bool SIKLex::isDigit(const char& c) {
		return SIKLang::LangIsNumberAllowed(c);
	}
	/** Indicate if supplied string, s, is a keyword
	*
	* @param string s
	* @return boolean
	*/
	bool SIKLex::isKeyword(char& c) {
		std::string str(1, c);
		return isKeyword(str);
	}
	bool SIKLex::isKeyword(std::string s) {
		if (SIKLang::LangIsKeyword(s)) {
			return true;
		}
		return false;
	}
	/** Indicate if supplied string, s, is a naming allowed
	*
	* @param string s
	* @return boolean
	*/
	bool SIKLex::isNaming(char& c) {
		std::string str(1, c);
		return isNaming(str);
	}
	bool SIKLex::isNaming(std::string s) {
		if (SIKLang::LangIsNaming(s)) {
			return true;
		}
		return false;
	}

	/** Parse any string to lower ASCII chars
	*
	* @param string s
	* @return string
	*/
	std::string SIKLex::toLowerString(std::string *s) {
		std::string outBuffer = *s;
		std::transform(s->begin(), s->end(), outBuffer.begin(), ::tolower);
		return outBuffer;
	}
	/** Parse any string to lower ASCII chars
	*
	* @param string s
	* @return string
	*/
	std::string SIKLex::toUpperString(std::string *s) {
		std::string outBuffer = *s;
		std::transform(s->begin(), s->end(), outBuffer.begin(), ::toupper);
		return outBuffer;
	}
    /*
     * Get the current set operating mode of the lexer
     */
    sik::OperatingModes SIKLex::getOperatingMode() {
        return this->operateMode;
    }
    /* Prints an Expression line into the console:
	 * @param string exp
	 */
	void SIKLex::outputExpressionLine(const std::string& exp, int smallestLine, int biggestLine) {
		if (exp.length() > 50) {
			std::cout << "LINE : " << "[" << smallestLine << " - " << biggestLine << "] -> " << std::string(exp.begin(), exp.begin() + 47) << "..." << std::endl;
		}
		else
		{
			std::cout << "LINE : " << "[" << smallestLine << " - " << biggestLine << "] -> " << exp << std::endl;
		}
	}
    void SIKLex::outputTokens() {
        this->tokens.renderTokenSet();
    }
    /*
    * Virtual Destructor
    */
    SIKLex::~SIKLex() {

    }

}