//
//  SIKObj.cpp
//  SIK
//
//  Created by Shlomo Hassid on 06/11/2016.
//  Copyright � 2016 Shlomo Hassid. All rights reserved.
//

#include "SIKObj.hpp"

#include <iostream>
#include <sstream>
#include <stdlib.h> // atof

namespace sik {

	SIKObj::SIKObj() {
        this->Number = SIK_NAN;
		this->Type = sik::OBJ_NAN;
	}
    SIKObj::SIKObj(int value) {
        this->Type = sik::OBJ_NUMBER;
        this->Number = (double)value;
    }
	SIKObj::SIKObj(double value) {
		this->Type = sik::OBJ_NUMBER;
		this->Number = value;
	}
	SIKObj::SIKObj(std::string value) {
		this->Type = sik::OBJ_STRING;
		this->String = value;
        this->Number = SIK_NAN;
	}
	SIKObj::SIKObj(bool value) {
		this->Type = sik::OBJ_BOOL;
		this->Number = value ? 1 : 0;
	}
	SIKObj::SIKObj(sik::ObjectTypes _type, std::string value) {
		this->Type = _type;
		switch (_type) {
			case sik::OBJ_NUMBER:
					this->Number = std::atof(value.c_str());
				break;
			case sik::OBJ_STRING:
				this->String = value;
                this->Number = SIK_NAN;
				break;
			default:
                this->Number = SIK_NAN;
				this->Type = OBJ_NAN;
		}
	}
    /* Always return as a number
     */
    double SIKObj::getAsNumber() {
        switch (this->Type) {
            case sik::OBJ_STRING:
                return (double)this->String.length();
                break;
            case sik::OBJ_BOOL:
            case sik::OBJ_ARRAY:
            case sik::OBJ_NULL:
            default:
                return this->Number;
        }
    }
    /* Always return as a string
     */
    std::string SIKObj::getAsString() {
        
        switch (this->Type) {
            case sik::OBJ_NUMBER: {
                std::stringstream ss;
                ss << this->Number;
                return ss.str();
                }
                break;
            case sik::OBJ_BOOL:
                if (this->Number > 0) { return "TRUE"; }
                return "FALSE";
                break;
            case sik::OBJ_NULL:
            default:
                return this->String;
        }
    }
    /* Always return as a Boolean
     */
    double SIKObj::getAsBool() {
        switch (this->Type) {
            case sik::OBJ_BOOL:
                return this->Number;
			case sik::OBJ_NUMBER:
				return this->Number == 0 ? 0 : 1;
			case sik::OBJ_STRING:
				return this->String.length() > 0 ? 1 : 0;
			case sik::OBJ_ARRAY:
				return 1;
            default:
                return 0;
        }
    }
    void SIKObj::mutate(SIKObj* obj) {
        this->Type = obj->Type;
        this->Number = obj->Number;
        this->String = obj->String;
    }
	SIKObj::~SIKObj()
	{
        std::cout << "delete obj " << this->Type << std::endl;
	}
}