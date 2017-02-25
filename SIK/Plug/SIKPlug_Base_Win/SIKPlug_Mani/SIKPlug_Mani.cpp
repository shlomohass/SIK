//
//  SIKPlug_Mani.cpp
//  SIK
//
//  Created by Shlomo Hassid on 06/11/2016.
//  Copyright � 2016 Shlomo Hassid. All rights reserved.
//

#include "SIKPlugin.hpp"
#include "SIKPlug_Mani.hpp"
#include <iostream>

DECLARE_PLUGIN(sik::SIKPlug_Mani)
SET_PLUGIN_TYPE("Plugin_Object")
SET_PLUGIN_NAME("Mani")

namespace sik {

	SIKPlug_Mani::SIKPlug_Mani()
	{
		this->lookup_members["concat"] = std::bind(&SIKPlug_Mani::concat, this, std::placeholders::_1);
	}


	SIKPlug_Mani::~SIKPlug_Mani()
	{
	}

	int SIKPlug_Mani::Activate()
	{
		std::cout << " Activate Mani" << std::endl;
		return 0;
	}

	int SIKPlug_Mani::Execute()
	{
		std::cout << " Execute Mani" << std::endl;
		return 0;
	}

	int SIKPlug_Mani::Destroy()
	{
		std::cout << " Destroy Mani" << std::endl;
		return 0;
	}

	//Special Methods:
	sik::SIKObj SIKPlug_Mani::concat(std::vector<sik::SIKObj> args) {
		if (args.size() < 2) return sik::SIKObj();
		sik::SIKObj result(args[0].getAsString() + args[1].getAsString());
		return result;
	}
}