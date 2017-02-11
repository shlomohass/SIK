//
//  SIKPluginManager.hpp
//  SIK
//
//  Created by Shlomo Hassid on 06/11/2016 .
//  Copyright � 2016 Shlomo Hassid. All rights reserved.
//

#ifndef SIKPluginManager_hpp
#define SIKPluginManager_hpp

#include "SIKPlugin.hpp"
#include "SIKPluginDetails.hpp"
#include <windows.h>
#include <iostream>
#include <vector>

namespace sik
{
	class PluginManager
	{
	private:

		void clearPluginList()
		{
			for (unsigned int i = 0; i < this->pluginRegister.size(); i++)
			{
				delete this->pluginRegister.at(i);
			}
			this->pluginRegister.clear();
		}

		std::vector <sik::PluginDetails*> pluginRegister;

	public:

		PluginManager() {}
		~PluginManager() { this->clearPluginList(); }
		void getPluginList(char * dirPath, bool addToList = false);
		int hasPlugin(const std::string& name);
		sik::PluginInterface * makeNewPluginInstance(int index)
		{
			return this->pluginRegister.at(index)->makeNewInstance();
		}
		std::string CombinePaths(std::string const &pattern, LPCSTR filename);
		int getNumPlugins()
		{
			return int(this->pluginRegister.size());
		}
		std::string getPluginName(int index)
		{
			return this->pluginRegister.at(index)->getName();
		}
		std::string getPluginType(int index)
		{
			return this->pluginRegister.at(index)->getType();
		}
	};
}

#endif /* SIKPluginManager_hpp */
