#pragma once

#include "custom-types/shared/macros.hpp"
#include "UnityEngine/MonoBehaviour.hpp"
#include "UnityEngine/Sprite.hpp"

#include "HMUI/HoverHint.hpp"
#include <string>
#include "Contributor.hpp"

DECLARE_CLASS_CODEGEN(PinkCore::UI, ContributorElement, UnityEngine::MonoBehaviour,
	
	DECLARE_CTOR(ctor);
	DECLARE_INSTANCE_FIELD(UnityEngine::Sprite*, sprite);
	
	public:
		void EnableObjectIfIsCurrentContributor();
		void Init(Contributor& contributor);
		bool GetIsSame(Contributor& other);
		
	private:
		bool inited;
		Contributor contributor;
)