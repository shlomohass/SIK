//
//  SIKParser.cpp
//  SIK
//
//  Created by Shlomo Hassid on 06/11/2016.
//  Copyright � 2016 Shlomo Hassid. All rights reserved.
//

#include "SIKParser.hpp"
#include "SIKLang.hpp"
#include "SIKException.hpp"

#include <ostream>
#include <iomanip>
#include <deque>

namespace sik {

	SIKParser::SIKParser()
	{
		BlockInCheck.reserve(20);
	}

	SIKAst* SIKParser::BuildAst(SIKTokens* TokenSet)
	{
		while (TokenSet->hasUnparse()) {
			SIKAst* node = new SIKAst();
			Token* tok = TokenSet->getHighestPriorityTokenPointer();
			switch (tok->type) {

				case sik::NOPE:
					TokenSet->removeFromeSet(tok->index, tok->index, true);
					break;
				case sik::DELI_COMMA:
					TokenSet->removeFromeSet(tok->index, tok->index, true);
					break;
				case sik::KEYWORD:

					// If statement:
					if (tok->obj == sik::SIKLang::dicLangKey_cond_if) {
						this->BuildAst_KeyIf(node, tok, TokenSet);
						BlockInCheck.push_back(sik::BLOCK_IF); //Mark block
					}
					// Else statement:
					else if (tok->obj == sik::SIKLang::dicLangKey_cond_else) {
						this->BuildAst_KeyElse(node, tok, TokenSet);
						BlockInCheck.push_back(sik::BLOCK_ELSE); //Mark block
					}
					// ElseIf statement:
					else if (tok->obj == sik::SIKLang::dicLangKey_cond_elseif) {
						this->BuildAst_KeyElseIf(node, tok, TokenSet);
						BlockInCheck.push_back(sik::BLOCK_ELSEIF); //Mark block
					}
					// Definition
					else if (tok->obj == SIKLang::dicLangKey_variable) {
						this->BuildAst_KeyDefine(node, tok, TokenSet);
					}

					break;
				case sik::DELI_BRCCLOSE:
					this->BuildAst_BlockClose(node, tok, TokenSet);
					break;
				case sik::DELI_EQUAL:
					this->BuildAst_Assigning(node, tok, TokenSet);
					break;
				case sik::DELI_PLUS:
				case sik::DELI_MINUS:
				case sik::DELI_MULTI:
				case sik::DELI_DIVIDE:
					this->BuildAst_MathLR(node, tok, TokenSet);
					break;
				case sik::NAMING:
					this->BuildAst_Naming(node, tok, TokenSet);
					break;
			}
		}
		//Wrap result inside a node block;
		sik::SIKAst* nodeBlock = new sik::SIKAst();
		for (int i = 0; i < (int)TokenSet->getSetPointer()->size(); i++) {
			sik::SIKAst* nodeAppend = TokenSet->getAtPointer(i)->node;
			if (nodeAppend != nullptr) {
				nodeBlock->bulk.push_back(nodeAppend);
			}
		}
		return nodeBlock;
	}
	int SIKParser::BuildAst_Naming(sik::SIKAst* node, sik::Token* token ,sik::SIKTokens* TokenSet) {
		this->SetNodeFromToken(node, token);
		//Remove Both and replace:
		if (!TokenSet->replaceRangeWithNode(token->index, token->index, node)) {
			throw SIKException("Expected value before and after " + token->obj + " delimiter.", token->fromLine);
		}
		return 1;
	}
	int SIKParser::BuildAst_MathLR(sik::SIKAst* node, sik::Token* token, sik::SIKTokens* TokenSet) {
		this->SetNodeFromToken(node, token);
		Token* tokL = TokenSet->getAtPointer(token->index - 1);
		Token* tokR = TokenSet->getAtPointer(token->index + 1);
		if (tokL == nullptr || tokR == nullptr) {
			throw SIKException("Expected value before and after " + token->obj + " delimiter.", token->fromLine);
		} else {
			SIKAst* nodeLeft = new SIKAst();
			SIKAst* nodeRight = new SIKAst();
			this->SetNodeFromToken(nodeLeft, tokL);
			this->SetNodeFromToken(nodeRight, tokR);
			node->Left = nodeLeft;
			node->Right = nodeRight;

			//Remove Both and replace:
			if (!TokenSet->replaceRangeWithNode(tokL->index, tokR->index, node)) {
				throw SIKException("Expected value before and after " + token->obj + " delimiter.", token->fromLine);
			}
		}
		return 1;
	}
	int SIKParser::BuildAst_Assigning(sik::SIKAst* node, sik::Token* token, sik::SIKTokens* TokenSet) {
		this->SetNodeFromToken(node, token);
		Token* tokL = TokenSet->getAtPointer(token->index - 1);
		Token* tokR = TokenSet->getAtPointer(token->index + 1);
		if (tokL == nullptr || tokR == nullptr) {
			throw SIKException("Expected Variable Before equal sign and value after.", token->fromLine);
		} else {
			SIKAst* nodeLeft = new SIKAst();
			SIKAst* nodeRight = new SIKAst();
			this->SetNodeFromToken(nodeLeft, tokL);
			this->SetNodeFromToken(nodeRight, tokR);
			node->Left = nodeLeft;
			node->Right = nodeRight;

			//Remove Both and replace:
			if (!TokenSet->replaceRangeWithNode(tokL->index, tokR->index, node)) {
				throw SIKException("Expected Variable Before equal sign and value after.", token->fromLine);
			}
		}
		return 1;
	}
	int SIKParser::BuildAst_KeyDefine(sik::SIKAst* node, sik::Token* token, sik::SIKTokens* TokenSet) {
		//Single Variable Declaration node:
		sik::SIKAst* nodeChild = new sik::SIKAst();
		this->SetNodeFromToken(nodeChild, token);

		//Get parent:
		sik::Token* tokP = TokenSet->getAtPointer(token->index + 1);
		if (tokP == nullptr || (tokP->node == nullptr && tokP->type != NAMING)) {
			throw sik::SIKException("Expected Variable name after Definition.", tokP->fromLine);
		} else {

			//Set the parent:
			this->SetNodeFromToken(node, tokP);
			node->Left = nodeChild;
			nodeChild->Parent = node; // Set parent

			//Add name to bulk define and check for nested:
			std::vector<int> testNested = TokenSet->hasNestedCommas(tokP->index);
			for (int i = (int)testNested.size() - 1; i >= 0; i--) {
				sik::Token* tokMore = TokenSet->getAtPointer(testNested[i] + 1);
				if (tokMore != nullptr && tokP->type == NAMING) {
					//Create definition chain:
					sik::SIKAst* nodeDefinintion = new sik::SIKAst();
					sik::SIKAst* nodeDefineName = new sik::SIKAst();
					this->SetNodeFromToken(nodeDefinintion, token);
					this->SetNodeFromToken(nodeDefineName, tokMore);
					//Set pointers:
					nodeDefineName->Left = nodeDefinintion;
					nodeDefinintion->Parent = nodeDefineName;
					//Replace tokens:
					if (!TokenSet->replaceRangeWithNode(testNested[i], tokMore->index, nodeDefineName)) {
						throw sik::SIKException("Error in token extraction. 11", tokMore->fromLine);
					}
				} else {
					throw sik::SIKException("Expected Variable name after Definition.", tokMore->fromLine);
				}
			}

			//Remove The first Definitionand place the node chain:
			if (!TokenSet->replaceRangeWithNode(token->index, tokP->index, node)) {
				throw sik::SIKException("Error in token extraction. 22", tokP->fromLine);
			}
		}
		return 1;
	}
	int SIKParser::BuildAst_KeyIf(sik::SIKAst* node, sik::Token* token, sik::SIKTokens* TokenSet) {
		//Set if node:
		this->SetNodeFromToken(node, token);
		//Extract condition:
		int parenthesesEnd = TokenSet->getParenthesesFirstAndLast(token->index);
		//Validate If condition:
		if (parenthesesEnd < 0) {
			switch (parenthesesEnd) {
			case -1:
				throw sik::SIKException("Expected Parentheses after IF keyword.", token->fromLine);
				break;
			case -2:
			default:
				throw sik::SIKException("Missing closing Parentheses in IF statement.", token->fromLine);

			}
		}
		//Create Subset:
		sik::SIKTokens condSubSet = TokenSet->getFromeSet(token->index + 2, parenthesesEnd - 1);
		//Validate condition:
		if (condSubSet.size() < 1) {
			throw sik::SIKException("IF statement condition part must contain a statement. 11", token->fromLine);
		}
		//Recursively parse condition:
		sik::SIKAst* condNode = this->BuildAst(&condSubSet);
		if ((int)condNode->bulk.size() > 0) {
			node->Left = condNode->bulk[0];
			condNode->Parent = node;
		} else {
			throw sik::SIKException("IF statement condition part must contain a statement. 22", token->fromLine);
		}
		//Validate Check for Block:
		if (TokenSet->getAtPointer(parenthesesEnd + 1) == nullptr || TokenSet->getAtPointer(parenthesesEnd + 1)->type != sik::DELI_BRCOPEN) {
			throw sik::SIKException("IF statement condition must have a block body.", token->fromLine);
		}
		//Add the Block open
		sik::SIKAst* blockNode = new sik::SIKAst();
		blockNode->line = node->line;
		blockNode->Type = sik::SBLOCK;
		blockNode->Block = sik::BLOCK_IF;
		blockNode->Value = sik::SIKLang::dicLang_bracesOpen;
		node->Parent = blockNode;
		blockNode->Left = node;
		//Remove The first Definitionand place the node chain:
		if (!TokenSet->replaceRangeWithNode(token->index, parenthesesEnd + 1, blockNode)) {
			throw sik::SIKException("Error in token extraction. 22", token->fromLine);
		}
		return 1;
	}
	int SIKParser::BuildAst_KeyElseIf(sik::SIKAst* node, sik::Token* token, sik::SIKTokens* TokenSet) {
		//Set elseif node:
		this->SetNodeFromToken(node, token);
		//Extract condition:
		int parenthesesEnd = TokenSet->getParenthesesFirstAndLast(token->index);
		//Validate elseIf condition:
		if (parenthesesEnd < 0) {
			switch (parenthesesEnd) {
			case -1:
				throw sik::SIKException("Expected Parentheses after ELSEIF keyword.", token->fromLine);
				break;
			case -2:
			default:
				throw sik::SIKException("Missing closing Parentheses in ELSEIF statement.", token->fromLine);
			}
		}
		//Create Subset:
		sik::SIKTokens condSubSet = TokenSet->getFromeSet(token->index + 2, parenthesesEnd - 1);
		//Validate condition:
		if (condSubSet.size() < 1) {
			throw sik::SIKException("ELSEIF statement condition part must contain a statement. 11", token->fromLine);
		}
		//Recursively parse condition:
		sik::SIKAst* condNode = this->BuildAst(&condSubSet);
		if ((int)condNode->bulk.size() > 0) {
			node->Left = condNode->bulk[0];
			condNode->Parent = node;
		} else {
			throw sik::SIKException("ELSEIF statement condition part must contain a statement. 22", token->fromLine);
		}
		//Validate Check for Block:
		if (TokenSet->getAtPointer(parenthesesEnd + 1) == nullptr || TokenSet->getAtPointer(parenthesesEnd + 1)->type != sik::DELI_BRCOPEN) {
			throw sik::SIKException("ELSEIF statement condition must have a block body.", token->fromLine);
		}
		//Add the Block open
		sik::SIKAst* blockNode = new sik::SIKAst();
		blockNode->line = node->line;
		blockNode->Type = sik::SBLOCK;
		blockNode->Block = sik::BLOCK_ELSEIF;
		blockNode->Value = sik::SIKLang::dicLang_bracesOpen;
		node->Parent = blockNode;
		blockNode->Left = node;
		//Remove The first Definitionand place the node chain:
		if (!TokenSet->replaceRangeWithNode(token->index, parenthesesEnd + 1, blockNode)) {
			throw sik::SIKException("Error in token extraction. 44", token->fromLine);
		}
		return 1;
	}
	int SIKParser::BuildAst_KeyElse(sik::SIKAst* node, sik::Token* token, sik::SIKTokens* TokenSet) {
		//Set else node:
		this->SetNodeFromToken(node, token);

		//Validate Check for Block:
		if (TokenSet->getAtPointer(token->index + 1) == nullptr || TokenSet->getAtPointer(token->index + 1)->type != sik::DELI_BRCOPEN) {
			throw sik::SIKException("ELSE statement condition must have a block body.", token->fromLine);
		}
		//Add the Block open
		sik::SIKAst* blockNode = new sik::SIKAst();
		blockNode->line = node->line;
		blockNode->Type = sik::SBLOCK;
		blockNode->Block = sik::BLOCK_ELSE;
		blockNode->Value = sik::SIKLang::dicLang_bracesOpen;
		node->Parent = blockNode;
		blockNode->Left = node;
		//Remove The first Definitionand place the node chain:
		if (!TokenSet->replaceRangeWithNode(token->index, token->index + 1, blockNode)) {
			throw sik::SIKException("Error in token extraction. 33", token->fromLine);
		}
		return 1;
	}
	int SIKParser::BuildAst_BlockClose(sik::SIKAst* node, sik::Token* token, sik::SIKTokens* TokenSet) {
		//Check if we had any openning:
		if ((int)this->BlockInCheck.size() <= 0) {
			throw SIKException("Unexpected " + token->obj + " delimiter - no opening found.", token->fromLine);
		}
		//Set:
		this->SetNodeFromToken(node, token);
		node->Block = this->BlockInCheck.back();
		this->BlockInCheck.pop_back();
		//Remove Both and replace:
		if (!TokenSet->replaceRangeWithNode(token->index, token->index, node)) {
			throw SIKException("Expected value before and after " + token->obj + " delimiter.", token->fromLine);
		}
		return 1;
	}

	void SIKParser::SetNodeFromToken(SIKAst* node, Token* tok)
	{
		if (tok->type == sik::NODE) {
			//Deep copy node:
			node->Type = tok->node->Type;
			node->Priority = tok->node->Priority;
			node->line = tok->node->line;
			node->Value = tok->node->Value;
			node->Left = tok->node->Left;
			node->Right = tok->node->Right;
		} else {
			//Create new copy node:
			node->Type = tok->type;
			node->Priority = -1;
			node->line = tok->fromLine;
			node->Value = tok->obj;
		}
	}
	/* Walk the Ast given
	 * @param SIKAst* node -> the node
	 *
	*/
	void SIKParser::WalkAst(sik::SIKAst* nodeParent, std::vector<sik::SIKInstruct>* Instructions) {
		for (int i = 0; i < (int)nodeParent->bulk.size(); i++) {
			WalkAst(nullptr, nodeParent->bulk[i], Instructions);
		}
	}
	void SIKParser::WalkAst(sik::SIKAst* nodeParent, sik::SIKAst* nodeChild, std::vector<sik::SIKInstruct>* Instructions) {
		if (nodeChild->Left != nullptr && !nodeChild->Left->Mark) {
			WalkAst(nodeChild, nodeChild->Left, Instructions);
		}
		if (nodeChild->Right != nullptr && !nodeChild->Right->Mark) {
			WalkAst(nodeChild, nodeChild->Right, Instructions);
		}
		switch (nodeChild->Type) {
		case sik::KEYWORD:
			this->genForKeywords(nodeParent, nodeChild, Instructions);
			nodeChild->Mark = true;
			break;
		case sik::SBLOCK:
		case sik::NAMING:
		case sik::NUMBER:
		case sik::STRING:
		case sik::BOOLEAN:
		case sik::NULLTYPE:
			this->genForPrimitives(nodeParent, nodeChild, Instructions);
			nodeChild->Mark = true;
			break;
		case sik::DELI_EQUAL:
		case sik::DELI_PLUS:
		case sik::DELI_MINUS:
		case sik::DELI_MULTI:
			this->genForLR(nodeParent, nodeChild, Instructions);
			nodeChild->Mark = true;
			break;
		case sik::DELI_BRCCLOSE:
			this->genForBlockClose(nodeParent, nodeChild, Instructions);
			break;
		}
	}
	void SIKParser::genForKeywords(SIKAst* nodeParent, SIKAst* nodeChild, std::vector<sik::SIKInstruct>* Instructions) {
		// IF statement:
		if (nodeChild->Value == SIKLang::dicLangKey_cond_if) {
			Instructions->push_back(sik::SIKInstruct(nodeChild, sik::INS_IF));
			return;
		}
		// ELSE statement:
		if (nodeChild->Value == SIKLang::dicLangKey_cond_else) {
			Instructions->push_back(sik::SIKInstruct(nodeChild, sik::INS_ELSE));
			return;
		}
		// ELSEIF statement:
		if (nodeChild->Value == SIKLang::dicLangKey_cond_elseif) {
			Instructions->push_back(sik::SIKInstruct(nodeChild, sik::INS_ELSEIF));
			return;
		}
		// Variable definition:
		if (nodeChild->Value == SIKLang::dicLangKey_variable) {
			Instructions->push_back(sik::SIKInstruct(nodeChild, sik::INS_DEFINE));
			return;
		}
	}
	void SIKParser::genForPrimitives(SIKAst* nodeParent, SIKAst* nodeChild, std::vector<sik::SIKInstruct>* Instructions) {
		switch (nodeChild->Type) {
			case sik::SBLOCK:
				Instructions->push_back(sik::SIKInstruct(nodeChild));
				break;
			case sik::NAMING:
			case sik::NUMBER:
			case sik::STRING:
			case sik::BOOLEAN:
			case sik::NULLTYPE:
				Instructions->push_back(sik::SIKInstruct(nodeChild));
				break;
		}
	}
	void SIKParser::genForLR(SIKAst* nodeParent, SIKAst* nodeChild, std::vector<sik::SIKInstruct>* Instructions) {
		Instructions->push_back(sik::SIKInstruct(nodeChild));
	}
	void SIKParser::genForBlockClose(SIKAst* nodeParent, SIKAst* nodeChild, std::vector<sik::SIKInstruct>* Instructions) {
		Instructions->push_back(sik::SIKInstruct(nodeChild));
	}
	// Find the maximum height of the binary tree
	int SIKParser::maxHeight(SIKAst *p) {
		if (!p) return 0;
		int leftHeight = maxHeight(p->Left);
		int rightHeight = maxHeight(p->Right);
		return (leftHeight > rightHeight) ? leftHeight + 1 : rightHeight + 1;
	}

	// Print the arm branches (eg, /    \ ) on a line
	void SIKParser::printBranches(int branchLen, int nodeSpaceLen, int startLen, int nodesInThisLevel, const std::deque<SIKAst*>& nodesQueue, std::ostream& out) {
		std::deque<SIKAst*>::const_iterator iter = nodesQueue.begin();
		for (int i = 0; i < nodesInThisLevel / 2; i++) {
			out << ((i == 0) ? std::setw(startLen - 1) : std::setw(nodeSpaceLen - 2)) << "" << ((*iter++) ? "/" : " ");
			out << std::setw(2 * branchLen + 2) << "" << ((*iter++) ? "\\" : " ");
		}
		out << std::endl;
	}

	// Print the branches and node (eg, ___10___ )
	void SIKParser::printNodes(int branchLen, int nodeSpaceLen, int startLen, int nodesInThisLevel, const std::deque<SIKAst*>& nodesQueue, std::ostream& out) {
		std::deque<SIKAst*>::const_iterator iter = nodesQueue.begin();
		for (int i = 0; i < nodesInThisLevel; i++, iter++) {
			out << ((i == 0) ? std::setw(startLen) : std::setw(nodeSpaceLen)) << "" << ((*iter && (*iter)->Left) ? std::setfill('_') : std::setfill(' '));
			out << std::setw(branchLen + 2) << ((*iter) ? this->truncateString((*iter)->Value) : "");
			out << ((*iter && (*iter)->Right) ? std::setfill('_') : std::setfill(' ')) << std::setw(branchLen) << "" << std::setfill(' ');
		}
		out << std::endl;
	}

	// Print the leaves only (just for the bottom row)
	void SIKParser::printLeaves(int indentSpace, int level, int nodesInThisLevel, const std::deque<SIKAst*>& nodesQueue, std::ostream& out) {
		std::deque<SIKAst*>::const_iterator iter = nodesQueue.begin();
		for (int i = 0; i < nodesInThisLevel; i++, iter++) {
			out << ((i == 0) ? std::setw(indentSpace + 2) : std::setw(2 * level + 2)) << ((*iter) ? this->truncateString((*iter)->Value) : "");
		}
		out << std::endl;
	}

	// Pretty formatting of a binary tree to the output stream
	// @ param
	// level  Control how wide you want the tree to sparse (eg, level 1 has the minimum space between nodes, while level 2 has a larger space between nodes)
	// indentSpace  Change this to add some indent space to the left (eg, indentSpace of 0 means the lowest level of the left node will stick to the left margin)
	void SIKParser::printTree(SIKAst *root, int level, int indentSpace, std::ostream& out) {
		int h = maxHeight(root);
		int nodesInThisLevel = 1;

		int branchLen = 2 * ((int)pow(2.0, h) - 1) - (3 - level)*(int)pow(2.0, h - 1);  // eq of the length of branch for each node of each level
		int nodeSpaceLen = 2 + (level + 1)*(int)pow(2.0, h);  // distance between left neighbor node's right arm and right neighbor node's left arm
		int startLen = branchLen + (3 - level) + indentSpace;  // starting space to the first node to print of each level (for the left most node of each level only)

		std::deque<SIKAst*> nodesQueue;
		nodesQueue.push_back(root);

		std::cout << "GENERATED AST:" << std::endl;

		for (int r = 1; r < h; r++) {
			printBranches(branchLen, nodeSpaceLen, startLen, nodesInThisLevel, nodesQueue, out);
			branchLen = branchLen / 2 - 1;
			nodeSpaceLen = nodeSpaceLen / 2 + 1;
			startLen = branchLen + (3 - level) + indentSpace;
			printNodes(branchLen, nodeSpaceLen, startLen, nodesInThisLevel, nodesQueue, out);

			for (int i = 0; i < nodesInThisLevel; i++) {
				SIKAst *currNode = nodesQueue.front();
				nodesQueue.pop_front();
				if (currNode) {
					nodesQueue.push_back(currNode->Left);
					nodesQueue.push_back(currNode->Right);
				}
				else {
					nodesQueue.push_back(NULL);
					nodesQueue.push_back(NULL);
				}
			}
			nodesInThisLevel *= 2;
		}
		printBranches(branchLen, nodeSpaceLen, startLen, nodesInThisLevel, nodesQueue, out);
		printLeaves(indentSpace, level, nodesInThisLevel, nodesQueue, out);
	}
	std::string SIKParser::truncateString(const std::string& str) {
		if (str.length() > 3) {
			return std::string(str.begin(), str.begin() + 2) + ">";
		}
		return str;
	}
	SIKParser::~SIKParser()
	{

	}

}