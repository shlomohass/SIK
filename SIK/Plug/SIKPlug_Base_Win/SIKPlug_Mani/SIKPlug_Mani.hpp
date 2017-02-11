//
//  SIKPlug_Mani.hpp
//  SIK
//
//  Created by Shlomo Hassid on 06/11/2016 .
//  Copyright � 2016 Shlomo Hassid. All rights reserved.
//

#ifndef SIKPlug_Mani_hpp
#define SIKPlug_Mani_hpp

#include "SIKPlugin.hpp"

namespace sik
{

	class SIKPlug_Mani : public sik::PluginInterface
	{
	public:
		SIKPlug_Mani();
		virtual ~SIKPlug_Mani();

		int Activate();
		int Execute();
		int Destroy();
	};

}

#endif /* SIKPlug_Mani_hpp */