//
//  SIKPlug_Math.cpp
//  SIK
//
//  Created by Shlomo Hassid on 06/11/2016.
//  Copyright � 2016 Shlomo Hassid. All rights reserved.
//

#include "SIKPlugin.hpp"
#include "SIKPlug_Math.hpp"
#include <iostream>

DECLARE_PLUGIN(sik::SIKPlug_Math)
SET_PLUGIN_TYPE("Plugin_Class")
SET_PLUGIN_NAME("Math")

namespace sik {

	SIKPlug_Math::SIKPlug_Math()
	{
	}


	SIKPlug_Math::~SIKPlug_Math()
	{
	}

	int SIKPlug_Math::Activate()
	{
		std::cout << " Activate Math" << std::endl;
		return 0;
	}

	int SIKPlug_Math::Execute()
	{
		std::cout << " Execute Math" << std::endl;
		return 0;
	}

	int SIKPlug_Math::Destroy()
	{
		std::cout << " Destroy Math" << std::endl;
		return 0;
	}
}