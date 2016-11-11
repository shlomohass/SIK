//
//  SID_Lex.cpp
//  SIK
//
//  Created by Shlomo Hassid on 06/11/2016.
//  Copyright © 2016 Shlomo Hassid. All rights reserved.
//

#include "SIK_Lex.hpp"

#include <string>

namespace sik
{
    /*
    * Constructors:
    */
    SIKLex::SIKLex() {
        this->operateMode = ONORMAL;
        this->inString = false;
        this->hadEscape = false;
    }

    SIKLex::SIKLex(sik::OperatingModes debug) {
        this->operateMode = debug;
        this->inString = false;
        this->hadEscape = false;
    }

    void SIKLex::SIKLex::parse(std::string code, int line) {
        
        int size = (int)code.size();
        this->hadEscape = false;
        
        std::string token = "";
        
        for (int i = 0; i < size; ++i) {
            
            char t = code[i];
            //Toggles
            if ((t == ';' && !this->hadEscape) || (t == ' ' && !this->inString)) {
                
                //Generate tokens:
                this->addToken(token, line);
                token = "";
                continue;
                
            } else if (t == '"' && !this->hadEscape) {
                this->inString = this->inString ? false : true;
            } else if (t == '\\' && this->inString) {
                this->hadEscape = this->hadEscape ? false : true;
                continue;
            }
            //Add:
            token += t;
            if (this->hadEscape) this->hadEscape = false;
        }
        if (!this->inString && token.size() > 0) {
            this->addToken(token, line);
            token = "";        }
    }
    void SIKLex::addToken(std::string tok, int line) {
        sik::Token token;
        token.fromLine = line;
        token.obj = tok;
        token.priority = 10;
        token.type = this->evalTokenType(&tok);
        if (token.type == STRING) {
            this->stripStringQ(&token.obj);
        }
        this->tokens.insert(token);
    }
    sik::TokenTypes SIKLex::evalTokenType(std::string* token) {
        if (token->at(0) == '"' && token->at(token->size() - 1) == '"') {
            return sik::STRING;
        }
        return sik::NAN;
    }
    void SIKLex::stripStringQ(std::string* token) {
        token->erase(0, 1);
        token->erase(token->size() - 1);
    }
    /*
     * Get the current set operating mode of the lexer
     */
    sik::OperatingModes SIKLex::getOperatingMode() {
        return this->operateMode;
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