//
//  SIKPlug_math.hpp
//  SIK
//
//  Created by Shlomo Hassid on 06/11/2016 .
//  Copyright � 2016 Shlomo Hassid. All rights reserved.
//

#ifndef SIKPlug_math_hpp
#define SIKPlug_math_hpp

#include "../SIKPlugin.h"

namespace sik
{
	class __declspec(dllexport) Plugin_math : public sik::PluginInterface
	{
		public:
		Plugin_math();
		virtual ~Plugin_math();

		int Activate();
		int Execute();
		int Destroy();
	};
}

#endif /* SIKPlug_math_hpp */
