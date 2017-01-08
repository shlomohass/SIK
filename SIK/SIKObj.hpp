//
//  SIKObj.hpp
//  SIK
//
//  Created by Shlomo Hassid on 06/11/2016 .
//  Copyright � 2016 Shlomo Hassid. All rights reserved.
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
		double			 Number;
		std::string      String;


		SIKObj();
		SIKObj(int value);
		SIKObj(double value);
		SIKObj(std::string value);
		SIKObj(bool value);
		SIKObj(sik::ObjectTypes, std::string value);
        
        //Methods:
        double getAsNumber();
        std::string getAsString();
        double getAsBool();
        
        void mutate(SIKObj* obj);
		virtual ~SIKObj();
	};
}

#endif /* SIKObj_hpp */
