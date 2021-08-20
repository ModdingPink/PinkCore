#pragma once

#include "custom-types/shared/macros.hpp"
#include "UnityEngine/MonoBehaviour.hpp"
#include "UnityEngine/Sprite.hpp"

#include "TMPro/TextMeshProUGUI.hpp"
#include <string>

enum RequirementState {
	ReqFound,
	ReqMissing,
	SugFound,
	SugMissing
};

DECLARE_CLASS_CODEGEN(PinkCore::UI, RequirementElement, UnityEngine::MonoBehaviour,
	
	DECLARE_CTOR(ctor);
	DECLARE_INSTANCE_FIELD(TMPro::TextMeshProUGUI*, missingMessage);
	DECLARE_INSTANCE_FIELD(Array<UnityEngine::Sprite*>*, sprites);
	DECLARE_INSTANCE_FIELD(bool, arrayCreated);

	public:
		void Init(std::string name);
		void CheckRequirementState();
	private:
		void set_state(RequirementState newstate);
		bool inited;
		std::string requirementName;
		RequirementState state;
)