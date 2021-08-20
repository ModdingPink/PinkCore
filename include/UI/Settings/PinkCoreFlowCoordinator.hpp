#pragma once

#include "custom-types/shared/macros.hpp"
#include "custom-types/shared/coroutine.hpp"
#include "HMUI/FlowCoordinator.hpp"
#include "HMUI/ViewController.hpp"

DECLARE_CLASS_CODEGEN(PinkCore::UI, PinkCoreFlowCoordinator, HMUI::FlowCoordinator,
	DECLARE_OVERRIDE_METHOD(void, DidActivate, il2cpp_utils::FindMethodUnsafe("HMUI", "FlowCoordinator", "DidActivate", 3), bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling);
	DECLARE_OVERRIDE_METHOD(void, BackButtonWasPressed, il2cpp_utils::FindMethodUnsafe("HMUI", "FlowCoordinator", "BackButtonWasPressed", 1), HMUI::ViewController* topViewController);
	DECLARE_INSTANCE_FIELD(HMUI::ViewController*, pinkCoreSettingsViewController);
	DECLARE_INSTANCE_FIELD(HMUI::ViewController*, pinkCoreDonationViewController);
)