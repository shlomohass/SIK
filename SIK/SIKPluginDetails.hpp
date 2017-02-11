//
//  SIKPluginDetails.hpp
//  SIK
//
//  Created by Shlomo Hassid on 06/11/2016 .
//  Copyright � 2016 Shlomo Hassid. All rights reserved.
//

#ifndef SIKPluginDetails_hpp
#define SIKPluginDetails_hpp

#include "SIKPlugin.hpp"
#include <windows.h>
#include <iostream>

namespace sik
{
	class PluginDetails
	{

	private:

		void clearMembers()
		{
			this->pluginType = "";
			this->pluginName = "";
			this->filename = "";
			this->pluginPath = "";
			this->dllHandle = NULL;
		}

		std::string filename;
		std::string pluginType;
		std::string pluginName;
		std::string pluginPath;
		HINSTANCE dllHandle;
		PLUGIN_FACTORYFUNC funcHandle;

	public:

		PluginDetails() { this->clearMembers(); }

		~PluginDetails() { }

		sik::PluginInterface * makeNewInstance();

		std::string getName() { return this->pluginName; }
		std::string getType() { return this->pluginType; }
		std::string getPath() { return this->pluginPath; }
		void setFileName(char * nm) { this->filename = nm; }
		void setFilePath(char * nm) { this->pluginPath = nm; }
		void setName(char * nm) { this->pluginName = nm; }
		void setType(char * nm) { this->pluginType = nm; }
	};
}

#endif /* SIKPluginDetails_hpp */

