//
//  SIKInstruct.cpp
//  SIK
//
//  Created by Shlomo Hassid on 06/11/2016.
//  Copyright © 2016 Shlomo Hassid. All rights reserved.
//

#include "SIKInstruct.hpp"

namespace sik {

	SIKInstruct::SIKInstruct()
	{
		this->Type = sik::INS_NONE;
		this->SubType = sik::NOPE;
		this->Value = '\0';
		this->Block = sik::BLOCK_NONE;
		this->lineOrigin = -1;
		this->cache = -1;
		this->pointToInstruct = -1;
		this->InternalJumper = -1;
		this->MyInternalNumber = -1;
	}
	SIKInstruct::~SIKInstruct()
	{

	}
}