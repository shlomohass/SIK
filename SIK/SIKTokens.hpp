//
//  SIKTokens.hpp
//  SIK
//
//  Created by Shlomo Hassid on 06/11/2016.
//  Copyright © 2016 Shlomo Hassid. All rights reserved.
//

#ifndef SIKTokens_hpp
#define SIKTokens_hpp

#include "SIKAdd.hpp"
#include <string>
#include <vector>

namespace sik {
    
    struct Token {
        
        int fromLine;
        int priority;
        sik::TokenTypes type;
        std::string obj;
        
    };
    
    class SIKTokens {

        std::vector<sik::Token> tokenSet;
        
    public:
        
        SIKTokens();
        
        int size();
		void clear();
		void pop(int howMany); // Safe
        void insert(sik::Token token);
        void insert(sik::SIKTokens* tokens);
        std::vector<sik::Token> getSet();
        std::vector<sik::Token>* getSetPointer();
        sik::Token getAt(int i);
        sik::Token* getAtPointer(int i);
        int hasType(sik::TokenTypes type);
        int findHighestPriority();
        sik::Token getHighestPriorityToken();
        sik::Token* getHighestPriorityTokenPointer();
        void renderTokenSet();
		std::string typeToString(sik::TokenTypes type);

        virtual ~SIKTokens();
        
    };
}
#endif /* SIKTokens_hpp */
