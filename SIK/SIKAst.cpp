//
//  SIKAst.cpp
//  SIK
//
//  Created by Shlomo Hassid on 06/11/2016.
//  Copyright © 2016 Shlomo Hassid. All rights reserved.
//

#include "SIKAst.hpp"


namespace sik {


	SIKAst::SIKAst()
	{
		this->Type = NOPE;
		this->Value = "\0";
		this->Priority = -1;
		this->line = -1;
		this->Left = nullptr;
		this->Right = nullptr;
	}


	SIKAst::~SIKAst()
	{
		delete this->Left;
		delete this->Right;
	}

}