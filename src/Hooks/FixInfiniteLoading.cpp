#include "beatsaber-hook/shared/utils/logging.hpp"
#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "logging.hpp"
#include "hooks.hpp"

#include "GlobalNamespace/BeatmapCallbacksController.hpp"
#include "GlobalNamespace/CallbacksInTime.hpp"
#include "GlobalNamespace/BeatmapDataItem.hpp"
#include "System/Collections/Generic/Dictionary_2.hpp"
#include "System/Collections/Generic/KeyValuePair_2.hpp"
#include "System/Collections/Generic/LinkedListNode_1.hpp"

#include "logging.hpp"


// Port of the ArcGap bug fix on PC 
// Infinite loop caused by a bug introduced in BS 1.20+
// CustomJSONData orig hooks/transpiles this method, so we work around it by using a PreFix hook
// In any case, CustomJSONData introduces the transpile bug fix as a safeguard
// - Fern
MAKE_AUTO_HOOK_MATCH(BeatmapCallbacksController_ManualUpdate_Fix_InfiniteLoop, &BeatmapCallbacksController::ManualUpdate, void, BeatmapCallbacksController* self, float songTime)
{

    if (songTime != self->prevSongTime) {
        auto callbacksInTimesEnumerator = self->callbacksInTimes->GetEnumerator();

        while (callbacksInTimesEnumerator.MoveNext()) {
            auto keyValuePair2 = callbacksInTimesEnumerator.get_Current();

            // essentially, stop the infinite while loop by
            // assigning a new lastProcessedNode if neither of the conditions are met
            auto callback = keyValuePair2.value;
            auto node = callback->lastProcessedNode;
            while (node) {
                auto item = node->get_Value();

                // If either condition is met, no infinite loop will occur
                // so let the game handle it
                if (item->type == BeatmapDataItem::BeatmapDataItemType::BeatmapEvent || item->time - callback->aheadTime <= songTime) {
                    break;
                }

                node = node->get_Previous();
            }
            callback->lastProcessedNode = node;
        }
        callbacksInTimesEnumerator.Dispose();
    }

    return BeatmapCallbacksController_ManualUpdate_Fix_InfiniteLoop(self, songTime);
}