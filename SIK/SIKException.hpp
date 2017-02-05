//
//  SIKException.hpp
//  SIK
//
//  Created by Shlomo Hassid on 06/11/2016.
//  Copyright � 2016 Shlomo Hassid. All rights reserved.
//

#ifndef SIKException_hpp
#define SIKException_hpp

#include "SIKAdd.hpp"
#include <string>

namespace sik {

	class SIKException
	{
		std::string Message;
		int Line;
		int Inst;
        sik::ExcepTypes Type;

	public:

		SIKException();
        SIKException(sik::ExcepTypes type);
		SIKException(sik::ExcepTypes type, const std::string& message);
		SIKException(sik::ExcepTypes type, const std::string& message, int line);
		SIKException(sik::ExcepTypes type, const std::string& message, int line, int inst);
		std::string what();
		std::string where(); 
		std::string instruct();
		void render(int debug);
	};
}
#endif /* SIKException_hpp */