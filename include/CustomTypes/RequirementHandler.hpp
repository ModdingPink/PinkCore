#pragma once

#include "custom-types/shared/macros.hpp"
#include "UnityEngine/MonoBehaviour.hpp"
#include <string>

DECLARE_CLASS_CODEGEN(PinkCore::UI, RequirementHandler, UnityEngine::MonoBehaviour,
	DECLARE_INSTANCE_METHOD(void, OnEnable);

	public:
		void CheckAllRequirements();
		void AddID(std::string id);
	private:
)