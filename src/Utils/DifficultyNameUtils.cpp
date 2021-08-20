#include "Utils/DifficultyNameUtils.hpp"
#include "Utils/SongUtils.hpp"


#include <algorithm>

extern Logger& getLogger();


namespace DifficultyNameUtils
{
	std::string easyCache = "";
	std::string normalCache = "";
	std::string hardCache = "";
	std::string expertCache = "";
	std::string expertPlusCache = "";

	void setDifficultyNameCacheFromDifficulty(GlobalNamespace::BeatmapDifficulty difficulty, std::string name){
		switch (difficulty) {
		case GlobalNamespace::BeatmapDifficulty::Easy:
			easyCache = name;
			break;
		case GlobalNamespace::BeatmapDifficulty::Normal:
			normalCache = name;
			break;
		case GlobalNamespace::BeatmapDifficulty::Hard:
			hardCache = name;
			break;
		case GlobalNamespace::BeatmapDifficulty::Expert:
			expertCache = name;
			break;
		case GlobalNamespace::BeatmapDifficulty::ExpertPlus:
			expertPlusCache = name;
			break;
		}
	}

	std::string GetDifficultyNameFromDoc(rapidjson::Document& d, GlobalNamespace::BeatmapDifficulty difficulty)
	{
		std::string customDifficultyName = "";
		rapidjson::Value customData;
		if (SongUtils::CustomData::GetCurrentCustomData(d, customData, difficulty)) {
			auto customDiffItr = customData.FindMember("_difficultyLabel");
			if (customDiffItr != customData.MemberEnd()) {
				customDifficultyName = customDiffItr->value.GetString();
			}
		}
		return customDifficultyName;
	}


	std::string GetDifficultyNameFromCache(GlobalNamespace::BeatmapDifficulty difficulty) {
		std::string diffLabel = "";
		switch (difficulty) {
			case GlobalNamespace::BeatmapDifficulty::Easy:
				diffLabel = easyCache;
				break;
			case GlobalNamespace::BeatmapDifficulty::Normal:
				diffLabel = normalCache;
				break;
			case GlobalNamespace::BeatmapDifficulty::Hard:
				diffLabel = hardCache;
				break;
			case GlobalNamespace::BeatmapDifficulty::Expert:
				diffLabel = expertCache;
				break;
			case GlobalNamespace::BeatmapDifficulty::ExpertPlus:
				diffLabel = expertPlusCache;
				break;
			default:
				break;
		}
		setDifficultyNameCacheFromDifficulty(difficulty, "");
		return diffLabel;
	}

	void SetDifficultyNameCacheFromArray(::Array<GlobalNamespace::IDifficultyBeatmap*>* difficultyArray) {
		auto& doc = SongUtils::GetCurrentInfoDat();
		for (int i = 0; i < difficultyArray->Length(); i++) {
			GlobalNamespace::BeatmapDifficulty difficulty = difficultyArray->values[i]->get_difficulty();
			setDifficultyNameCacheFromDifficulty(difficulty, GetDifficultyNameFromDoc(doc, difficulty));
		}
	}

}