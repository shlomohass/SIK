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
        sik::SIKTokens tokens;
        
        bool inString;
        bool hadEscape;
        
    public:
        
        SIKLex();
        SIKLex(sik::OperatingModes operate);
        
        void parse(std::string code, int line);
        void addToken(std::string code, int line);
        sik::TokenTypes evalTokenType(std::string* token);
        void stripStringQ(std::string* token);
        
        sik::OperatingModes getOperatingMode();
        void outputTokens();
        
        virtual ~SIKLex();
    
    };
}

#endif /* SID_Lex_hpp */
