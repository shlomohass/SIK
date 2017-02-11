//
//  SIKPluginManager.cpp
//  SIK
//
//  Created by Shlomo Hassid on 06/11/2016.
//  Copyright � 2016 Shlomo Hassid. All rights reserved.
//

#include "SIKPluginManager.hpp"
#include "SIKPluginDetails.hpp"
#include <windows.h>
#include <iostream>
//#include <commdlg.h>
#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")


namespace sik {


	void PluginManager::getPluginList(char * dirPath, bool addToList)
	{
		if (!addToList) this->clearPluginList();

		//Construct the directory path and use a wildcard
		char fname[MAX_PATH];
		char path[MAX_PATH];
		strcpy_s(fname, dirPath);
		strcpy_s(path, dirPath);

		size_t len = std::strlen(fname);
		if (fname[len - 1] == '/' || fname[len - 1] == '\\') strcat_s(fname, "*.dll");
		else strcat_s(fname, "\\*.dll");

		size_t len_path = std::strlen(path);
		if (path[len - 1] == '/' || path[len - 1] == '\\') strcat_s(path, "");
		else strcat_s(path, "\\");


		WIN32_FIND_DATA fd;
		HANDLE hFind = FindFirstFile(fname, &fd);

		if (hFind == INVALID_HANDLE_VALUE)
		{
			FindClose(hFind);
			return;
		}

		do
		{
			HINSTANCE dllHandle = NULL;

			//Use a 'try' just in case there are any badly formed DLLs
			try
			{
				if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
				{
					dllHandle = LoadLibrary(this->CombinePaths(path, fd.cFileName).c_str());
					if (dllHandle != NULL)
					{
						PLUGIN_FACTORYFUNC funcHandle;
						funcHandle = reinterpret_cast<PLUGIN_FACTORYFUNC>
							(GetProcAddress(dllHandle, "makePlugin"));

						//Check to see if the factory function is present                 
						if (funcHandle != NULL)
						{
							PluginDetails * curPlugin = new PluginDetails();

							//Set the plugin file name:
							curPlugin->setFileName(fd.cFileName);

							//Set the full path:
							curPlugin->setFilePath(strdup(this->CombinePaths(path, fd.cFileName).c_str()));

							PLUGIN_TEXTFUNC textFunc;
							//Store the plugin name
							textFunc = reinterpret_cast<PLUGIN_TEXTFUNC>
								(GetProcAddress(dllHandle, "getPluginName"));
							curPlugin->setName(textFunc());

							//Store the plugin type
							textFunc = reinterpret_cast<PLUGIN_TEXTFUNC>
								(GetProcAddress(dllHandle, "getPluginType"));
							curPlugin->setType(textFunc());

							//Add the plugin
							this->pluginRegister.push_back(curPlugin);
						}
						FreeLibrary(dllHandle);
					}
				}
			}
			catch (...)
			{
				if (dllHandle != NULL) FreeLibrary(dllHandle);
			}

		} while (FindNextFile(hFind, &fd));
		FindClose(hFind);
	}

	std::string PluginManager::CombinePaths(std::string const &pattern, LPCSTR filename) {
		std::string tmp(pattern);
		tmp.push_back('\0');
		PathRemoveFileSpec(&tmp[0]);
		std::string retVal(MAX_PATH, '\0');
		PathCombine(&retVal[0], tmp.c_str(), filename);
		return retVal.c_str();
	}

	int PluginManager::hasPlugin(const std::string& name) {
		unsigned int all = this->getNumPlugins();
		for (unsigned int i = 0; i < all; i++) {
			if (this->pluginRegister.at(i)->getName() == name) {
				return i;
			}
		}
		return -1;
	}
}