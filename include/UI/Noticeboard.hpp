#pragma once

#include "custom-types/shared/macros.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Transform.hpp"
#include "HMUI/ViewController.hpp"

#include "TMPro/TextMeshProUGUI.hpp"

enum BoardState {
	Board,
	Donation
};

DECLARE_CLASS_CODEGEN(PinkCore::UI, NoticeBoard, HMUI::ViewController,
	DECLARE_OVERRIDE_METHOD(void, DidActivate, il2cpp_utils::FindMethodUnsafe("HMUI", "ViewController", "DidActivate", 3), bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling);
	DECLARE_INSTANCE_FIELD(UnityEngine::GameObject*, container);
	DECLARE_INSTANCE_FIELD(UnityEngine::GameObject*, boardLayout);
	DECLARE_INSTANCE_FIELD(UnityEngine::GameObject*, donationLayout);
	DECLARE_INSTANCE_FIELD(TMPro::TextMeshProUGUI*, title);

	public:
		static PinkCore::UI::NoticeBoard* get_instance();
		static void clear_instance();
	private:
		static PinkCore::UI::NoticeBoard* instance;
		BoardState state = Board;
		void CreateSwitchingButtons();
		static void CreateTextLayout(UnityEngine::Transform* parent, const std::string& text, UnityEngine::GameObject*& output);
)