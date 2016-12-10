//
//  SIKAst.cpp
//  SIK
//
//  Created by Shlomo Hassid on 06/11/2016.
//  Copyright © 2016 Shlomo Hassid. All rights reserved.
//

#include "SIKAst.hpp"
#include <iostream>

namespace sik {


	SIKAst::SIKAst()
	{
		this->PreventBulkDelete = false;
		this->Type		= sik::NOPE;
		this->Value		= "\0";
		this->Block		= sik::BLOCK_NONE;
		this->Priority	= -1;
		this->line		= -1;
		this->Left		= nullptr;
		this->Right		= nullptr;
		this->Parent	= nullptr;
		this->Mark		= false;
	}

	void SIKAst::mutateTo(SIKAst* target) {
		this->PreventBulkDelete = target->PreventBulkDelete;
		this->Type = target->Type;
		this->Value = target->Value;
		this->Block = target->Block;
		this->Priority = target->Priority;
		this->line = target->line;
		this->Left = target->Left;
		this->Right = target->Right;
		this->Parent = target->Parent;
		this->Mark = target->Mark;
	}
	SIKAst::~SIKAst()
	{
		//std::cout << "ASTNode delete - " << this->Value << std::endl;
		delete this->Left;
		delete this->Right;
		if (!this->PreventBulkDelete) {
			for (int i = 0; i < (int)this->bulk.size(); i++) {
				delete this->bulk[i];
			}
		}
	}

}