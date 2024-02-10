#include "beatsaber-hook/shared/utils/logging.hpp"
#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "hooks.hpp"
#include "config.hpp"

#include "Utils/SongUtils.hpp"
#include "Utils/DifficultyNameUtils.hpp"

#include "GlobalNamespace/LevelListTableCell.hpp"
#include "GlobalNamespace/StandardLevelDetailViewController.hpp"
#include "GlobalNamespace/BeatmapDifficultySegmentedControlController.hpp"
#include "GlobalNamespace/BeatmapDifficulty.hpp"
#include "GlobalNamespace/BeatmapCharacteristicSO.hpp"
#include "GlobalNamespace/IDifficultyBeatmap.hpp"
#include "GlobalNamespace/IDifficultyBeatmapSet.hpp"
#include "GlobalNamespace/BeatmapCharacteristicSegmentedControlController.hpp"
#include "TMPro/TextMeshProUGUI.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/UI/Button.hpp"
#include "UnityEngine/Vector3.hpp"
#include "sombrero/shared/ColorUtils.hpp"
#include "sombrero/shared/FastVector3.hpp"
#include "UnityEngine/UI/Image.hpp"
#include <cstdlib>
#include <string>

std::u16string ReplaceAll(std::u16string str, const std::u16string& from, const std::u16string& to) {
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != std::u16string::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
	}
	return str;
}

const Sombrero::FastVector3 favouriteSize = Sombrero::FastVector3(1.4,1.4,1.4);

MAKE_AUTO_HOOK_MATCH(LevelListTableCell_SetDataFromLevelAsync, &GlobalNamespace::LevelListTableCell::SetDataFromLevelAsync, void, GlobalNamespace::LevelListTableCell* self, GlobalNamespace::IPreviewBeatmapLevel* level, bool isFavorite, bool isPromoted, bool isUpdated)
{
	LevelListTableCell_SetDataFromLevelAsync(self, level, isFavorite, isPromoted, isUpdated);

	if (!level)
	{
		return;
	}

	// Rounding BPM display for all maps, including official ones
	StringW BPMIl2cppString(std::to_string((int)level->get_beatsPerMinute()));
	self->_songBpmText->set_text(BPMIl2cppString);
	bool isCustom = SongUtils::SongInfo::isCustom(level);
	if (isCustom && config.enableExtraSongDetails)
	{
		self->_songAuthorText->set_richText(true);
		if (!level->get_levelAuthorName()->IsNullOrWhiteSpace(level->get_levelAuthorName())) {
			std::u16string songAuthorName(level->get_songAuthorName());
			std::u16string levelAuthorName(level->get_levelAuthorName());

			levelAuthorName = ReplaceAll(levelAuthorName, u"<", u"\\u200B");
			levelAuthorName = ReplaceAll(levelAuthorName, u">", u"\\u200B");

			std::u16string colourToUse = u"ff69b4";

			if (!(rand() % 100))
			{
				//RedBrumblerMoment
				colourToUse = u"db4848";
			}

			StringW newAuthorIl2cppString(u"<size=80%>" + songAuthorName + u"</size> <size=90%>[<color=#" + colourToUse + u">" + levelAuthorName + u"</color>]</size>");
			self->_songAuthorText->set_text(newAuthorIl2cppString);
		}
		
	}
	if(isFavorite) {
		Sombrero::FastColor newColour = Sombrero::FastColor(1,1,1,1);
		if(isCustom) newColour = {1, 0.41, 0.71, 0.7};
		self->_favoritesBadgeImage->set_color(newColour);
		self->_favoritesBadgeImage->get_transform()->set_localScale(favouriteSize);
	}
}