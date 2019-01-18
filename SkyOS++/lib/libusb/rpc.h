/* MollenOS
 *
 * Copyright 2016, Philip Meulengracht
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
 * MollenOS MCore - Remote Procedure Definitions & Structures
 * - This header describes the base remote procedure-structure, prototypes
 *   and functionality, refer to the individual things for descriptions
 */

#ifndef __RPC_INTERFACE__
#define __RPC_INTERFACE__

#ifndef __IPC_INTERFACE__
#error "You must include ipc.h and not this directly"
#endif

#include <osdefs.h>
#include <string.h>
//#include <assert.h>

/* MRemoteCallAddress
 * A mailbox address for a remote call procedure. A remote call
 * always include both a From and To address. */
PACKED_TYPESTRUCT(MRemoteCallAddress, {
    UUId_t                  Process;
    UUId_t                  Thread;
    int                     Port;
});

/* MRemoteCallArgument
 * The argument package that can be passed
 * to an IPC function request, we support up
 * to 5 arguments */
PACKED_TYPESTRUCT(MRemoteCallArgument, {
    uint8_t                 Type;
    union {
        const void*         Buffer;
        size_t              Value;
    } Data;
    size_t                  Length;
});

/* MRemoteCall
 * The base event message structure, any IPC
 * action going through pipes in MollenOS must
 * inherit from this structure for security */
PACKED_TYPESTRUCT(MRemoteCall, {
    MRemoteCallAddress_t    To;
    MRemoteCallAddress_t    From;

    // Interface Information
    int                     Version;
    int                     Function;
    size_t                  DataLength;
    MRemoteCallArgument_t   Arguments[IPC_MAX_ARGUMENTS];
    MRemoteCallArgument_t   Result;
});

_CODE_BEGIN

/* RPCInitialize 
 * Initializes a new RPC message of the given type and length */
SERVICEAPI void SERVICEABI
RPCInitialize(
    _In_ MRemoteCall_t* RemoteCall,
    _In_ UUId_t         Target,
    _In_ int            Version,
    _In_ int            Function)
{
    // Initialize structure
    memset((void*)RemoteCall, 0, sizeof(MRemoteCall_t));
    RemoteCall->Version     = Version;
    RemoteCall->Function    = Function;

    // Setup from/to as much as possible
    RemoteCall->To.Process  = Target;
    RemoteCall->To.Port     = PIPE_REMOTECALL;
}

/* RPCSetArgument
 * Adds a new argument for the RPC request at the given argument index. 
 * It's not possible to override a current argument */
SERVICEAPI void SERVICEABI
RPCSetArgument(
    _In_ MRemoteCall_t* RemoteCall,
    _In_ int            Index, 
    _In_ const void*    Data, 
    _In_ size_t         Length)
{
    // Sanitize input parameters
    assert((RemoteCall->DataLength + Length) <= IPC_MAX_MESSAGELENGTH);
    assert((Index >= 0 && Index < IPC_MAX_ARGUMENTS) && Length > 0);
    assert(RemoteCall->Arguments[Index].Type == ARGUMENT_NOTUSED);

    // Determine the type of argument
#if __BITS == 64
    if (Length <= 8) {
#elif __BITS == 32
    if (Length <= 4) {
#endif
        RemoteCall->Arguments[Index].Type = ARGUMENT_REGISTER;

        if (Length == 1) {
            RemoteCall->Arguments[Index].Data.Value = *((uint8_t*)Data);
        }
        else if (Length == 2) {
            RemoteCall->Arguments[Index].Data.Value = *((uint16_t*)Data);
        }
        else if (Length == 4) {
            RemoteCall->Arguments[Index].Data.Value = *((uint32_t*)Data);
        }
#if __BITS == 64
        else if (Length == 8) {
            RemoteCall->Arguments[Index].Data.Value = *((uint64_t*)Data);
        }
#endif
    }
    else {
        RemoteCall->Arguments[Index].Type           = ARGUMENT_BUFFER;
        RemoteCall->Arguments[Index].Data.Buffer    = Data;
        RemoteCall->Arguments[Index].Length         = Length;
    }
    RemoteCall->DataLength += Length;
}

/* RPCSetResult
 * Installs a result buffer that will be filled with the response from the RPC request */
SERVICEAPI void SERVICEABI
RPCSetResult(
    _In_ MRemoteCall_t* RemoteCall,
    _In_ const void*    Data, 
    _In_ size_t         Length)
{
    // Always a buffer element as we need a target to copy the data into
    RemoteCall->Result.Type         = ARGUMENT_BUFFER;
    RemoteCall->Result.Data.Buffer  = Data;
    RemoteCall->Result.Length       = Length;
}

/* RPCGetStringArgument
 * Casts the argument index to a string safely and handling cases where string
 * fits entirely into the register argument. */
SERVICEAPI const char* SERVICEABI
RPCGetStringArgument(
    _In_ MRemoteCall_t* RemoteCall,
    _In_ int            Index)
{
    if (RemoteCall == NULL || Index < 0 || Index >= IPC_MAX_ARGUMENTS) {
        return NULL;
    }
    if (RemoteCall->Arguments[Index].Type == ARGUMENT_REGISTER) {
        return (const char*)&RemoteCall->Arguments[Index].Data.Value;
    }
    else if (RemoteCall->Arguments[Index].Type == ARGUMENT_BUFFER) {
        return (const char*)RemoteCall->Arguments[Index].Data.Buffer;
    }
    return NULL;
}

/* RPCCastArgumentToPointer
 * Casts the argument to a pointer safely and handling cases where data
 * fits entirely into the register argument. */
SERVICEAPI OsStatus_t SERVICEABI
RPCCastArgumentToPointer(
    _In_  MRemoteCallArgument_t*    Argument,
    _Out_ void**                    DataOut)
{
    if (Argument == NULL) {
        return OsError;
    }
    if (Argument->Type == ARGUMENT_REGISTER) {
        *DataOut = (void*)&Argument->Data.Value;
    }
    else if (Argument->Type == ARGUMENT_BUFFER) {
        *DataOut = (void*)Argument->Data.Buffer;
    }
    return OsSuccess;
}

/* RPCListen 
 * Call this to wait for a new RPC message, it automatically
 * reads the message, and all the arguments. To avoid freeing
 * an argument, set InUse to 0 */
CRTDECL(OsStatus_t,
RPCListen(
    _In_ MRemoteCall_t*         Message,
    _In_ void*                  ArgumentBuffer));

/* RPCExecute/RPCEvent
 * To get a reply from the RPC request, the user
 * must use RPCExecute, this will automatically wait
 * for a reply, whereas RPCEvent will send the request
 * and not block/wait for reply */
CRTDECL(OsStatus_t,
RPCExecute(
    _In_ MRemoteCall_t*         RemoteCall));

CRTDECL(OsStatus_t,
RPCEvent(
    _In_ MRemoteCall_t*         RemoteCall));

/* RPCRespond
 * This is a wrapper to return a respond message/buffer to the
 * sender of the message, it's good practice to always wait for
 * a result when there is going to be one */
CRTDECL(OsStatus_t,
RPCRespond(
    _In_ MRemoteCallAddress_t*  RemoteAddress,
    _In_ const void*            Buffer, 
    _In_ size_t                 Length));
_CODE_END

#endif //!__RPC_INTERFACE__
