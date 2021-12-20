#include "CustomTypes/ContributorHandler.hpp"
#include "CustomTypes/ContributorElement.hpp"
#include "Utils/SongUtils.hpp"
#include "Utils/ContributorUtils.hpp"
#include "Contributor.hpp"

#include "UnityEngine/GameObject.hpp"

#include "questui/shared/BeatSaberUI.hpp"

#include "UnityEngine/UI/LayoutRebuilder.hpp"
#include "logging.hpp"

DEFINE_TYPE(PinkCore::UI, ContributorHandler);

using namespace UnityEngine;
using namespace UnityEngine::UI;
using namespace QuestUI;
using namespace QuestUI::BeatSaberUI;

namespace PinkCore::UI
{
	void ContributorHandler::OnEnable()
	{
		ActivateAllElements();
	}

	void ContributorHandler::ActivateAllElements()
	{
		ArrayW<ContributorElement*> elements = GetComponentsInChildren<ContributorElement*>(true);
		for (auto elem : elements)
		{
			elem->EnableObjectIfIsCurrentContributor();
		}
	}

	void ContributorHandler::GetAllCurrentContributors()
	{
		INFO("Fetching List");

		// get the list of contributors
		auto& contributors = ContributorUtils::GetContributors();
		INFO("Have %lu contributors", contributors.size());
		// if there are no contributors, everything should just be disabled
		if (contributors.size() == 0)
		{
			ArrayW<ContributorElement*> elements = GetComponentsInChildren<ContributorElement*>(true);
			for (auto elem : elements)
				elem->get_gameObject()->SetActive(false);
		}
		// if we do have some contributors
		else
		{
			// add each contributor
			for (auto contributor : contributors)
			{
				AddContributor(contributor);
			}
		}
	}

	void ContributorHandler::AddContributor(Contributor& contributor)
	{
		ArrayW<ContributorElement*> elements = GetComponentsInChildren<ContributorElement*>(true);
		for (auto elem : elements)
		{
			// if we find an element that is the same, return, and dont add a new one
			if (elem->GetIsSame(contributor)) return;
		}
		
		// if we get here that means this was a new contributor
		UnityEngine::UI::HorizontalLayoutGroup* object = QuestUI::BeatSaberUI::CreateHorizontalLayoutGroup(get_transform());
		// last sibling, so they show up at the end of the list at all times
		object->get_transform()->SetAsLastSibling();
		auto elem = object->get_gameObject()->AddComponent<ContributorElement*>();
		elem->Init(contributor);
	}
}