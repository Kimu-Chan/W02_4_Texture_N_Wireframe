#pragma once

#include "Core/HAL/PlatformType.h"

class UEngineStatics
{
public:
    static uint32 GenUUID()
    {
        return NextUUID++;
    }

    static uint32 NextUUID;
};
