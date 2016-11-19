//
//  SIKException.cpp
//  SIK
//
//  Created by Shlomo Hassid on 06/11/2016.
//  Copyright © 2016 Shlomo Hassid. All rights reserved.
//

#include "SIKException.hpp"

#include <iostream>

namespace sik {

	SIKException::SIKException()
	{
		this->message = "Unknown Exeception occured.";
		this->line = -1;
	}
	SIKException::SIKException(const std::string& message)
	{
		this->message = message;
		this->line = -1;
	}
	SIKException::SIKException(const std::string& message, int line)
	{
		this->message = message;
		this->line = line;
	}
	std::string SIKException::what()
	{
		return "SIK EXCEPTION -> " + this->message + " ";
	}
	std::string SIKException::where()
	{
		return "[ In Line : " + std::to_string(this->line) + " ] ";
	}
	void SIKException::render(int debug) {
		if (debug > 0)
			std::cout << this->what() << this->where() << std::endl;
	}
}