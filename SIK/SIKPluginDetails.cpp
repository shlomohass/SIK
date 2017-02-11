//
//  SIKPluginDetails.cpp
//  SIK
//
//  Created by Shlomo Hassid on 06/11/2016.
//  Copyright � 2016 Shlomo Hassid. All rights reserved.
//

#include "SIKPluginDetails.hpp"
#include <windows.h>
#include <iostream>


namespace sik {

	sik::PluginInterface * PluginDetails::makeNewInstance()
	{
		if (!this->dllHandle) this->dllHandle = LoadLibrary(this->pluginPath.c_str());

		if (this->dllHandle != NULL)
		{
			this->funcHandle = reinterpret_cast<PLUGIN_FACTORYFUNC>
				(GetProcAddress(this->dllHandle, "makePlugin"));
			if (this->funcHandle != NULL)
			{
				return this->funcHandle();
			}
		}
		return nullptr;
	}
}