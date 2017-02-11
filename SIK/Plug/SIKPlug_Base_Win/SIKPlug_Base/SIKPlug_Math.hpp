﻿//
//  SIKPlug_Math.hpp
//  SIK
//
//  Created by Shlomo Hassid on 06/11/2016 .
//  Copyright � 2016 Shlomo Hassid. All rights reserved.
//

#ifndef SIKPlug_Math_hpp
#define SIKPlug_Math_hpp

#include "SIKPlugin.hpp"

namespace sik
{

	class SIKPlug_Math : public sik::PluginInterface
	{
	public:
		SIKPlug_Math();
		virtual ~SIKPlug_Math();

		int Activate();
		int Execute();
		int Destroy();
	};

}

#endif /* SIKPlug_Math_hpp */