//
// Copyright 1999-2002 Jeff Bush
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

/// @file ServerConnection.h
///
/// This is the servers end of the connection to the client
///
#ifndef __SERVER_CONNECTION_H
#define __SERVER_CONNECTION_H

#include "protocol.h"

#define READ_BUFFER_LENGTH 128

class ServerConnection {
public:
	ServerConnection();
	~ServerConnection();
	void write(int value);
	void write(const char *data, int length);
	void flush();
	static int doWork();
	void sendEvent(EventType type, int destinationId, int param1, int param2, int param3,
		int param4);

	static void startServer();

private:
	bool fillBuffer(int size);
	int readInt();

	/// @returns
	///   - true if more messages are available
	///   - false if no more messages can be read without blocking
	bool readMessage();
	void processMessage();

	// XXX make deliver event call this
	void sendResponse(int messageId, int windowId, int numParams, int param1 = 0, int param2 = 0,
		int param3 = 0, int param4 = 0);

	// XXX socket connection stuff
	bool fClosed;
	ServerConnection *fNext;
	ServerConnection **fPrev;
	int fFD;
	char *fWriteBuffer;
	int fDecodeOffset;
	int fWriteBufferLength;
	unsigned char fReadBuffer[READ_BUFFER_LENGTH];
	int fReadBufferLength;

	static ServerConnection *fConnectionList;
};

#endif
