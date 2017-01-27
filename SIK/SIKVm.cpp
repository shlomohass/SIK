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
		this->BaseInstructions = nullptr;
		this->ObjectDefinitions = nullptr;
		this->FunctionInstructions = nullptr;
		this->jumperFired = false;
		this->allowElseIf = false;
	}

	SIKVm::SIKVm(std::vector<sik::SIKInstruct>* _Instructions, std::vector<std::vector<sik::SIKInstruct>>* _ObjectDefinitions, std::map<std::pair<int, std::string>, std::vector<sik::SIKInstruct>>* _FunctionInstructions)
	{
		this->InstPointer = 0;
		this->Instructions = _Instructions;
		this->BaseInstructions = _Instructions;
		this->ObjectDefinitions = _ObjectDefinitions;
		this->FunctionInstructions = _FunctionInstructions;
		this->InstSize = (int)_Instructions->size();
		this->scopes.reserve(20);
		this->jumperFired = false;
		this->allowElseIf = false;
	}
	/* Run the program.
	*/
	int SIKVm::run() {

		//Set Vm:
		this->createContainers();

		//Loop through Instructions:
		int exitCode = -1;
        if (this->InstructionsSize() <= 0) return exitCode;
		while (exitCode == -1) {
			exitCode = this->execute();
		}

		//return with exit code:
		return exitCode;
	}
	/* Execute an Instruction and advaced Internal pointer 
	 * Also can auto execute several Instructions up to an index.
	*/
	int SIKVm::execute() {
		return this->execute(-1);
	}
	int SIKVm::execute(int uptoIndex) {
		sik::SIKInstruct* Inst = this->getInstPointer(this->InstPointer);
		switch (Inst->Type) {
		case sik::INS_FUNC_BLOCK:
		case sik::INS_OBLOCK:
		case sik::INS_OSBLOCK:
			this->exec_block(Inst);
			break;
		case sik::INS_FUNC_CBLOCK:
		case sik::INS_CBLOCK:
			this->exec_exit_block(Inst);
			break;
		case sik::INS_PUSH:
		case sik::INS_FUNC_NAME:
			this->exec_push(Inst);
			break;
		case sik::INS_FUNC_CALL:
			this->exec_func_call(Inst);
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
		case sik::INS_DEVIDE:
			this->exec_Math_division(Inst);
			break;
		case sik::INS_INCREMENT:
		case sik::INS_DECREMENT:
		case sik::INS_PINCREMENT:
		case sik::INS_PDECREMENT:
			this->exec_Math_IncDec(Inst);
			break;
		case sik::INS_ASSIGN:
			this->exec_assign(Inst);
			break;
		case sik::INS_ASSIGNADD:
			this->exec_assignAdd(Inst);
			break;
		case sik::INS_ASSIGNSUB:
			this->exec_assignSub(Inst);
			break;
		case sik::INS_CEQUAL:
			this->exec_comparison_equality(Inst);
			break;
		case sik::INS_CNEQUAL:
			this->exec_comparison_nequality(Inst);
			break;
		case sik::INS_CTEQUAL:
			this->exec_comparison_tequality(Inst);
			break;
		case sik::INS_CNTEQUAL:
			this->exec_comparison_ntequality(Inst);
			break;
		case sik::INS_CGRT:
		case sik::INS_CGRTE:
		case sik::INS_CLST:
		case sik::INS_CLSTE:
			this->exec_comparison_greatersmaller(Inst);
			break;
		case sik::INS_CAND:
		case sik::INS_COR:
			this->exec_cond_andor(Inst);
			break;
		case sik::INS_DEFINE:
			this->exec_define(Inst);
			break;
		case sik::INS_DOPRINT:
		case sik::INS_PRINT:
			this->exec_print(Inst);
			break;
		case sik::INS_ELSEIF:
		case sik::INS_ELSE: {
			if (this->allowElseIf && Inst->Type == sik::INS_ELSEIF) {
				this->allowElseIf = false;
				this->exec_ifcondition(Inst);
			} else {
				this->exec_jumpCondBlock(Inst);
			}
		} break;
		case sik::INS_IF:
			this->exec_ifcondition(Inst);
			break;
		case sik::INS_ARRC:
			this->exec_createArray(Inst);
			break;
		case sik::INS_ARRT:
			this->exec_traverseArray(Inst);
			break;
		case sik::INS_ARRP:
			this->exec_prepareToArrayPush(Inst);
			break;
		}

		//Advance the pointer or stop:
		if (uptoIndex > -1) {
			if (this->InstPointer + 1 < uptoIndex) {
				this->InstPointer++;
				return this->execute(uptoIndex);
			} else {
				return -1;
			}
		} else {
			this->InstPointer++;
		}

		//If we finished auto exit:
		if (this->InstPointer >= this->InstSize) { return 0; }

		return -1;
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
			if (this->scopes.back()->Type == sik::SCOPE_MAIN) break;
			delete this->scopes.back();
			this->scopes.pop_back();
		}
	}
	/* Remove Names from current scope and release objects:
	*/
	void SIKVm::deleteNamesInScope() {
		std::map<std::string, sik::SIKObj*>* naming = &this->scopes.back()->objects;
		typedef std::map<std::string, sik::SIKObj*>::iterator it_type;
		for (it_type iterator = naming->begin(); iterator != naming->end(); iterator++) {
			// iterator->first = key
			// iterator->second = value
			delete iterator->second;
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
		this->scopes.back()->DefinitionOrder.push_back(name);
	}
	/* Check If the current scope if force scoped: 
	*/
	bool SIKVm::scopeIsForced() {
		switch (this->scopes.back()->Type) {
			case sik::SCOPE_FUNCTION:
			case sik::SCOPE_FORCE:
			case sik::SCOPE_MAIN:
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
	/* find a internal jumper
	*/
	int SIKVm::getInternalJumper(int jumpto) {
		for (int i = this->InstPointer + 1; i < this->InstSize; i++) {
			if (this->Instructions->at(i).MyInternalNumber == jumpto) {
				return i;
			}
		}
		return -1;
	}

	/* test for Internal jumping:
	*/
	bool SIKVm::testForInternalNeedJump(sik::SIKInstruct* Inst) {
		if (Inst->InternalJumper > -1) {
			int jump = this->getInternalJumper(Inst->InternalJumper);
			if (jump > -1) {
				sik::SIKInstruct* candid = &this->Instructions->at(jump);
				sik::SIKStackData* data = this->getFromStack();
				if (data != nullptr && candid->Type == sik::INS_CAND && data->obj->getAsBool() < 1) {
					this->InstPointer = jump - 1;
					this->jumperFired = true;
					return true;
				}
				else if (data != nullptr && candid->Type == sik::INS_COR && data->obj->getAsBool() > 0) {
					this->InstPointer = jump - 1;
					this->jumperFired = true;
					return true;
				}
			}
		}
		return false;
	}

    /* All the execution operations:
     */
	void SIKVm::exec_push(sik::SIKInstruct* Inst) {

		//Create a Temp StachData:
		sik::SIKStackData *STData = new sik::SIKStackData();

		//Mutate to needed:
		switch (Inst->SubType) {
			case sik::NAMING:
				STData->obj = this->getNameFromScope(Inst->Value);
				STData->objectType = sik::SDT_ATTACHED;
				if (STData->obj == nullptr) {
					throw sik::SIKException(sik::EXC_RUNTIME, "Called to undifined variable. 987544", Inst->lineOrigin);
				}
				break;
			case sik::NUMBER:
				STData->obj = new SIKObj(sik::OBJ_NUMBER, Inst->Value);
				break;
			case sik::STRING:
				STData->obj = new SIKObj(sik::OBJ_STRING, Inst->Value);
				break;
			case sik::BOOLEAN:
				STData->obj = new SIKObj(Inst->cache == 1 ? true : false);
				break;
			case sik::TOK_CALL: {
					std::pair<int, std::string> key = std::pair<int, std::string>(Inst->cache, Inst->Value);
					std::map<std::pair<int, std::string>, std::vector<sik::SIKInstruct>>::iterator it = this->FunctionInstructions->find(key);
					if (it != this->FunctionInstructions->end())
					{
						//Function found: store
						STData->obj = new SIKObj(key, &it->second);
					} else {
						//Can't find this:
						throw sik::SIKException(sik::EXC_RUNTIME, "Called or tried to set an undifined function. 7784454", Inst->lineOrigin);
					}
				} break;
		}

		//Make the Push:
		this->pushToStack(STData);

		//Try to injump for skipping CAND COR:
		this->testForInternalNeedJump(Inst);
		
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
			if (left->useArrayPush) {
				//Perform a push:
				sik::SIKObj _obj = SIKObj();
				_obj.mutate(right->obj);
				left->obj->pushToArray(_obj);
			} else {
				//Mutate:
				left->obj->mutate(right->obj);
			}
        }
        
        //Release mem:
        delete right;
        delete left;
    }
    void SIKVm::exec_assignAdd(sik::SIKInstruct* Inst) {
        //Pop From stack:
        sik::SIKStackData* right = this->popFromStack();
        sik::SIKStackData* left = this->popFromStack();
        
        //Validate the Stack data:
        this->validateStackDataIsAttached(left, false);
        this->validateStackDataAvailable(right, false);
        
        //Create a Temp StackData:
        sik::SIKStackData STData = sik::SIKStackData();
        
        //Validate the stack data:
        if (this->validateStackDataForMathOp(left, right, false)) {
            
            //Number,Any
            if (left->obj->Type == sik::OBJ_NUMBER) {
                STData.obj = new SIKObj(left->obj->Number + right->obj->getAsNumber());
            }
            //String, Any
            if (left->obj->Type == sik::OBJ_STRING) {
                STData.obj = new SIKObj(left->obj->String + right->obj->getAsString());
            }
            //Bool, Any
            if (left->obj->Type == sik::OBJ_BOOL) {
                STData.obj = new SIKObj(left->obj->Number + right->obj->getAsBool());
            }
        }
        
        //Mutate:
        left->obj->mutate(STData.obj);
        
        //Release mem:
        delete right;
        delete left;
    }
    void SIKVm::exec_assignSub(sik::SIKInstruct* Inst) {
        
        //Pop From stack:
        sik::SIKStackData* right = this->popFromStack();
        sik::SIKStackData* left = this->popFromStack();
        
        //Validate the Stack data:
        this->validateStackDataIsAttached(left, false);
        this->validateStackDataAvailable(right, false);
        
        //Create a Temp StackData:
        sik::SIKStackData STData = sik::SIKStackData();
        
        //Validate the stack data:
        if (this->validateStackDataForMathOp(left, right, false)) {
            
            //Number,Any
            if (left->obj->Type == sik::OBJ_NUMBER) {
                STData.obj = new SIKObj(left->obj->Number - right->obj->getAsNumber());
            }
            //String, Any
            if (left->obj->Type == sik::OBJ_STRING) {
                STData.obj = new SIKObj(this->removeFromString(left->obj->String, (int)right->obj->getAsNumber()));
            }
            //Bool, Any
            if (left->obj->Type == sik::OBJ_BOOL) {
                STData.obj = new SIKObj(left->obj->Number - right->obj->getAsBool());
            }
        }
        
        //Mutate:
        left->obj->mutate(STData.obj);
        
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

		//Try to injump for skipping CAND COR:
		this->testForInternalNeedJump(Inst);

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
                STData->obj = new SIKObj(this->removeFromString(left->obj->String, (int)right->obj->getAsNumber()));
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

		//Try to injump for skipping CAND COR:
		this->testForInternalNeedJump(Inst);
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

		//Try to injump for skipping CAND COR:
		this->testForInternalNeedJump(Inst);
    }
    void SIKVm::exec_Math_division(sik::SIKInstruct* Inst) {
        
        //Pop From stack:
        sik::SIKStackData* right = this->popFromStack();
        sik::SIKStackData* left = this->popFromStack();
        
        //Create a Temp StackData:
        sik::SIKStackData *STData = new sik::SIKStackData();
        
        //Validate the stack data:
        if (this->validateStackDataForMathOpNumbers(left, right, false)) {
            
            //Number,Any
            if (left->obj->Type == sik::OBJ_NUMBER) {
                if (left->obj->Number == 0) {
                    throw sik::SIKException(sik::EXC_RUNTIME,"Devision by Zero",Inst->lineOrigin);
                }
                STData->obj = new SIKObj(left->obj->Number / right->obj->getAsNumber());
            }
        }
        
        //Release mem:
        delete left;
        delete right;
        
        //Push new created:
        this->pushToStack(STData);

		//Try to injump for skipping CAND COR:
		this->testForInternalNeedJump(Inst);

    }
	void SIKVm::exec_Math_IncDec(sik::SIKInstruct* Inst) {

		//Pop From stack:
		sik::SIKStackData* candid = this->getFromStack();

		//Create a Temp StackData:
		sik::SIKStackData *STData = new sik::SIKStackData();

		//Validate the stack data:
		if (this->validateStackDataAvailable(candid, false)) {
			if (candid->obj->Type == sik::OBJ_NUMBER) {

				if (Inst->Type == sik::INS_PDECREMENT) {
					candid->obj->Number--;
				} else if (Inst->Type == sik::INS_PINCREMENT) {
					candid->obj->Number++;
				} else if (Inst->Type == sik::INS_DECREMENT && candid->objectType == sik::SDT_ATTACHED) {
					STData->obj = new SIKObj(candid->obj->Number);
					candid->obj->Number--;
				} else if (Inst->Type == sik::INS_INCREMENT && candid->objectType == sik::SDT_ATTACHED) {
					STData->obj = new SIKObj(candid->obj->Number);
					candid->obj->Number++;
				}

			} else {
				throw sik::SIKException(sik::EXC_RUNTIME, "Trying to opperate on a none numeric value. 65484", Inst->lineOrigin);
			}
		}

		if (STData->obj != nullptr) {
			//release mem:
			delete candid;
			this->popFromStack();
			//Push new created:
			this->pushToStack(STData);
		} else {
			delete STData;
		}

		//Try to injump for skipping CAND COR:
		this->testForInternalNeedJump(Inst);
	}
	void SIKVm::exec_comparison_equality(sik::SIKInstruct* Inst) {

		//Pop From stack:
		sik::SIKStackData* right = this->popFromStack();
		sik::SIKStackData* left = this->popFromStack();

		//Create a Temp StackData:
		sik::SIKStackData *STData = new sik::SIKStackData();

		//Validate the stack data:
		if (this->validateStackDataAvailable(left, false) && this->validateStackDataAvailable(right, false)) {

			//Base type:
			STData->obj = new SIKObj(false);

			if (
				left->obj->Type != sik::OBJ_ARRAY && left->obj->Type != sik::OBJ_FUNC && left->obj->Type != sik::OBJ_OBJ && left->obj->Type != sik::OBJ_NAN &&
				right->obj->Type != sik::OBJ_ARRAY && right->obj->Type != sik::OBJ_FUNC && right->obj->Type != sik::OBJ_OBJ && right->obj->Type != sik::OBJ_NAN
				) {

				if (left->obj->Type == sik::OBJ_BOOL) {
					STData->obj->Number = left->obj->Number == right->obj->getAsBool() ? 1 : 0;
				} else if (left->obj->Type == sik::OBJ_NUMBER) {
					STData->obj->Number = left->obj->Number == right->obj->getAsNumber() ? 1 : 0;
				} else if (left->obj->Type == sik::OBJ_STRING) {
					STData->obj->Number = left->obj->String == right->obj->getAsString() ? 1 : 0;
				} else if (left->obj->Type == sik::OBJ_NULL) {
					STData->obj->Number = left->obj->Type == right->obj->Type ? 1 : 0;
				}
			}

			//Release mem:
			delete left;
			delete right;
		}

		//Push new created:
		this->pushToStack(STData);

		//Try to injump for skipping CAND COR:
		this->testForInternalNeedJump(Inst);

	}
	void SIKVm::exec_comparison_nequality(sik::SIKInstruct* Inst) {

		//Pop From stack:
		sik::SIKStackData* right = this->popFromStack();
		sik::SIKStackData* left = this->popFromStack();

		//Create a Temp StackData:
		sik::SIKStackData *STData = new sik::SIKStackData();

		//Validate the stack data:
		if (this->validateStackDataAvailable(left, false) && this->validateStackDataAvailable(right, false)) {

			//Base type:
			STData->obj = new SIKObj(false);

			if (left->obj->Type != sik::OBJ_ARRAY && left->obj->Type != sik::OBJ_FUNC && left->obj->Type != sik::OBJ_OBJ && left->obj->Type != sik::OBJ_NAN &&
				right->obj->Type != sik::OBJ_ARRAY && right->obj->Type != sik::OBJ_FUNC && right->obj->Type != sik::OBJ_OBJ && right->obj->Type != sik::OBJ_NAN
				) {

				if (left->obj->Type == sik::OBJ_BOOL) {
					STData->obj->Number = left->obj->Number != right->obj->getAsBool() ? 1 : 0;
				}
				else if (left->obj->Type == sik::OBJ_NUMBER) {
					STData->obj->Number = left->obj->Number != right->obj->getAsNumber() ? 1 : 0;
				}
				else if (left->obj->Type == sik::OBJ_STRING) {
					STData->obj->Number = left->obj->String != right->obj->getAsString() ? 1 : 0;
				}
				else if (left->obj->Type == sik::OBJ_NULL) {
					STData->obj->Number = left->obj->Type != right->obj->Type ? 1 : 0;
				}
			}

			//Release mem:
			delete left;
			delete right;
		}

		//Push new created:
		this->pushToStack(STData);

		//Try to injump for skipping CAND COR:
		this->testForInternalNeedJump(Inst);
	}
	void SIKVm::exec_comparison_tequality(sik::SIKInstruct* Inst) {
		//Pop From stack:
		sik::SIKStackData* right = this->popFromStack();
		sik::SIKStackData* left = this->popFromStack();

		//Create a Temp StackData:
		sik::SIKStackData *STData = new sik::SIKStackData();

		//Validate the stack data:
		if (this->validateStackDataAvailable(left, false) && this->validateStackDataAvailable(right, false)) {

			//Base type:
			STData->obj = new SIKObj(left->obj->Type == right->obj->Type ? true : false);

			//Release mem:
			delete left;
			delete right;
		}

		//Push new created:
		this->pushToStack(STData);

		//Try to injump for skipping CAND COR:
		this->testForInternalNeedJump(Inst);
	}
	void SIKVm::exec_comparison_ntequality(sik::SIKInstruct* Inst) {

		//Pop From stack:
		sik::SIKStackData* right = this->popFromStack();
		sik::SIKStackData* left = this->popFromStack();

		//Create a Temp StackData:
		sik::SIKStackData *STData = new sik::SIKStackData();

		//Validate the stack data:
		if (this->validateStackDataAvailable(left, false) && this->validateStackDataAvailable(right, false)) {

			//Base type:
			STData->obj = new SIKObj(left->obj->Type != right->obj->Type ? true : false);

			//Release mem:
			delete left;
			delete right;
		}

		//Push new created:
		this->pushToStack(STData);

		//Try to injump for skipping CAND COR:
		this->testForInternalNeedJump(Inst);
	}
	void SIKVm::exec_comparison_greatersmaller(sik::SIKInstruct* Inst) {

		//Pop From stack:
		sik::SIKStackData* right = this->popFromStack();
		sik::SIKStackData* left = this->popFromStack();

		//Create a Temp StackData:
		sik::SIKStackData *STData = new sik::SIKStackData();

		//Validate the stack data:
		if (this->validateStackDataForMathOp(left, right, false)) {

			//Base type:
			STData->obj = new SIKObj(false);
			int leftCandid = (int)left->obj->getAsNumber();
			int rightCandid = (int)right->obj->getAsNumber();
			if (Inst->Type == sik::INS_CGRT && leftCandid > rightCandid) {
				STData->obj->Number = 1;
			} else if (Inst->Type == sik::INS_CGRTE && leftCandid >= rightCandid) {
				STData->obj->Number = 1;
			} else if (Inst->Type == sik::INS_CLST && leftCandid < rightCandid) {
				STData->obj->Number = 1;
			} else if (Inst->Type == sik::INS_CLSTE && leftCandid <= rightCandid) {
				STData->obj->Number = 1;
			}

			//Release mem:
			delete left;
			delete right;
		}

		//Push new created:
		this->pushToStack(STData);

		//Try to injump for skipping CAND COR:
		this->testForInternalNeedJump(Inst);
	}
	void SIKVm::exec_cond_andor(sik::SIKInstruct* Inst) {

		sik::SIKStackData* left;
		sik::SIKStackData* right;
		

		//Create a Temp StackData:
		sik::SIKStackData *STData = new sik::SIKStackData();

		if (this->jumperFired) {

			//Base type:
			STData->obj = new SIKObj(false);
			left = this->popFromStack();
			int boolTest = (int)left->obj->getAsBool();
			if (boolTest < 1 && Inst->Type == sik::INS_CAND) {
				delete left;
				this->pushToStack(STData);
			} else if (boolTest > 0 && Inst->Type == sik::INS_COR) {
				STData->obj->Number = 1;
				delete left;
				this->pushToStack(STData);
			}
			this->jumperFired = false;
			this->testForInternalNeedJump(Inst);
			return;

		} else {

			right = this->popFromStack();
			left = this->popFromStack();

		}

		//Validate the stack data:
		if (this->validateStackDataCanBeBool(left, false) && this->validateStackDataCanBeBool(right, false)) {

			//Base type:
			STData->obj = new SIKObj(false);
			int boolTestRight = (int)right->obj->getAsBool();
			int boolTestLeft = (int)left->obj->getAsBool();
			if (
				(Inst->Type == sik::INS_CAND && boolTestLeft > 0 && boolTestRight > 0)
				||
				(Inst->Type == sik::INS_COR && (boolTestLeft > 0 || boolTestRight > 0))
			) {
				STData->obj->Number = 1;
			}
			//Release mem:
			delete left;
			delete right;
		}

		//Push new created:
		this->pushToStack(STData);

		//Try to injump for skipping CAND COR:
		this->testForInternalNeedJump(Inst);
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
                std::cout << right->obj->getAsString();
            }
            //release:
            delete right;
        }
    }
	int SIKVm::exec_ifcondition(sik::SIKInstruct* Inst) {
		//Prepare stack:
		if ((int)this->scopes.back()->Stack->Stack.size() > 0) {
			this->clearCurrentStack();
		}
		
		//Execute condition:
		this->InstPointer++;
		int returnCode = this->execute(Inst->InternalJumper);
		if (returnCode != -1) { return returnCode; }

		//Pop From stack:
		sik::SIKStackData* left = this->popFromStack();
		
		//Validate first and then perform:
		if (this->validateStackDataCanBeBool(left, false)) {
			//Number,Any
			if (left->obj->getAsBool() == 0) {
				//Move pointer to end of If true block:
				this->InstPointer = this->Instructions->at(Inst->InternalJumper).InternalJumper;
				//Check Whether has an else:
				if (this->Instructions->at(this->InstPointer + 1).Type == sik::INS_ELSE) {
					this->InstPointer++;
				} else if (this->Instructions->at(this->InstPointer + 1).Type == sik::INS_ELSEIF) {
					this->allowElseIf = true;
				}
			} else {
				//We move to the true body so decrement to execute the block openning:
			}
		}
		
		//release:
		delete left;

		return -1;
	}
	void SIKVm::exec_jumpCondBlock(sik::SIKInstruct* Inst) {
		if (Inst->Type == sik::INS_ELSE) {
			this->InstPointer = this->Instructions->at(this->InstPointer + 1).InternalJumper;
		} else if (Inst->Type == sik::INS_ELSEIF) {
			this->InstPointer = this->Instructions->at(Inst->InternalJumper).InternalJumper;
		}
	}
	void SIKVm::exec_block(sik::SIKInstruct* Inst) {
		if (Inst->Type == sik::INS_OBLOCK) {
		
		} else if (Inst->Type == sik::INS_FUNC_BLOCK) {
			this->createScope(sik::SCOPE_FUNCTION);
		} else if (Inst->Type == sik::INS_OSBLOCK) {
			this->createScope(sik::SCOPE_FORCE);
		}
		return;
	}
	void SIKVm::exec_exit_block(sik::SIKInstruct* Inst) {
		switch (Inst->Block) {
			case sik::BLOCK_WHILE:
			case sik::BLOCK_EACH:
			case sik::BLOCK_FOR:
				break;
			case sik::BLOCK_FUNC: {
				this->deleteNamesInScope();
				this->removeScope(1);
			} break;
			default: {
				if (Inst->cache == 1) { // cache 1 means forced
					//Destroy the scope:
					this->deleteNamesInScope();
					this->removeScope(1);
				}
			};
		}
	}
	int SIKVm::exec_createArray(sik::SIKInstruct* Inst) {

		//How many in the declaration phase:
		sik::SIKStackData* argNumObj = this->popFromStack();
		this->validateStackDataAvailable(argNumObj, false);
		int argNum = (int)argNumObj->obj->Number;
		delete argNumObj;

		if (argNum > 0) {
			//Execute condition:
			this->InstPointer++;
			int returnCode = this->execute(Inst->InternalJumper);
			if (returnCode != -1) { return returnCode; }
		}
		
		//Create the array:
		sik::SIKStackData* STData = new SIKStackData();
		STData->obj = new SIKObj(sik::OBJ_ARRAY);

		//Push All:
		for (int i = 0; i < argNum; i++) {
			sik::SIKStackData* element = this->popFromStack();
			sik::SIKObj _obj = SIKObj();
			if (this->validateStackDataAvailable(element, false)) {
				_obj.mutate(element->obj);
				STData->obj->pushToArray(_obj);
			}
			delete element;
		}

		//Push new created:
		this->pushToStack(STData);

		//Try to injump for skipping CAND COR:
		this->testForInternalNeedJump(Inst);

		return -1;
	}
	int SIKVm::exec_traverseArray(sik::SIKInstruct* Inst) {

		//How many in the declaration phase:
		sik::SIKStackData* argNumObj = this->popFromStack();
		sik::SIKStackData* nameObj = this->popFromStack();
		int argNum = 0;
		//Validate availability:
		if (
			this->validateStackDataIsAttached(nameObj, false) &&
			this->validateStackDataAvailable(argNumObj, false)
			) {
			argNum = (int)argNumObj->obj->Number;
		}
		//Clear some mem:
		delete argNumObj;
		
		//Validate we are handlig with an array:
		if (nameObj->obj->Type != sik::OBJ_ARRAY) {
			throw sik::SIKException(sik::EXC_RUNTIME, "Tried to perform operation on a none array variable. 332254", Inst->lineOrigin);
		}

		if (argNum > 0) {
			//Execute condition:
			this->InstPointer++;
			int returnCode = this->execute(Inst->InternalJumper);
			if (returnCode != -1) { return returnCode; }
		}

		//Create the array:
		sik::SIKStackData* STData = new SIKStackData();
		STData->objectType = sik::SDT_ATTACHED;

		//Traverse All:
		for (int i = 0; i < argNum; i++) {
			sik::SIKStackData* element = this->popFromStack();
			if (this->validateStackDataAvailable(element, false)) {
				int WalkTo = (int)element->obj->getAsNumber();
				if (i == 0) {
					if (WalkTo >= (int)nameObj->obj->Array.size()) {
						throw sik::SIKException(sik::EXC_RUNTIME, "Tried to call to undefined array element. 658475", Inst->lineOrigin);
					}
					STData->obj = &nameObj->obj->Array.at(WalkTo);
				} else {
					if (WalkTo >= (int)STData->obj->Array.size()) {
						throw sik::SIKException(sik::EXC_RUNTIME, "Tried to call to undefined array element. 658475", Inst->lineOrigin);
					}
					STData->obj = &STData->obj->Array.at(WalkTo);
				}
			}
			delete element;
		}

		delete nameObj;

		//Push new created:
		this->pushToStack(STData);

		//Try to injump for skipping CAND COR:
		this->testForInternalNeedJump(Inst);

		return -1;
	}
	void SIKVm::exec_prepareToArrayPush(sik::SIKInstruct* Inst) {

		//Pop From stack:
		sik::SIKStackData* num = this->popFromStack();
		sik::SIKStackData* to = this->getFromStack();
		


		//Validate the Stack data:
		if (this->validateStackDataIsAttached(to, false) && to->obj->Type == sik::OBJ_ARRAY) {
			to->useArrayPush = true;
		} else {
			throw sik::SIKException(sik::EXC_RUNTIME, "Can't make a push on a none array type variable. 11245", Inst->lineOrigin);
		}

		//Clear some mem:
		delete num;
	}


	void SIKVm::exec_func_call(sik::SIKInstruct* Inst) {

		int Args = Inst->cache;
		std::vector<sik::SIKStackData*> ArgsContainer;

		//Pop From stack the args needed:
		for (int i = 0; i < Args; i++) {
			sik::SIKStackData* arg = this->popFromStack();
			if (this->validateStackDataAvailable(arg, false)) {
				ArgsContainer.push_back(arg);
			}
		}

		//Pop the func:
		sik::SIKStackData* func = this->popFromStack();

		//Validate the Stack data:
		if (this->validateStackDataIsCallable(func, false)) {
			int returnCode = this->exec_func(func->obj->FuncSpace, &ArgsContainer);
			if (returnCode != -1) { 
				throw sik::SIKException(sik::EXC_RUNTIME, "Problem in function call", this->getCurrentLineOrigin());
			}
		}
		delete func;
	}
	int SIKVm::exec_func(std::vector<sik::SIKInstruct>* InstExecs, std::vector<sik::SIKStackData*>* _Args) {

		int position = this->InstPointer;

		//Reset the position:
		this->InstPointer = 0;

		//Set the new Instructions set:
		this->Instructions = InstExecs;
		this->InstSize = (int)this->Instructions->size();

		//Run the definition execution:
		int theDefinitionEnd = InstExecs->front().InternalJumper;
		int returnCode = this->execute(theDefinitionEnd + 1);

		//Set the args pass:
		int sentArgs = (int)_Args->size();
		for (int i = 0; i < sentArgs; i++) {
			sik::SIKStackData* sup = _Args->back();
			_Args->pop_back();
			if (this->scopes.back()->DefinitionOrder.size() > i) {
				std::string to = this->scopes.back()->DefinitionOrder.at(i);
				this->getNameFromScope(to)->mutate(sup->obj);
			}
			delete sup;
		}

		//Run the block:
		returnCode = this->execute(this->InstSize);

		//Restore session:
		this->Instructions = this->BaseInstructions;
		this->InstPointer = position;
		this->InstSize = (int)this->Instructions->size();
		return -1;
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
    bool SIKVm::validateStackDataForMathOpNumbers(sik::SIKStackData* Left, sik::SIKStackData* Right, bool preventExcep) {
        if (Left == nullptr || Right == nullptr) {
            if (preventExcep) return false;
            throw sik::SIKException(sik::EXC_RUNTIME, "Null Stack. 543439874", this->getCurrentLineOrigin());
        }
        if (Left->obj->Type != sik::OBJ_NUMBER) {
            if (preventExcep) return false;
            throw sik::SIKException(sik::EXC_RUNTIME, "Math with unsupported types. 543654556", this->getCurrentLineOrigin());
        }
        if (Right->obj->Type == sik::OBJ_FUNC || Right->obj->Type == sik::OBJ_OBJ || Right->obj->Type == sik::OBJ_NAN) {
            if (preventExcep) return false;
            throw sik::SIKException(sik::EXC_RUNTIME, "Math with unsupported types. 5222257", this->getCurrentLineOrigin());
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
	bool SIKVm::validateStackDataCanBeBool(sik::SIKStackData* sd, bool preventExcep) {
		if (sd == nullptr) {
			if (preventExcep) return false;
			throw sik::SIKException(sik::EXC_RUNTIME, "Null Stack. 65326773", this->getCurrentLineOrigin());
		}
		if (sd->obj->Type == sik::OBJ_NAN || sd->obj->Type == sik::OBJ_FUNC || sd->obj->Type == sik::OBJ_NULL || sd->obj->Type == sik::OBJ_OBJ) {
			if (preventExcep) return false;
			throw sik::SIKException(sik::EXC_RUNTIME, "Expected Boolean Value Instead a none boolean and none primitive type. 9878877", this->getCurrentLineOrigin());
		}
		return true;
	}
	bool SIKVm::validateStackDataIsCallable(sik::SIKStackData* sd, bool preventExcep) {
		if (sd == nullptr) {
			if (preventExcep) return false;
			throw sik::SIKException(sik::EXC_RUNTIME, "Null Stack. 9888456", this->getCurrentLineOrigin());
		}
		if (sd->obj->Type != sik::OBJ_FUNC) {
			if (preventExcep) return false;
			throw sik::SIKException(sik::EXC_RUNTIME, "Tried to execute a none function or not properly declared function. 111245", this->getCurrentLineOrigin());
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