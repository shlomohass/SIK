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
		this->BlockInCheck.reserve(20);
		this->jumperCounter = 0;
		this->Instructions = nullptr;
		this->ObjectDefinitions = nullptr;
		this->FunctionInstructions = nullptr;
		
	}

	SIKParser::SIKParser(std::vector<sik::SIKInstruct>* _Instructions, std::vector<std::vector<sik::SIKInstruct>>* _ObjectDefinitions, std::vector<std::vector<sik::SIKInstruct>>* _FunctionInstructions)
	{
		this->BlockInCheck.reserve(20);
		this->jumperCounter = 0;
		this->Instructions = _Instructions;
		this->ObjectDefinitions = _ObjectDefinitions;
		this->FunctionInstructions = _FunctionInstructions;
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
				case sik::DELI_OPEND:
					TokenSet->removeFromeSet(tok->index, tok->index, true);
					break;
				case sik::DELI_BRKOPEN:
					this->BuildAst_BracketOpen(node, tok, TokenSet);
					break;
				case sik::DELI_SBRKOPEN:
					this->BuildAst_SquareBracketOpen(node, tok, TokenSet);
					break;
				case sik::KEYWORD:
					// If statement:
					if (tok->obj == sik::SIKLang::dicLangKey_cond_if) {
						this->BuildAst_KeyIf(node, tok, TokenSet, sik::BLOCK_IF);
						this->BlockInCheck.push_back(sik::BLOCK_IF); //Mark block
					}
					// ElseIf statement:
					else if (tok->obj == sik::SIKLang::dicLangKey_cond_elseif) {
						this->BuildAst_KeyIf(node, tok, TokenSet, sik::BLOCK_ELSEIF);
						this->BlockInCheck.push_back(sik::BLOCK_ELSEIF); //Mark block
					}
					// Else statement:
					else if (tok->obj == sik::SIKLang::dicLangKey_cond_else) {
						this->BuildAst_KeyElse(node, tok, TokenSet);
						this->BlockInCheck.push_back(sik::BLOCK_ELSE); //Mark block
					}
					// Definition
					else if (tok->obj == SIKLang::dicLangKey_variable) {
						this->BuildAst_KeyDefine(node, tok, TokenSet);
					}
					//For loop:
					else if (tok->obj == SIKLang::dicLangKey_loop_for) {
						this->BuildAst_KeyForLoop(node, tok, TokenSet);
						this->BlockInCheck.push_back(sik::BLOCK_FOR); //Mark block
					}
					//While loop:
					else if (tok->obj == SIKLang::dicLangKey_loop_while) {
						this->BuildAst_KeyWhileLoop(node, tok, TokenSet);
						this->BlockInCheck.push_back(sik::BLOCK_WHILE); //Mark block
					}
					//Each loop:
					else if (tok->obj == SIKLang::dicLangKey_loop_each) {
						this->BuildAst_KeyEachLoop(node, tok, TokenSet);
						this->BlockInCheck.push_back(sik::BLOCK_EACH); //Mark block
					}
					//Break loop:
					else if (tok->obj == SIKLang::dicLangKey_loop_break) {
						this->BuildAst_KeyBreakLoop(node, tok, TokenSet);
					}
					//Break Condition:
					else if (tok->obj == SIKLang::dicLangKey_cond_break) {
						this->BuildAst_KeyBreakCond(node, tok, TokenSet);
					}
					//Continue loop:
					else if (tok->obj == SIKLang::dicLangKey_loop_continue) {
						this->BuildAst_KeyContinueLoop(node, tok, TokenSet);
					}
					//Print:
					else if (tok->obj == SIKLang::dicLangKey_print) {
						this->BuildAst_KeyPrint(node, tok, TokenSet);
					}
					break;
				case sik::DELI_BRCOPEN:
					this->BuildAst_BlockOpen(node, tok, TokenSet);
					break;
				case sik::DELI_BRCCLOSE:
					this->BuildAst_BlockClose(node, tok, TokenSet);
					break;
				case sik::DELI_EQUAL:
				case sik::DELI_EQUALADD:
				case sik::DELI_EQUALSUB:
				case sik::DELI_CHILDSET:
					this->BuildAst_Assigning(node, tok, TokenSet);
					break;
				case sik::DELI_PLUS:
				case sik::DELI_MINUS:
				case sik::DELI_MULTI:
				case sik::DELI_DIVIDE:
					this->BuildAst_MathLR(node, tok, TokenSet);
					break;
				case sik::DELI_DECR:
				case sik::DELI_INCR:
					this->BuildAst_OpSingleSide(node, tok, TokenSet);
					break;
				case sik::DELI_CEQUAL:
				case sik::DELI_CNEQUAL:
				case sik::DELI_CTEQUAL:
				case sik::DELI_CTNEQUAL:
					this->BuildAst_CondEquality(node, tok, TokenSet);
					break;
				case sik::DELI_GRT:
				case sik::DELI_LST:
				case sik::DELI_GRTE:
				case sik::DELI_LSTE:
					this->BuildAst_CondCompare(node, tok, TokenSet);
					break;
				case sik::DELI_CAND:
					this->BuildAst_CondAND(node, tok, TokenSet);
					break;
				case sik::DELI_COR:
					this->BuildAst_CondOR(node, tok, TokenSet);
					break;
				case sik::NAMING:
				case sik::STRING:
				case sik::NUMBER:
				case sik::BOOLEAN:
				case sik::NULLTYPE:
					this->BuildAst_Naming(node, tok, TokenSet);
					break;
			}
		}
		//Wrap result inside a node block;
		sik::SIKAst* nodeBlock = new sik::SIKAst();
		for (int i = 0; i < TokenSet->size(); i++) {
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
	int SIKParser::BuildAst_CondEquality(sik::SIKAst* node, sik::Token* token, sik::SIKTokens* TokenSet) {
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
	int SIKParser::BuildAst_CondCompare(sik::SIKAst* node, sik::Token* token, sik::SIKTokens* TokenSet) {
		this->SetNodeFromToken(node, token);
		Token* tokL = TokenSet->getAtPointer(token->index - 1);
		Token* tokR = TokenSet->getAtPointer(token->index + 1);
		if (tokL == nullptr || tokR == nullptr) {
			throw SIKException("Expected value before and after " + token->obj + " delimiter.", token->fromLine);
		}
		else {
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
	int SIKParser::BuildAst_CondAND(sik::SIKAst* node, sik::Token* token, sik::SIKTokens* TokenSet) {
		this->SetNodeFromToken(node, token);
		node->MyInternalNumber = this->jumperCounter++;
		Token* tokL = TokenSet->getAtPointer(token->index - 1);
		Token* tokR = TokenSet->getAtPointer(token->index + 1);
		if (tokL == nullptr || tokR == nullptr) {
			throw SIKException("Expected value before and after " + token->obj + " delimiter.", token->fromLine);
		} else {
			SIKAst* nodeLeft = new SIKAst();
			SIKAst* nodeRight = new SIKAst();
			this->SetNodeFromToken(nodeLeft, tokL);
			this->SetNodeFromToken(nodeRight, tokR);
			nodeLeft->InternalJumper = node->MyInternalNumber;
			node->Left = nodeLeft;
			node->Right = nodeRight;

			//Remove Both and replace:
			if (!TokenSet->replaceRangeWithNode(tokL->index, tokR->index, node)) {
				throw SIKException("Expected value before and after " + token->obj + " delimiter.", token->fromLine);
			}
		}
		return 1;
	}
	int SIKParser::BuildAst_CondOR(sik::SIKAst* node, sik::Token* token, sik::SIKTokens* TokenSet) {
		this->SetNodeFromToken(node, token);
		node->MyInternalNumber = this->jumperCounter++;
		Token* tokL = TokenSet->getAtPointer(token->index - 1);
		Token* tokR = TokenSet->getAtPointer(token->index + 1);
		if (tokL == nullptr || tokR == nullptr) {
			throw SIKException("Expected value before and after " + token->obj + " delimiter.", token->fromLine);
		} else {
			SIKAst* nodeLeft = new SIKAst();
			SIKAst* nodeRight = new SIKAst();
			this->SetNodeFromToken(nodeLeft, tokL);
			this->SetNodeFromToken(nodeRight, tokR);
			nodeLeft->InternalJumper = node->MyInternalNumber;
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
			throw SIKException("Expected Variable Before assigning sign and value or object after.", token->fromLine);
		} else if (node->Type == sik::DELI_CHILDSET && tokL != nullptr) {
			Token* tokLL = TokenSet->getAtPointer(token->index - 2);
			if (tokL->type != NAMING) {
				throw SIKException("In Object definition property must be a valid variable name.", tokL->fromLine);
			}
			if (tokLL != nullptr && tokLL->type != sik::NODE && tokLL->type != sik::DELI_BRCOPEN && tokLL->type != sik::DELI_COMMA) {
				throw SIKException("In Object definition properties must be seperated by commas.", tokL->fromLine);
			}
		}

		SIKAst* nodeLeft = new SIKAst();
		SIKAst* nodeRight = new SIKAst();
		this->SetNodeFromToken(nodeLeft, tokL);
		this->SetNodeFromToken(nodeRight, tokR);
		node->Left = nodeLeft;
		node->Right = nodeRight;

		//Remove Both and replace:
		if (!TokenSet->replaceRangeWithNode(tokL->index, tokR->index, node)) {
			throw SIKException("Expected Variable Before assigning sign and value or object after.", token->fromLine);
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
	int SIKParser::BuildAst_KeyIf(sik::SIKAst* node, sik::Token* token, sik::SIKTokens* TokenSet, sik::BlocksIn type) {	
		
		//Extract condition:
		int parenthesesEnd = TokenSet->getParenthesesFirstAndLast(token->index);
		
		//Validate If/ElseIf condition:
		if (parenthesesEnd < 0) {
			switch (parenthesesEnd) {
			case -1:
				throw sik::SIKException("Expected Parentheses after " + token->obj + " keyword.", token->fromLine);
				break;
			case -2:
			default:
				throw sik::SIKException("Missing closing Parentheses in " + token->obj + " statement.", token->fromLine);

			}
		}

		//Create Subset:
		sik::SIKTokens condSubSet = TokenSet->getFromeSet(token->index + 2, parenthesesEnd - 1);

		//Validate condition:
		if (condSubSet.empty()) {
			throw sik::SIKException(token->obj + " statement condition part must contain a statement. 11", token->fromLine);
		}

		//Validate Check for Block:
		sik::Token* openBlockToken = TokenSet->getAtPointer(parenthesesEnd + 1);
		if (openBlockToken == nullptr || openBlockToken->type != sik::DELI_BRCOPEN) {
			throw sik::SIKException(token->obj + " statement condition must have a block body.", token->fromLine);
		}

		//Add the Block open
		node->line = openBlockToken->fromLine;
		node->Type = sik::SBLOCK;
		node->Block = type;
		node->Notation = openBlockToken->notation;
		node->Value = sik::SIKLang::dicLang_bracesOpen;

		//Recursively parse condition:
		sik::SIKAst* condNode = this->BuildAst(&condSubSet);
		if ((int)condNode->bulk.size() > 0) {

			node->Left = condNode->bulk[0];
			condNode->bulk[0]->Parent = node;

			//Apply the key:
			sik::SIKAst* IfNode = new sik::SIKAst();
			this->SetNodeFromToken(IfNode, token);
			this->applyNodeToMostLeft(IfNode, node);

		} else {
			throw sik::SIKException(token->obj + " statement condition part must contain a statement. 22", token->fromLine);
		}

		//Destroy Container:
		condNode->PreventBulkDelete = true;
		delete condNode;

		//Remove The first Definitionand place the node chain:
		if (!TokenSet->replaceRangeWithNode(token->index, parenthesesEnd + 1, node)) {
			throw sik::SIKException("Error in token extraction. 22", token->fromLine);
		}
		return 1;
	}
	int SIKParser::BuildAst_KeyElse(sik::SIKAst* node, sik::Token* token, sik::SIKTokens* TokenSet) {
		
		//Validate Check for Block:
		sik::Token* openBlockToken = TokenSet->getAtPointer(token->index + 1);
		if (openBlockToken == nullptr || openBlockToken->type != sik::DELI_BRCOPEN) {
			throw sik::SIKException("ELSE statement condition must have a block body.", token->fromLine);
		}

		//Add the Block open
		node->line = openBlockToken->fromLine;
		node->Type = sik::SBLOCK;
		node->Block = sik::BLOCK_ELSE;
		node->Notation = openBlockToken->notation;
		node->Value = sik::SIKLang::dicLang_bracesOpen;
		
		//Apply the key:
		sik::SIKAst* ElseNode = new sik::SIKAst();
		this->SetNodeFromToken(ElseNode, token);
		this->applyNodeToMostLeft(ElseNode, node);
		node->Left = ElseNode;
		ElseNode->Parent = node;

		//Remove The first Definitionand place the node chain:
		if (!TokenSet->replaceRangeWithNode(token->index, token->index + 1, node)) {
			throw sik::SIKException("Error in token extraction. 33", token->fromLine);
		}
		return 1;
	}
	int SIKParser::BuildAst_BlockOpen(sik::SIKAst* node, sik::Token* token, sik::SIKTokens* TokenSet) {

		//Set:
		this->SetNodeFromToken(node, token);
		this->BlockInCheck.push_back(sik::BLOCK_OBJ);

		//Extract Closing:
		int BlockEnd = TokenSet->getBlockFirstAndLast(token->index);

		//Validate condition:
		if (BlockEnd < 0) {
			throw sik::SIKException("No matching closing block braces. 1123", token->fromLine);
		}
		sik::SIKTokens blockSubSet = TokenSet->getFromeSet(token->index + 1, BlockEnd);

		//Recursively parse condition:
		sik::SIKAst* blockNode = this->BuildAst(&blockSubSet);
		
		if ((int)blockNode->bulk.size() > 0) {
			//Set as bulk in the tree:
			node->bulk = blockNode->bulk;
			blockNode->PreventBulkDelete = true;
			delete blockNode;
		} else {
			throw sik::SIKException("Block Open Must Contain Atleast a closing statement. 22", token->fromLine);
		}

		//Remove Open and replace:
		if (!TokenSet->replaceRangeWithNode(token->index, BlockEnd, node)) {
			throw SIKException("Expected value before and after " + token->obj + " delimiter.", token->fromLine);
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
		this->BlockInCheck.pop_back();

		//Remove Both and replace:
		if (!TokenSet->replaceRangeWithNode(token->index, token->index, node)) {
			throw SIKException("Expected value before and after " + token->obj + " delimiter.", token->fromLine);
		}
		return 1;
	}
	int SIKParser::BuildAst_BracketOpen(sik::SIKAst* node, sik::Token* token, sik::SIKTokens* TokenSet) {
		//Extract Bracket content:
		int parenthesesEnd = TokenSet->getParenthesesFirstAndLast(token->index - 1);
		//Validate Closing:
		if (parenthesesEnd < 0) {
			switch (parenthesesEnd) {
			case -1:
				throw sik::SIKException("Expected Open Parentheses.", token->fromLine);
				break;
			case -2:
			default:
				throw sik::SIKException("Missing closing Parentheses in statement.", token->fromLine);

			}
		}
		//Create Subset:
		sik::SIKTokens condSubSet = TokenSet->getFromeSet(token->index + 1, parenthesesEnd - 1);
		//Validate Subset:
		if (condSubSet.empty()) {
			throw sik::SIKException("Parentheses In statement must contain a statement. 11", token->fromLine);
		}

		//Recursively parse Parentheses content:
		sik::SIKAst* condNode = this->BuildAst(&condSubSet);
		if ((int)condNode->bulk.size() > 0) {
			node->mutateTo(condNode->bulk[0]);
		} else {
			throw sik::SIKException("IF statement condition part must contain a statement. 22", token->fromLine);
		}

		//Clean container:
		condNode->PreventBulkDelete = true;
		delete condNode;

		//Remove The first Definitionand place the node chain:
		if (!TokenSet->replaceRangeWithNode(token->index, parenthesesEnd, node)) {
			throw sik::SIKException("Error in token extraction. 22", token->fromLine);
		}

		return 1;
	}
	int SIKParser::BuildAst_SquareBracketOpen(sik::SIKAst* node, sik::Token* token, sik::SIKTokens* TokenSet) {
		//Extract Bracket content:
		int parenthesesEnd = TokenSet->getSBracketFirstAndLast(token->index);

		//Validate Closing:
		if (parenthesesEnd < 0) {
			switch (parenthesesEnd) {
			case -1:
				throw sik::SIKException("Expected Open Array char" + token->obj + ".", token->fromLine);
				break;
			case -2:
			default:
				throw sik::SIKException("Missing closing Array char" + token->obj + ".", token->fromLine);

			}
		}

		//Create Subset:
		sik::SIKTokens SubSet = TokenSet->getFromeSet(token->index + 1, parenthesesEnd - 1);

		//If Last is comma remove:
		if (!SubSet.empty() && SubSet.back()->type == sik::DELI_COMMA) {
			SubSet.pop(1);
		}

		//check for cells:
		std::vector<int> countEle = SubSet.hasNestedCommas(0);

		//Validate cells:
		if (!countEle.empty() && (SubSet.hasEmptyNestedCommas(0) || countEle[0] == 0 || countEle.back() == SubSet.size() - 1)) {
			throw sik::SIKException("Missing value in array declaration / traverse.", token->fromLine);
		}
		
		//Validate key words & blocks:
		/*
		if (SubSet.hasTypeDeep(sik::KEYWORD) >= 0 || SubSet.hasTypeDeep(sik::DELI_BRCOPEN) >= 0) {
			throw sik::SIKException("You can't use keywords or Blocks in an array declaration / traverse after the " + token->obj  + " chars.", token->fromLine);
		}
		*/
		
		//Recursively parse Parentheses content:
		sik::SIKAst* subNode = this->BuildAst(&SubSet);
		if (!countEle.empty() && (int)subNode->bulk.size() != ((int)countEle.size() + 1)) {
			throw sik::SIKException("Expected " + sik::SIKLang::toString((int)countEle.size() + 1) + " in array declaration / traverse.", token->fromLine);
		}

		//Build the node:
		this->SetNodeFromToken(node, token);
		sik::SIKAst* numNode = new SIKAst();
		numNode->line = node->line;
		numNode->Priority = 0;
		numNode->Type = sik::NUMBER;
		numNode->Value = sik::SIKLang::toString((int)subNode->bulk.size());
		numNode->Parent = node;
		node->Left = numNode;
		int theEleSize = (int)subNode->bulk.size();
		for (int i = theEleSize - 1; i >= 0; i--) {
			node->bulk.push_back(subNode->bulk[i]);
		}

		//Clean container:
		subNode->PreventBulkDelete = true;
		delete subNode;

		//Is variable traverse or push?
		if (token->index > 0) {
			sik::Token* candid = TokenSet->getAtPointer(token->index - 1);
			if (
				candid != nullptr && 
					(
						candid->type == sik::NAMING || 
							(
								candid->type == sik::NODE && 
								candid->node->Type == sik::DELI_SBRKOPEN && 
								candid->node->Right != nullptr && 
								candid->node->Right->Type == sik::NAMING
							)
					)
				) 
			{
				sik::SIKAst* varNode = new SIKAst();
				this->SetNodeFromToken(varNode,candid);
				varNode->Parent = node;
				node->Right = varNode;
				node->Notation = 1;
				if (node->Left->Value == "0") {
					delete node->Left;
					node->Left = node->Right;
					node->Right = nullptr;
				}
			}
		}

		
		//Remove The first Definitionand place the node chain:
		if (node->Notation == 1) {
			if (!TokenSet->replaceRangeWithNode(token->index - 1, parenthesesEnd, node)) {
				throw sik::SIKException("Error in token extraction. 876165", token->fromLine);
			}
		} else {
			if (!TokenSet->replaceRangeWithNode(token->index, parenthesesEnd, node)) {
				throw sik::SIKException("Error in token extraction. 665485", token->fromLine);
			}
		}
		return 1;
	}
	int SIKParser::BuildAst_KeyForLoop(sik::SIKAst* node, sik::Token* token, sik::SIKTokens* TokenSet) {

		//Extract condition:
		int parenthesesEnd = TokenSet->getParenthesesFirstAndLast(token->index);

		//Validate FOR condition loop:
		if (parenthesesEnd < 0) {
			switch (parenthesesEnd) {
				case -1:
					throw sik::SIKException("Expected Parentheses after FOR keyword.", token->fromLine);
				break;
					case -2:
				default:
					throw sik::SIKException("Missing closing Parentheses in FOR statement.", token->fromLine);
			}
		}

		//Create Subset:
		sik::SIKTokens loopSubSet = TokenSet->getFromeSet(token->index + 2, parenthesesEnd - 1);

		//Validate condition:
		if (loopSubSet.size() < 3) {
			throw sik::SIKException("FOR statement condition part must contain a statement with at least two parts (Definition, Condition, [optional]Callback).", token->fromLine);
		}

		//Validate commas and empty parts:
		bool test = loopSubSet.hasEmptyNestedCommas(0);
		if (test || loopSubSet.back()->type == sik::DELI_COMMA) {
			throw sik::SIKException("FOR loop must contain at least two parts seperated by commas and can't be empty between them.", token->fromLine);
		}

		//Validate Check for Block:
		sik::Token* openBlockToken = TokenSet->getAtPointer(parenthesesEnd + 1);
		if (openBlockToken == nullptr || openBlockToken->type != sik::DELI_BRCOPEN) {
			throw sik::SIKException("FOR statement condition must have a block body.", token->fromLine);
		}

		//Add the Block open
		node->line = openBlockToken->fromLine;
		node->Type = sik::SBLOCK;
		node->Block = sik::BLOCK_FOR;
		node->Value = sik::SIKLang::dicLang_bracesOpen;
		node->Notation = openBlockToken->notation;

		//handle nested definition:
		sik::Token checkDefine = loopSubSet.getAt(0);
		bool addDefinition = false;
		if (checkDefine.type == sik::KEYWORD && checkDefine.obj == SIKLang::dicLangKey_variable) {
			addDefinition = true;
			loopSubSet.removeFromeSet(0, 0, true);
		}
		if (loopSubSet.hasType(sik::KEYWORD) != -1) {
			throw sik::SIKException("In FOR Loop you can't use keywords expect of a variable declaration in the declration part.", token->fromLine);
		}

		//Recursively parse condition:
		sik::SIKAst* forNode = this->BuildAst(&loopSubSet);
		int chunks = (int)forNode->bulk.size();
		if (chunks > 1 && chunks < 4) {
			this->SetNodeFromToken(forNode, token);
			node->Left = forNode;
			forNode->Parent = node;
		} else {
			throw sik::SIKException("FOR loop must contain at least two parts (Definition, Condition, [optional]Callback).", token->fromLine);
		}

		//Validate parts and add definition if needed:
		for (int i = 0; i < (int)forNode->bulk.size(); i++) {
			if (i == 0 && addDefinition) {
				sik::SIKAst* defNode = new SIKAst();
				this->SetNodeFromToken(defNode, &checkDefine);
				this->applyNodeToMostLeft(defNode,forNode->bulk[i]);
			}
		}
		//Remove The first Definitionand place the node chain:
		if (!TokenSet->replaceRangeWithNode(token->index, parenthesesEnd + 1, node)) {
			throw sik::SIKException("Error in token extraction. 656", token->fromLine);
		}
		return 1;
	}
	int SIKParser::BuildAst_KeyWhileLoop(sik::SIKAst* node, sik::Token* token, sik::SIKTokens* TokenSet) {
		//Extract condition:
		int parenthesesEnd = TokenSet->getParenthesesFirstAndLast(token->index);

		//Validate WHILE condition loop:
		if (parenthesesEnd < 0) {
			switch (parenthesesEnd) {
			case -1:
				throw sik::SIKException("Expected Parentheses after WHILE keyword.", token->fromLine);
				break;
			case -2:
			default:
				throw sik::SIKException("Missing closing Parentheses in WHILE condition statement.", token->fromLine);
			}
		}

		//Create Subset:
		sik::SIKTokens loopSubSet = TokenSet->getFromeSet(token->index + 2, parenthesesEnd - 1);

		//Validate condition:
		if (loopSubSet.empty()) {
			throw sik::SIKException("WHILE statement condition part must contain a statement with one or two part (Condition, [optional integer]Max Iter).", token->fromLine);
		}

		//Validate commas and empty parts:
		bool test = loopSubSet.hasEmptyNestedCommas(0);
		if (test || loopSubSet.back()->type == sik::DELI_COMMA) {
			throw sik::SIKException("WHILE loop must contain at most two parts seperated by commas and can't be empty between them.", token->fromLine);
		}

		//Validate Check for Block:
		sik::Token* openBlockToken = TokenSet->getAtPointer(parenthesesEnd + 1);
		if (openBlockToken == nullptr || openBlockToken->type != sik::DELI_BRCOPEN) {
			throw sik::SIKException("WHILE statement condition must have a block body.", token->fromLine);
		}

		//Add the Block open
		node->line = openBlockToken->fromLine;
		node->Type = sik::SBLOCK;
		node->Block = sik::BLOCK_WHILE;
		node->Value = sik::SIKLang::dicLang_bracesOpen;
		node->Notation = openBlockToken->notation;

		//handle nested keywords:
		if (loopSubSet.hasType(sik::KEYWORD) != -1) {
			throw sik::SIKException("In WHILE Loop Definition you can't use keywords in the condition part.", token->fromLine);
		}

		//Recursively parse condition:
		sik::SIKAst* whileNode = this->BuildAst(&loopSubSet);
		int chunks = (int)whileNode->bulk.size();
		if (chunks > 0 && chunks < 3) {
			this->SetNodeFromToken(whileNode, token);
			node->Left = whileNode;
			whileNode->Parent = node;
		} else {
			throw sik::SIKException("WHILE loop must contain one or two parts (Condition, [optional integer]Max Iter).", token->fromLine);
		}

		//Validate parts and add definition if needed:
		for (int i = 0; i < (int)whileNode->bulk.size(); i++) {

		}

		//Remove The first Definition and place the node chain:
		if (!TokenSet->replaceRangeWithNode(token->index, parenthesesEnd + 1, node)) {
			throw sik::SIKException("Error in token extraction. 73456", token->fromLine);
		}
		return 1;
	}
	int SIKParser::BuildAst_KeyEachLoop(sik::SIKAst* node, sik::Token* token, sik::SIKTokens* TokenSet) {

		//Extract condition:
		int parenthesesEnd = TokenSet->getParenthesesFirstAndLast(token->index);

		//Validate EACH condition loop:
		if (parenthesesEnd < 0) {
			switch (parenthesesEnd) {
			case -1:
				throw sik::SIKException("Expected Parentheses after EACH keyword.", token->fromLine);
				break;
			case -2:
			default:
				throw sik::SIKException("Missing closing Parentheses in EACH condition statement.", token->fromLine);
			}
		}

		//Create Subset:
		sik::SIKTokens loopSubSet = TokenSet->getFromeSet(token->index + 2, parenthesesEnd - 1);

		//Validate condition:
		if (loopSubSet.empty()) {
			throw sik::SIKException("EACH statement loop must contain a statement with at least two parts and three parts max (Element to Iterate, Index , node Element).", token->fromLine);
		}

		//Validate commas and empty parts:
		bool test = loopSubSet.hasEmptyNestedCommas(0);
		if (test || loopSubSet.back()->type == sik::DELI_COMMA) {
			throw sik::SIKException("EACH loop must contain at most two parts seperated by commas and can't be empty between them.", token->fromLine);
		}

		//Validate Check for Block:
		sik::Token* openBlockToken = TokenSet->getAtPointer(parenthesesEnd + 1);
		if (openBlockToken == nullptr || openBlockToken->type != sik::DELI_BRCOPEN) {
			throw sik::SIKException("EACH statement condition must have a block body.", token->fromLine);
		}

		//Add the Block open
		node->line = openBlockToken->fromLine;
		node->Type = sik::SBLOCK;
		node->Block = sik::BLOCK_EACH;
		node->Value = sik::SIKLang::dicLang_bracesOpen;
		node->Notation = openBlockToken->notation;

		//handle nested keywords:
		if (loopSubSet.hasType(sik::KEYWORD) != -1) {
			throw sik::SIKException("In EACH Loop Definition you can't use keywords in the condition part.", token->fromLine);
		}

		//Recursively parse condition:
		sik::SIKAst* eachNode = this->BuildAst(&loopSubSet);
		int chunks = (int)eachNode->bulk.size();
		if (chunks > 1 && chunks < 4) {
			this->SetNodeFromToken(eachNode, token);
			node->Left = eachNode;
			eachNode->Parent = node;
		} else {
			throw sik::SIKException("EACH loop must contain one or two parts (Element to Iterate, Index , node Element).", token->fromLine);
		}

		//Validate parts and add definition if needed:
		for (int i = 0; i < (int)eachNode->bulk.size(); i++) {

		}

		//Remove The first Definition and place the node chain:
		if (!TokenSet->replaceRangeWithNode(token->index, parenthesesEnd + 1, node)) {
			throw sik::SIKException("Error in token extraction. 73456", token->fromLine);
		}

		return 1;
	}
	int SIKParser::BuildAst_OpSingleSide(sik::SIKAst* node, sik::Token* token, sik::SIKTokens* TokenSet) {
		this->SetNodeFromToken(node, token);
		Token* tokL = TokenSet->getAtPointer(token->index - 1);
		Token* tokR = TokenSet->getAtPointer(token->index + 1);
		if (tokL == nullptr && tokR == nullptr) {
			throw SIKException("Expected number or variable before or after " + token->obj + " delimiter.", token->fromLine);
		} else {
			if (
				tokL != nullptr && 
				(
				(tokL->type == sik::NODE && (tokL->node->Type == sik::NAMING || tokL->node->Type == sik::NUMBER || tokL->node->Type == sik::DELI_SBRKOPEN))
				||
				(tokL->type == sik::NAMING || tokL->type == sik::NUMBER)
				)
			) {
				SIKAst* nodeLeft = new SIKAst();
				this->SetNodeFromToken(nodeLeft, tokL);
				node->Left = nodeLeft;
				node->preVariable = false;
				nodeLeft->Parent = node;
			} else if (
				tokR != nullptr &&
				(
				(tokR->type == sik::NODE && (tokR->node->Type == sik::NAMING || tokR->node->Type == sik::NUMBER || tokR->node->Type == sik::DELI_SBRKOPEN))
				||
				(tokR->type == sik::NAMING || tokR->type == sik::NUMBER)
				)
			) {
				SIKAst* nodeRight = new SIKAst();
				this->SetNodeFromToken(nodeRight, tokR);
				node->Left = nodeRight;
				node->preVariable = true;
				nodeRight->Parent = node;
			} else {
				throw SIKException("Expected number or variable before or after " + token->obj + " delimiter 11.", token->fromLine);
			}
			if (node->preVariable) {
				//Remove Both and replace:
				if (!TokenSet->replaceRangeWithNode(token->index, tokR->index, node)) {
					throw SIKException("Expected value before or after " + token->obj + " delimiter. 33", token->fromLine);
				}
			} else {
				//Remove Both and replace:
				if (!TokenSet->replaceRangeWithNode(tokL->index, token->index, node)) {
					throw SIKException("Expected value before or after " + token->obj + " delimiter. 44", token->fromLine);
				}
			}
			
		}
		return 1;
	}
	int SIKParser::BuildAst_KeyBreakLoop(sik::SIKAst* node, sik::Token* token, sik::SIKTokens* TokenSet) {

		//Break node:
		this->SetNodeFromToken(node, token);

		//Extract howmany part:
		int statementEnd = TokenSet->getSatementLast(token->index);

		//Validate BREAK howmany part:
		if (statementEnd < 0) {
			switch (statementEnd) {
			case -1:
				throw sik::SIKException("Expected Statement End after BREAK keyword.", token->fromLine);
				break;
			default:
				throw sik::SIKException("Expected Statement End after BREAK keyword.", token->fromLine);
			}
		}

		//Create Subset:
		sik::SIKTokens SubSet = TokenSet->getFromeSet(token->index + 1, statementEnd - 1);

		//Parse Howmany part:
		if (!SubSet.empty()) {
			//handle nested keywords:
			if (SubSet.hasType(sik::KEYWORD) != -1) {
				throw sik::SIKException("In " + token->obj + " statement you can't use keywords.", token->fromLine);
			}
			//Recursively parse subset:
			sik::SIKAst* subNode = this->BuildAst(&SubSet);
			int chunks = (int)subNode->bulk.size();
			if (chunks == 1) {
				node->bulk.push_back(subNode->bulk[0]);
			} else {
				throw sik::SIKException("In " + token->obj + " statement you can't use sub expression or commas.", token->fromLine);
			}

			//Destroy Container:
			subNode->PreventBulkDelete = true;
			delete subNode;
		}

		//Remove The Definition and place the node chain:
		if (!TokenSet->replaceRangeWithNode(token->index, statementEnd, node)) {
			throw sik::SIKException("Error in token extraction. 784516", token->fromLine);
		}

		return 1;
	}
	int SIKParser::BuildAst_KeyBreakCond(sik::SIKAst* node, sik::Token* token, sik::SIKTokens* TokenSet) {
		//Break condition node:
		this->SetNodeFromToken(node, token);

		//Validate BREAK condition no statement:
		sik::Token* semiEnd = TokenSet->getAtPointer(token->index + 1);
		if (semiEnd == nullptr || semiEnd->type != sik::DELI_OPEND) {
			throw sik::SIKException("Expected Statement End after " + token->obj + " keyword.", token->fromLine);
		}

		//Remove The Definition and place the node chain:
		if (!TokenSet->replaceRangeWithNode(token->index, semiEnd->index, node)) {
			throw sik::SIKException("Error in token extraction. 648548", token->fromLine);
		}

		return 1;
	}
	int SIKParser::BuildAst_KeyContinueLoop(sik::SIKAst* node, sik::Token* token, sik::SIKTokens* TokenSet) {
		//Continue node:
		this->SetNodeFromToken(node, token);
		//Validate Continue part  no statement:
		sik::Token* semiEnd = TokenSet->getAtPointer(token->index + 1);
		if (semiEnd == nullptr || semiEnd->type != sik::DELI_OPEND) {
			throw sik::SIKException("Expected Statement End after " + token->obj + " keyword.", token->fromLine);
		}
		//Remove The Definition and place the node chain:
		if (!TokenSet->replaceRangeWithNode(token->index, semiEnd->index, node)) {
			throw sik::SIKException("Error in token extraction. 6548421", token->fromLine);
		}
		return 1;
	}
	int SIKParser::BuildAst_KeyPrint(sik::SIKAst* node, sik::Token* token, sik::SIKTokens* TokenSet) {

		//Break node:
		this->SetNodeFromToken(node, token);

		//Extract what part:
		int statementEnd = TokenSet->getSatementLast(token->index);

		//Validate PRINT what part:
		if (statementEnd < 0) {
			switch (statementEnd) {
			case -1:
				throw sik::SIKException("Expected Statement End after " + token->obj + " keyword.", token->fromLine);
				break;
			default:
				throw sik::SIKException("Expected Statement End after " + token->obj + " keyword.", token->fromLine);
			}
		}

		//Create Subset:
		sik::SIKTokens SubSet = TokenSet->getFromeSet(token->index + 1, statementEnd - 1);

		//Parse Howmany part:
		if (!SubSet.empty()) {
			//handle nested keywords:
			if (SubSet.hasType(sik::KEYWORD) != -1) {
				throw sik::SIKException("In " + token->obj + " statement you can't use keywords.", token->fromLine);
			}
			//Recursively parse subset:
			sik::SIKAst* subNode = this->BuildAst(&SubSet);
			int chunks = (int)subNode->bulk.size();
			for (int i = 0; i < chunks; i++) {
				node->bulk.push_back(subNode->bulk[i]);
			}

			//Destroy Container:
			subNode->PreventBulkDelete = true;
			delete subNode;

		} else {
			throw sik::SIKException("Expected a Statement after the " + token->obj + " keyword.", token->fromLine);
		}

		//Remove The Keyword and place the node chain:
		if (!TokenSet->replaceRangeWithNode(token->index, statementEnd, node)) {
			throw sik::SIKException("Error in token extraction. 325648", token->fromLine);
		}

		return 1;
	}
	/* Sets a node from token object:
	* @param SIKAst* node -> the node
	* @param Token* tok -> the node
	*/
	void SIKParser::SetNodeFromToken(sik::SIKAst* node, sik::Token* tok)
	{
		if (tok->type == sik::NODE) {

			//Deep copy node:
			node->Type = tok->node->Type;
			node->Priority = tok->node->Priority;
			node->line = tok->node->line;
			node->Value = tok->node->Value;
			node->Left = tok->node->Left;
			node->Right = tok->node->Right;
			node->Block = tok->node->Block;
			node->bulk = tok->node->bulk;
			node->InsBlockPointer = tok->node->InsBlockPointer;
			node->InternalJumper = tok->node->InternalJumper;
			node->MyInternalNumber = tok->node->MyInternalNumber;
			node->preVariable = tok->node->preVariable;
			node->Notation = tok->node->Notation;

		} else {
			//Create new copy node:
			node->Type = tok->type;
			node->Priority = -1;
			node->line = tok->fromLine;
			node->Value = tok->obj;
		}
	}

	/* Apply a node to most left of a node tree
	* @param SIKAst* node -> the node
	*
	*/
	void SIKParser::applyNodeToMostLeft(sik::SIKAst* toApplyNode, sik::SIKAst* applyToNodeTree) {
		if (applyToNodeTree->Left != nullptr) {
			this->applyNodeToMostLeft(toApplyNode, applyToNodeTree->Left);
		} else {
			applyToNodeTree->Left = toApplyNode;
			toApplyNode->Parent = applyToNodeTree->Left;
		}
	}
	
	/* Walk the Ast given
	 * @param SIKAst* node -> the node
	 *
	*/
	void SIKParser::WalkAst(sik::SIKAst* nodeParent) {
		for (int i = 0; i < (int)nodeParent->bulk.size(); i++) {
			WalkAst(nullptr, nodeParent->bulk[i]);
		}
	}
	void SIKParser::WalkAst(sik::SIKAst* nodeParent, sik::SIKAst* nodeChild) {
		//Always walk left in the tree first and than right and finaly parent:
		if (nodeChild->Left != nullptr && !nodeChild->Left->Mark) {
			WalkAst(nodeChild, nodeChild->Left);
		}
		if (nodeChild->Right != nullptr && !nodeChild->Right->Mark) {
			WalkAst(nodeChild, nodeChild->Right);
		}

		//Handle Parse Token by type:
		switch (nodeChild->Type) {
		case sik::KEYWORD:
			this->genForKeywords(nodeParent, nodeChild);
			nodeChild->Mark = true;
			break;
		case sik::DELI_BRCOPEN:
		case sik::SBLOCK:
		case sik::NAMING:
		case sik::NUMBER:
		case sik::STRING:
		case sik::BOOLEAN:
		case sik::NULLTYPE:
			this->genForPrimitives(nodeParent, nodeChild);
			nodeChild->Mark = true;
			break;
		case sik::DELI_SBRKOPEN:
			this->genForArray(nodeParent, nodeChild);
			nodeChild->Mark = true;
			break;
		case sik::DELI_EQUAL:
		case sik::DELI_EQUALADD:
		case sik::DELI_EQUALSUB:
		case sik::DELI_CHILDSET:
		case sik::DELI_PLUS:
		case sik::DELI_MINUS:
		case sik::DELI_MULTI:

		case sik::DELI_CEQUAL:
		case sik::DELI_CNEQUAL:
		case sik::DELI_CTEQUAL:
		case sik::DELI_CTNEQUAL:

		case sik::DELI_GRT:
		case sik::DELI_LST:
		case sik::DELI_GRTE:
		case sik::DELI_LSTE:

		case sik::DELI_CAND:
		case sik::DELI_COR:
			this->genForLR(nodeParent, nodeChild);
			nodeChild->Mark = true;
			break;
		case sik::DELI_INCR:
		case sik::DELI_DECR:
			this->genForOpSingleSide(nodeParent, nodeChild);
			nodeChild->Mark = true;
			break;
		case sik::DELI_BRCCLOSE:
			this->genForBlockClose(nodeParent, nodeChild);
			nodeChild->Mark = true;
			break;
		}
	}
	void SIKParser::AddToInstructions(const sik::SIKInstruct& instruct) {
		this->AddToInstructions(instruct, nullptr);
	}
	void SIKParser::AddToInstructions(const sik::SIKInstruct& instruct, sik::SIKAst* nodeParent) {
		int testForBlocks = (int)this->BlockChunksContainer.size();
		if (testForBlocks > 0 && this->BlockChunksContainer[testForBlocks - 1] == sik::BLOCK_OBJ) {
			this->pushToObjectsInstructions(instruct);
		//Inside main:
		} else {
			this->Instructions->push_back(instruct);
		}
		
		//This will start a object definition at the correct place: 
		if (instruct.Type == sik::INS_OBJCREATE && testForBlocks > 0 && this->BlockChunksContainer[testForBlocks - 1]) {
			int blockContainer = this->ObjectDefinitions->size();
			this->ObjectDefinitions->back().back().pointToInstruct = blockContainer;
			this->BlockChunksContainer.push_back(sik::BLOCK_OBJ);
			this->ObjectDefinitions->push_back(std::vector<sik::SIKInstruct>());
		} else if (instruct.Type == sik::INS_OBJCREATE) {
			int blockContainer = this->ObjectDefinitions->size();
			this->Instructions->back().pointToInstruct = blockContainer;
			this->BlockChunksContainer.push_back(sik::BLOCK_OBJ);
			this->ObjectDefinitions->push_back(std::vector<sik::SIKInstruct>());
		}
	}
	void SIKParser::pushToObjectsInstructions(const sik::SIKInstruct& instruct) {
		//Does it has a specific Point to block:
		if (instruct.pointToInstruct != -1) {
			this->ObjectDefinitions->at(instruct.pointToInstruct).push_back(instruct);
		} else {
			int size = (int)this->ObjectDefinitions->size();
			for (int i = size - 1; i >= 0; i--) {
				if ((int)this->ObjectDefinitions->at(i).size() == 0 || this->ObjectDefinitions->at(i).back().Type != sik::INS_OBJDONE) {
					this->ObjectDefinitions->at(i).push_back(instruct);
					break;
				}
			}
		}
	}
	void SIKParser::genForKeywords(sik::SIKAst* nodeParent, sik::SIKAst* nodeChild) {
		// IF statement:
		if (nodeChild->Value == SIKLang::dicLangKey_cond_if) {
			this->AddToInstructions(sik::SIKInstruct(nodeChild, sik::INS_IF));
			return;
		}
		// ELSE statement:
		if (nodeChild->Value == SIKLang::dicLangKey_cond_else) {
			this->AddToInstructions(sik::SIKInstruct(nodeChild, sik::INS_ELSE));
			return;
		}
		// ELSEIF statement:
		if (nodeChild->Value == SIKLang::dicLangKey_cond_elseif) {
			this->AddToInstructions(sik::SIKInstruct(nodeChild, sik::INS_ELSEIF));
			return;
		}
		// Variable definition:
		if (nodeChild->Value == SIKLang::dicLangKey_variable) {
			this->AddToInstructions(sik::SIKInstruct(nodeChild, sik::INS_DEFINE));
			return;
		}
		//While Loop:
		if (nodeChild->Value == SIKLang::dicLangKey_loop_while) {
			this->AddToInstructions(sik::SIKInstruct(nodeChild, sik::INS_WHLL));
			int forPartsCount = nodeChild->bulk.size();
			for (int i = 0; i < forPartsCount; i++) {
				switch (i) {
				case 0:
					this->WalkAst(nodeChild, nodeChild->bulk[i]);
					break;
				case 1:
					this->AddToInstructions(sik::SIKInstruct(nodeChild, sik::INS_WHLM));
					this->WalkAst(nodeChild, nodeChild->bulk[i]);
					return;
					break; // max 2 parts -> so go out.
				}
			}
			return;
		}
		//For Loop:
		if (nodeChild->Value == SIKLang::dicLangKey_loop_for) {
			this->AddToInstructions(sik::SIKInstruct(nodeChild, sik::INS_FORL));
				int forPartsCount = nodeChild->bulk.size();
				int count = 0;
				for (int i = 0; i < forPartsCount; i++) {
					count++;
					switch (i) {
						case 0:
							this->WalkAst(nodeChild, nodeChild->bulk[i]);
							break;
						case 1:
							this->AddToInstructions(sik::SIKInstruct(nodeChild, sik::INS_FRCO));
							this->WalkAst(nodeChild, nodeChild->bulk[i]);
							break;
						case 2:
							this->AddToInstructions(sik::SIKInstruct(nodeChild, sik::INS_FRCA));
							this->WalkAst(nodeChild, nodeChild->bulk[i]);
							return; // max 3 parts -> so go out.
							break;
					}
				}
				if (count < 3) {
					this->AddToInstructions(sik::SIKInstruct(nodeChild, sik::INS_FRCA));
				}
			return;
		}
		//Each Loop:
		if (nodeChild->Value == SIKLang::dicLangKey_loop_each) {
			this->AddToInstructions(sik::SIKInstruct(nodeChild, sik::INS_EACH));
			int forPartsCount = nodeChild->bulk.size();
			int count = 0;
			for (int i = 0; i < forPartsCount; i++) {
				switch (i) {
				case 0:
					this->WalkAst(nodeChild, nodeChild->bulk[i]);
					break;
				case 1:
					if (forPartsCount == 2) {
						this->AddToInstructions(sik::SIKInstruct(nodeChild, sik::INS_EACHI));
						this->AddToInstructions(sik::SIKInstruct(nodeChild, sik::INS_EACHE));
						this->WalkAst(nodeChild, nodeChild->bulk[i]);
						return; // max 3 parts -> so go out.
					} else {
						this->AddToInstructions(sik::SIKInstruct(nodeChild, sik::INS_EACHI));
						this->WalkAst(nodeChild, nodeChild->bulk[i]);
						break;
					}
				case 2:
					this->AddToInstructions(sik::SIKInstruct(nodeChild, sik::INS_EACHE));
					this->WalkAst(nodeChild, nodeChild->bulk[i]);
					return; // max 3 parts -> so go out.
					break;
				}
			}
			if (count < 3) {
				this->AddToInstructions(sik::SIKInstruct(nodeChild, sik::INS_FRCA));
			}
			return;
		}
		//Break Loop:
		if (nodeChild->Value == SIKLang::dicLangKey_loop_break) {
			this->AddToInstructions(sik::SIKInstruct(nodeChild, sik::INS_BRKL));
			if ((int)nodeChild->bulk.size() == 1) {
				this->WalkAst(nodeChild, nodeChild->bulk[0]);
			}
			this->AddToInstructions(sik::SIKInstruct(nodeChild, sik::INS_DOBRK));
			return;
		}
		//Break Condition:
		if (nodeChild->Value == SIKLang::dicLangKey_cond_break) {
			this->AddToInstructions(sik::SIKInstruct(nodeChild, sik::INS_BRKC));
			return;
		}
		//Continue Loop:
		if (nodeChild->Value == SIKLang::dicLangKey_loop_continue) {
			this->AddToInstructions(sik::SIKInstruct(nodeChild, sik::INS_LCON));
			return;
		}
		//Print:
		if (nodeChild->Value == SIKLang::dicLangKey_print) {
			int forPartsCount = nodeChild->bulk.size();
			for (int i = 0; i < forPartsCount; i++) {
				this->AddToInstructions(sik::SIKInstruct(nodeChild, sik::INS_PRINT));
				this->WalkAst(nodeChild, nodeChild->bulk[i]);
				this->AddToInstructions(sik::SIKInstruct(nodeChild, sik::INS_DOPRINT));
			}
			return;
		}
		return;
	}
	void SIKParser::genForPrimitives(sik::SIKAst* nodeParent, sik::SIKAst* nodeChild) {
		switch (nodeChild->Type) {
			case sik::SBLOCK:
				if (nodeChild->Notation == 1) {
					this->AddToInstructions(sik::SIKInstruct(nodeChild, sik::INS_OSBLOCK));
				} else {
					this->AddToInstructions(sik::SIKInstruct(nodeChild, sik::INS_OBLOCK));
				}
				break;
			case sik::DELI_BRCOPEN:
				this->AddToInstructions(sik::SIKInstruct(nodeChild, sik::INS_OBJCREATE), nodeParent);
				this->Instructions->back().Block = sik::BLOCK_OBJ;
				if ((int)nodeChild->bulk.size() > 0) {
					this->WalkAst(nodeChild);
				}
				break;
			case sik::NAMING:
			case sik::NUMBER:
			case sik::STRING:
			case sik::BOOLEAN:
			case sik::NULLTYPE:
				this->AddToInstructions(sik::SIKInstruct(nodeChild));
				break;
		}
	}
	void SIKParser::genForLR(sik::SIKAst* nodeParent, sik::SIKAst* nodeChild) {
		this->AddToInstructions(sik::SIKInstruct(nodeChild));
	}
	void SIKParser::genForBlockClose(sik::SIKAst* nodeParent, sik::SIKAst* nodeChild) {
		//Incase we are wrapping an Object definition:
		if ((int)this->BlockChunksContainer.size() > 0 && this->BlockChunksContainer.back() == sik::BLOCK_OBJ) {
			this->AddToInstructions(sik::SIKInstruct(nodeChild, sik::INS_OBJDONE));
			this->BlockChunksContainer.pop_back();
		//Normal Object:
		} else {
			this->AddToInstructions(sik::SIKInstruct(nodeChild));
		}
		
	}
	void SIKParser::genForOpSingleSide(sik::SIKAst* nodeParent, sik::SIKAst* nodeChild) {
		if (nodeChild->preVariable && nodeChild->Type == sik::DELI_INCR) {
			this->AddToInstructions(sik::SIKInstruct(nodeChild, sik::INS_PINCREMENT), nodeParent);
		} else if (!nodeChild->preVariable && nodeChild->Type == sik::DELI_INCR) {
			this->AddToInstructions(sik::SIKInstruct(nodeChild, sik::INS_INCREMENT), nodeParent);
		} else if (nodeChild->preVariable && nodeChild->Type == sik::DELI_DECR) {
			this->AddToInstructions(sik::SIKInstruct(nodeChild, sik::INS_PDECREMENT), nodeParent);
		} else if (!nodeChild->preVariable && nodeChild->Type == sik::DELI_DECR) {
			this->AddToInstructions(sik::SIKInstruct(nodeChild, sik::INS_DECREMENT), nodeParent);
		}
	}
	void SIKParser::genForArray(sik::SIKAst* nodeParent, sik::SIKAst* nodeChild) {
		if (
			nodeChild->Notation == 1 && 
			(
				nodeChild->Left->Type == sik::NAMING || 
					(
						nodeChild->Left->Type == sik::DELI_SBRKOPEN && 
						nodeChild->Notation == 1)
				)
		) {
			this->AddToInstructions(sik::SIKInstruct(nodeChild, sik::INS_ARRP));
			return;
		} else if (nodeChild->Notation == 1) {
			this->AddToInstructions(sik::SIKInstruct(nodeChild, sik::INS_ARRT));
		} else {
			this->AddToInstructions(sik::SIKInstruct(nodeChild, sik::INS_ARRC));
		}
		int size = (int)nodeChild->bulk.size();
		for (int i = 0; i < size; i++) {
			this->WalkAst(nodeChild, nodeChild->bulk[i]);
		}
		this->AddToInstructions(sik::SIKInstruct(nodeChild, sik::INS_ARRD));
	}

	// Find the maximum height of the binary tree
	int SIKParser::maxHeight(sik::SIKAst *p) {
		if (!p) return 0;
		int leftHeight = maxHeight(p->Left);
		int rightHeight = maxHeight(p->Right);
		return (leftHeight > rightHeight) ? leftHeight + 1 : rightHeight + 1;
	}

	// Print the arm branches (eg, /    \ ) on a line
	void SIKParser::printBranches(int branchLen, int nodeSpaceLen, int startLen, int nodesInThisLevel, const std::deque<sik::SIKAst*>& nodesQueue, std::ostream& out) {
		std::deque<SIKAst*>::const_iterator iter = nodesQueue.begin();
		for (int i = 0; i < nodesInThisLevel / 2; i++) {
			out << ((i == 0) ? std::setw(startLen - 1) : std::setw(nodeSpaceLen - 2)) << "" << ((*iter++) ? "/" : " ");
			out << std::setw(2 * branchLen + 2) << "" << ((*iter++) ? "\\" : " ");
		}
		out << std::endl;
	}

	// Print the branches and node (eg, ___10___ )
	void SIKParser::printNodes(int level, int indentSpace, int branchLen, int nodeSpaceLen, int startLen, int nodesInThisLevel, const std::deque<sik::SIKAst*>& nodesQueue, std::ostream& out, std::vector<sik::SIKAst*>* later) {
		std::deque<SIKAst*>::const_iterator iter = nodesQueue.begin();
		std::string sValue = "";
		for (int i = 0; i < nodesInThisLevel; i++, iter++) {
			if ((*iter) && (int)(*iter)->bulk.size() > 0) {
				sValue = ((*iter) ? this->truncateString((*iter)->Value) + "*" : "*");
			} else {
				sValue = ((*iter) ? this->truncateString((*iter)->Value) : "");
			}
			out << ((i == 0) ? std::setw(startLen) : std::setw(nodeSpaceLen)) << "" << ((*iter && (*iter)->Left) ? std::setfill('_') : std::setfill(' '));
			out << std::setw(branchLen + 2) << sValue;
			out << ((*iter && (*iter)->Right) ? std::setfill('_') : std::setfill(' ')) << std::setw(branchLen) << "" << std::setfill(' ');
			if ((*iter) && (int)(*iter)->bulk.size() > 0) {
				for (int i = 0; i < (int)(*iter)->bulk.size(); i++) {
					later->push_back((*iter)->bulk[i]);
				}
			}
		}
		out << std::endl;
	}

	// Print the leaves only (just for the bottom row)
	void SIKParser::printLeaves(int indentSpace, int level, int nodesInThisLevel, const std::deque<sik::SIKAst*>& nodesQueue, std::ostream& out, std::vector<sik::SIKAst*>* later) {
		std::deque<SIKAst*>::const_iterator iter = nodesQueue.begin();
		std::string sValue = "";
		for (int i = 0; i < nodesInThisLevel; i++, iter++) {
			if ((*iter) && (int)(*iter)->bulk.size() > 0) {
				sValue = ((*iter) ? this->truncateString((*iter)->Value) + "*" : "*");
			} else {
				sValue = ((*iter) ? this->truncateString((*iter)->Value) : "");
			}
			out << ((i == 0) ? std::setw(indentSpace + 2) : std::setw(2 * level + 2)) << sValue;
			if ((*iter) && (int)(*iter)->bulk.size() > 0) {
				for (int i = 0; i < (int)(*iter)->bulk.size(); i++) {
					later->push_back((*iter)->bulk[i]);
				}
			}
		}
		out << std::endl;
		
	}

	// Pretty formatting of a binary tree to the output stream
	// @ param
	// level  Control how wide you want the tree to sparse (eg, level 1 has the minimum space between nodes, while level 2 has a larger space between nodes)
	// indentSpace  Change this to add some indent space to the left (eg, indentSpace of 0 means the lowest level of the left node will stick to the left margin)
	void SIKParser::printTree(sik::SIKAst *root, int level, int indentSpace, std::ostream& out) {
		std::vector<SIKAst*> later;
		if (root->Left == nullptr && root->Right == nullptr && root->bulk.size() > 0) {
			if (root->Type != sik::NODE && root->Type != sik::NOPE) {
				std::cout << root->Value << std::endl;
			}
			for (int i = 0; i < (int)root->bulk.size(); i++) {
				this->printTree(root->bulk[i], level, indentSpace, out);
				SIKLang::printEmpLine(1);
			}
		} else {
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
				printNodes(level, indentSpace, branchLen, nodeSpaceLen, startLen, nodesInThisLevel, nodesQueue, out, &later);

				for (int i = 0; i < nodesInThisLevel; i++) {
					SIKAst *currNode = nodesQueue.front();
					nodesQueue.pop_front();
					if (currNode) {
						nodesQueue.push_back(currNode->Left);
						nodesQueue.push_back(currNode->Right);
					} else {
						nodesQueue.push_back(NULL);
						nodesQueue.push_back(NULL);
					}
				}
				nodesInThisLevel *= 2;
			}
			printBranches(branchLen, nodeSpaceLen, startLen, nodesInThisLevel, nodesQueue, out);
			printLeaves(indentSpace, level, nodesInThisLevel, nodesQueue, out, &later);
			//Print nested bulk:
			if (!later.empty()) {
				std::cout << "NESTED GENERATED AST:" << std::endl << std::endl;
				for (int i = 0; i < (int)later.size(); i++) {
					this->printTree(later[i], level, indentSpace, out);
					SIKLang::printEmpLine(1);
				}
				std::cout << "END NESTED." << std::endl << std::endl;
			}
		}
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