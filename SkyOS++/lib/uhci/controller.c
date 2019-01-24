/* MollenOS
 *
 * Copyright 2011, Philip Meulengracht
 *
 * This program is free software : you can redistribute it and / or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation ? , either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.If not, see <http://www.gnu.org/licenses/>.
 *
 *
 * MollenOS MCore - Universal Host Controller Interface Driver
 * Todo:
 * Power Management
 */
//#define __TRACE

/* Includes
 * - System */
#include "utils.h"
#include "uhci.h"

/* Includes
 * - Library */
/*#include <threads.h>
#include <stdlib.h>
#include <string.h>*/

/* Prototypes 
 * This is to keep the create/destroy at the top of the source file */
OsStatus_t          UhciSetup(UhciController_t *Controller);
InterruptStatus_t   OnFastInterrupt(FastInterruptResources_t*, void*);

/* Globals
 * State-keeping and single-time stuff. */
static int TimerRegistered = 0;

/* HciTimerCallback 
 * The function to check up on ports at regular intervals. */
void
HciTimerCallback(void)
{
    // Do a port-check and perform transaction checks
    foreach(cNode, UsbManagerGetControllers()) {
        UhciUpdateCurrentFrame((UhciController_t*)cNode->Data);
        UhciPortsCheck((UhciController_t*)cNode->Data);
        UsbManagerProcessTransfers((UsbManagerController_t*)cNode->Data);
    }
}

/* UhciControllerDump 
 * Dumps all the controller registers */
void
UhciControllerDump(
    _In_ UhciController_t*          Controller)
{
    TRACE("Command(0x%x), Status(0x%x), Interrupt(0x%x)", 
        UhciRead16(Controller, UHCI_REGISTER_COMMAND),
        UhciRead16(Controller, UHCI_REGISTER_STATUS),
        UhciRead16(Controller, UHCI_REGISTER_INTR));
    TRACE("FrameNumber(0x%x), BaseAddress(0x%x), Sofmod(0x%x)", 
        UhciRead16(Controller, UHCI_REGISTER_FRNUM),
        UhciRead16(Controller, UHCI_REGISTER_FRBASEADDR),
        UhciRead16(Controller, UHCI_REGISTER_SOFMOD));
}

/* HciControllerCreate 
 * Initializes and creates a new Hci Controller instance
 * from a given new system device on the bus. */
UsbManagerController_t*
HciControllerCreate(
    _In_ MCoreDevice_t*             Device)
{
    // Variables
    UhciController_t *Controller    = NULL;
    DeviceIo_t *IoBase              = NULL;
    Flags_t IoctlValue              = 0;
    int i;
    
    // Debug
    TRACE("UhciControllerCreate(%s)", &Device->Name[0]);

    // Register the callback if it's not already
    if (TimerRegistered == 0) {
        UsbManagerRegisterTimer(MSEC_PER_SEC, HciTimerCallback);
        TimerRegistered = 1;
    }

    // Allocate a new instance of the controller
    Controller = (UhciController_t*)malloc(sizeof(UhciController_t));
    memset(Controller, 0, sizeof(UhciController_t));
    memcpy(&Controller->Base.Device, Device, Device->Length);

    // Fill in some basic stuff needed for init
    Controller->Base.Contract.DeviceId  = Controller->Base.Device.Id;
    Controller->Base.Type               = UsbUHCI;
    Controller->Base.TransactionList    = CollectionCreate(KeyInteger);
    Controller->Base.Endpoints          = CollectionCreate(KeyInteger);
    SpinlockReset(&Controller->Base.Lock);

    // Get I/O Base, and for UHCI it'll be the first address we encounter
    // of type IO
    for (i = 0; i < __DEVICEMANAGER_MAX_IOSPACES; i++) {
        if (Controller->Base.Device.IoSpaces[i].Type == DeviceIoPortBased) {
            TRACE(" > found io-space at bar %i", i);
            IoBase = &Controller->Base.Device.IoSpaces[i];
            break;
        }
    }

    // Sanitize that we found the io-space
    if (IoBase == NULL) {
        ERROR("No memory space found for uhci-controller");
        free(Controller);
        return NULL;
    }

    // Trace
    TRACE("Found Io-Space (Type %u, Physical 0x%x, Size 0x%x)",
        IoBase->Type, IoBase->PhysicalBase, IoBase->Size);

    // Acquire the io-space
    if (AcquireDeviceIo(IoBase) != OsSuccess) {
        ERROR("Failed to create and acquire the io-space for uhci-controller");
        free(Controller);
        return NULL;
    }
    else {
        // Store information
        Controller->Base.IoBase = IoBase;
    }

    // Start out by initializing the contract
    InitializeContract(&Controller->Base.Contract, Controller->Base.Contract.DeviceId, 1,
        ContractController, "UHCI Controller Interface");

    // Initialize the interrupt settings
    RegisterFastInterruptHandler(&Controller->Base.Device.Interrupt, OnFastInterrupt);
    RegisterFastInterruptIoResource(&Controller->Base.Device.Interrupt, IoBase);
    RegisterFastInterruptMemoryResource(&Controller->Base.Device.Interrupt, (uintptr_t)Controller, sizeof(UhciController_t), 0);

    // Register contract before interrupt
    if (RegisterContract(&Controller->Base.Contract) != OsSuccess) {
        ERROR("Failed to register contract for uhci-controller");
        ReleaseDeviceIo(Controller->Base.IoBase);
        free(Controller);
        return NULL;
    }

    // Register interrupt
    RegisterInterruptContext(&Controller->Base.Device.Interrupt, Controller);
    Controller->Base.Interrupt = RegisterInterruptSource(
        &Controller->Base.Device.Interrupt, INTERRUPT_USERSPACE);

    // Enable device
    if (IoctlDevice(Controller->Base.Device.Id, __DEVICEMANAGER_IOCTL_BUS,
        (__DEVICEMANAGER_IOCTL_ENABLE | __DEVICEMANAGER_IOCTL_IO_ENABLE
            | __DEVICEMANAGER_IOCTL_BUSMASTER_ENABLE)) != OsSuccess) {
        ERROR("Failed to enable the uhci-controller");
        UnregisterInterruptSource(Controller->Base.Interrupt);
        ReleaseDeviceIo(Controller->Base.IoBase);
        free(Controller);
        return NULL;
    }

    // Claim the BIOS ownership and enable pci interrupts
    IoctlValue = 0x2000;
    if (IoctlDeviceEx(Controller->Base.Device.Id, __DEVICEMANAGER_IOCTL_EXT_WRITE, 
            UHCI_USBLEGEACY, &IoctlValue, 2) != OsSuccess) {
        return NULL;
    }

    // If vendor is Intel we null out the intel register
    if (Controller->Base.Device.VendorId == 0x8086) {
        IoctlValue = 0x00;
        if (IoctlDeviceEx(Controller->Base.Device.Id, __DEVICEMANAGER_IOCTL_EXT_WRITE, 
                UHCI_USBRES_INTEL, &IoctlValue, 1) != OsSuccess) {
            return NULL;
        }
    }

    // Now that all formalities has been taken care
    // off we can actually setup controller
    if (UhciSetup(Controller) == OsSuccess) {
        return &Controller->Base;
    }
    else {
        HciControllerDestroy(&Controller->Base);
        return NULL;
    }
}

/* HciControllerDestroy
 * Destroys an existing controller instance and cleans up
 * any resources related to it */
OsStatus_t
HciControllerDestroy(
    _In_ UsbManagerController_t*    Controller)
{
    // Cleanup scheduler
    UhciQueueDestroy((UhciController_t*)Controller);

    // Unregister the interrupt
    UnregisterInterruptSource(Controller->Interrupt);

    // Release the io-space
    ReleaseDeviceIo(Controller->IoBase);

    // Clean up allocated lists
    CollectionDestroy(Controller->TransactionList);
    CollectionDestroy(Controller->Endpoints);
    free(Controller);
    return OsSuccess;
}

/* UhciStart
 * Boots the controller, if it succeeds OsSuccess is returned. */
OsStatus_t
UhciStart(
    _In_ UhciController_t*  Controller,
    _In_ int                Wait)
{
    // Variables
    uint16_t OldCmd = 0;
    
    // Debug
    TRACE("UhciStart()");

    // Read current command register
    // to preserve information, then assert some flags
    OldCmd  = UhciRead16(Controller, UHCI_REGISTER_COMMAND);
    OldCmd  |= (UHCI_COMMAND_CONFIGFLAG | UHCI_COMMAND_RUN | UHCI_COMMAND_MAXPACKET64);

    // Update
    UhciWrite16(Controller, UHCI_REGISTER_COMMAND, OldCmd);
    if (Wait == 0) {
        return OsSuccess;
    }

    // Wait for controller to start
    OldCmd  = 0;
    WaitForConditionWithFault(OldCmd, 
        (UhciRead16(Controller, UHCI_REGISTER_STATUS) & UHCI_STATUS_HALTED) == 0, 100, 10);
    return (OldCmd == 0) ? OsSuccess : OsError;
}

/* UhciStop
 * Stops the controller, if it succeeds OsSuccess is returned. */
OsStatus_t
UhciStop(
    _In_ UhciController_t*  Controller)
{
    // Variables
    uint16_t OldCmd = 0;
    
    // Read current command register
    // to preserve information, then deassert run flag
    OldCmd          = UhciRead16(Controller, UHCI_REGISTER_COMMAND);
    OldCmd          &= ~(UHCI_COMMAND_RUN);

    // Update
    UhciWrite16(Controller, UHCI_REGISTER_COMMAND, OldCmd);
    return OsSuccess;
}

/* UhciReset
 * Resets the controller back to usable state, does not restart the controller. */
OsStatus_t
UhciReset(
    _In_ UhciController_t*  Controller)
{
    // Variables
    uint16_t Temp = 0;

    // Assert the host-controller reset bit
    UhciWrite16(Controller, UHCI_REGISTER_COMMAND, UHCI_COMMAND_HCRESET);

    // Wait for it to stop being asserted
    WaitForConditionWithFault(Temp, (UhciRead16(Controller, UHCI_REGISTER_COMMAND) & UHCI_COMMAND_HCRESET) == 0, 100, 25);
    if (Temp == 1) {
        WARNING("UHCI: Reset signal is still active..");
        thrd_sleepex(200); // give it another try
        if (UhciRead16(Controller, UHCI_REGISTER_COMMAND) & UHCI_COMMAND_HCRESET) {
            ERROR("UHCI::Giving up on controller reset");
            return OsError;
        }
    }

    // Clear out command and interrupt register
    UhciWrite16(Controller, UHCI_REGISTER_COMMAND,  0x0000);
    UhciWrite16(Controller, UHCI_REGISTER_INTR,     0x0000);

    // Now reconfigure the controller
    UhciWrite8(Controller, UHCI_REGISTER_SOFMOD,    64); // Frame length 1 ms
    UhciWrite32(Controller, UHCI_REGISTER_FRBASEADDR, 
        Controller->Base.Scheduler->Settings.FrameListPhysical);
    UhciWrite16(Controller, UHCI_REGISTER_FRNUM,    (Controller->Frame & UHCI_FRAME_MASK));

    // Enable the interrupts that are relevant
    UhciWrite16(Controller, UHCI_REGISTER_INTR, 
        (UHCI_INTR_TIMEOUT | UHCI_INTR_SHORT_PACKET
        | UHCI_INTR_RESUME | UHCI_INTR_COMPLETION));
    return OsSuccess;
}

/* UhciSetup
 * Initializes the controller state and resources */
OsStatus_t
UhciSetup(
    _In_ UhciController_t *Controller)
{
    // Variables
    uint16_t Temp       = 0;
    int i;
    
    // Debug
    TRACE("UhciSetup()");

    // Disable interrupts while configuring (and stop controller)
    UhciWrite16(Controller, UHCI_REGISTER_COMMAND,  0x0000);
    UhciWrite16(Controller, UHCI_REGISTER_INTR,     0x0000);

    // Perform a global reset, we must wait 100 ms for this complete
    UhciWrite16(Controller, UHCI_REGISTER_COMMAND,  UHCI_COMMAND_GRESET);
    thrd_sleepex(100);
    UhciWrite16(Controller, UHCI_REGISTER_COMMAND,  0x0000);

    // Initialize queues and controller
    TRACE(" > Initializing queue system");
    UhciQueueInitialize(Controller);
    TRACE(" > Resetting controller");
    if (UhciReset(Controller) != OsSuccess) {
        ERROR("UHCI::Failed to reset controller");
        return OsError;
    }

    // Enumerate all available ports
    for (i = 0; i <= UHCI_MAX_PORTS; i++) {
        Temp = UhciRead16(Controller, (UHCI_REGISTER_PORT_BASE + (i * 2)));
        if (!(Temp & UHCI_PORT_RESERVED) || Temp == 0xFFFF) {
            // This reserved bit must be 1
            // And we must have 2 ports atleast
            break;
        }
    }

    // Store the number of available ports
    TRACE(" > Ports(%i)", i);
    Controller->Base.PortCount = i;

    // Register the controller before starting
    TRACE(" > Registering");
    if (UsbManagerRegisterController(&Controller->Base) != OsSuccess) {
        ERROR("Failed to register uhci controller with the system.");
    }

    // Start the controller and return result from that
    TRACE(" > Booting");
    return UhciStart(Controller, 1);
}
