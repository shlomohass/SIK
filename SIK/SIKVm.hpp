//
//  SIKVm.hpp
//  SIK
//
//  Created by Shlomo Hassid on 06/11/2016 .
//  Copyright � 2016 Shlomo Hassid. All rights reserved.
//

#ifndef SIKVm_hpp
#define SIKVm_hpp

#include "SIKAdd.hpp"
#include "SIKInstruct.hpp"
#include "SIKObj.hpp"
#include <iostream>
#include <map>
#include <vector>

namespace sik
{

	struct SIKStackData {
		sik::SatckDataTypes objectType;
		sik::SIKObj* obj;
		SIKStackData() {
			this->objectType = sik::SDT_TEMP;
			this->obj = nullptr;
		}
		SIKStackData(sik::SIKObj* _obj) {
			this->objectType = sik::SDT_ATTACHED;
			this->obj = _obj;
		}
		~SIKStackData() {
			//std::cout << "destroy StackData - " << this->obj->getAsString() << std::endl;
			if (this->objectType == sik::SDT_TEMP && this->obj != nullptr) {
				delete this->obj;
			}
		}
	};
	struct SIKStack {
		std::vector<sik::SIKStackData*> Stack;
		SIKStack() {
			this->Stack.reserve(50);
		}
		~SIKStack() {
			//std::cout << "destroy Stack" << std::endl;
			for (int i = (int)Stack.size() - 1; i >= 0; i++) {
				std::cout << i << " - ";
				delete Stack.at(i);
			}
		}
	};
	struct SIKScope {
		sik::ScopeTypes Type;
		std::map<std::string, sik::SIKObj*> objects;
		sik::SIKStack* Stack;
		SIKScope() {
			this->Type = sik::SCOPE_GENERAL;
			this->Stack = new SIKStack();
		}
		SIKScope(sik::ScopeTypes type) {
			this->Type = type;
			this->Stack = new SIKStack();
		}
		~SIKScope() {
			//std::cout << "destroy scope" << std::endl;
			delete this->Stack;
		}
	};



	class SIKVm
	{
		//From Script containers:
		std::vector<sik::SIKInstruct>* Instructions;
		std::vector<std::vector<sik::SIKInstruct>>* ObjectDefinitions;
		std::vector<std::vector<sik::SIKInstruct>>* FunctionInstructions;

		//Instruction helpers:
		int InstPointer;
		int InstSize;
		bool jumperFired;

		//Vm containers:
		std::vector<SIKScope*> scopes;

	public:
		SIKVm();
		SIKVm(std::vector<sik::SIKInstruct>* _Instructions, std::vector<std::vector<sik::SIKInstruct>>* _ObjectDefinitions, std::vector<std::vector<sik::SIKInstruct>>* _FunctionInstructions);
		
		//Vm controls:
		int run();
		int execute();
		int execute(int uptoIndex);
		void createContainers();

		//Vm Componenets controls:
		void createScope();
		void createScope(sik::ScopeTypes type);
		void removeScope(int num);
		void deleteNamesInScope();
		void createNameInScope(const std::string& name);
		bool scopeHasName(const std::string& name);
		bool scopeIsForced();
		sik::SIKObj* getNameFromScope(const std::string& name);
		void pushToStack(sik::SIKStackData* STData);
        sik::SIKStackData* popFromStack(); //will earasse the mem too.
        sik::SIKStackData* getFromStack();
        void clearCurrentStack();
        
		//Vm Helpers
		sik::SIKInstruct getInst(int pos);
		sik::SIKInstruct* getInstPointer(int pos);
		int InstructionsSize();
		int getCurrentLineOrigin();
		int getInternalJumper(int jumpto);
		bool testForInternalNeedJump(sik::SIKInstruct* Inst);

		//Execution and Operations:
		void exec_push(sik::SIKInstruct* Inst);
		void exec_define(sik::SIKInstruct* Inst);
        void exec_assign(sik::SIKInstruct* Inst);
        void exec_assignAdd(sik::SIKInstruct* Inst);
        void exec_assignSub(sik::SIKInstruct* Inst);
        void exec_Math_addition(sik::SIKInstruct* Inst);
        void exec_Math_subtraction(sik::SIKInstruct* Inst);
        void exec_Math_multiplication(sik::SIKInstruct* Inst);
        void exec_Math_division(sik::SIKInstruct* Inst);
		void exec_Math_IncDec(sik::SIKInstruct* Inst);
		void exec_comparison_equality(sik::SIKInstruct* Inst);
		void exec_comparison_nequality(sik::SIKInstruct* Inst);
		void exec_comparison_tequality(sik::SIKInstruct* Inst);
		void exec_comparison_ntequality(sik::SIKInstruct* Inst);
		void exec_cond_andor(sik::SIKInstruct* Inst);

        void exec_print(sik::SIKInstruct* Inst);
	    int  exec_ifcondition(sik::SIKInstruct* Inst);
		void exec_block(sik::SIKInstruct* Inst);
		void exec_exit_block(sik::SIKInstruct* Inst);

        //Validation Methods:
        bool validateStackDataForMathOp(sik::SIKStackData* Left, sik::SIKStackData* Right, bool preventExcep);
        bool validateStackDataForMathOpNumbers(sik::SIKStackData* Left, sik::SIKStackData* Right, bool preventExcep);
        bool validateStackDataIsAttached(sik::SIKStackData* Left, bool preventExcep);
        bool validateStackDataAvailable(sik::SIKStackData* sd, bool preventExcep);
		bool validateStackDataCanBeBool(sik::SIKStackData* sd, bool preventExcep);
        
        //Support methods:
        std::string removeFromString(const std::string& str, int num);
        
        
        
        
		virtual ~SIKVm();
	};
}

#endif /* SIKVm_hpp */