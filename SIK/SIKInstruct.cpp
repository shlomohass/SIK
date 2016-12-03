//
//  SIKInstruct.cpp
//  SIK
//
//  Created by Shlomo Hassid on 06/11/2016.
//  Copyright © 2016 Shlomo Hassid. All rights reserved.
//

#include "SIKInstruct.hpp"
#include "SIKLang.hpp"

namespace sik {

	SIKInstruct::SIKInstruct()
	{
		this->Type = sik::INS_NONE;
		this->SubType = sik::NOPE;
		this->Value = '\0';
		this->Block = sik::BLOCK_NONE;
		this->lineOrigin = -1;
		this->cache = -1;
	}
	SIKInstruct::SIKInstruct(sik::SIKAst* node, sik::InstructType type) {
		this->SubType = sik::NOPE;
		this->Block = node->Block;
		this->Value = node->Value;
		this->lineOrigin = node->line;
		this->Type = type;
	}
	SIKInstruct::SIKInstruct(sik::SIKAst* node)
	{
		this->SubType = sik::NOPE;
		this->Block = node->Block;
		this->Value = node->Value;
		this->lineOrigin = node->line;

		switch (node->Type) {
			case sik::DELI_BRCOPEN:
			case sik::SBLOCK:
				this->Type = sik::INS_OSBLOCK;
				break;
			case sik::DELI_BRCCLOSE:
				this->Type = sik::INS_CSBLOCK;
				break;
			case sik::NUMBER:
				this->Type = sik::INS_PUSH;
				this->SubType = sik::NUMBER;
				break;
			case sik::STRING:
				this->Type = sik::INS_PUSH;
				this->SubType = sik::STRING;
				break;
			case sik::BOOLEAN:
				this->Type = sik::INS_PUSH;
				this->SubType = sik::BOOLEAN;
				this->cache = sik::SIKLang::LangIsBoolean(this->Value);
				break;
			case sik::NULLTYPE:
				this->Type = sik::INS_PUSH;
				this->SubType = sik::NULLTYPE;
				break;
			case sik::NAMING:
				this->Type = sik::INS_PUSH;
				this->SubType = sik::NAMING;
				break;
			case sik::DELI_PLUS:
				this->Type = sik::INS_ADD;
				break;
			case sik::DELI_MINUS:
				this->Type = sik::INS_SUBTRACT;
				break;
			case sik::DELI_INCR:
				this->Type = sik::INS_INCREMENT;
				break;
			case sik::DELI_DECR:
				this->Type = sik::INS_DECREMENT;
				break;
			case sik::DELI_MULTI:
				this->Type = sik::INS_MULTI;
				break;
			case sik::DELI_DIVIDE:
				this->Type = sik::INS_DEVIDE;
				break;
			case sik::DELI_POW:
				this->Type = sik::INS_POW;
				break;
			case sik::DELI_EQUAL:
				this->Type = sik::INS_ASSIGN;
				break;
			case sik::DELI_EQUALADD:
				this->Type = sik::INS_ASSIGNADD;
				break;
			case sik::DELI_EQUALSUB:
				this->Type = sik::INS_ASSIGNSUB;
				break;
			case sik::DELI_POINT:
			case sik::DELI_BRKOPEN:
			case sik::DELI_BRKCLOSE:
			case sik::DELI_SBRKOPEN:
			case sik::DELI_SBRKCLOSE:
			case sik::DELI_EXCL:
			case sik::DELI_GRT:
			case sik::DELI_LST:
			case sik::DELI_GRTE:
			case sik::DELI_LSTE:
			case sik::DELI_COMMA:
			case sik::DELI_CTEQUAL:
			case sik::DELI_CTNEQUAL:
			case sik::DELI_CEQUAL:
			case sik::DELI_CNEQUAL:
			case sik::DELI_CAND:
			case sik::DELI_COR:
			case sik::DELI_OBJCALL:
			case sik::DELIMITER:
			case sik::KEYWORD:
			case sik::NODE:
			case sik::NOPE:
				this->Type = sik::INS_NONE;
				break;
		}
	}
	SIKInstruct::~SIKInstruct()
	{

	}
}