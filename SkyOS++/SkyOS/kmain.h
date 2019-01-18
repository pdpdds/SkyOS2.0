#pragma once
#include "windef.h"
#include "BasicStruct.h"
#include "Constants.h"
#include "PlatformAPI.h"
#include "MultbootUtil.h"
#include "SkyOSCore.h"
#include "BootParams.h"
#include "SystemAPI.h"
#include "SkyFacade.h"
#include "syscall.h"
#include "KernelProcedure.h"
#include "SkyStartOption.h"
#include "SkyVirtualInput.h"
#include "SkyGUISystem.h"
#include "SkyInputHandler.h"

#ifdef SKY_EMULATOR
#include "SkyOSWin32Stub.h"
#endif

bool InitDisplaySystem();
