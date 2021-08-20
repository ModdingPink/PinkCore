#pragma once

#include <string>
#include "custom-types/shared/macros.hpp"
#include "UnityEngine/MonoBehaviour.hpp"
#include "beatsaber-hook/shared/rapidjson/include/rapidjson/document.h"
#include "Contributor.hpp"

DECLARE_CLASS_CODEGEN(PinkCore::UI, ContributorHandler, UnityEngine::MonoBehaviour,
	DECLARE_INSTANCE_METHOD(void, OnEnable);

	public:
		void GetAllCurrentContributors();
		void AddContributor(Contributor& contributor);
		void ActivateAllElements();
		std::string lastLevelName;
	private:
)