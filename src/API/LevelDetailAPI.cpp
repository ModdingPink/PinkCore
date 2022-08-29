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
            rapidjson::GenericValue<rapidjson::UTF16<char16_t>> customData;
            if(SongUtils::CustomData::GetCustomDataJsonFromDifficultyAndCharacteristic(in, customData, difficulty, characteristic)){
                RequirementUtils::HandleRequirementDetails(newLevelDetail);
                ContributorUtils::FetchListOfContributors(newLevelDetail);
                newLevelDetail.environmentType = SongUtils::CustomData::MapEnvironmentTypeChecker(customData, difficulty, characteristic);
                newLevelDetail.hasCustomColours = SongUtils::CustomData::MapHasColoursChecker(customData, difficulty, characteristic);
                newLevelDetail.saberCount = SongUtils::CustomData::MapSaberCountChecker(customData, difficulty, characteristic);
                newLevelDetail.showRotationSpwanLines = SongUtils::CustomData::MapShouldShowRotationSpawnLines(customData, difficulty, characteristic);
            }else{
                newLevelDetail.environmentType = u"Default";
                newLevelDetail.hasCustomColours = false;
                newLevelDetail.showRotationSpwanLines = true;
                newLevelDetail.saberCount = -1; //-1 = No Data, dont do anything
            }
            newLevelDetail.isCustom = true;
            newLevelDetail.isWIP = SongUtils::SongInfo::isWIP(level);
        }else{
			newLevelDetail.environmentType = u"Default";
			newLevelDetail.hasCustomColours = false;
			newLevelDetail.showRotationSpwanLines = true;
			newLevelDetail.saberCount = -1; //-1 = No Data, dont do anything
			newLevelDetail.isCustom = false;
			newLevelDetail.isWIP = false;
        }	
        return newLevelDetail;
	}

    PinkCore::API::LevelDetails GetCurrrentMapData()
    {
        return SongUtils::SongInfo::get_mapData();
    }

}

