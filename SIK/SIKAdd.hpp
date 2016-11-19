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
    
    enum TokenTypes { 
		NODE,
		NOPE, 
		NUMBER, 
		STRING, 
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
			DELI_CTEQUAL,
			DELI_CTNEQUAL,
			DELI_CEQUAL,
			DELI_CNEQUAL,
			DELI_CAND,
			DELI_COR,
			DELI_OBJCALL
	};

}

#endif /* SIKAdd_hpp */
