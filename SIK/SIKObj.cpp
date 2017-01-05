//
//  SIKObj.cpp
//  SIK
//
//  Created by Shlomo Hassid on 06/11/2016.
//  Copyright © 2016 Shlomo Hassid. All rights reserved.
//

#include "SIKObj.hpp"

#include <iostream>
#include <stdlib.h> // atof

namespace sik {

	SIKObj::SIKObj() {
		this->Type = sik::OBJ_NAN;
	}
	SIKObj::SIKObj(float value) {
		this->Type = sik::OBJ_NUMBER;
		this->Number = value;
	}
	SIKObj::SIKObj(std::string value) {
		this->Type = sik::OBJ_STRING;
		this->String = value;
	}
	SIKObj::SIKObj(sik::ObjectTypes _type, std::string value) {
		this->Type = _type;
		switch (_type) {
			case sik::OBJ_NUMBER:
					this->Number = std::atof(value.c_str());
				break;
			case sik::OBJ_STRING:
				this->String = value;
				break;
			default:
				this->Type = OBJ_NAN;
		}
	}
	SIKObj::~SIKObj()
	{

	}
}