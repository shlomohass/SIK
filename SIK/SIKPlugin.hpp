//
//  SIKPlugin.hpp
//  SIK
//
//  Created by Shlomo Hassid on 06/11/2016 .
//  Copyright � 2016 Shlomo Hassid. All rights reserved.
//

#ifndef SIKPlugin_hpp
#define SIKPlugin_hpp


#define DECLARE_PLUGIN(x) extern "C"{ __declspec(dllexport) sik::PluginInterface * makePlugin(){ return new x; } }
#define SET_PLUGIN_NAME(x) extern "C"{ __declspec(dllexport) char * getPluginName(){ return x; } }
#define SET_PLUGIN_TYPE(x) extern "C"{ __declspec(dllexport) char * getPluginType(){ return x; } }

namespace sik
{
	class PluginInterface
	{
	public:
		PluginInterface() {}
		virtual ~PluginInterface() {}

		virtual int Activate() = 0;
		virtual int Execute() = 0;
		virtual int Destroy() = 0;
	};

	typedef PluginInterface * (*PLUGIN_FACTORYFUNC)();
	typedef char * (*PLUGIN_TEXTFUNC)();
}

#endif /* SIKPlugin_hpp */
