#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "hooks.hpp"
#include "config.hpp"

#include "UnityEngine/Vector2.hpp"

#include "GlobalNamespace/BeatmapObjectsInTimeRowProcessor.hpp"
#include "GlobalNamespace/NoteData.hpp"
#include "GlobalNamespace/SliderData.hpp"
#include "GlobalNamespace/BeatmapObjectsInTimeRowProcessor_TimeSliceContainer_1.hpp"
#include "GlobalNamespace/StaticBeatmapObjectSpawnMovementData.hpp"
#include "GlobalNamespace/NoteCutDirectionExtensions.hpp"
#include "GlobalNamespace/Vector2Extensions.hpp"
#include "GlobalNamespace/BeatmapObjectsInTimeRowProcessor_SliderTailData.hpp"

#include "sombrero/shared/linq_functional.hpp"

using namespace GlobalNamespace;

template<typename T>
auto OfType(auto range) {
    using namespace Sombrero::Linq::Functional;

    return range | Where([](auto i) {
        return il2cpp_utils::AssignableFrom<T>(reinterpret_cast<Il2CppObject*>(i)->klass);
    }) | Select([](auto i) {
        return reinterpret_cast<T>(i);
    });
}

void BeatmapObjectsInTimeRowProcessor_HandleCurrentTimeSliceAllNotesAndSlidersDidFinishTimeSliceTranspile(BeatmapObjectsInTimeRowProcessor* self,
               GlobalNamespace::BeatmapObjectsInTimeRowProcessor::TimeSliceContainer_1<::GlobalNamespace::BeatmapDataItem*>* allObjectsTimeSlice, float nextTimeSliceTime) {


    auto notesInColumnsReusableProcessingListOfLists = self->notesInColumnsReusableProcessingListOfLists;
    for (auto const &l: notesInColumnsReusableProcessingListOfLists) {
        l->Clear();
    }

    auto items = ListWrapper<BeatmapDataItem*>(allObjectsTimeSlice->items);


    auto enumerable = OfType<NoteData *>(items);
    auto enumerable2 = OfType<SliderData *>(items);
    auto enumerable3 = OfType<BeatmapObjectsInTimeRowProcessor::SliderTailData *>(items);
    for (auto noteData : enumerable) {

        // TRANSPILE HERE
        // CLAMP
        auto list = ListWrapper<GlobalNamespace::NoteData*>(self->notesInColumnsReusableProcessingListOfLists[std::clamp(noteData->lineIndex, 0, 3)]);
        // TRANSPILE HERE

        bool flag = false;

        for (int j = 0; j < list.size(); j++) {
            if (list[j]->noteLineLayer > noteData->noteLineLayer) {
                list->Insert(j, noteData);
                flag = true;
                break;
            }
        }
        if (!flag) {
            list->Add(noteData);
        }
    }
    for (auto const& notesInColumnsReusableProcessingListOfList : self->notesInColumnsReusableProcessingListOfLists) {
        auto list2 = ListWrapper<GlobalNamespace::NoteData*>(notesInColumnsReusableProcessingListOfList);
        for (int l = 0; l < list2.size(); l++) {
            list2[l]->SetBeforeJumpNoteLineLayer((NoteLineLayer) l);
        }
    }
    for (auto sliderData: enumerable2) {
        for (auto noteData2: enumerable) {
            if (BeatmapObjectsInTimeRowProcessor::SliderHeadPositionOverlapsWithNote(sliderData, noteData2)) {
                sliderData->SetHasHeadNote(true);
                sliderData->SetHeadBeforeJumpLineLayer(noteData2->beforeJumpNoteLineLayer);
                if (sliderData->sliderType == SliderData::Type::Burst) {
                    noteData2->ChangeToBurstSliderHead();
                    if (noteData2->cutDirection == sliderData->tailCutDirection) {
                        auto line = StaticBeatmapObjectSpawnMovementData::Get2DNoteOffset(noteData2->lineIndex,
                                                                                          self->numberOfLines,
                                                                                          noteData2->noteLineLayer) -
                                    StaticBeatmapObjectSpawnMovementData::Get2DNoteOffset(sliderData->tailLineIndex,
                                                                                          self->numberOfLines,
                                                                                          sliderData->tailLineLayer);
                        float num = Vector2Extensions::SignedAngleToLine(
                                NoteCutDirectionExtensions::Direction(noteData2->cutDirection), line);
                        if (std::abs(num) <= 40.0f) {
                            noteData2->SetCutDirectionAngleOffset(num);
                            sliderData->SetCutDirectionAngleOffset(num, num);
                        }
                    }
                } else {
                    noteData2->ChangeToSliderHead();
                }
            }
        }
    }
    for (auto sliderTailData: enumerable3) {
        auto slider = sliderTailData->slider;
        for (auto noteData3: enumerable) {
            if (BeatmapObjectsInTimeRowProcessor::SliderTailPositionOverlapsWithNote(slider, noteData3)) {
                slider->SetHasTailNote(true);
                slider->SetTailBeforeJumpLineLayer(noteData3->beforeJumpNoteLineLayer);
                noteData3->ChangeToSliderTail();
            }
        }

    }
}

MAKE_AUTO_HOOK_ORIG_MATCH(BeatmapObjectsInTimeRowProcessor_HandleCurrentTimeSliceAllNotesAndSlidersDidFinishTimeSlice,
                &BeatmapObjectsInTimeRowProcessor::HandleCurrentTimeSliceAllNotesAndSlidersDidFinishTimeSlice,
                void,
                BeatmapObjectsInTimeRowProcessor* self,
                GlobalNamespace::BeatmapObjectsInTimeRowProcessor::TimeSliceContainer_1<::GlobalNamespace::BeatmapDataItem*>* allObjectsTimeSlice, float nextTimeSliceTime) {
    return BeatmapObjectsInTimeRowProcessor_HandleCurrentTimeSliceAllNotesAndSlidersDidFinishTimeSliceTranspile(self, allObjectsTimeSlice, nextTimeSliceTime);
}
