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
			case DELI_BRCOPEN:
			case SBLOCK:
				this->Type = sik::INS_OSBLOCK;
				break;
			case DELI_BRCCLOSE:
				this->Type = sik::INS_CSBLOCK;
				break;
			case NUMBER:
				this->Type = sik::INS_PUSH;
				this->SubType = sik::NUMBER;
				break;
			case STRING:
				this->Type = sik::INS_PUSH;
				this->SubType = sik::STRING;
				break;
			case NAMING:
				this->Type = sik::INS_PUSH;
				this->SubType = sik::NAMING;
				break;
			case DELI_PLUS:
				this->Type = sik::INS_ADD;
				break;
			case DELI_MINUS:
				this->Type = sik::INS_SUBTRACT;
				break;
			case DELI_INCR:
				this->Type = sik::INS_INCREMENT;
				break;
			case DELI_DECR:
				this->Type = sik::INS_DECREMENT;
				break;
			case DELI_MULTI:
				this->Type = sik::INS_MULTI;
				break;
			case DELI_DIVIDE:
				this->Type = sik::INS_DEVIDE;
				break;
			case DELI_POW:
				this->Type = sik::INS_POW;
				break;
			case DELI_EQUAL:
				this->Type = sik::INS_ASSIGN;
				break;
			case DELI_EQUALADD:
				this->Type = sik::INS_ASSIGNADD;
				break;
			case DELI_EQUALSUB:
				this->Type = sik::INS_ASSIGNSUB;
				break;
			case DELI_POINT:
			case DELI_BRKOPEN:
			case DELI_BRKCLOSE:
			case DELI_SBRKOPEN:
			case DELI_SBRKCLOSE:
			case DELI_EXCL:
			case DELI_GRT:
			case DELI_LST:
			case DELI_GRTE:
			case DELI_LSTE:
			case DELI_COMMA:
			case DELI_CTEQUAL:
			case DELI_CTNEQUAL:
			case DELI_CEQUAL:
			case DELI_CNEQUAL:
			case DELI_CAND:
			case DELI_COR:
			case DELI_OBJCALL:
			case DELIMITER:
			case KEYWORD:
			case NODE:
			case NOPE:
				this->Type = sik::INS_NONE;
				break;
		}
	}
	SIKInstruct::~SIKInstruct()
	{

	}
}