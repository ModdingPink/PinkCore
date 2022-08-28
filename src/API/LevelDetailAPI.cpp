#include "Utils/RequirementUtils.hpp"
#include "Utils/SongUtils.hpp"
#include "Utils/ContributorUtils.hpp"
#include "beatsaber-hook/shared/utils/typedefs-wrappers.hpp"
#include "beatsaber-hook/shared/rapidjson/include/rapidjson/document.h"

namespace PinkCore::API
{
    PinkCore::API::LevelDetails GetNewMapData(rapidjson::GenericDocument<rapidjson::UTF16<char16_t>>& in, GlobalNamespace::BeatmapDifficulty difficulty, GlobalNamespace::BeatmapCharacteristicSO* characteristic, GlobalNamespace::IPreviewBeatmapLevel* level){
        PinkCore::API::LevelDetails newLevelDetail;
        newLevelDetail.difficulty = difficulty;
        newLevelDetail.characteristic = characteristic;
        if(SongUtils::SongInfo::isCustom(level)){
            RequirementUtils::HandleRequirementDetails(newLevelDetail);
			ContributorUtils::FetchListOfContributors(newLevelDetail);
            rapidjson::GenericValue<rapidjson::UTF16<char16_t>> customData;
            SongUtils::CustomData::GetCustomDataJsonFromDifficultyAndCharacteristic(in, customData, difficulty, characteristic);
            newLevelDetail.environmentType = SongUtils::CustomData::MapEnvironmentTypeChecker(customData, difficulty, characteristic);
            newLevelDetail.hasCustomColours = SongUtils::CustomData::MapHasColoursChecker(customData, difficulty, characteristic);
            newLevelDetail.saberCount = SongUtils::CustomData::MapSaberCountChecker(customData, difficulty, characteristic);
            newLevelDetail.showRotationSpwanLines = SongUtils::CustomData::MapShouldShowRotationSpawnLines(customData, difficulty, characteristic);
            newLevelDetail.isCustom = true;
            newLevelDetail.isWIP = SongUtils::SongInfo::isWIP(level);
        }else{
			newLevelDetail.environmentType = u"Default";
			newLevelDetail.hasCustomColours = false;
			newLevelDetail.isCustom = false;
			newLevelDetail.saberCount = -1; //-1 = No Data, dont do anything
			newLevelDetail.isWIP = false;
			newLevelDetail.showRotationSpwanLines = true;
        }	
        return newLevelDetail;
	}

    PinkCore::API::LevelDetails GetCurrrentMapData()
    {
        return SongUtils::SongInfo::get_mapData();
    }

}

