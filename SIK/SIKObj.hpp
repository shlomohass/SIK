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
#include "SIKInstruct.hpp"
#include <string>
#include <vector>
#include <unordered_map>

namespace sik
{

	class SIKObj
	{

	public:

		sik::ObjectTypes							 Type;
		double										 Number;
		std::string									 String;
		std::vector<sik::SIKObj>					 Array;
		std::unordered_map<std::string, sik::SIKObj> Obj;
		std::pair<int, std::string>					 Func;
		std::vector<sik::SIKInstruct>*				 FuncSpace;
		bool										 isPerma; //This is a special flag for preventing scope cleaning such as in for loops definition;
		

		SIKObj();
		SIKObj(int value);
		SIKObj(double value);
		SIKObj(const std::string& value);
		SIKObj(bool value);
		SIKObj(const std::pair<int, std::string> _func, std::vector<sik::SIKInstruct>* _space);
		SIKObj(sik::ObjectTypes _type, const std::string& value);
		SIKObj(sik::ObjectTypes _type);

		//Flags:

        //Methods:
        double getAsNumber();
        std::string getAsString();
        double getAsBool();
		void pushToArray(const sik::SIKObj& _obj);
		sik::SIKObj* setInObject(const std::string& name);
		void setInObject(const std::string& name,sik::SIKObj* _obj);
		sik::SIKObj* getFromObject(const std::string& name);

        void mutate(sik::SIKObj* obj);
		virtual ~SIKObj();
	};
}

#endif /* SIKObj_hpp */
