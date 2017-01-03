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
		int jumperCounter;
		std::vector<sik::BlocksIn> BlockChunksContainer;
		//From Script containers:
		std::vector<sik::SIKInstruct>* Instructions;
		std::vector<std::vector<sik::SIKInstruct>>* ObjectDefinitions;
		std::vector<std::vector<sik::SIKInstruct>>* FunctionInstructions;

	public:

		SIKParser();
		SIKParser(std::vector<sik::SIKInstruct>* _Instructions, std::vector<std::vector<sik::SIKInstruct>>* _ObjectDefinitions, std::vector<std::vector<sik::SIKInstruct>>* _FunctionInstructions);
		
		//Operations AST:
		sik::SIKAst* BuildAst(sik::SIKTokens* TokenSet);

		int BuildAst_Naming(sik::SIKAst* node, sik::Token* token, sik::SIKTokens* TokenSet);
		int BuildAst_MathLR(sik::SIKAst* node, sik::Token* token, sik::SIKTokens* TokenSet);
		int BuildAst_CondEquality(sik::SIKAst* node, sik::Token* token, sik::SIKTokens* TokenSet);
		int BuildAst_CondCompare(sik::SIKAst* node, sik::Token* token, sik::SIKTokens* TokenSet);
		int BuildAst_CondAND(sik::SIKAst* node, sik::Token* token, sik::SIKTokens* TokenSet);
		int BuildAst_CondOR(sik::SIKAst* node, sik::Token* token, sik::SIKTokens* TokenSet);
		int BuildAst_Assigning(sik::SIKAst* node, sik::Token* token, sik::SIKTokens* TokenSet);
		int BuildAst_KeyDefine(sik::SIKAst* node, sik::Token* token, sik::SIKTokens* TokenSet);
		int BuildAst_KeyIf(sik::SIKAst* node, sik::Token* token, sik::SIKTokens* TokenSet, sik::BlocksIn type);
		int BuildAst_KeyElse(sik::SIKAst* node, sik::Token* token, sik::SIKTokens* TokenSet);
		int BuildAst_BlockOpen(sik::SIKAst* node, sik::Token* token, sik::SIKTokens* TokenSet);
		int BuildAst_BlockClose(sik::SIKAst* node, sik::Token* token, sik::SIKTokens* TokenSet);
		int BuildAst_BracketOpen(sik::SIKAst* node, sik::Token* token, sik::SIKTokens* TokenSet);
		int BuildAst_KeyForLoop(sik::SIKAst* node, sik::Token* token, sik::SIKTokens* TokenSet);
		int BuildAst_KeyWhileLoop(sik::SIKAst* node, sik::Token* token, sik::SIKTokens* TokenSet);
		int BuildAst_KeyEachLoop(sik::SIKAst* node, sik::Token* token, sik::SIKTokens* TokenSet);
		int BuildAst_OpSingleSide(sik::SIKAst* node, sik::Token* token, sik::SIKTokens* TokenSet);
		int BuildAst_KeyBreakLoop(sik::SIKAst* node, sik::Token* token, sik::SIKTokens* TokenSet);
		int BuildAst_KeyBreakCond(sik::SIKAst* node, sik::Token* token, sik::SIKTokens* TokenSet);
		int BuildAst_KeyContinueLoop(sik::SIKAst* node, sik::Token* token, sik::SIKTokens* TokenSet);
		int BuildAst_KeyPrint(sik::SIKAst* node, sik::Token* token, sik::SIKTokens* TokenSet);
		//Operations:
		void SetNodeFromToken(sik::SIKAst* node, sik::Token* tok);
		void applyNodeToMostLeft(sik::SIKAst* toApplyNode, sik::SIKAst* applyToNodeTree);
		void WalkAst(sik::SIKAst* nodeParent);
		void WalkAst(sik::SIKAst* nodeParent, sik::SIKAst* nodeChild);

		//Generate codes:
		void AddToInstructions(const sik::SIKInstruct& instruct);
		void AddToInstructions(const sik::SIKInstruct& instruct, SIKAst* nodeParent);
		void pushToObjectsInstructions(const sik::SIKInstruct& instruct);
		void genForKeywords(sik::SIKAst* nodeParent, sik::SIKAst* nodeChild);
		void genForPrimitives(sik::SIKAst* nodeParent, sik::SIKAst* nodeChild);
		void genForLR(sik::SIKAst* nodeParent, sik::SIKAst* nodeChild);
		void genForBlockClose(SIKAst* nodeParent, SIKAst* nodeChild);
		void genForOpSingleSide(SIKAst* nodeParent, SIKAst* nodeChild);

		//Printing Trees:
		int maxHeight(sik::SIKAst *p);
		void printBranches(int branchLen, int nodeSpaceLen, int startLen, int nodesInThisLevel, const std::deque<sik::SIKAst*>& nodesQueue, std::ostream& out);
		void printNodes(int level, int indentSpace, int branchLen, int nodeSpaceLen, int startLen, int nodesInThisLevel, const std::deque<sik::SIKAst*>& nodesQueue, std::ostream& out, std::vector<SIKAst*>* later);
		void printLeaves(int indentSpace, int level, int nodesInThisLevel, const std::deque<sik::SIKAst*>& nodesQueue, std::ostream& out, std::vector<SIKAst*>* later);
		void printTree(sik::SIKAst *root, int level, int indentSpace, std::ostream& out);
		std::string truncateString(const std::string& str);

		virtual ~SIKParser();

	};

}

#endif /* SIKParser_hpp */