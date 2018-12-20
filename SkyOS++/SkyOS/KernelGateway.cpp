#include "KernelGateway.h"
#include "Screen.h"
#include "Surface.h"
#include "PlatformAPI.h"
#include "intrinsic.h"
#include "input.h"
#include "VirtualScreen.h"

static VirtualScreen screen;

Screen *getScreen()
{
	return &screen;
}
Surface *getBitmap()
{
	return nullptr;
}

// Big lock is a hack, related to the way we handle input on windows.
void acquireBigLock()
{
	platformAPI._processInterface.sky_enter_critical_section();
}

void releaseBigLock()
{
	platformAPI._processInterface.sky_leave_critical_section();
}

struct InputEvent {
	InputEvent *next;
	enum NativeEventType type;
	int x;
	int y;
	bool lButtonDown;
	bool rButtonDown;
	int scanCode;
};

struct DebugCommand {
	const char *name;
	void(*callback)(int argc, const char **argv);
};

#define MAX_DEBUG_COMMANDS 32
static DebugCommand debugCommands[MAX_DEBUG_COMMANDS];

bool getNextInputEvent(NativeEventType *outType, int *outParam1, int *outParam2, int *outParam3, bool wait)
{
	/*InputEvent *event;

	if (wait)
		WaitForSingleObject(inputQueueWait, INFINITE);

	EnterCriticalSection(&inputQueueLock);
	event = inputQueueHead;
	if (event != NULL) {
		inputQueueHead = inputQueueHead->next;
		if (inputQueueHead == NULL)
			inputQueueTail = NULL;
	}

	LeaveCriticalSection(&inputQueueLock);

	if (event != NULL) {
		*outType = event->type;
		switch (event->type) {
		case MOUSE_MOVE:
			*outParam1 = event->x;
			*outParam2 = event->y;
			break;

		case MOUSE_BUTTON:
			*outParam1 = event->x;
			*outParam2 = event->y;
			*outParam3 = (event->lButtonDown ? 1 : 0) | (event->rButtonDown ? 2 : 0);
			break;

		case KEY_DOWN:
			*outParam1 = event->scanCode;
			break;

		case KEY_UP:
			*outParam1 = event->scanCode;
			break;
		}

		free(event);
		return true;
	}
	*/
	return false;
}

void addDebugCommand(const char *name, void(*callback)(int argc, const char **argv))
{
	for (int i = 0; i < MAX_DEBUG_COMMANDS; i++) {
		if (debugCommands[i].name == NULL) {
			debugCommands[i].name = name;
			debugCommands[i].callback = callback;
			break;
		}
	}
}