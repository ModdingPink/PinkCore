#include "CustomTypes/RequirementHandler.hpp"
#include "CustomTypes/RequirementElement.hpp"
#include "CustomTypes/WIPElement.hpp"

#include "questui/shared/BeatSaberUI.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/UI/LayoutRebuilder.hpp"

DEFINE_TYPE(PinkCore::UI, RequirementHandler);

using namespace UnityEngine;
using namespace UnityEngine::UI;
using namespace QuestUI;
using namespace QuestUI::BeatSaberUI;

extern Logger& getLogger();
namespace PinkCore::UI
{
	void RequirementHandler::OnEnable()
	{
		CheckAllRequirements();
	}

	void RequirementHandler::CheckAllRequirements()
	{
		/// check the state for all requirements, even the inactive ones
		Array<RequirementElement*>* elements = GetComponentsInChildren<RequirementElement*>(true);
		int length = elements->Length();

		for (int i = 0; i < length; i++)
		{
			auto elem = elements->values[i];
			if (elem) elem->CheckRequirementState();
		}

		// WIP is sort of a requirement so it can live here
		auto wipElement = GetComponentInChildren<WIPElement*>();
		if (!wipElement) 
		{
			HorizontalLayoutGroup* object = CreateHorizontalLayoutGroup(get_transform());
			wipElement = object->get_gameObject()->AddComponent<WIPElement*>();
			wipElement->Init();
		}

		wipElement->CheckWipState();
	}

	void RequirementHandler::AddID(std::string id)
	{
		Array<RequirementElement*>* elements = GetComponentsInChildren<RequirementElement*>(true);
		Il2CppString* name = il2cpp_utils::newcsstr(id);
		
		// if we find an object with the same name, that means this requirement already exists
		if (get_transform()->Find(name)) return;

		HorizontalLayoutGroup* object = CreateHorizontalLayoutGroup(get_transform());
		object->get_transform()->SetAsFirstSibling();
		object->get_gameObject()->set_name(name);
		auto elem = object->get_gameObject()->AddComponent<RequirementElement*>();
		elem->Init(id);
	}
}