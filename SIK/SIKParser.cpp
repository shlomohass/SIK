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

	}

	SIKAst* SIKParser::BuildAst(SIKTokens* TokenSet)
	{
		while (TokenSet->size() > 1) {
			SIKAst* node = new SIKAst();
			Token* tok = TokenSet->getHighestPriorityTokenPointer();
			switch (tok->type) {

				case sik::NOPE:
					TokenSet->removeFromeSet(tok->index, tok->index, true);
					break;
				case sik::KEYWORD:
					if (tok->obj == SIKLang::dicLangKey_variable) {

						//Single Variable Declaration node:
						SIKAst* nodeChild = new SIKAst();
						this->SetNodeFromToken(nodeChild, tok);

						//Get parent:
						Token* tokP = TokenSet->getAtPointer(tok->index + 1);
						if (tokP == nullptr || ( tokP->node == nullptr && tokP->type != NAMING )) {
							throw SIKException("Expected Variable name after Definition.", tokP->fromLine);
						} else {
							//Set the parent:
							this->SetNodeFromToken(node, tokP);
							node->Left = nodeChild;

							//Remove Both and replace:
							if (!TokenSet->replaceRangeWithNode(tok->index, tokP->index, node)) {
								throw SIKException("Expected Variable name after Definition.", tokP->fromLine);
							}
						}
					}
					break;
				case sik::DELI_EQUAL: {
						this->SetNodeFromToken(node, tok);
						Token* tokL = TokenSet->getAtPointer(tok->index - 1);
						Token* tokR = TokenSet->getAtPointer(tok->index + 1);
						if (tokL == nullptr || tokR == nullptr) {
							throw SIKException("Expected Variable Before equal sign and value after.", tok->fromLine);
						} else {

							SIKAst* nodeLeft = new SIKAst();
							SIKAst* nodeRight = new SIKAst();
							this->SetNodeFromToken(nodeLeft, tokL);
							this->SetNodeFromToken(nodeRight, tokR);
							node->Left = nodeLeft;
							node->Right = nodeRight;

							//Remove Both and replace:
							if (!TokenSet->replaceRangeWithNode(tokL->index, tokR->index, node)) {
								throw SIKException("Expected Variable Before equal sign and value after.", tok->fromLine);
							}

						}
					}
					break;
				case sik::DELI_PLUS:
				case sik::DELI_MINUS:
				case sik::DELI_MULTI:
				case sik::DELI_DIVIDE: {
						this->SetNodeFromToken(node, tok);
						Token* tokL = TokenSet->getAtPointer(tok->index - 1);
						Token* tokR = TokenSet->getAtPointer(tok->index + 1);
						if (tokL == nullptr || tokR == nullptr) {
							throw SIKException("Expected value before and after " + tok->obj + " delimiter.", tok->fromLine);
						} else {
							SIKAst* nodeLeft = new SIKAst();
							SIKAst* nodeRight = new SIKAst();
							this->SetNodeFromToken(nodeLeft, tokL);
							this->SetNodeFromToken(nodeRight, tokR);
							node->Left = nodeLeft;
							node->Right = nodeRight;

							//Remove Both and replace:
							if (!TokenSet->replaceRangeWithNode(tokL->index, tokR->index, node)) {
								throw SIKException("Expected value before and after " + tok->obj + " delimiter.", tok->fromLine);
							}

						}
					}
					break;
				case sik::NAMING:
					break;

			}
		}
		return TokenSet->getAtPointer(0)->node;
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