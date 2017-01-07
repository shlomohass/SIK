//
//  SIKVm.cpp
//  SIK
//
//  Created by Shlomo Hassid on 06/11/2016.
//  Copyright � 2016 Shlomo Hassid. All rights reserved.
//

#include "SIKVm.hpp"
#include "SIKLang.hpp"
#include "SIKException.hpp"

#include <ostream>

namespace sik {

	SIKVm::SIKVm()
	{
		this->InstPointer = 0;
		this->InstSize = 0;
		this->Instructions = nullptr;
		this->ObjectDefinitions = nullptr;
		this->FunctionInstructions = nullptr;
	}

	SIKVm::SIKVm(std::vector<sik::SIKInstruct>* _Instructions, std::vector<std::vector<sik::SIKInstruct>>* _ObjectDefinitions, std::vector<std::vector<sik::SIKInstruct>>* _FunctionInstructions)
	{
		this->InstPointer = 0;
		this->Instructions = _Instructions;
		this->ObjectDefinitions = _ObjectDefinitions;
		this->FunctionInstructions = _FunctionInstructions;
		this->InstSize = (int)_Instructions->size();
		this->scopes.reserve(20);
	}
	/* Run the program.
	*/
	int SIKVm::run() {

		this->createContainers();
		int exitCode = -1;
        if (this->InstructionsSize() <= 0) return exitCode;
		while (exitCode == -1) {
			sik::SIKInstruct* Inst = this->getInstPointer(this->InstPointer);
			switch (Inst->Type) {
				case sik::INS_PUSH:
					this->exec_push(Inst);
					break;
				case sik::INS_ADD:
                    this->exec_Math_addition(Inst);
					break;
                case sik::INS_SUBTRACT:
                    this->exec_Math_subtraction(Inst);
                    break;
                case sik::INS_MULTI:
                    this->exec_Math_multiplication(Inst);
                    break;
				case sik::INS_ASSIGN:
                    this->exec_assign(Inst);
                    break;
				case sik::INS_DEFINE:
					this->exec_define(Inst);
					break;
                case sik::INS_DOPRINT:
				case sik::INS_PRINT:
                    this->exec_print(Inst);
                    break;
			}

			//Advance the pointer:
			this->InstPointer++;

			//If we finished auto exit:
			if (this->InstPointer >= this->InstSize) { break; }
		}
		return exitCode;
	}
	/* First Env creation.
	*/
	void SIKVm::createContainers() {

		//Empty all:
		this->scopes.clear();
		//Add the main scope:
		this->createScope(sik::SCOPE_MAIN);

	}
	/* Create a new scope.
	*/
	void SIKVm::createScope(sik::ScopeTypes type) {
		this->scopes.push_back(new sik::SIKScope(type));
	}
	void SIKVm::createScope() {
		this->scopes.push_back(new sik::SIKScope());
	}
	/* Remove a scope safely.
	*/
	void SIKVm::removeScope(int num) {
		for (int i = 0; i < num; i++) {
			if (this->scopes.at(i)->Type == sik::SCOPE_MAIN) break;
			delete this->scopes.at(i);
			this->scopes.pop_back();
		}
	}
	/* Create a name in the scope
	*/
	void SIKVm::createNameInScope(const std::string& name) {
		//Validate Names:
		if (this->scopeHasName(name)) {
            throw sik::SIKException(sik::EXC_RUNTIME, "Tried to redefine a variable. 323256", this->getCurrentLineOrigin());
		}
		//Create Object:
		sik::SIKObj* obj = new SIKObj();
		this->scopes.back()->objects.insert(std::pair<std::string, sik::SIKObj*>(name, obj));
	}
	/* Check If the current scope if force scoped: 
	*/
	bool SIKVm::scopeIsForced() {
		switch (this->scopes.back()->Type) {
			case sik::SCOPE_FUNCTION:
			case sik::SCOPE_F_GENERAL:
			case sik::SCOPE_F_FOR:
			case sik::SCOPE_F_WHILE:
			case sik::SCOPE_F_EACH:
			case sik::SCOPE_F_IF:
				return true;
			default:
				return false;
		}
	}
	bool SIKVm::scopeHasName(const std::string& name) {
		if (this->scopes.empty() || this->scopes.back()->objects.empty()) {
			return false;
		}
		return this->scopes.back()->objects.find(name) != this->scopes.back()->objects.end();
	}
	sik::SIKObj* SIKVm::getNameFromScope(const std::string& name) {
		if (this->scopeIsForced()) {
			if (!this->scopeHasName(name)) return nullptr;
			return this->scopes.back()->objects[name];
		} else {
			for (int i = (int)this->scopes.size() - 1; i >= 0; i--) {
				if (this->scopes[i]->objects.find(name) != this->scopes[i]->objects.end()) {
					return this->scopes[i]->objects[name];
				}				
			}
		}
		return nullptr;
	}
	/* Push to stack some data.
	*/
	void SIKVm::pushToStack(sik::SIKStackData* STData) {
		this->scopes.back()->Stack->Stack.push_back(STData);
	}
    /* Pop from current active stack -> will remove the stackdata too
     */
    sik::SIKStackData* SIKVm::popFromStack() {
        if (this->scopes.back()->Stack->Stack.empty()) {
            return nullptr;
        }
        SIKStackData* data = this->scopes.back()->Stack->Stack.back();
        this->scopes.back()->Stack->Stack.pop_back();
        return data;
    }
    /* Get from current active stack the latest stackdata
     */
    sik::SIKStackData* SIKVm::getFromStack() {
        if (this->scopes.back()->Stack->Stack.empty()) {
            return nullptr;
        }
        return this->scopes.back()->Stack->Stack.back();
    }
    void SIKVm::clearCurrentStack() {
        SIKStack* curStack = this->scopes.back()->Stack;
        //Release Data:
        for (int i = (int)curStack->Stack.size() - 1; i >= 0; i--) {
            delete curStack->Stack.at(i);
        }
        //Empty Stack:
        curStack->Stack.clear();
    }
	/* Get an Instruction at a specific Index.
	*/
	sik::SIKInstruct SIKVm::getInst(int pos) {
		return this->Instructions->at(InstPointer);
	}
	/* Get an Instruction pointer at a specific Index.
	*/
	sik::SIKInstruct* SIKVm::getInstPointer(int pos) {
		return &this->Instructions->at(InstPointer);
	}
	/* Get the Instructions size casted to Integer.
	*/
	int SIKVm::InstructionsSize() {
		return (int)this->Instructions->size();
	}
	/* Get the Instruction Line Origin.
	*/
	int SIKVm::getCurrentLineOrigin() {
		sik::SIKInstruct* Inst = this->getInstPointer(this->InstPointer);
		return Inst != nullptr ? Inst->lineOrigin : -1;
	}


	void SIKVm::exec_push(sik::SIKInstruct* Inst) {
		//Create a Temp StachData:
		sik::SIKStackData *STData = new sik::SIKStackData();

		//Mutate to needed:
		switch (Inst->SubType) {
			case NAMING:
				STData->obj = this->getNameFromScope(Inst->Value);
				STData->objectType = sik::SDT_ATTACHED;
				if (STData->obj == nullptr) {
					throw sik::SIKException(sik::EXC_RUNTIME, "Called to undifined variable. 987544", Inst->lineOrigin);
				}
				break;
			case NUMBER:
				STData->obj = new SIKObj(sik::OBJ_NUMBER, Inst->Value);
				break;
			case STRING:
				STData->obj = new SIKObj(sik::OBJ_STRING, Inst->Value);
				break;
		}

		//Make the Push:
		this->pushToStack(STData);
	}
	void SIKVm::exec_define(sik::SIKInstruct* Inst) {
		this->createNameInScope(Inst->Value);
	}
    void SIKVm::exec_assign(SIKInstruct* Inst) {
        
        //Pop From stack:
        sik::SIKStackData* right = this->popFromStack();
        sik::SIKStackData* left = this->popFromStack();
        
        //Validate the Stack data:
        if (this->validateStackDataIsAttached(left, false) && this->validateStackDataAvailable(right, false)) {
            left->obj->mutate(right->obj);
        }
        
        //Release mem:
        delete right;
        delete left;
    }
    void SIKVm::exec_Math_addition(sik::SIKInstruct* Inst) {
        
        //Pop From stack:
        sik::SIKStackData* right = this->popFromStack();
        sik::SIKStackData* left = this->popFromStack();
        
        //Create a Temp StackData:
        sik::SIKStackData *STData = new sik::SIKStackData();
        
        //Validate the stack data:
        if (this->validateStackDataForMathOp(left, right, false)) {
            
            //Number,Any
            if (left->obj->Type == sik::OBJ_NUMBER) {
                STData->obj = new SIKObj(left->obj->Number + right->obj->getAsNumber());
            }
            //String, Any
            if (left->obj->Type == sik::OBJ_STRING) {
                STData->obj = new SIKObj(left->obj->String + right->obj->getAsString());
            }
            //Bool, Any
            if (left->obj->Type == sik::OBJ_BOOL) {
                STData->obj = new SIKObj(left->obj->Number + right->obj->getAsBool());
            }
        }
        
        //Release mem:
        delete left;
        delete right;
        
        //Push new created:
        this->pushToStack(STData);
    }
    void SIKVm::exec_Math_subtraction(sik::SIKInstruct* Inst) {
        
        //Pop From stack:
        sik::SIKStackData* right = this->popFromStack();
        sik::SIKStackData* left = this->popFromStack();
        
        //Create a Temp StackData:
        sik::SIKStackData *STData = new sik::SIKStackData();
        
        //Validate the stack data:
        if (this->validateStackDataForMathOp(left, right, false)) {
            
            //Number,Any
            if (left->obj->Type == sik::OBJ_NUMBER) {
                STData->obj = new SIKObj(left->obj->Number - right->obj->getAsNumber());
            }
            //String, Any
            if (left->obj->Type == sik::OBJ_STRING) {
                STData->obj = new SIKObj(this->removeFromString(left->obj->String, right->obj->getAsNumber()));
            }
            //Bool, Any
            if (left->obj->Type == sik::OBJ_BOOL) {
                STData->obj = new SIKObj(left->obj->Number - right->obj->getAsBool());
            }
        }
        
        //Release mem:
        delete left;
        delete right;
        
        //Push new created:
        this->pushToStack(STData);
    }
    void SIKVm::exec_Math_multiplication(sik::SIKInstruct* Inst) {
        
        //Pop From stack:
        sik::SIKStackData* right = this->popFromStack();
        sik::SIKStackData* left = this->popFromStack();
        
        //Create a Temp StackData:
        sik::SIKStackData *STData = new sik::SIKStackData();
        
        //Validate the stack data:
        if (this->validateStackDataForMathOp(left, right, false)) {
            
            //Number,Any
            if (left->obj->Type == sik::OBJ_NUMBER) {
                STData->obj = new SIKObj(left->obj->Number * right->obj->getAsNumber());
            }
            //String, Any
            if (left->obj->Type == sik::OBJ_STRING) {
                STData->obj = new SIKObj(left->obj->getAsNumber() * right->obj->getAsNumber());
            }
            //Bool, Any
            if (left->obj->Type == sik::OBJ_BOOL) {
                STData->obj = new SIKObj(left->obj->Number * right->obj->getAsBool());
            }
        }
        
        //Release mem:
        delete left;
        delete right;
        
        //Push new created:
        this->pushToStack(STData);
    }
    void SIKVm::exec_print(sik::SIKInstruct* Inst) {
        //Only prepare stack:
        if (Inst->Type == sik::INS_PRINT && (int)this->scopes.back()->Stack->Stack.size() > 0) {
            this->clearCurrentStack();
        //Do a print:
        } else if (Inst->Type == sik::INS_DOPRINT){
            //Pop From stack:
            sik::SIKStackData* right = this->popFromStack();
            //Validate first and then perform print:
            if (this->validateStackDataAvailable(right, false)) {
                std::cout << right->obj->getAsString() << std::endl;
            }
            //release:
            delete right;
        }
    }
    bool SIKVm::validateStackDataForMathOp(sik::SIKStackData* Left, sik::SIKStackData* Right, bool preventExcep) {
        if (Left == nullptr || Right == nullptr) {
            if (preventExcep) return false;
            throw sik::SIKException(sik::EXC_RUNTIME, "Null Stack. 54343555", this->getCurrentLineOrigin());
        }
        if (Left->obj->Type == sik::OBJ_FUNC || Left->obj->Type == sik::OBJ_OBJ || Left->obj->Type == sik::OBJ_NAN) {
            if (preventExcep) return false;
            throw sik::SIKException(sik::EXC_RUNTIME, "Math with unsupported types. 54343556", this->getCurrentLineOrigin());
        }
        if (Right->obj->Type == sik::OBJ_FUNC || Right->obj->Type == sik::OBJ_OBJ || Right->obj->Type == sik::OBJ_NAN) {
            if (preventExcep) return false;
            throw sik::SIKException(sik::EXC_RUNTIME, "Math with unsupported types. 54343557", this->getCurrentLineOrigin());
        }
        return true;
    }
    bool SIKVm::validateStackDataIsAttached(sik::SIKStackData* Left, bool preventExcep) {
        if (Left == nullptr) {
            if (preventExcep) return false;
            throw sik::SIKException(sik::EXC_RUNTIME, "Null Stack. 54343558", this->getCurrentLineOrigin());
        }
        if (Left->objectType != sik::SDT_ATTACHED || Left->obj == nullptr) {
            if (preventExcep) return false;
            throw sik::SIKException(sik::EXC_RUNTIME, "Trying to opperate on an undeclared object. 54343559", this->getCurrentLineOrigin());
        }
        return true;
    }
    bool SIKVm::validateStackDataAvailable(sik::SIKStackData* sd, bool preventExcep) {
        if (sd == nullptr) {
            if (preventExcep) return false;
            throw sik::SIKException(sik::EXC_RUNTIME, "Null Stack. 5434360", this->getCurrentLineOrigin());
        }
        return true;
    }

    std::string SIKVm::removeFromString(const std::string& str, int num) {
        if ((int)str.length() - num <= 0) {
            return "";
        }
        return std::string(str.begin(), str.end() - num);
    }
    
	SIKVm::~SIKVm()
	{

	}
}