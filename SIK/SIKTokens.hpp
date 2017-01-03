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
#include "SIKAst.hpp"
#include <string>
#include <vector>

namespace sik {
    
    struct Token {
		int index;
        int fromLine;
        int priority;
        sik::TokenTypes type;
        std::string obj;
		int notation; // to mark early changes.
		sik::SIKAst* node;
    };
    
    class SIKTokens {

        std::vector<sik::Token> tokenSet;
        
    public:
        
        SIKTokens();
        
        int size();
		void clear();
		void pop(int howMany); // Safe
		sik::Token* back(); // Safe
		bool empty();
        void insert(sik::Token token);
        void insert(sik::SIKTokens* tokens);
		void insertAt(sik::Token token, int index);
		void addIndexes();
        std::vector<sik::Token> getSet();
        std::vector<sik::Token>* getSetPointer();
        sik::Token getAt(int i);
        sik::Token* getAtPointer(int i);
        int hasType(sik::TokenTypes type);
		int hasTypeDeep(sik::TokenTypes type);

		int findHighestPriority();
        sik::Token getHighestPriorityToken();
        sik::Token* getHighestPriorityTokenPointer();

		std::vector<int> hasNestedCommas(int indexStart);
		bool hasEmptyNestedCommas(int indexStart);
		int getParenthesesFirstAndLast(int indexStart);
		int getSBracketFirstAndLast(int indexStart);
		int getBlockFirstAndLast(int indexStart);
		int getSatementLast(int indexStart);
		bool hasUnparse();

		bool replaceRangeWithNode(int start, int end, sik::SIKAst* node);
		bool removeFromeSet(int start, int end, bool resetIndexes);
		sik::SIKTokens getFromeSet(int start, int end);
        void renderTokenSet();
		std::string typeToString(sik::TokenTypes type);
		
        virtual ~SIKTokens();
        
    };
}
#endif /* SIKTokens_hpp */
