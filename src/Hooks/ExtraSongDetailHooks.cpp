#include "beatsaber-hook/shared/utils/logging.hpp"
#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "Hooks.hpp"
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

std::string ReplaceAll(std::string str, const std::string& from, const std::string& to) {
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
	}
	return str;
}

MAKE_HOOK_MATCH(LevelListTableCell_SetDataFromLevelAsync, &GlobalNamespace::LevelListTableCell::SetDataFromLevelAsync, void, GlobalNamespace::LevelListTableCell* self, GlobalNamespace::IPreviewBeatmapLevel* level, bool isFavorite)
{
	LevelListTableCell_SetDataFromLevelAsync(self, level, isFavorite);

	if (!level)
	{
		return;
	}

	// Rounding BPM display for all maps, including official ones
	std::string BPMString = std::to_string((int)level->get_beatsPerMinute());
	Il2CppString* BPMIl2cppString = il2cpp_utils::createcsstr(BPMString);
	self->songBpmText->set_text(BPMIl2cppString);
	
	if (SongUtils::SongInfo::isCustom(level) && config.enableExtraSongDetails)
	{
		self->songAuthorText->set_richText(true);
		if (!level->get_levelAuthorName()->IsNullOrWhiteSpace(level->get_levelAuthorName())) {
			std::string songAuthorName = to_utf8(csstrtostr(level->get_songAuthorName()));
			std::string levelAuthorName = to_utf8(csstrtostr(level->get_levelAuthorName()));

			levelAuthorName = ReplaceAll(levelAuthorName, "<", "\\u200B");
			levelAuthorName = ReplaceAll(levelAuthorName, ">", "\\u200B");

			std::string colourToUse = "ff69b4";

			if (!(rand() % 100))
			{
				//RedBrumblerMoment
				colourToUse = "db4848";
			}

			Il2CppString* newAuthorIl2cppString = il2cpp_utils::createcsstr("<size=80%>" + songAuthorName + "</size> <size=90%>[<color=#" + colourToUse + ">" + levelAuthorName + "</color>]</size>");
			self->songAuthorText->set_text(newAuthorIl2cppString);
		}
	}

}

void InstallDetailHooks(Logger& logger)
{
	SIMPLE_INSTALL_HOOK(LevelListTableCell_SetDataFromLevelAsync);
}

// using a macro to register the method pointer to the class that stores all of the install methods, for automatic execution
PCInstallHooks(InstallDetailHooks)