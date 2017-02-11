//
//  SIKPlug_math.cpp
//  SIK
//
//  Created by Shlomo Hassid on 06/11/2016.
//  Copyright � 2016 Shlomo Hassid. All rights reserved.
//

#include "../SIKPlugin.h"
#include "SIKPlug_math.h"
#include <iostream>

DECLARE_PLUGIN(sik::Plugin_math)
SET_PLUGIN_TYPE("Plugin_math_Class")
SET_PLUGIN_NAME("Plugin_math")

namespace sik {

	Plugin_math::Plugin_math()
	{

	}

	Plugin_math::~Plugin_math()
	{

	}

	int Plugin_math::Activate()
	{
		std::cout << " Activate math" << std::endl;
		return 0;
	}

	int Plugin_math::Execute()
	{
		std::cout << " Execute math" << std::endl;
		return 0;
	}

	int Plugin_math::Destroy()
	{
		std::cout << " Destroy math" << std::endl;
		return 0;
	}

}