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
		bool useArrayPush;
		SIKStackData() {
			this->objectType = sik::SDT_TEMP;
			this->obj = nullptr;
			this->useArrayPush = false;
		}
		SIKStackData(sik::SIKObj* _obj) {
			this->objectType = sik::SDT_ATTACHED;
			this->obj = _obj;
			this->useArrayPush = false;
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
			for (int i = (int)Stack.size() - 1; i >= 0; i--) {
				//std::cout << i << " - ";
				delete Stack.at(i);
			}
		}
	};
	struct SIKScope {
		sik::ScopeTypes Type;
		std::map<std::string, sik::SIKObj*> objects;
		std::vector<std::string> DefinitionOrder;
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
		std::vector<sik::SIKInstruct>* BaseInstructions;
		std::vector<std::vector<sik::SIKInstruct>>* ObjectDefinitions;
		std::vector<sik::SIKObj*> ObjectOnionBuilder;
		std::map<std::pair<int, std::string>, std::vector<sik::SIKInstruct>>* FunctionInstructions;

		//Instruction helpers:
		int InstPointer;
		int InstSize;
		bool jumperFired;
		bool allowElseIf;

		//Vm containers:
		std::vector<SIKScope*> scopes;

	public:
		SIKVm();
		SIKVm(	std::vector<sik::SIKInstruct>* _Instructions, 
				std::vector<std::vector<sik::SIKInstruct>>* _ObjectDefinitions, 
				std::map<std::pair<int, std::string>, 
				std::vector<sik::SIKInstruct>>* _FunctionInstructions
		);
		
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
		void cleanNamesInScope();
		void protectAllNamesInScope();
		void unProtectAllNamesInScope();
		void createNameInScope(const std::string& name);
		bool scopeHasName(const std::string& name);
		bool scopeIsForced();
		sik::SIKObj* getNameFromScope(const std::string& name);
		sik::SIKObj* findGlobalFuncAndCache(const std::string& name, sik::SIKInstruct* Inst, bool cache);
		void pushToStack(sik::SIKObj* CandidObj);
		void pushToStack(sik::SIKStackData* STData);
		void pushToStack(sik::SIKObj* CandidObj, sik::SatckDataTypes type);
        sik::SIKStackData* popFromStack(); //will erase the cell too <- Should always remmember to delete mem.
        sik::SIKStackData* getFromStack();
        void clearCurrentStack();
        
		//Vm Helpers
		sik::SIKInstruct getInst(int pos);
		sik::SIKInstruct* getInstPointer(int pos);
		int InstructionsSize();
		int getCurrentLineOrigin();
		int getInternalJumper(int jumpto);
		bool testForInternalNeedJump(sik::SIKInstruct* Inst);
		std::pair<int, sik::SIKInstruct*> getLoopForSpecialBlocks(sik::InstructType type, sik::SIKInstruct* Inst);
		std::pair<int, sik::SIKInstruct*> getLoopWhileSpecialBlocks(sik::InstructType type, sik::SIKInstruct* Inst);

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
		void exec_comparison_greatersmaller(sik::SIKInstruct* Inst);
		void exec_cond_andor(sik::SIKInstruct* Inst);
		
        void exec_print(sik::SIKInstruct* Inst);
	    int  exec_ifcondition(sik::SIKInstruct* Inst);
		void exec_jumpCondBlock(sik::SIKInstruct* Inst);
		void exec_block(sik::SIKInstruct* Inst);
		void exec_exit_block(sik::SIKInstruct* Inst);
		int exec_createArray(sik::SIKInstruct* Inst);
		int exec_traverseArray(sik::SIKInstruct* Inst);
		void exec_prepareToArrayPush(sik::SIKInstruct* Inst);

		void exec_func_call(sik::SIKInstruct* Inst);
		int  exec_func(std::vector<sik::SIKInstruct>* InstExecs, std::vector<sik::SIKStackData*>* _Args);
		void exec_func_return(sik::SIKInstruct* Inst);

		void exec_loopFor(sik::SIKInstruct* Inst);
		void exec_loopWhile(sik::SIKInstruct* Inst);

		void exec_objCreate(sik::SIKInstruct* Inst);
		void exec_objDone(sik::SIKInstruct* Inst);
		void exec_objAdd(sik::SIKInstruct* Inst);
		void exec_objChild(sik::SIKInstruct* Inst);
		void exec_objAccess(sik::SIKInstruct* Inst);

        //Validation Methods:
        bool validateStackDataForMathOp(sik::SIKStackData* Left, sik::SIKStackData* Right, bool preventExcep);
        bool validateStackDataForMathOpNumbers(sik::SIKStackData* Left, sik::SIKStackData* Right, bool preventExcep);
        bool validateStackDataIsAttached(sik::SIKStackData* Left, bool preventExcep);
		bool validateStackDataIsObject(sik::SIKStackData* Left, bool preventExcep);
        bool validateStackDataAvailable(sik::SIKStackData* sd, bool preventExcep);
		bool validateStackDataCanBeBool(sik::SIKStackData* sd, bool preventExcep);
		bool validateStackDataCanBeNumeric(sik::SIKStackData* sd, bool preventExcep);
		bool validateStackDataIsCallable(sik::SIKStackData* sd, bool preventExcep);

        //Support methods:
        std::string removeFromString(const std::string& str, int num);
        
        
        
        
		virtual ~SIKVm();
	};
}

#endif /* SIKVm_hpp */