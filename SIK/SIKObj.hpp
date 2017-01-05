//
//  SIKObj.hpp
//  SIK
//
//  Created by Shlomo Hassid on 06/11/2016 .
//  Copyright © 2016 Shlomo Hassid. All rights reserved.
//

#ifndef SIKObj_hpp
#define SIKObj_hpp

#include "SIKAdd.hpp"
#include <string>

namespace sik
{
	class SIKObj
	{

	public:

		sik::ObjectTypes Type;
		float			 Number;
		std::string      String;


		SIKObj();
		SIKObj(int value);
		SIKObj(float value);
		SIKObj(std::string value);
		SIKObj::SIKObj(sik::ObjectTypes, std::string value);

		virtual ~SIKObj();
	};
}

#endif /* SIKObj_hpp */
