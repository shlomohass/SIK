//
//  SIKParser.hpp
//  SIK
//
//  Created by Shlomo Hassid on 06/11/2016.
//  Copyright © 2016 Shlomo Hassid. All rights reserved.
//

#ifndef SIKParser_hpp
#define SIKParser_hpp

#include "SIKAst.hpp"
#include "SIKTokens.hpp"
#include <ostream>
#include <deque>

namespace sik {

	class SIKParser
	{

	public:

		SIKParser();

		SIKAst* BuildAst(SIKTokens* TokenSet);
		void SetNodeFromToken(SIKAst* node, Token* tok);
		
		//Printing Trees:
		int maxHeight(SIKAst *p);
		void printBranches(int branchLen, int nodeSpaceLen, int startLen, int nodesInThisLevel, const std::deque<SIKAst*>& nodesQueue, std::ostream& out);
		void printNodes(int branchLen, int nodeSpaceLen, int startLen, int nodesInThisLevel, const std::deque<SIKAst*>& nodesQueue, std::ostream& out);
		void printLeaves(int indentSpace, int level, int nodesInThisLevel, const std::deque<SIKAst*>& nodesQueue, std::ostream& out);
		void printTree(SIKAst *root, int level, int indentSpace, std::ostream& out);
		std::string truncateString(const std::string& str);

		virtual ~SIKParser();

	};

}

#endif /* SIKParser_hpp */