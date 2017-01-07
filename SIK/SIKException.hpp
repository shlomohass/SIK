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
        sik::ExcepTypes Type;

	public:

		SIKException();
        SIKException(sik::ExcepTypes type);
		SIKException(sik::ExcepTypes type, const std::string& message);
		SIKException(sik::ExcepTypes type, const std::string& message, int line);
		std::string what();
		std::string where();
		void render(int debug);
	};
}
#endif /* SIKException_hpp */