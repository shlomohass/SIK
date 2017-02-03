//
//  SIKAnaCode.cpp
//  SIK
//
//  Created by Shlomo Hassid on 06/11/2016.
//  Copyright � 2016 Shlomo Hassid. All rights reserved.
//

#include "SIKAnaCode.hpp"
#include "SIKLang.hpp"
#include "SIKException.hpp"

#include <ostream>
#include <vector>

namespace sik {
	SIKAnaCode::SIKAnaCode() {

		this->InternalJumper = 0;
		this->BlockInCheck.reserve(20);
		this->Instructions = nullptr;
		this->ObjectDefinitions = nullptr;
		this->FunctionInstructions = nullptr;

	}
	SIKAnaCode::SIKAnaCode(std::vector<sik::SIKInstruct>* _Instructions, std::vector<std::vector<sik::SIKInstruct>>* _ObjectDefinitions, std::map<std::pair<int, std::string>, std::vector<sik::SIKInstruct>>* _FunctionInstructions, int jumperSet)
	{
		this->InternalJumper = jumperSet;
		this->BlockInCheck.reserve(20);
		this->Instructions = _Instructions;
		this->ObjectDefinitions = _ObjectDefinitions;
		this->FunctionInstructions = _FunctionInstructions;
	}
	void SIKAnaCode::splitCodeChunks(std::vector<sik::SIKInstruct>* _Instructions) {
		int size = (int)_Instructions->size();
		std::vector<std::pair<int, int>> toRemove;
		for (int i = 0; i < size; i++) {
			sik::SIKInstruct* inst = &_Instructions->at(i);
			if (inst->Type == sik::INS_FUNC_NAME) {
				std::pair<int, int> toRem = std::make_pair(i+1, -1);
				sik::SIKInstruct* instAdd = inst;
				std::vector<sik::SIKInstruct>* pushTo = &this->FunctionInstructions->at(std::pair<int,std::string>(inst->cache,inst->Value));
				while (instAdd->Type != sik::INS_FUNC_CBLOCK) {
					i++;
					instAdd = &_Instructions->at(i);
					pushTo->push_back(*instAdd);
					//Go to instructions pool:
					if (instAdd->pointToInstruct > -1 && (int)this->ObjectDefinitions->size() > instAdd->pointToInstruct) {
						this->splitCodeChunks(&this->ObjectDefinitions->at(instAdd->pointToInstruct));
					}
				}
				toRem.second = i + 1;
				toRemove.push_back(toRem);
			}
			//Go to instructions pool:
			if (inst->pointToInstruct > -1 && (int)this->ObjectDefinitions->size() > inst->pointToInstruct) {
				this->splitCodeChunks(&this->ObjectDefinitions->at(inst->pointToInstruct));
			}
		}

		//Remove un-needed:
		for (int i = (int)toRemove.size() - 1; i >= 0; i--) {
			_Instructions->erase(_Instructions->begin() + toRemove[i].first, _Instructions->begin() + toRemove[i].second);
		}
		return;
	}
	void SIKAnaCode::postCompiler(std::vector<sik::SIKInstruct>* _Instructions) {
		int size = (int)_Instructions->size();
		for (int i = 0; i < size; i++) {
			sik::SIKInstruct* inst = &_Instructions->at(i);

			//Blocks control:
			if (inst->Type == sik::INS_OBLOCK || inst->Type == sik::INS_OSBLOCK || inst->Type == sik::INS_OBJCREATE) {
				this->BlockInCheck.push_back(inst->Block);
			}
			if (inst->Type == sik::INS_CBLOCK || inst->Type == sik::INS_OBJDONE) {
				inst->Block = this->BlockInCheck.back();
				this->BlockInCheck.pop_back();
			}
			
			//Set Block control if needed
			if (!this->BlockInCheck.empty()) {
				inst->Block = this->BlockInCheck.back();
			}

			this->setMatchigJump(i, inst, _Instructions);

			//Go to instructions pool:
			if (inst->pointToInstruct > -1 && (int)this->ObjectDefinitions->size() > inst->pointToInstruct) {
				this->postCompiler(&this->ObjectDefinitions->at(inst->pointToInstruct));
			}

			//Go to Function Definition:
			if (inst->Type == sik::INS_FUNC_NAME) {
				this->postCompiler(&this->FunctionInstructions->at(std::pair<int, std::string>(inst->cache, inst->Value)));
			}
		}
	}
	void SIKAnaCode::setMatchigJump(int i, sik::SIKInstruct* inst, std::vector<sik::SIKInstruct>* _Instructions) {
		int size = (int)_Instructions->size();
		int nested = 0;
		if (inst->Type == sik::INS_IF || inst->Type == sik::INS_ELSEIF) {
			for (int j = i + 1; j < size; j++) {
				if (_Instructions->at(j).Type == sik::INS_OBLOCK || _Instructions->at(j).Type == sik::INS_OSBLOCK) {
					inst->InternalJumper = j;
					return;
				}
			}
		} else if (inst->Type == sik::INS_FUNC_NUM) {
			for (int j = i + 1; j < size; j++) {
				if (_Instructions->at(j).Type == sik::INS_FUNC_DEFE) {
					inst->InternalJumper = j;
					return;
				}
			}
		} else if (inst->Type == sik::INS_RETURN) {
			for (int j = i + 1; j < size; j++) {
				if (_Instructions->at(j).Type == sik::INS_FUNC_CBLOCK) {
					inst->InternalJumper = j;
					return;
				}
			}
		} else if (inst->Type == sik::INS_PRINT) {
			for (int j = i + 1; j < size; j++) {
				if (_Instructions->at(j).Type == sik::INS_DOPRINT) {
					inst->InternalJumper = j;
					return;
				}
			}
		} else if (inst->Type == sik::INS_WHLL) {
			for (int j = i + 1; j < size; j++) {
				if (_Instructions->at(j).Type == sik::INS_WHLM) {
					inst->InternalJumper = j;
					return;
				}
			}
		} else if (inst->Type == sik::INS_WHLM) {
			for (int j = i + 1; j < size; j++) {
				if ((_Instructions->at(j).Type == sik::INS_OSBLOCK || _Instructions->at(j).Type == sik::INS_OBLOCK) && _Instructions->at(j).Block == sik::BLOCK_WHILE) {
					inst->InternalJumper = j;
					return;
				}
			}
		}
		else if (inst->Type == sik::INS_FORL) {
			for (int j = i + 1; j < size; j++) {
				if (_Instructions->at(j).Type == sik::INS_FRCO) {
					inst->InternalJumper = j;
					return;
				}
			}
		}
		else if (inst->Type == sik::INS_FRCO) {
			for (int j = i + 1; j < size; j++) {
				if (_Instructions->at(j).Type == sik::INS_FRCA) {
					inst->InternalJumper = j;
					return;
				}
			}
		}
		else if (inst->Type == sik::INS_FRCA) {
			for (int j = i + 1; j < size; j++) {
				if ((_Instructions->at(j).Type == sik::INS_OSBLOCK || _Instructions->at(j).Type == sik::INS_OBLOCK) && _Instructions->at(j).Block == sik::BLOCK_FOR) {
					inst->InternalJumper = j;
					return;
				}
			}
		}
		else if (inst->Type == sik::INS_FORL) {
			for (int j = i + 1; j < size; j++) {
				if (_Instructions->at(j).Type == sik::INS_FRCO) {
					inst->InternalJumper = j;
					return;
				}
			}
		}
		else if (inst->Type == sik::INS_FRCO) {
			for (int j = i + 1; j < size; j++) {
				if (_Instructions->at(j).Type == sik::INS_FRCA) {
					inst->InternalJumper = j;
					return;
				}
			}
		}
		else if (inst->Type == sik::INS_EACH) {
			for (int j = i + 1; j < size; j++) {
				if (_Instructions->at(j).Type == sik::INS_EACHI) {
					inst->InternalJumper = j;
					return;
				}
			}
		}
		else if (inst->Type == sik::INS_EACHI) {
			for (int j = i + 1; j < size; j++) {
				if (_Instructions->at(j).Type == sik::INS_EACHE) {
					inst->InternalJumper = j;
					return;
				}
			}
		} else if (inst->Type == sik::INS_EACHE) {
			for (int j = i + 1; j < size; j++) {
				if ((_Instructions->at(j).Type == sik::INS_OSBLOCK || _Instructions->at(j).Type == sik::INS_OBLOCK) && _Instructions->at(j).Block == sik::BLOCK_EACH) {
					inst->InternalJumper = j;
					return;
				}
			}
		} else if (inst->Type == sik::INS_BRKL) {
			for (int j = i + 1; j < size; j++) {
				if (_Instructions->at(j).Type == sik::INS_DOBRK) {
					inst->InternalJumper = j;
					return;
				}
			}
		} else if (inst->Type == sik::INS_OBLOCK || inst->Type == sik::INS_OSBLOCK) {
			for (int j = i + 1; j < size; j++) {
				sik::SIKInstruct* candid = &_Instructions->at(j);
				if (candid->Type == sik::INS_OBLOCK || candid->Type == sik::INS_OSBLOCK) {
					nested++;
				}
				if (candid->Type == sik::INS_CBLOCK && nested == 0) {
					inst->InternalJumper = j;
					candid->cache = inst->Type == sik::INS_OSBLOCK ? 1 : 0;
					return;
				} else if (candid->Type == sik::INS_CBLOCK) {
					nested--;
				}
			}
		} else if (inst->Type == sik::INS_ARRC || inst->Type == sik::INS_ARRT) {
			for (int j = i + 1; j < size; j++) {
				sik::SIKInstruct* candid = &_Instructions->at(j);
				if (candid->Type == sik::INS_ARRC || candid->Type == sik::INS_ARRT) {
					nested++;
				}
				if (candid->Type == sik::INS_ARRD && nested == 0) {
					inst->InternalJumper = j;
					return;
				}
				else if (candid->Type == sik::INS_ARRD) {
					nested--;
				}
			}
		}
	}
	SIKAnaCode::~SIKAnaCode()
	{

	}
}