#include "main.h"
#include "windef.h"
#include "memory.h"
#include "string.h"
#include "usb.h"
#include "manager.h"
#include "utils.h"

extern "C" void* malloc(u32int sz)
{
	return nullptr;
}

extern "C" void free(void* p)
{
	return;
}

extern "C" int printf(const char *Format, ...)
{
	return 0;
}



/* OnLoad
* The entry-point of a server, this is called
* as soon as the server is loaded in the system */
OsStatus_t
OnLoad(void)
{
	// Initialize core-systems
	if (UsbCoreInitialize() != OsSuccess) {
		ERROR("Failed to initialize usb-core systems.");
		return OsError;
	}

	// Register us with server manager
	return RegisterService(__USBMANAGER_TARGET);
}

/* OnUnload
* This is called when the server is being unloaded
* and should free all resources allocated by the system */
OsStatus_t
OnUnload(void)
{
	// Destroy core-systems and let them cleanup
	return UsbCoreDestroy();
}

/* OnEvent
* This is called when the server recieved an external evnet
* and should handle the given event*/
OsStatus_t
OnEvent(
	_In_ MRemoteCall_t *Message)
{
	// Variables
	OsStatus_t Result = OsSuccess;

	// Which function is called?
	switch (Message->Function) {
	case __USBMANAGER_REGISTERCONTROLLER: {
		// Register controller
		return UsbCoreControllerRegister(Message->From.Process,
			(MCoreDevice_t*)Message->Arguments[0].Data.Buffer,
			(UsbControllerType_t)Message->Arguments[1].Data.Value,
			Message->Arguments[2].Data.Value);
	} break;

	case __USBMANAGER_UNREGISTERCONTROLLER: {
		// Unregister controller
		return UsbCoreControllerUnregister(Message->From.Process,
			(UUId_t)Message->Arguments[0].Data.Value);
	} break;

	case __USBMANAGER_QUERYCONTROLLERCOUNT: {
		int ControllerCount = UsbCoreGetControllerCount();
		return RPCRespond(&Message->From, &ControllerCount, sizeof(int));
	} break;

	case __USBMANAGER_QUERYCONTROLLER: {
		UsbHcController_t HcController = { { 0 }, (UsbControllerType_t)0 };
		UsbController_t *Controller = NULL;
		Controller = UsbCoreGetControllerIndex((int)Message->Arguments[0].Data.Value);
		if (Controller != NULL) {
			memcpy(&HcController.Device, &Controller->Device, sizeof(MCoreDevice_t));
			HcController.Type = Controller->Type;
		}
		return RPCRespond(&Message->From, &HcController, sizeof(UsbHcController_t));
	} break;

	case __USBMANAGER_PORTEVENT: {
		// Handle port event
		return UsbCoreEventPort(Message->From.Process,
			(UUId_t)Message->Arguments[0].Data.Value,
			LOBYTE(Message->Arguments[1].Data.Value),
			LOBYTE(Message->Arguments[2].Data.Value));
	} break;

		// Don't handle anything else tbh
	default:
		break;
	}

	// Done
	return Result;
}


/*
int kmain()
{


	UsbInitialize();
	UsbCleanup();
	UsbRetrievePool();
	UsbTransferInitialize(nullptr, nullptr, nullptr, ControlTransfer, 0);
	UsbTransferSetup(0, 0, 0, 0, OutTransaction);
	UsbSetFeature(0, 0, 0, 0, 0, 0, 0);
	UsbExecutePacket(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	return 0;
}*/
	