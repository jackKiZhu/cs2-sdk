#pragma once

#include <sigscan/sigscan.hpp>

namespace signatures {
    extern CSigScan GetBaseEntity, GetHighestEntityIndex;
    extern CSigScan GetCSGOInput;
    extern CSigScan GetMatricesForView;
    extern CSigScan GetHitboxSet;
    extern CSigScan HitboxToWorldTransforms;
    extern CSigScan GetFunctions;
    extern CSigScan GetFunctions2;
    extern CSigScan CSVCMsg_UserMessage_Setup;
    extern CSigScan GetSequenceNumber;
    extern CSigScan GetUserCmd;
}  // namespace signatures
