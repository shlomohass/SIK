//
//  main.cpp
//  SIK
//
//  Created by Shlomo Hassid on 06/11/2016.
//  Copyright © 2016 Shlomo Hassid. All rights reserved.
//

#include <iostream>

#include "SIKAdd.hpp"
#include "SIK_Lex.hpp"

int main(int argc, const char * argv[]) {
    
    std::string code = "print \"shlomi hassid\" and test;";
    
    //Build the needed tools:
    sik::SIKLex lexer = sik::SIKLex();
    
    //Lex the lines:
    lexer.parse(code, 1);
    lexer.outputTokens();
    std::cout << lexer.getOperatingMode() << "\n";
    
    return 0;
}
