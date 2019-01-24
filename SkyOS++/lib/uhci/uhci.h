/* MollenOS
 *
 * Copyright 2011 - 2017, Philip Meulengracht
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

#ifndef __USB_UHCI__
#define __USB_UHCI__

#include "osdefs.h"
#include "usbhost.h"
#include "collection.h"

#include "manager.h"
#include "hci.h"

/* UHCI Definitions 
 * Definitions and constants used in general for the controller setup */
#define UHCI_MAX_PORTS                  7
#define UHCI_NUM_FRAMES                 1024
#define UHCI_FRAME_MASK                 2047
#define UHCI_USBLEGEACY                 0xC0
#define UHCI_USBRES_INTEL               0xC4

/* UHCI Register Definitions
 * A list of all the fixed-offsets registers that exist in the io-space of
 * the uhci-controller. */
#define UHCI_REGISTER_COMMAND           0x00
#define UHCI_REGISTER_STATUS            0x02
#define UHCI_REGISTER_INTR              0x04
#define UHCI_REGISTER_FRNUM             0x06
#define UHCI_REGISTER_FRBASEADDR        0x08
#define UHCI_REGISTER_SOFMOD            0x0C
#define UHCI_REGISTER_PORT_BASE         0x10

/* UHCI Register Definitions
 * Bit definitions for the Command register */
#define UHCI_COMMAND_RUN                0x1
#define UHCI_COMMAND_HCRESET            0x2
#define UHCI_COMMAND_GRESET             0x4
#define UHCI_COMMAND_SUSPENDMODE        0x8
#define UHCI_COMMAND_GLBRESUME          0x10
#define UHCI_COMMAND_DEBUG              0x20
#define UHCI_COMMAND_CONFIGFLAG         0x40
#define UHCI_COMMAND_MAXPACKET64        0x80

/* UHCI Register Definitions
 * Bit definitions for the Status register */
#define UHCI_STATUS_USBINT              0x1
#define UHCI_STATUS_INTR_ERROR          0x2
#define UHCI_STATUS_RESUME_DETECT       0x4
#define UHCI_STATUS_HOST_SYSERR         0x8
#define UHCI_STATUS_PROCESS_ERR         0x10
#define UHCI_STATUS_HALTED              0x20
#define UHCI_STATUS_INTMASK             0x1F

/* UHCI Register Definitions
 * Bit definitions for the Interrupt register */
#define UHCI_INTR_TIMEOUT               0x1
#define UHCI_INTR_RESUME                0x2
#define UHCI_INTR_COMPLETION            0x4
#define UHCI_INTR_SHORT_PACKET          0x8

/* UHCI Register Definitions
 * Bit definitions for the Port register */
#define UHCI_PORT_CONNECT_STATUS        0x1
#define UHCI_PORT_CONNECT_EVENT         0x2
#define UHCI_PORT_ENABLED               0x4
#define UHCI_PORT_ENABLED_EVENT         0x8
#define UHCI_PORT_LINE_STATUS           0x30
#define UHCI_PORT_RESUME_DETECT         0x40
#define UHCI_PORT_RESERVED              0x80
#define UHCI_PORT_LOWSPEED              0x100
#define UHCI_PORT_RESET                 0x200
#define UHCI_PORT_RESERVED1             0x400
#define UHCI_PORT_RESERVED2             0x800
#define UHCI_PORT_SUSPEND               0x1000

/* UhciTransferDescriptor::Link & UhciQueueHead::Link,Child
 * Contains definitions and bitfield definitions for UhciTransferDescriptor::Link */
#define UHCI_LINK_END                   0x1
#define UHCI_LINK_QH                    0x2        // 1 => Qh, 0 => Td
#define UHCI_LINK_DEPTH                 0x4        // 1 => Depth, 0 => Breadth

/* UhciTransferDescriptor
 * Describes a generic transfer-descriptor that can be either of all
 * four different transaction types. Must be 16 byte aligned. */
PACKED_TYPESTRUCT(UhciTransferDescriptor, {
    reg32_t                 Link;
    reg32_t                 Flags;
    reg32_t                 Header;
    reg32_t                 Buffer;

    // Software meta-data
    UsbSchedulerObject_t    Object;
    reg32_t                 OriginalFlags;
    reg32_t                 OriginalHeader;
});

/* UhciTransferDescriptor::Flags
 * Contains definitions and bitfield definitions for UhciTransferDescriptor::Flags */
#define UHCI_TD_LENGTH_MASK             0x7FF
#define UHCI_TD_ACTIVE                  0x800000
#define UHCI_TD_IOC                     0x1000000
#define UHCI_TD_ISOCHRONOUS             0x2000000
#define UHCI_TD_LOWSPEED                0x4000000
#define UHCI_TD_SETCOUNT(n)             ((n & 0x3) << 27)
#define UHCI_TD_SHORT_PACKET            (1 << 29)
#define UHCI_TD_ACTUALLENGTH(n)         (n & UHCI_TD_LENGTH_MASK)

#define UHCI_TD_GETCOUNT(n)             ((n >> 27) & 0x3)
#define UHCI_TD_STATUS(n)               ((n >> 17) & 0x3F)

/* UhciTransferDescriptor::Header
 * Contains definitions and bitfield definitions for UhciTransferDescriptor::Header */
#define UHCI_TD_PID_SETUP               0x2D
#define UHCI_TD_PID_IN                  0x69
#define UHCI_TD_PID_OUT                 0xE1
#define UHCI_TD_DEVICE_ADDR(n)          ((n & 0x7F) << 8)
#define UHCI_TD_EP_ADDR(n)              ((n & 0xF) << 15)
#define UHCI_TD_DATA_TOGGLE             (1 << 19)
#define UHCI_TD_MAX_LEN(n)              ((n & UHCI_TD_LENGTH_MASK) << 21)
#define UHCI_TD_GET_LEN(n)              ((n >> 21) & UHCI_TD_LENGTH_MASK)

/* Queue Head
 * @todo */
PACKED_TYPESTRUCT(UhciQueueHead, {
    reg32_t                 Link;
    reg32_t                 Child;

    // Software meta-data
    UsbSchedulerObject_t    Object;
    reg32_t                 BufferBase;
    uint8_t                 Queue;
});

/* Uhci Definitions
 * Pool sizes and helper functions. */
#define UHCI_QH_ALIGNMENT                   16
#define UHCI_QH_POOL                        0
#define UHCI_QH_COUNT                       50

#define UHCI_TD_ALIGNMENT                   16
#define UHCI_TD_POOL                        1
#define UHCI_TD_COUNT                       400

/* Uhci Definitions
 * Uhci fixed pool indicies that are already in use. */
#define UHCI_POOL_QH_NULL                   0
#define UHCI_POOL_QH_ISOCHRONOUS            1
#define UHCI_POOL_QH_ASYNC                  9
#define UHCI_POOL_QH_LCTRL                  10
#define UHCI_POOL_QH_FCTRL                  11
#define UHCI_POOL_QH_FBULK                  12
#define UHCI_POOL_QH_START                  13

#define UHCI_POOL_TD_NULL                   0
#define UHCI_POOL_TD_START                  1

/* UhciController 
 * Contains all per-controller information that is
 * needed to control, queue and handle devices on an uhci-controller. */
typedef struct _UhciController {
    UsbManagerController_t  Base;
    int                     Frame;
} UhciController_t;

/*******************************************************************************
 * Input/Output Methods
 *******************************************************************************/

/* UhciRead16
 * Reads a 2-byte value from the control-space of the controller */
__EXTERN uint16_t
UhciRead16(
    _In_ UhciController_t*  Controller, 
    _In_ uint16_t           Register);

/* UhciRead32
 * Reads a 4-byte value from the control-space of the controller */
__EXTERN uint32_t
UhciRead32(
    _In_ UhciController_t*  Controller, 
    _In_ uint16_t           Register);

/* UhciWrite8
 * Writes a single byte value to the control-space of the controller */
__EXTERN void
UhciWrite8(
    _In_ UhciController_t*  Controller, 
    _In_ uint16_t           Register, 
    _In_ uint8_t            Value);

/* UhciWrite16
 * Writes a 2-byte value to the control-space of the controller */
__EXTERN void
UhciWrite16(
    _In_ UhciController_t*  Controller, 
    _In_ uint16_t           Register, 
    _In_ uint16_t           Value);

/* UhciWrite32
 * Writes a 4-byte value to the control-space of the controller */
__EXTERN void 
UhciWrite32(
    _In_ UhciController_t*  Controller, 
    _In_ uint16_t           Register, 
    _In_ uint32_t           Value);

/*******************************************************************************
 * Controller Methods
 *******************************************************************************/

/* UhciGetStatusCode
 * Retrieves a status-code from a given condition code */
__EXTERN UsbTransferStatus_t
UhciGetStatusCode(
    _In_ int                        ConditionCode);

/* UhciStart
 * Boots the controller, if it succeeds OsSuccess is returned. */
__EXTERN OsStatus_t
UhciStart(
    _In_ UhciController_t*          Controller,
    _In_ int                        Wait);

/* UhciStop
 * Stops the controller, if it succeeds OsSuccess is returned. */
__EXTERN OsStatus_t
UhciStop(
    _In_ UhciController_t*          Controller);

/* UhciReset
 * Resets the controller back to usable state, does not restart the controller. */
__EXTERN OsStatus_t
UhciReset(
    _In_ UhciController_t*          Controller);

/* UhciQueueInitialize
 * Initialize the controller's queue resources and resets counters */
__EXTERN OsStatus_t
UhciQueueInitialize(
    _In_ UhciController_t*          Controller);

/* UhciQueueReset
 * Removes and cleans up any existing transfers, then reinitializes. */
__EXTERN OsStatus_t
UhciQueueReset(
    _In_ UhciController_t*          Controller);

/* UhciQueueDestroy
 * Cleans up any resources allocated by QueueInitialize */
__EXTERN OsStatus_t
UhciQueueDestroy(
    _In_ UhciController_t*          Controller);

/* UhciPortPrepare
 * Resets the port and also clears out any event on the port line. */
__EXTERN OsStatus_t
UhciPortPrepare(
    _In_ UhciController_t*          Controller, 
    _In_ int                        Index);

/* UhciPortsCheck
 * Enumerates ports and checks for any pending events. This also
 * notifies the usb-service if any connection changes appear */
__EXTERN OsStatus_t
UhciPortsCheck(
    _In_ UhciController_t*          Controller);

/* UhciUpdateCurrentFrame
 * Updates the current frame and stores it in the controller given.
 * OBS: Needs to be called regularly */
__EXTERN void
UhciUpdateCurrentFrame(
    _In_ UhciController_t*          Controller);

/* UhciConditionCodeToIndex
 * Converts the given condition-code in a TD to a string-index */
__EXTERN int
UhciConditionCodeToIndex(
    _In_ int                        ConditionCode);

/*******************************************************************************
 * Queue Head Methods
 *******************************************************************************/

/* UhciQhInitialize
 * Initializes the queue head data-structure and the associated
 * hcd flags. Afterwards the queue head is ready for use. */
__EXTERN OsStatus_t
UhciQhInitialize(
    _In_ UhciController_t*          Controller,
    _In_ UsbManagerTransfer_t*      Transfer);

/* UhciQhDump
 * Dumps the information contained in the queue-head by writing it to stdout */
__EXTERN void
UhciQhDump(
    _In_ UhciController_t*          Controller,
    _In_ UhciQueueHead_t*           Qh);

/* UhciQhRestart
 * Restarts an interrupt QH by resetting it to it's start state */
__EXTERN void
UhciQhRestart(
    _In_ UhciController_t*          Controller,
    _In_ UsbManagerTransfer_t*      Transfer);

/* UhciQhLink 
 * Link a given queue head into the correct queue determined by Qh->Queue.
 * This can handle linkage of async and interrupt transfers. */
__EXTERN void
UhciQhLink(
    _In_ UhciController_t*          Controller,
    _In_ UhciQueueHead_t*           Qh);

/* UhciQhUnlink 
 * Unlinks a given queue head from the correct queue determined by Qh->Queue.
 * This can handle removal of async and interrupt transfers. */
__EXTERN void
UhciQhUnlink(
    _In_ UhciController_t*          Controller,
    _In_ UhciQueueHead_t*           Qh);

/*******************************************************************************
 * Transfer Descriptor Methods
 *******************************************************************************/

/* UhciTdSetup
 * Creates a new setup token td and initializes all the members.
 * The Td is immediately ready for execution. */
__EXTERN void
UhciTdSetup(
    _In_ UhciTransferDescriptor_t*  Td,
    _In_ uintptr_t                  BufferAddress,
    _In_ size_t                     Address, 
    _In_ size_t                     Endpoint,
    _In_ UsbSpeed_t                 Speed);

/* UhciTdIo
 * Creates a new io token td and initializes all the members.
 * The Td is immediately ready for execution. */
__EXTERN void
UhciTdIo(
    _In_ UhciTransferDescriptor_t*  Td,
    _In_ UsbTransferType_t          Type,
    _In_ uint32_t                   PId,
    _In_ int                        Toggle,
    _In_ size_t                     Address, 
    _In_ size_t                     Endpoint,
    _In_ size_t                     MaxPacketSize,
    _In_ UsbSpeed_t                 Speed,
    _In_ uintptr_t                  BufferAddress,
    _In_ size_t                     Length);

/* UhciTdDump
 * Dumps the information contained in the descriptor by writing it. */
__EXTERN void
UhciTdDump(
    _In_ UhciController_t*          Controller,
    _In_ UhciTransferDescriptor_t*  Td);

/* UhciTdValidate
 * Checks the transfer descriptors for errors and updates the transfer that is attached
 * with the bytes transferred and error status. */
__EXTERN void
UhciTdValidate(
    _In_  UsbManagerTransfer_t*     Transfer,
    _In_  UhciTransferDescriptor_t* Td);

/* UhciTdSynchronize
 * Synchronizes the toggle status of the transfer descriptor by retrieving
 * current and updating the pipe toggle. */
__EXTERN void
UhciTdSynchronize(
    _In_  UsbManagerTransfer_t*     Transfer,
    _In_  UhciTransferDescriptor_t* Td);

/* UhciTdRestart
 * Restarts a transfer descriptor by resettings it's status and updating buffers if the
 * trasnfer type is an interrupt-transfer that uses circularbuffers. */
__EXTERN void
UhciTdRestart(
    _In_  UsbManagerTransfer_t*     Transfer,
    _In_  UhciTransferDescriptor_t* Td);

/*******************************************************************************
 * Transaction Methods
 *******************************************************************************/

/* UhciTransactionDispatch
 * Queues the transfer up in the controller hardware, after finalizing the
 * transactions and preparing them. */
__EXTERN UsbTransferStatus_t
UhciTransactionDispatch(
    _In_ UhciController_t*          Controller,
    _In_ UsbManagerTransfer_t*      Transfer);

#endif // !__USB_UHCI__
