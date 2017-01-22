//
//  SIKTokens.cpp
//  SIK
//
//  Created by Shlomo Hassid on 06/11/2016.
//  Copyright © 2016 Shlomo Hassid. All rights reserved.
//

#include "SIKLang.hpp"
#include "SIKTokens.hpp"

#include <iostream>

namespace sik {
    SIKTokens::SIKTokens() {
        
    }
    
    int SIKTokens::size() {
        return (int)this->tokenSet.size();
    }
	/* Truncates all the tokens container:
	*/
	void SIKTokens::clear() {
		this->tokenSet.clear();
	}
	/* Pop a token from vector is safe to pop several:
	*/
	void SIKTokens::pop(int howMany) {
		for (int i = 0; i < howMany; i++) {
			if (this->size() > 0) {
				this->tokenSet.pop_back();
			}
		}
	}
	/* Get the Last token from vector is safe:
	*/
	sik::Token* SIKTokens::back() {
		if (!this->empty()) {
			return &this->tokenSet.back();
		}
		return nullptr;
	}
	/* Get empty state of th tokens set:
	*/
	bool SIKTokens::empty() {
		return this->tokenSet.empty();
	}
	/** Insert a token at the end:
	*   @param Token token -> the token to insert.
	*/
    void SIKTokens::insert(sik::Token token) {
        this->tokenSet.push_back(token);
    }
    void SIKTokens::insert(sik::SIKTokens* tokens) {
        int i = tokens->size();
        for (int j = 0; j < i; j++) {
            this->insert(tokens->getAt(j));
        }
    }
	/** Insert a token at a specific index:
	*   @param Token token -> the token to insert.
	*   @param int index   -> At a position
	*/
	void SIKTokens::insertAt(sik::Token token, int index) {
		this->tokenSet.insert(this->tokenSet.begin() + index, token);
	}
	/** Add the correct indexes to the Token set:
	 *
	*/
	void SIKTokens::addIndexes() {
		for (int i = 0; i < this->size(); i++) {
			this->tokenSet[i].index = i;
		}
	}
	/** Get a copy of the token set:
	*/
    std::vector<sik::Token> SIKTokens::getSet() {
        return this->tokenSet;
    }
	/** Get a pointer to the token set:
	*/
    std::vector<sik::Token>* SIKTokens::getSetPointer() {
        return &this->tokenSet;
    }
	/** Get a token at an index:
	*  @param int i   -> At a position
	*/
    sik::Token SIKTokens::getAt(int i) {
        if (i < 0 || i >= this->size()) {
            //Return Error.
        }
        return this->tokenSet[i];
    }
	/** Get a pointer to the token at an index:
	 *  @param int i   -> At a position
	*/
    sik::Token* SIKTokens::getAtPointer(int i) {
        if (i < 0 || i >= this->size()) {
			return nullptr;
        }
        return &this->tokenSet[i];
    }
	/** Check if set has a token of type:
	*  @param TokenTypes type   -> At a position
	*/
    int SIKTokens::hasType(sik::TokenTypes type) {
        int i = this->size();
        for (int j = 0; j < i; j++) {
            if (this->getAtPointer(j)->type == type) {
                return j;
            }
        }
        return -1;
    }
	int SIKTokens::hasTypeDeep(sik::TokenTypes type) {
		int i = this->size();
		for (int j = 0; j < i; j++) {
			if (this->getAtPointer(j)->type == type) {
				return j;
			}
		}
		return -1;
	}

	/** Get the position of the highest priority token in a set:
	*/
    int SIKTokens::findHighestPriority() {
        int s = this->size();
        int at = -1;
        int high = -1;
        for (int i = 0; i < s; i++) {
            if (this->tokenSet[i].priority > high && this->tokenSet[i].node == nullptr) {
                high = this->tokenSet[i].priority;
                at = i;
            }
        }
        return at;
    }
	/** Get the token copy of the highest priority token in a set:
	*/
    sik::Token SIKTokens::getHighestPriorityToken() {
        int f = this->findHighestPriority();
        if (f == -1) {
            //Return error;
        }
        return this->getAt(f);
    }
	/** Get the token pointer of the highest priority token in a set:
	*/
    sik::Token* SIKTokens::getHighestPriorityTokenPointer() {
        int f = this->findHighestPriority();
        if (f == -1) {
            //Return error;
        }
        return this->getAtPointer(f);
    }

	/** Check if set has nested commas:
	 * @param int indexStart -> from where to seek
	 * @return vector<int> -> a set of positions.
	*/
	std::vector<int> SIKTokens::hasNestedCommas(int indexStart) {
		int countNest = 0;
		int size = this->size();
		std::vector<int> vecnum;

		for (int i = indexStart; i < size; i++) {
			sik::Token* tok = this->getAtPointer(i);
			if (tok->type == sik::DELI_BRKOPEN ||
				tok->type == sik::DELI_SBRKOPEN ||
				tok->type == sik::DELI_BRCOPEN
			) { 
				countNest++;
				continue;
			}
			if (tok->type == sik::DELI_BRKCLOSE ||
				tok->type == sik::DELI_SBRKCLOSE ||
				tok->type == sik::DELI_BRCCLOSE
				) {
				countNest--;
				continue;
			}
			if (tok->type == sik::DELI_COMMA && countNest < 1) {
				vecnum.push_back(i);
			}
		}
		return vecnum;
	}
	/** Check if set has nested commas that are repeated e.g -> 1,3,4 : ,??,,? :
	* @param int indexStart -> from where to seek
	* @return bool -> true if has otherwise false.
	*/
	bool SIKTokens::hasEmptyNestedCommas(int indexStart) {
		std::vector<int> vecnum = this->hasNestedCommas(indexStart);
		int sizecheck = (int)vecnum.size();
		if (sizecheck < 2) return false;
		int prev = -2;
		for (int i = 0; i < sizecheck; i++) {
			if (vecnum[i] - prev == 1) {
				return true;
			}
			prev = vecnum[i];
		}
		return false;
	}
	int SIKTokens::getParenthesesFirstAndLast(int indexStart) {
		int countNested = 0;
		//Validate first:
		if (indexStart + 1 >= this->size() || this->tokenSet[indexStart + 1].type != sik::DELI_BRKOPEN) {
			return -1;
		}
		int size = this->size();
		for (int i = indexStart + 2; i < size; i++) {
			sik::Token* tok = this->getAtPointer(i);
			if (tok->type == sik::DELI_BRKOPEN) {
				countNested++;
				continue;
			}
			if (tok->type == sik::DELI_BRKCLOSE && countNested > 0
				) {
				countNested--;
				continue;
			}
			if (tok->type == sik::DELI_BRKCLOSE) {
				return i;
			}
		}
		return -2;
	}
	int SIKTokens::getSBracketFirstAndLast(int indexStart) {
		int countNested = 0;
		//Validate first:
		if (this->tokenSet[indexStart].type != sik::DELI_SBRKOPEN) {
			return -1;
		}
		int size = this->size();
		for (int i = indexStart + 1; i < size; i++) {
			sik::Token* tok = this->getAtPointer(i);
			if (tok->type == sik::DELI_SBRKOPEN) {
				countNested++;
				continue;
			}
			if (tok->type == sik::DELI_SBRKCLOSE && countNested > 0
				) {
				countNested--;
				continue;
			}
			if (tok->type == sik::DELI_SBRKCLOSE) {
				return i;
			}
		}
		return -2;
	}
	int SIKTokens::getBlockFirstAndLast(int indexStart) {
		int countNested = 0;
		//Validate first:
		if (indexStart + 1 >= this->size()) {
			return -1;
		}
		int size = this->size();
		for (int i = indexStart + 1; i < size; i++) {
			sik::Token* tok = this->getAtPointer(i);
			if (tok->type == sik::DELI_BRCOPEN) {
				countNested++;
				continue;
			}
			if (tok->type == sik::DELI_BRCCLOSE && countNested > 0
				) {
				countNested--;
				continue;
			}
			if (tok->type == sik::DELI_BRCCLOSE) {
				return i;
			}
		}
		return -2;
	}
	int SIKTokens::getSatementLast(int indexStart) {
		int countNested = 0;

		//Validate first:
		if (indexStart + 1 >= this->size() || this->tokenSet[indexStart].type == sik::DELI_OPEND) {
			return -1;
		}
		int size = this->size();
		for (int i = indexStart + 1; i < size; i++) {
			sik::Token* tok = this->getAtPointer(i);
			if (tok->type == sik::DELI_BRCOPEN) {
				countNested++;
				continue;
			}
			if (tok->type == sik::DELI_BRCCLOSE && countNested > 0
				) {
				countNested--;
				continue;
			}
			if (tok->type == sik::DELI_OPEND && countNested == 0) {
				return i;
			}
		}
		return -1;
	}
	bool SIKTokens::hasUnparse() {
		int size = this->size();
		for (int i = 0; i < size; i++) {
			if (this->tokenSet[i].node == nullptr) return true;
		}
		return false;
	}
	/* Replace a range in the token set and puts a node instead:
	 * @param int start -> start index
	 * @param int end   -> end index
	 * @param SIKAst* node -> the node pointer
	 * @return bool
	*/
	bool SIKTokens::replaceRangeWithNode(int start, int end, sik::SIKAst* node) {
		if (node == nullptr) { return false; }

		sik::Token token;
		token.type = sik::NODE;
		token.priority = 0;
		token.index = start;
		token.fromLine = -1;
		token.obj = "\0";
		token.node = node;
		token.addBlock = sik::BLOCK_NONE;

		//Will perform the erase:
		if (!this->removeFromeSet(start, end, false)) { return false; }

		//Push the new one:
		this->insertAt(token, start);

		//Reindex Set:
		this->addIndexes();

		return true;
	}
	/* Earases a range in the token set:
	* @param int start -> start index
	* @param int end   -> end index
	* @param bool resetIndexes   -> whether to reset the indexes or not
	* @return bool
	*/
	bool SIKTokens::removeFromeSet(int start, int end, bool resetIndexes) {
		int howMany = end - start + 1;
		if (howMany < 1 || end > this->size() - 1) {
			return false;
		}
		//Will perform the erase:
		for (int i = 0; i < howMany; i++) {
			this->tokenSet.erase(this->tokenSet.begin() + start);
		}
		if (resetIndexes) {
			this->addIndexes();
		}
		return true;
	}
	/* Get from set a subset:
	* @param int start -> start index
	* @param int end   -> end index
	* @return bool
	*/
	sik::SIKTokens SIKTokens::getFromeSet(int start, int end) {
		sik::SIKTokens subset;
		for (int i = start; i < end + 1; i++) {
			subset.insert(this->getAt(i));
		}
		subset.addIndexes();
		return subset;
	}
    void SIKTokens::renderTokenSet() {
        int i = this->size();
		std::cout << "TYPE : ";
        for (int j = 0; j < i; j++) {
			std::cout << "[ " << this->typeToString(this->tokenSet[j].type) << "\t]";
        }
		std::cout << std::endl << "OBJ  : ";
		for (int j = 0; j < i; j++) {
			if (this->tokenSet[j].obj.length() > 4) {
				std::cout << "[ " << std::string(this->tokenSet[j].obj.begin(), this->tokenSet[j].obj.begin() + 4) << "\t]";
			} else {
				std::cout << "[ " << this->tokenSet[j].obj << "\t]";
			}
		}
		std::cout << std::endl << "PRI  : ";
		for (int j = 0; j < i; j++) {
			std::cout << "[ " << this->tokenSet[j].priority << "\t]";
		}
		std::cout << std::endl;
    }
	std::string SIKTokens::typeToString(sik::TokenTypes type) {
		switch (type) {
		case sik::KEYWORD: 
			return "KEY";
		case sik::DELI_BRKOPEN:
			return "BRKO";
		case sik::DELI_BRKCLOSE:
			return "BRKC";
		case sik::DELI_SBRKOPEN:
			return "SBRO";
		case sik::DELI_SBRKCLOSE:
			return "SBRC";
		case sik::DELI_MEMACCESS:
			return "MEMA";
		case sik::DELI_INCR:
			return "INCR";
		case sik::DELI_DECR:
			return "DECR";
		case sik::DELI_POW:
			return "POW";
		case sik::DELI_MULTI:
			return "MULT";
		case sik::DELI_DIVIDE:
			return "DIVI";
		case sik::DELI_PLUS:
			return "PLUS";
		case sik::DELI_MINUS:
			return "MINU";
		case sik::DELI_GRT:
			return "GRT";
		case sik::DELI_LST:
			return "LST";
		case sik::DELI_GRTE:
			return "GRTE";
		case sik::DELI_LSTE:
			return "LSTE";
		case sik::DELI_CTEQUAL:
			return "TEQ";
		case sik::DELI_CTNEQUAL:
			return "TNEQ";
		case sik::DELI_CEQUAL:
			return "CEQU";
		case sik::DELI_CNEQUAL:
			return "NEQU";
		case sik::DELI_CAND:
			return "AND";
		case sik::DELI_COR:
			return "COR";
		case sik::DELI_POINT:
			return "POI";
		case sik::DELI_EQUAL:
			return "EQU";
		case sik::DELI_EQUALADD:
			return "EQUA";
		case sik::DELI_EQUALSUB:
			return "EQUS";
		case sik::DELI_CHILDSET:
			return "CEQU";
		case sik::DELIMITER:
			return "DELI";
		case sik::DELI_COMMA:
			return "COMA";
		case sik::DELI_SCOPE:
			return "SCOP";
		case sik::DELI_BRCOPEN:
			return "BRCO";
		case sik::DELI_BRCCLOSE:
			return "BRCC";
		case sik::DELI_OPEND:
			return "OEND";
		case sik::NAMING:
			return "NAME";
		case sik::STRING:
			return "STR";
		case sik::NUMBER:
			return "NUM";
		case sik::BOOLEAN:
			return "BOOL";
		case sik::NULLTYPE:
			return "NULL";
		case sik::NOPE:
			return "NOPE";
		case sik::NODE:
			return "NODE";
		case sik::SBLOCK:
			return "BLOC";
		default:
			return "UNK";
		}
	}
    /*
     * Virtual Destructor
     */
    SIKTokens::~SIKTokens() {
        
    }
}