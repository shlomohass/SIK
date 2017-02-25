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
#include <math.h>       /* pow */

DECLARE_PLUGIN(sik::SIKPlug_Math)
SET_PLUGIN_TYPE("Plugin_Class")
SET_PLUGIN_NAME("Math")

namespace sik {

	SIKPlug_Math::SIKPlug_Math()
	{
		this->lookup_members["pow"] = std::bind(&SIKPlug_Math::pow, this, std::placeholders::_1);
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

	//Special Methods:
	sik::SIKObj SIKPlug_Math::pow(std::vector<sik::SIKObj> args) {
		if (args.size() < 2) return sik::SIKObj();
		sik::SIKObj result(std::pow(args[0].getAsNumber(), args[1].getAsNumber()));
		return result;
	}
}