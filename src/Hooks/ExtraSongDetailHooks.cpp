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

#include "UnityEngine/UI/Button.hpp"

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

MAKE_AUTO_HOOK_MATCH(LevelListTableCell_SetDataFromLevelAsync, &GlobalNamespace::LevelListTableCell::SetDataFromLevelAsync, void, GlobalNamespace::LevelListTableCell* self, GlobalNamespace::IPreviewBeatmapLevel* level, bool isFavorite)
{
	LevelListTableCell_SetDataFromLevelAsync(self, level, isFavorite);

	if (!level)
	{
		return;
	}

	// Rounding BPM display for all maps, including official ones
	std::string BPMString = std::to_string((int)level->get_beatsPerMinute());
	Il2CppString* BPMIl2cppString = il2cpp_utils::newcsstr(BPMString);
	self->songBpmText->set_text(BPMIl2cppString);
	
	if (SongUtils::SongInfo::isCustom(level) && config.enableExtraSongDetails)
	{
		self->songAuthorText->set_richText(true);
		if (!level->get_levelAuthorName()->IsNullOrWhiteSpace(level->get_levelAuthorName())) {
			std::u16string songAuthorName(csstrtostr(level->get_songAuthorName()));
			std::u16string levelAuthorName(csstrtostr(level->get_levelAuthorName()));

			levelAuthorName = ReplaceAll(levelAuthorName, u"<", u"\\u200B");
			levelAuthorName = ReplaceAll(levelAuthorName, u">", u"\\u200B");

			std::u16string colourToUse = u"ff69b4";

			if (!(rand() % 100))
			{
				//RedBrumblerMoment
				colourToUse = u"db4848";
			}

			Il2CppString* newAuthorIl2cppString = il2cpp_utils::newcsstr(u"<size=80%>" + songAuthorName + u"</size> <size=90%>[<color=#" + colourToUse + u">" + levelAuthorName + u"</color>]</size>");
			self->songAuthorText->set_text(newAuthorIl2cppString);
		}
	}

}