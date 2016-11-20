//
//  SIKAst.hpp
//  SIK
//
//  Created by Shlomo Hassid on 06/11/2016.
//  Copyright © 2016 Shlomo Hassid. All rights reserved.
//

#ifndef SIKAst_hpp
#define SIKAst_hpp

#include "SIKAdd.hpp"
#include <string>
#include <vector>

namespace sik {

	class SIKAst
	{
	public:

		sik::TokenTypes		 Type;
		std::string			 Value;
		int					 Priority;
		int					 line;
		sik::SIKAst*		 Left;
		sik::SIKAst*		 Right;
		std::vector<SIKAst*> bulk;
		bool				 Mark;

		SIKAst();
		virtual ~SIKAst();
	};

}
#endif /* SIKAst_hpp */