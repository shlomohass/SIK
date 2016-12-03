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
#include "SIKInstruct.hpp"
#include <ostream>
#include <vector>
#include <deque>

namespace sik {

	class SIKParser
	{

		std::vector<sik::BlocksIn> BlockInCheck;

	public:

		SIKParser();

		//Operations AST:
		sik::SIKAst* BuildAst(sik::SIKTokens* TokenSet);

		int BuildAst_Naming(sik::SIKAst* node, sik::Token* token, sik::SIKTokens* TokenSet);
		int BuildAst_MathLR(sik::SIKAst* node, sik::Token* token, sik::SIKTokens* TokenSet);
		int BuildAst_Assigning(sik::SIKAst* node, sik::Token* token, sik::SIKTokens* TokenSet);
		int BuildAst_KeyDefine(sik::SIKAst* node, sik::Token* token, sik::SIKTokens* TokenSet);
		int BuildAst_KeyIf(sik::SIKAst* node, sik::Token* token, sik::SIKTokens* TokenSet);
		int BuildAst_KeyElseIf(sik::SIKAst* node, sik::Token* token, sik::SIKTokens* TokenSet);
		int BuildAst_KeyElse(sik::SIKAst* node, sik::Token* token, sik::SIKTokens* TokenSet);
		int BuildAst_BlockClose(sik::SIKAst* node, sik::Token* token, sik::SIKTokens* TokenSet);

		//Operations:
		void SetNodeFromToken(sik::SIKAst* node, sik::Token* tok);
		void WalkAst(sik::SIKAst* nodeParent, std::vector<sik::SIKInstruct>* Instructions);
		void WalkAst(sik::SIKAst* nodeParent, sik::SIKAst* nodeChild, std::vector<sik::SIKInstruct>* Instructions);

		//Generate codes:
		void genForKeywords(sik::SIKAst* nodeParent, sik::SIKAst* nodeChild, std::vector<sik::SIKInstruct>* Instructions);
		void genForPrimitives(sik::SIKAst* nodeParent, sik::SIKAst* nodeChild, std::vector<sik::SIKInstruct>* Instructions);
		void genForLR(sik::SIKAst* nodeParent, sik::SIKAst* nodeChild, std::vector<sik::SIKInstruct>* Instructions);
		void genForBlockClose(SIKAst* nodeParent, SIKAst* nodeChild, std::vector<sik::SIKInstruct>* Instructions);

		//Printing Trees:
		int maxHeight(sik::SIKAst *p);
		void printBranches(int branchLen, int nodeSpaceLen, int startLen, int nodesInThisLevel, const std::deque<sik::SIKAst*>& nodesQueue, std::ostream& out);
		void printNodes(int branchLen, int nodeSpaceLen, int startLen, int nodesInThisLevel, const std::deque<sik::SIKAst*>& nodesQueue, std::ostream& out);
		void printLeaves(int indentSpace, int level, int nodesInThisLevel, const std::deque<sik::SIKAst*>& nodesQueue, std::ostream& out);
		void printTree(sik::SIKAst *root, int level, int indentSpace, std::ostream& out);
		std::string truncateString(const std::string& str);

		virtual ~SIKParser();

	};

}

#endif /* SIKParser_hpp */