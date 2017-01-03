//
//  SIKAdd.hpp
//  SIK
//
//  Created by Shlomo Hassid on 06/11/2016 .
//  Copyright © 2016 Shlomo Hassid. All rights reserved.
//

#ifndef SIKAdd_hpp
#define SIKAdd_hpp

namespace sik
{
    
    enum OperatingModes { ODEBUG, ONORMAL };

    enum ExecCodes { SUCCESS, GENEROOR };
    
	enum InstructType {
		INS_NONE,
		INS_PUSH,

		INS_ADD,
		INS_SUBTRACT,
		INS_MULTI,
		INS_DEVIDE,
		INS_POW,

		INS_INCREMENT,
		INS_DECREMENT,
		INS_PINCREMENT,
		INS_PDECREMENT,

		INS_CEQUAL,
		INS_CNEQUAL,
		INS_CTEQUAL,
		INS_CNTEQUAL,
		INS_CGRT,
		INS_CLST,
		INS_CGRTE,
		INS_CLSTE,
		INS_CAND,
		INS_COR,

		INS_ASSIGN,
		INS_ASSIGNADD,
		INS_ASSIGNSUB,
		INS_CHILDASSIGN,

		INS_DEFINE, //OBJECTS
		INS_OBJCREATE,
		INS_OBJDONE,
		INS_IF,     //IFs
		INS_ELSEIF,
		INS_ELSE,

		INS_FORL, //FOR
		INS_FRCO,
		INS_FRCA,

		INS_WHLL, //WHILE
		INS_WHLM, //WHILE MAX part

		INS_EACH, //EACH Loop
		INS_EACHI, //EACH Index
		INS_EACHE, //EACH Element

		INS_BRKL,  // MAke a LOOP break:
		INS_DOBRK, // Execute Break LOOP
		 
		INS_BRKC,  // BREAK Condition
		INS_LCON,  // Do a loop continue
		
		INS_PRINT,
		INS_DOPRINT,

		INS_OSBLOCK, //Auto scope block
		INS_OBLOCK,  //Forced scope block
		INS_CBLOCK,  //General block close.

	};
	enum BlocksIn {
		BLOCK_NONE,
		BLOCK_IF,
		BLOCK_ELSE,
		BLOCK_ELSEIF,
		BLOCK_OBJ,
		BLOCK_FUNC,
		BLOCK_WHILE,
		BLOCK_FOR,
		BLOCK_EACH
	};
    enum TokenTypes {
		NODE,
		SBLOCK,
		NOPE, 
		NUMBER, 
		STRING,
		BOOLEAN,
		NULLTYPE,
		KEYWORD,  
		NAMING, 
		DELIMITER,
		DELI_PLUS,
		DELI_MINUS,
		DELI_INCR,
		DELI_DECR,
		DELI_MULTI,
		DELI_DIVIDE,
		DELI_EQUAL,
		DELI_EQUALADD,
		DELI_EQUALSUB,
		DELI_POINT,
		DELI_CHILDSET,
		DELI_BRKOPEN,
		DELI_BRKCLOSE,
		DELI_BRCOPEN,
		DELI_BRCCLOSE,
		DELI_SBRKOPEN,
		DELI_SBRKCLOSE,
		DELI_POW,
		DELI_EXCL,
		DELI_GRT,
		DELI_LST,
		DELI_GRTE,
		DELI_LSTE,
		DELI_COMMA,
		DELI_SCOPE,
		DELI_CTEQUAL,
		DELI_CTNEQUAL,
		DELI_CEQUAL,
		DELI_CNEQUAL,
		DELI_CAND,
		DELI_COR,
		DELI_OBJCALL,
		DELI_OPEND
	};

}

#endif /* SIKAdd_hpp */
