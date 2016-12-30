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
		this->InsBlockPointer = -1;
		this->InternalJumper = -1;
		this->MyInternalNumber = -1;
		this->preVariable = false;
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
		this->InsBlockPointer = target->InsBlockPointer;
		this->InternalJumper = target->InternalJumper;
		this->MyInternalNumber = target->MyInternalNumber;
		this->preVariable = target->preVariable;
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