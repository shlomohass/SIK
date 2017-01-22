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
		bool				 PreventBulkDelete; //Prevents Bulk delete in virtual destructor
		sik::TokenTypes		 Type;				//The node type
		std::string			 Value;				//Native string value
		sik::BlocksIn        Block;				//Block definition
		int					 Priority;			//The token original priority
		int					 line;				//The source line number
		sik::SIKAst*		 Left;				// Left hand.
		sik::SIKAst*		 Right;				// Right hand
		sik::SIKAst*		 Parent;			// The node parent
		std::vector<SIKAst*> bulk;				// Bulk nodes to parse
		std::pair<int, std::string> funcName;	// Use to store the key which we used for the func name store.
		bool				 Mark;				// Flag when finished parsing
		int                  InsBlockPointer;	// INSPO -> point to an object deleration space
		int                  InternalJumper;	// Wher the end of the node finishes
		int					 MyInternalNumber;	// Gives a unique number as a junper
		bool				 preVariable;		// Has a variable attached
		int					 Notation;			// Dynamic space to mark some stuff

		
		SIKAst();
		void mutateTo(SIKAst* target);

		virtual ~SIKAst();
	};

}
#endif /* SIKAst_hpp */