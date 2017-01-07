//
//  SIKException.cpp
//  SIK
//
//  Created by Shlomo Hassid on 06/11/2016.
//  Copyright � 2016 Shlomo Hassid. All rights reserved.
//

#include "SIKException.hpp"

#include <iostream>

namespace sik {

	SIKException::SIKException()
	{
        this->Type = sik::EXC_GENERAL;
		this->Message = "Unknown Exeception occured.";
		this->Line = -1;
	}
    SIKException::SIKException(sik::ExcepTypes type)
    {
        this->Type = type;
        this->Message = "Unknown Exeception occured.";
        this->Line = -1;
    }
	SIKException::SIKException(sik::ExcepTypes type, const std::string& message)
	{
        this->Type = type;
		this->Message = message;
		this->Line = -1;
	}
	SIKException::SIKException(sik::ExcepTypes type, const std::string& message, int line)
	{
        this->Type = type;
		this->Message = message;
		this->Line = line;
	}
	std::string SIKException::what()
	{
        switch (this->Type) {
            case sik::EXC_GENERAL:
                return "SIK GENERAL EXCEPTION -> " + this->Message + " ";
                break;
            case sik::EXC_COMPILATION:
                return "SIK COMPILATION EXCEPTION -> " + this->Message + " ";
                break;
            case sik::EXC_RUNTIME:
                return "SIK RUNTIME EXCEPTION -> " + this->Message + " ";
                break;
        }
		
	}
	std::string SIKException::where()
	{
		return "[ Line : " + std::to_string(this->Line) + " ] ";
	}
	void SIKException::render(int debug) {
		if (debug > 0)
			std::cout << this->what() << this->where() << std::endl;
	}
}