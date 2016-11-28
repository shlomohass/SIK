//
//  SIKParser.cpp
//  SIK
//
//  Created by Shlomo Hassid on 06/11/2016.
//  Copyright © 2016 Shlomo Hassid. All rights reserved.
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

						//Set if node:
						this->SetNodeFromToken(node, tok);

						//Extract condition:
						int parenthesesEnd = TokenSet->getParenthesesFirstAndLast(tok->index);

						//Validate If condition:
						if (parenthesesEnd < 0) {
							switch (parenthesesEnd) {
								case -1:
									throw SIKException("Expected Parentheses after IF keyword.", tok->fromLine);
									break;
								case -2:
								default:
									throw SIKException("Missing closing Parentheses in IF statement.", tok->fromLine);

							}
						}

						//Create Subset:
						sik::SIKTokens condSubSet = TokenSet->getFromeSet(tok->index + 2, parenthesesEnd - 1);

						//Validate condition:
						if (condSubSet.size() < 1) {
							throw SIKException("IF statement condition part must contain a statement. 11", tok->fromLine);
						}

						//Recursively parse condition:
						sik::SIKAst* condNode = this->BuildAst(&condSubSet);
						
						if ((int)condNode->bulk.size() > 0) {
							node->Left = condNode->bulk[0];
							condNode->Parent = node;
						} else {
							throw SIKException("IF statement condition part must contain a statement. 22", tok->fromLine);
						}
						
						//Validate Check for Block:
						if (TokenSet->getAtPointer(parenthesesEnd + 1) == nullptr || TokenSet->getAtPointer(parenthesesEnd + 1)->type != sik::DELI_BRCOPEN) {
							throw SIKException("IF statement condition must have a block body.", tok->fromLine);
						}

						//Add the Block open
						sik::SIKAst* blockNode = new sik::SIKAst();
						blockNode->line = node->line;
						blockNode->Type = sik::SBLOCK;
						blockNode->Value = sik::SIKLang::dicLang_bracesOpen;
						node->Parent = blockNode;
						blockNode->Left = node;

						//Remove The first Definitionand place the node chain:
						if (!TokenSet->replaceRangeWithNode(tok->index, parenthesesEnd + 1, blockNode)) {
							throw SIKException("Error in token extraction. 22", tok->fromLine);
						}

					}
					// Definition
					else if (tok->obj == SIKLang::dicLangKey_variable) {

						//Single Variable Declaration node:
						sik::SIKAst* nodeChild = new sik::SIKAst();
						this->SetNodeFromToken(nodeChild, tok);

						//Get parent:
						sik::Token* tokP = TokenSet->getAtPointer(tok->index + 1);
						if (tokP == nullptr || ( tokP->node == nullptr && tokP->type != NAMING )) {
							throw SIKException("Expected Variable name after Definition.", tokP->fromLine);
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
									this->SetNodeFromToken(nodeDefinintion, tok);
									this->SetNodeFromToken(nodeDefineName, tokMore);

									//Set pointers:
									nodeDefineName->Left = nodeDefinintion;
									nodeDefinintion->Parent = nodeDefineName;

									//Replace tokens:
									if (!TokenSet->replaceRangeWithNode(testNested[i], tokMore->index, nodeDefineName)) {
										throw SIKException("Error in token extraction. 11", tokMore->fromLine);
									}

								} else {
									throw SIKException("Expected Variable name after Definition.", tokMore->fromLine);
								}
								
							}

							//Remove The first Definitionand place the node chain:
							if (!TokenSet->replaceRangeWithNode(tok->index, tokP->index, node)) {
								throw SIKException("Error in token extraction. 22", tokP->fromLine);
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
					this->SetNodeFromToken(node, tok);
					//Remove Both and replace:
					if (!TokenSet->replaceRangeWithNode(tok->index, tok->index, node)) {
						throw SIKException("Expected value before and after " + tok->obj + " delimiter.", tok->fromLine);
					}
					break;
			}
		}
		//Wrap result inside a node block;
		SIKAst* nodeBlock = new SIKAst();
		for (int i = 0; i < TokenSet->getSetPointer()->size(); i++) {
			SIKAst* nodeAppend = TokenSet->getAtPointer(i)->node;
			if (nodeAppend != nullptr) {
				nodeBlock->bulk.push_back(nodeAppend);
			}
		}
		return nodeBlock;
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
	void SIKParser::WalkAst(SIKAst* nodeParent) {
		for (int i = 0; i < (int)nodeParent->bulk.size(); i++) {
			WalkAst(nullptr, nodeParent->bulk[i]);
		}
	}
	void SIKParser::WalkAst(SIKAst* nodeParent, SIKAst* nodeChild) {
		if (nodeChild->Left != nullptr && !nodeChild->Left->Mark) {
			WalkAst(nodeChild, nodeChild->Left);
		}
		if (nodeChild->Right != nullptr && !nodeChild->Right->Mark) {
			WalkAst(nodeChild, nodeChild->Right);
		}
		switch (nodeChild->Type) {
		case sik::KEYWORD:
			this->genForKeywords(nodeParent, nodeChild);
			nodeChild->Mark = true;
			break;
		case sik::SBLOCK:
		case sik::NAMING:
		case sik::NUMBER:
			this->genForPrimitives(nodeParent, nodeChild);
			nodeChild->Mark = true;
			break;
		case sik::DELI_EQUAL:
		case sik::DELI_PLUS:
		case sik::DELI_MINUS:
		case sik::DELI_MULTI:
			this->genForLR(nodeParent, nodeChild);
			nodeChild->Mark = true;
			break;
		}
	}
	void SIKParser::genForKeywords(SIKAst* nodeParent, SIKAst* nodeChild) {
		// IF statement:
		if (nodeChild->Value == SIKLang::dicLangKey_cond_if) {
			std::cout << " OP: IF " << nodeChild->Value << std::endl;
		}
		// Variable definition:
		if (nodeChild->Value == SIKLang::dicLangKey_variable) {
			std::cout << " OP: DEFINE " << nodeChild->Parent->Value << std::endl;
		}

	}
	void SIKParser::genForPrimitives(SIKAst* nodeParent, SIKAst* nodeChild) {
		switch (nodeChild->Type) {
				case sik::SBLOCK:
					std::cout << " OP: SBLOCK " << nodeChild->Value << std::endl;
					break;
				case sik::NAMING:
				case sik::NUMBER:
					std::cout << " OP: PUSH " << nodeChild->Value << std::endl;
					break;
		}
	}
	void SIKParser::genForLR(SIKAst* nodeParent, SIKAst* nodeChild) {
		switch (nodeChild->Type) {
			case DELI_PLUS:
				std::cout << " OP: ADD " << nodeChild->Value << std::endl;
				break;
			case DELI_MINUS:
				std::cout << " OP: SUB " << nodeChild->Value << std::endl;
				break;
			case DELI_INCR:
				break;
			case DELI_DECR:
				break;
			case DELI_MULTI:
				std::cout << " OP: MUL " << nodeChild->Value << std::endl;
				break;
			case DELI_DIVIDE:
				break;
			case DELI_EQUAL:
				std::cout << " OP: ASSIGN " << nodeChild->Value << std::endl;
				break;
			case DELI_EQUALADD:
				break;
			case DELI_EQUALSUB:
				break;
			case DELI_POINT:
				break;
			case DELI_BRKOPEN:
				break;
			case DELI_BRKCLOSE:
				break;
			case DELI_BRCOPEN:
				break;
			case DELI_BRCCLOSE:
				break;
			case DELI_SBRKOPEN:
				break;
			case DELI_SBRKCLOSE:
				break;
			case DELI_POW:
				break;
			case DELI_EXCL:
				break;
			case DELI_GRT:
				break;
			case DELI_LST:
				break;
			case DELI_GRTE:
				break;
			case DELI_LSTE:
				break;
			case DELI_COMMA:
				break;
			case DELI_CTEQUAL:
				break;
			case DELI_CTNEQUAL:
				break;
			case DELI_CEQUAL:
				break;
			case DELI_CNEQUAL:
				break;
			case DELI_CAND:
				break;
			case DELI_COR:
				break;
			case DELI_OBJCALL:
				break;
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