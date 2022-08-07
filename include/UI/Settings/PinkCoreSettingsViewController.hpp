#pragma once

#include "custom-types/shared/macros.hpp"
#include "custom-types/shared/coroutine.hpp"
#include "HMUI/ViewController.hpp"
#include "UnityEngine/UI/Toggle.hpp"

DECLARE_CLASS_CODEGEN(PinkCore::UI, PinkCoreSettingsViewController, HMUI::ViewController,
	DECLARE_OVERRIDE_METHOD(void, DidActivate, il2cpp_utils::FindMethodUnsafe("HMUI", "ViewController", "DidActivate", 3), bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling);
	DECLARE_DEFAULT_CTOR();

	UnityEngine::UI::Toggle* customColourToggle = nullptr;
)