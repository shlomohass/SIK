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
            //Return Error.
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
        for (int j = 0; j < i; j++) {
            std::cout   << j
                        << " TYPE : "
                        << this->tokenSet[j].type
                        << " OBJ : "
                        << this->tokenSet[j].obj
                        << " PRI : "
                        << this->tokenSet[j].priority
                        << " \n";
        }
    }
    /*
     * Virtual Destructor
     */
    SIKTokens::~SIKTokens() {
        
    }
}