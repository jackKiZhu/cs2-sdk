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
    extern CSigScan SetViewAngles;
    extern CSigScan CanFire;
    extern CSigScan GetEngineTrace;
    extern CSigScan TraceShape;
    extern CSigScan TraceShape2;
    extern CSigScan GetSurfaceData;
    extern CSigScan GameTraceCtor;
    extern CSigScan CTraceFilter;
    extern CSigScan CTraceFilter;
}  // namespace signatures
