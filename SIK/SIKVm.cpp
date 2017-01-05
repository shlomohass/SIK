//
//  SIKVm.cpp
//  SIK
//
//  Created by Shlomo Hassid on 06/11/2016.
//  Copyright © 2016 Shlomo Hassid. All rights reserved.
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

		while (exitCode == -1) {
			sik::SIKInstruct* Inst = this->getInstPointer(this->InstPointer);
			switch (Inst->Type) {
				case sik::INS_PUSH:
					this->exec_push(Inst);
					break;
				case sik::INS_ADD:
					break;
				case sik::INS_ASSIGN:
				case sik::INS_DEFINE:
					this->exec_define(Inst);
					break;
				case sik::INS_ADD:
				case sik::INS_PRINT:
			}

			//Advance the pointer:
			this->InstPointer++;

			//If we finished auto exit:
			if (this->InstPointer > this->InstSize) { break; }
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
			throw sik::SIKException("SIK RunTime Error - Tried to redefine a variable. 323256", this->getCurrentLineOrigin());
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
					throw sik::SIKException("SIK RunTime Error - Called to undifined variable. 987544", Inst->lineOrigin);
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




	SIKVm::~SIKVm()
	{

	}
}