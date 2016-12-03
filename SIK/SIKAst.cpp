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


	SIKAst::~SIKAst()
	{
		// std::cout << "ASTNode delete - " << this->Value << std::endl;
		delete this->Left;
		delete this->Right;
		for (int i = 0; i < (int)this->bulk.size(); i++) {
			delete this->bulk[i];
		}
	}

}