//
//  SIKAst.hpp
//  SIK
//
//  Created by Shlomo Hassid on 06/11/2016.
//  Copyright � 2016 Shlomo Hassid. All rights reserved.
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
		bool				 PreventBulkDelete;
		sik::TokenTypes		 Type;
		std::string			 Value;
		sik::BlocksIn        Block;
		int					 Priority;
		int					 line;
		sik::SIKAst*		 Left;
		sik::SIKAst*		 Right;
		sik::SIKAst*		 Parent;
		std::vector<SIKAst*> bulk;
		bool				 Mark;
		int                  InsBlockPointer;
		int                  InternalJumper;
		int					 MyInternalNumber;
		bool				 preVariable;
		int					 Notation;

		
		SIKAst();
		void mutateTo(SIKAst* target);

		virtual ~SIKAst();
	};

}
#endif /* SIKAst_hpp */