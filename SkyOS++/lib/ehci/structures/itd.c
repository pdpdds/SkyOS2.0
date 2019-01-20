/* MollenOS
 *
 * Copyright 2018, Philip Meulengracht
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
 * MollenOS MCore - Enhanced Host Controller Interface Driver
 * TODO:
 * - Power Management
 * - Transaction Translator Support
 */
//#define __TRACE

/* Includes
 * - System */
#include "osdefs.h"
#include "usb.h"
#include "scheduler.h"
#include "utils.h"
#include "../ehci.h"

/* Includes
 * - Library */
//#include <stdlib.h>
//#include <assert.h>
//#include <string.h>

/* EhciTdIsochronous
 * This initiates any periodic scheduling information that might be needed */
OsStatus_t
EhciTdIsochronous(
    _In_ EhciController_t*              Controller,
    _In_ UsbTransfer_t*                 Transfer,
    _In_ EhciIsochronousDescriptor_t*   iTd,
    _In_ uintptr_t                      BufferAddress,
    _In_ size_t                         ByteCount,
    _In_ size_t                         Address,
    _In_ size_t                         Endpoint)
{
    // Variables
    uintptr_t BufferIterator    = BufferAddress;
    uintptr_t PageMask          = ~((uintptr_t)0xFFF);
    OsStatus_t Status           = OsSuccess;
    size_t BytesLeft            = ByteCount;
    size_t PageIndex            = 0;
    int i;

    // Set link
    iTd->Link                   = EHCI_LINK_END;
    iTd->Object.Flags           |= EHCI_LINK_iTD;

    // Fill in buffer page settings and initial page
    for (i = 0; i < 7; i++) {
        iTd->Buffers[i]         = 0;
        iTd->ExtBuffers[i]      = 0;
        if (i == 0) { // Initialize the status word/bp0
            iTd->Buffers[i]     = EHCI_iTD_DEVADDR(Address);
            iTd->Buffers[i]     |= EHCI_iTD_EPADDR(Endpoint);
            iTd->Buffers[i]     |= EHCI_iTD_BUFFER(BufferAddress);
#if __BITS == 64
            if (Controller->CParameters & EHCI_CPARAM_64BIT) {
                iTd->ExtBuffers[i] = EHCI_iTD_EXTBUFFER(BufferAddress);
            }
#endif
        }
        else if (i == 1) { // Initialize the mps word
            iTd->Buffers[i]     = EHCI_iTD_MPS(Transfer->Endpoint.MaxPacketSize);
            if (Transfer->Endpoint.Direction == USB_ENDPOINT_IN) {
                iTd->Buffers[i] |= EHCI_iTD_IN;
            }
        }
        else if (i == 2) { // Initialize the multi word
            iTd->Buffers[i]     = MAX(3, Transfer->Endpoint.Bandwidth);
        }
    }

    // Fill in transactions
    for (i = 0; i < 8; i++) {
        if (BytesLeft > 0) {
            size_t PageBytes        = MIN(BytesLeft, 1024 * MAX(3, Transfer->Endpoint.Bandwidth));
            iTd->Transactions[i]    = EHCI_iTD_OFFSET(BufferIterator);
            iTd->Transactions[i]    |= EHCI_iTD_PAGE(PageIndex);
            iTd->Transactions[i]    |= EHCI_iTD_LENGTH(PageBytes);
            iTd->Transactions[i]    |= EHCI_iTD_ACTIVE;
            if ((BufferIterator & PageMask) != ((BufferIterator + PageBytes) & PageMask)) {
                PageIndex++;
                iTd->Buffers[PageIndex] |= EHCI_iTD_BUFFER((BufferIterator + PageBytes));
#if __BITS == 64
                if (Controller->CParameters & EHCI_CPARAM_64BIT) {
                    iTd->ExtBuffers[PageIndex] = EHCI_iTD_EXTBUFFER((BufferIterator + PageBytes));
                }
#endif
            }
            BufferIterator          += PageBytes;
            BytesLeft               -= PageBytes;
            if (BytesLeft == 0) {
                iTd->Transactions[i] |= EHCI_iTD_IOC;
            }
        }
        else {
            iTd->Transactions[i]    = 0;
        }

        // Create copies of transaction details
        iTd->TransactionsCopy[i] = iTd->Transactions[i];
    }

    // Handle bandwidth allocation
    Status = UsbSchedulerAllocateBandwidth(Controller->Base.Scheduler, 
        &Transfer->Endpoint, ByteCount, Transfer->Type, Transfer->Speed, (uint8_t*)iTd);
    return Status;
}

/* EhciiTdDump
 * Dumps the information contained in the descriptor by writing it. */
void
EhciiTdDump(
    _In_ EhciController_t*              Controller,
    _In_ EhciIsochronousDescriptor_t*   Td)
{
    // Variables
    uintptr_t PhysicalAddress   = 0;

    UsbSchedulerGetPoolElement(Controller->Base.Scheduler, EHCI_iTD_POOL, 
        Td->Object.Index & USB_ELEMENT_INDEX_MASK, NULL, &PhysicalAddress);
    WARNING("EHCI: iTD(0x%x), Link(0x%x), Buffer0(0x%x:0x%x), Buffer1(0x%x:0x%x), Buffer2(0x%x:0x%x)",
        PhysicalAddress, Td->Link, Td->ExtBuffers[0], Td->Buffers[0],
        Td->ExtBuffers[1], Td->Buffers[1], Td->ExtBuffers[2], Td->Buffers[2]);
    WARNING("          Buffer3(0x%x), Buffer4(0x%x:0x%x), Buffer5(0x%x:0x%x)",
        Td->ExtBuffers[3], Td->Buffers[3], Td->ExtBuffers[4], Td->Buffers[4], Td->ExtBuffers[5], Td->Buffers[5]);
    WARNING("          Buffer6(0x%x:0x%x), XAction0(0x%x), XAction1(0x%x)", 
        Td->ExtBuffers[6], Td->Buffers[6], Td->Transactions[0], Td->Transactions[1]);
    WARNING("          XAction2(0x%x), XAction3(0x%x), XAction4(0x%x)", 
        Td->Transactions[2], Td->Transactions[3], Td->Transactions[4]);
    WARNING("          XAction5(0x%x), XAction6(0x%x), XAction7(0x%x)", 
        Td->Transactions[5], Td->Transactions[6], Td->Transactions[7]);
}

/* EhciiTdValidate
 * Checks the transfer descriptors for errors and updates the transfer that is attached
 * with the bytes transferred and error status. */
void
EhciiTdValidate(
    _In_ UsbManagerTransfer_t*          Transfer,
    _In_ EhciIsochronousDescriptor_t*   Td)
{
    // Variables
    int i;
    int ConditionCode                   = 0;

    // Don't check more if there is an error condition
    if (Transfer->Status != TransferFinished && Transfer->Status != TransferQueued) {
        return;
    }

    // Check status of the transactions
    for (i = 0; i < 8; i++) {
        if (Td->Transactions[i] & EHCI_iTD_ACTIVE) {
            break;
        }
        Transfer->Status            = TransferFinished;
        Transfer->TransactionsExecuted++;
        ConditionCode               = EhciConditionCodeToIndex(EHCI_iTD_CC(Td->Transactions[i]));
        switch (ConditionCode) {
            case 1:
                Transfer->Status    = TransferStalled;
                break;
            case 2:
                Transfer->Status    = TransferBabble;
                break;
            case 3:
                Transfer->Status    = TransferBufferError;
                break;
            default:
                break;
        }
        
        // Break out early if error is encountered
        if (Transfer->Status != TransferFinished) {
            break;
        }
    }
}

/* EhciiTdRestart
 * Restarts a transfer descriptor by resettings it's status and updating buffers if the
 * trasnfer type is an interrupt-transfer that uses circularbuffers. */
void
EhciiTdRestart(
    _In_ EhciController_t*              Controller,
    _In_ UsbManagerTransfer_t*          Transfer,
    _In_ EhciIsochronousDescriptor_t*   Td)
{
    // Restore transactions
    for (int i = 0; i < 8; i++) {
        Td->Transactions[i] = Td->TransactionsCopy[i];
    }
}
