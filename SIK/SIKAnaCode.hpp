//
//  SIKAnaCode.hpp
//  SIK
//
//  Created by Shlomo Hassid on 06/11/2016 .
//  Copyright © 2016 Shlomo Hassid. All rights reserved.
//

#ifndef SIKAnaCode_hpp
#define SIKAnaCode_hpp

#include "SIKInstruct.hpp"
#include <ostream>
#include <vector>
#include <map>

namespace sik
{
	class SIKAnaCode
	{
		std::vector<sik::BlocksIn> BlockInCheck;
		int InternalJumper;

		//From Script containers:
		std::vector<sik::SIKInstruct>* Instructions;
		std::vector<std::vector<sik::SIKInstruct>>* ObjectDefinitions;
		std::map<std::string, std::vector<sik::SIKInstruct>>* FunctionInstructions;

		public:

		SIKAnaCode();
		SIKAnaCode(std::vector<sik::SIKInstruct>* _Instructions, std::vector<std::vector<sik::SIKInstruct>>* _ObjectDefinitions, std::map<std::string, std::vector<sik::SIKInstruct>>* _FunctionInstructions, int jumperSet);
		
		void splitCodeChunks(std::vector<sik::SIKInstruct>* _Instructions);
		void postCompiler(std::vector<sik::SIKInstruct>* _Instructions);
		void setMatchigJump(int i, sik::SIKInstruct* inst, std::vector<sik::SIKInstruct>* _Instructions);
		
		virtual ~SIKAnaCode();

	};
}
#endif /* SIKAnaCode_hpp */