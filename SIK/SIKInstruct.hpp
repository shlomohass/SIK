//
//  SIKInstruct.hpp
//  SIK
//
//  Created by Shlomo Hassid on 06/11/2016.
//  Copyright © 2016 Shlomo Hassid. All rights reserved.
//

#ifndef SIKInstruct_hpp
#define SIKInstruct_hpp

#include "SIKAdd.hpp"
#include "SIKAst.hpp"
#include <string>

namespace sik {

	class SIKInstruct
	{
		public:
			
			sik::InstructType	 Type;
			sik::TokenTypes      SubType;
			std::string			 Value;
			sik::BlocksIn        Block;
			int					 lineOrigin;

			SIKInstruct();
			SIKInstruct(sik::SIKAst* node);
			SIKInstruct(sik::SIKAst* node, sik::InstructType type);
			virtual ~SIKInstruct();
	};

}

#endif /* SIKInstruct_hpp */