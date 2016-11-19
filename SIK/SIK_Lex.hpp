//
//  SID_Lex.hpp
//  SIK
//
//  Created by Shlomo Hassid on 06/11/2016.
//  Copyright © 2016 Shlomo Hassid. All rights reserved.
//

#ifndef SID_Lex_hpp
#define SID_Lex_hpp

#include "SIKAdd.hpp"
#include "SIKTokens.hpp"
#include <string>

namespace sik
{
    
    class SIKLex {
        
        sik::OperatingModes operateMode;
        
        
        bool inString;
        bool hadEscape;
        
    public:
        
		sik::SIKTokens tokens;

        SIKLex();
        SIKLex(sik::OperatingModes operate);
		sik::SIKTokens* GetTokensPoint();

        void parse(std::string code, int line);
        void addToken(std::string code, int line);
        sik::TokenTypes evalTokenType(const std::string& token);
		sik::TokenTypes evalDelimType(const std::string& token);
		int evalTokenPriority(sik::Token* token);
		void truncateTokens();

        void stripStringQ(std::string* token);
        
		bool isDelimiter(const std::string& c);
		bool isDelimiter(const char& c);
		bool isSpace(const std::string& c);
		bool isSpace(const char& c);
		bool isQstring(const std::string& c);
		bool isQstring(const char& c);
		bool isLetter(const std::string& c);
		bool isLetter(const char& c);
		bool isDigit(const std::string& c);
		bool isDigit(const char& c);
		bool isKeyword(std::string s);
		bool isKeyword(char& c);
		bool isNaming(char& c);
		bool isNaming(std::string s);

		std::string toLowerString(std::string *s);
		std::string toUpperString(std::string *s);

        sik::OperatingModes getOperatingMode();
		void outputExpressionLine(const std::string& exp, int line);
        void outputTokens();
        
        virtual ~SIKLex();
    
    };
}

#endif /* SID_Lex_hpp */
