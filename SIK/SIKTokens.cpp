//
//  SIKTokens.cpp
//  SIK
//
//  Created by Shlomo Hassid on 06/11/2016.
//  Copyright © 2016 Shlomo Hassid. All rights reserved.
//

#include "SIKTokens.hpp"

#include <iostream>

namespace sik {
    SIKTokens::SIKTokens() {
        
    }
    
    int SIKTokens::size() {
        return (int)this->tokenSet.size();
    }
	/* Truncates all the tokens container:
	*/
	void SIKTokens::clear() {
		this->tokenSet.clear();
	}
	/* Pop a token from vector is safe to pop several:
	*/
	void SIKTokens::pop(int howMany) {
		for (int i = 0; i < howMany; i++) {
			if (this->size() > 0) {
				this->tokenSet.pop_back();
			}
		}
	}
    void SIKTokens::insert(sik::Token token) {
        this->tokenSet.push_back(token);
    }
    void SIKTokens::insert(sik::SIKTokens* tokens) {
        int i = tokens->size();
        for (int j = 0; j < i; j++) {
            this->insert(tokens->getAt(j));
        }
    }
    std::vector<sik::Token> SIKTokens::getSet() {
        return this->tokenSet;
    }
    std::vector<sik::Token>* SIKTokens::getSetPointer() {
        return &this->tokenSet;
    }
    sik::Token SIKTokens::getAt(int i) {
        if (i < 0 || i >= this->size()) {
            //Return Error.
        }
        return this->tokenSet[i];
    }
    sik::Token* SIKTokens::getAtPointer(int i) {
        if (i < 0 || i >= this->size()) {
			return nullptr;
        }
        return &this->tokenSet[i];
    }
    int SIKTokens::hasType(sik::TokenTypes type) {
        int i = this->size();
        for (int j = 0; j < i; j++) {
            if (this->tokenSet[j].type == type) {
                return j;
            }
        }
        return -1;
    }
    int SIKTokens::findHighestPriority() {
        int s = this->size();
        int at = -1;
        int high = -1;
        for (int i = 0; i < s; i++) {
            if (this->tokenSet[i].priority > high) {
                high = this->tokenSet[i].priority;
                at = i;
            }
        }
        return at;
    }
    sik::Token SIKTokens::getHighestPriorityToken() {
        int f = this->findHighestPriority();
        if (f == -1) {
            //Return error;
        }
        return this->getAt(f);
    }
    sik::Token* SIKTokens::getHighestPriorityTokenPointer() {
        int f = this->findHighestPriority();
        if (f == -1) {
            //Return error;
        }
        return this->getAtPointer(f);
    }
    void SIKTokens::renderTokenSet() {
        int i = this->size();
		std::cout << "TYPE : ";
        for (int j = 0; j < i; j++) {
			std::cout << "[ " << this->typeToString(this->tokenSet[j].type) << "\t]";
        }
		std::cout << std::endl << "OBJ  : ";
		for (int j = 0; j < i; j++) {
			if (this->tokenSet[j].obj.length() > 4) {
				std::cout << "[ " << std::string(this->tokenSet[j].obj.begin(), this->tokenSet[j].obj.begin() + 4) << "\t]";
			} else {
				std::cout << "[ " << this->tokenSet[j].obj << "\t]";
			}
		}
		std::cout << std::endl << "PRI  : ";
		for (int j = 0; j < i; j++) {
			std::cout << "[ " << this->tokenSet[j].priority << "\t]";
		}
		std::cout << std::endl;
    }
	std::string SIKTokens::typeToString(sik::TokenTypes type) {
		switch (type) {
		case sik::KEYWORD: 
			return "KEY";
		case sik::DELI_BRKOPEN:
			return "BRKO";
		case sik::DELI_BRKCLOSE:
			return "BRKC";
		case sik::DELI_SBRKOPEN:
			return "SBRO";
		case sik::DELI_SBRKCLOSE:
			return "SBRC";
		case sik::DELI_OBJCALL:
			return "OBJC";
		case sik::DELI_INCR:
			return "INCR";
		case sik::DELI_DECR:
			return "DECR";
		case sik::DELI_POW:
			return "POW";
		case sik::DELI_MULTI:
			return "MULT";
		case sik::DELI_DIVIDE:
			return "DIVI";
		case sik::DELI_PLUS:
			return "PLUS";
		case sik::DELI_MINUS:
			return "MINU";
		case sik::DELI_GRT:
			return "GRT";
		case sik::DELI_LST:
			return "LST";
		case sik::DELI_GRTE:
			return "GRTE";
		case sik::DELI_LSTE:
			return "LSTE";
		case sik::DELI_CTEQUAL:
			return "TEQ";
		case sik::DELI_CTNEQUAL:
			return "TNEQ";
		case sik::DELI_CEQUAL:
			return "CEQU";
		case sik::DELI_CNEQUAL:
			return "NEQU";
		case sik::DELI_CAND:
			return "AND";
		case sik::DELI_COR:
			return "COR";
		case sik::DELI_POINT:
			return "POI";
		case sik::DELI_EQUAL:
			return "EQU";
		case sik::DELI_EQUALADD:
			return "EQUA";
		case sik::DELI_EQUALSUB:
			return "EQUS";
		case sik::DELIMITER:
			return "DELI";
		case sik::DELI_COMMA:
			return "COMA";
		case sik::DELI_BRCOPEN:
			return "BRCO";
		case sik::DELI_BRCCLOSE:
			return "BRCC";
		case sik::NAMING:
			return "NAME";
		case sik::STRING:
			return "STR";
		case sik::NUMBER:
			return "NUM";
		default:
			return "UNK";
		}
	}
    /*
     * Virtual Destructor
     */
    SIKTokens::~SIKTokens() {
        
    }
}