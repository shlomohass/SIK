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
		this->FuncSpace = nullptr;
	}
    SIKObj::SIKObj(int value) {
        this->Type = sik::OBJ_NUMBER;
        this->Number = (double)value;
		this->FuncSpace = nullptr;
    }
	SIKObj::SIKObj(double value) {
		this->Type = sik::OBJ_NUMBER;
		this->Number = value;
		this->FuncSpace = nullptr;
	}
	SIKObj::SIKObj(const std::string& value) {
		this->Type = sik::OBJ_STRING;
		this->String = value;
        this->Number = SIK_NAN;
		this->FuncSpace = nullptr;
	}
	SIKObj::SIKObj(bool value) {
		this->Type = sik::OBJ_BOOL;
		this->Number = value ? 1 : 0;
		this->FuncSpace = nullptr;
	}
	SIKObj::SIKObj(const std::pair<int, std::string> _func, std::vector<sik::SIKInstruct>* _space) {
		this->Type = sik::OBJ_FUNC;
		this->Number = SIK_NAN;
		this->Func = _func;
		this->FuncSpace = _space;
	}
	SIKObj::SIKObj(sik::ObjectTypes _type, const std::string& value) {
		this->Type = _type;
		this->FuncSpace = nullptr;
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
	SIKObj::SIKObj(sik::ObjectTypes _type) {
		this->Type = _type;
		this->Number = SIK_NAN;
		this->FuncSpace = nullptr;
	}
    /* Always return as a number
     */
    double SIKObj::getAsNumber() {
        switch (this->Type) {
            case sik::OBJ_STRING:
                return (double)this->String.length();
                break;
			case sik::OBJ_ARRAY:
				return (double)this->Array.size();
				break;
			case sik::OBJ_FUNC:
            case sik::OBJ_BOOL:
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
			case sik::OBJ_ARRAY: {
				std::stringstream ss;
				ss << "ARRAY[" << (int)this->Array.size() << "]";
				return ss.str();
			} break;
			case sik::OBJ_FUNC: {
				return "FUNC[" + this->Func.second + "]";
			} break;
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
				return this->String.empty() ? 0 : 1;
			case sik::OBJ_ARRAY:
				return this->Array.empty() ? 0 : 1;
            default:
                return 0;
        }
    }
	void SIKObj::pushToArray(sik::SIKObj _obj) {
		this->Array.push_back(_obj);
	}
    void SIKObj::mutate(SIKObj* obj) {
        this->Type = obj->Type;
        this->Number = obj->Number;
        this->String = obj->String;
		this->Array = obj->Array;
		this->Func = obj->Func;
		this->FuncSpace = obj->FuncSpace;
    }

	SIKObj::~SIKObj()
	{
        //std::cout << "delete obj " << this->Type << std::endl;
	}
}