#include "Utils/DifficultyNameUtils.hpp"
#include "Utils/SongUtils.hpp"


#include <algorithm>

extern Logger& getLogger();


namespace DifficultyNameUtils
{
	std::u16string easyCache = u"";
	std::u16string normalCache = u"";
	std::u16string hardCache = u"";
	std::u16string expertCache = u"";
	std::u16string expertPlusCache = u"";

	void setDifficultyNameCacheFromDifficulty(GlobalNamespace::BeatmapDifficulty difficulty, std::u16string_view name){
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

	std::u16string GetDifficultyNameFromDoc(rapidjson::GenericDocument<rapidjson::UTF16<char16_t>>& d, GlobalNamespace::BeatmapDifficulty difficulty)
	{
		std::u16string customDifficultyName = u"";
		rapidjson::GenericValue<rapidjson::UTF16<char16_t>> customData;
		if (SongUtils::CustomData::GetCurrentCustomData(d, customData, difficulty)) {
			auto customDiffItr = customData.FindMember(u"_difficultyLabel");
			if (customDiffItr != customData.MemberEnd()) {
				customDifficultyName = std::u16string(customDiffItr->value.GetString(), customDiffItr->value.GetStringLength());
			}
		}
		return customDifficultyName;
	}


	std::u16string GetDifficultyNameFromCache(GlobalNamespace::BeatmapDifficulty difficulty) {
		std::u16string diffLabel;
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
				diffLabel = u"";
				break;
		}
		setDifficultyNameCacheFromDifficulty(difficulty, u"");
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