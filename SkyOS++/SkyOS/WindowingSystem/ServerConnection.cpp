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

#include "windef.h"
#include <errno.h>
//#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
//#include <stdlib.h>
//#include <winsock2.h>
#include "ServerConnection.h"
#include "IDTable.h"
#include "WindowManager.h"
#include "TitledWindow.h"
#include "memory.h"


ServerConnection *ServerConnection::fConnectionList;
static IDTable idtab;
extern WindowManager *windowManager;

ServerConnection::ServerConnection()
	:	fWriteBuffer(NULL),
		fWriteBufferLength(0),
		fReadBufferLength(0),
		fClosed(false)
{
}

ServerConnection::~ServerConnection()
{
	/*if (fFD >= 0)
		closesocket(fFD);

	*fPrev = fNext;
	if (fNext != NULL)
		fNext->fPrev = fPrev;

	free(fWriteBuffer);*/
}

// Event:
// int length
// int class : 8
// int id : 24
// int windowid
// int param1
// int param2
// int param3
// int param4
void ServerConnection::sendEvent(EventType type, int destinationId, int param1, int param2, int param3,
	int param4)
{
	// Header
	write(28);	// length
	write((M_EVENT << 24) | type);
	write(destinationId);
	write(param1);
	write(param2);
	write(param3);
	write(param4);
}

int ServerConnection::readInt()
{
	if (fDecodeOffset + 4 > fReadBufferLength) {
		printf("Malformed message: truncated\n");
		return 0;
	}

	int value = (fReadBuffer[fDecodeOffset] << 24)
		| (fReadBuffer[fDecodeOffset + 1] << 16)
		| (fReadBuffer[fDecodeOffset + 2] << 8)
		| fReadBuffer[fDecodeOffset + 3];


	fDecodeOffset += 4;

	return value;
}

static DWORD WINAPI ProtocolWorker(LPVOID param)
{
	return ServerConnection::doWork();
}

void ServerConnection::startServer()
{
//	CloseHandle(CreateThread(NULL, 8192, ProtocolWorker, NULL, 0, NULL));
}

void acquireBigLock();
void releaseBigLock();

int ServerConnection::doWork()
{
	/*fd_set readfds;
	fd_set writefds;
	int accept_socket;
	ServerConnection *connection;
	ServerConnection *dead;
	int highest_fd;
	struct sockaddr_in sa;
	int sa_length;
	int ret;

	accept_socket = socket(PF_INET, SOCK_STREAM, 0);
	if (accept_socket < 0) {
		perror("error creating socket");
		return 1;
	}

	sa.sin_family = AF_INET;
	sa.sin_port = htons(8000);
	sa.sin_addr.s_addr = INADDR_ANY;
	if (bind(accept_socket, (struct sockaddr*) &sa, sizeof(sa)) < 0) {
		perror("error binding socket");
		return 1;
	}

	if (listen(accept_socket, 4) < 0) {
		perror("error listening");
		return 1;
	}

	for (;;) {
		// Prepare to wait for I/O
		FD_ZERO(&readfds);
		FD_ZERO(&writefds);

		// Walk through all active sockets and set bits to
		// select on those sockets
		FD_SET(accept_socket, &readfds);
		highest_fd = accept_socket;
		int num_sockets = 0;
		for (connection = fConnectionList; connection != NULL; connection = connection->fNext) {
			if (connection->fFD >= 0) {
				num_sockets++;
				FD_SET(connection->fFD, &readfds);
				if (connection->fWriteBufferLength != 0)
					FD_SET(connection->fFD, &writefds);

				if (connection->fFD > highest_fd)
					highest_fd = connection->fFD;
			}
		}

		// Block waiting for socket I/O or a timeout to expire
		ret = select(highest_fd + 1, &readfds, &writefds, NULL,
			NULL);

		acquireBigLock();

		// Check to see if there is activity on any of the sockets.
		// If so, process that here.
		if (ret > 0) {
			//
			// Handle new sockets.
			//
			if (FD_ISSET(accept_socket, &readfds)) {
				connection = new ServerConnection();
				sa_length = sizeof(struct sockaddr_in);
				connection->fFD = accept(accept_socket, (struct sockaddr*) &sa,
					&sa_length);
				connection->fNext = fConnectionList;
				connection->fPrev = &fConnectionList;
				if (connection->fNext != NULL)
					connection->fNext->fPrev = &connection->fNext;

				fConnectionList = connection;

				int mode = 1;
				ioctlsocket(connection->fFD, FIONBIO, (u_long FAR*) &mode);	// Set socket non-blocking
			}

			//
			// Handle IO on existing sockets.
			//
			for (connection = fConnectionList; connection != NULL; connection = connection->fNext) {
				if (connection->fFD >= 0 && FD_ISSET(connection->fFD, &readfds)) {
					while (connection->readMessage())
						;
				}

				// XXX handle write buffering
//				if (connection->fFD >= 0 && FD_ISSET(connection->fFD, &writefds))
//					connection->flush();
			}
		}

		//
	    // Clean up dead sockets.
		//
		for (connection = fConnectionList; connection != NULL; ) {
			if (connection->fClosed) {
				lprintf("clean up dead connection %p\n", connection);
				dead = connection;
				connection = connection->fNext;
				delete dead;
			} else
				connection = connection->fNext;
		}

		releaseBigLock();
	}*/

	return 0;
}

void ServerConnection::write(const char *data, int length)
{
	/*int totalWritten = 0;
	int thisSlice;

	do {
		thisSlice = send(fFD, data + totalWritten, length - totalWritten, 0);
		if (thisSlice < 0)
			break;

		totalWritten += thisSlice;
	} while (totalWritten < length);*/
}

void ServerConnection::write(int value)
{
	/*unsigned char buf[4];

	buf[0] = (value >> 24) & 0xff;
	buf[1] = (value >> 16) & 0xff;
	buf[2] = (value >> 8) & 0xff;
	buf[3] = (value) & 0xff;

	write((const char*) buf, 4);*/
}

//
// Flush data in the write buffer to a specific player
//
void ServerConnection::flush()
{
//	int written;

	/*written = send(fFD, fWriteBuffer, fWriteBufferLength, 0);
	if (written > 0) {
		if (written < fWriteBufferLength) {
			
			memcpy(fWriteBuffer, fWriteBuffer + written,
				fWriteBufferLength - written);
			fWriteBufferLength -= written;
		} else {
		
			fWriteBufferLength = 0;
		}
	}*/
}

void ServerConnection::processMessage()
{
	int n = readInt();
	int messageClass = (n >> 24) & 0xff;
	int messageId = n & 0xffffff;
	int id = readInt();
	Atom *atom = NULL;
	if (id != -1)
		atom = idtab.lookUpAtom(id);

	if (messageClass == M_REQUEST) {
		int transactionId = readInt();
		switch (messageId) {
			case CREATE_WINDOW:
				if (atom == NULL) {
					int left = readInt();
					int top = readInt();
					int right = readInt();
					int bottom = readInt();

					// new top level window
					Rect windowRect(left, top, right, bottom);

					TitledWindow *title = new TitledWindow(windowRect);

					windowRect.insetBy(BORDER_WIDTH, BORDER_WIDTH);
					windowRect.top += TITLE_HEIGHT - BORDER_WIDTH;
					ServerWindow *window = new ServerWindow(windowRect, false);
					window->setServerConnection(this);


					int windowId = idtab.registerAtom(window);
					// Send a response before actually setting up the window
					// so events don't get queued up before the response
					sendResponse(CREATE_WINDOW, windowId, 1, transactionId);

					title->addChild(window);
					windowManager->newWindow(title);
				} else {
					// Add as a child
					if (atom == NULL || atom->getType() != ATOM_SERVER_WINDOW) {
						printf("create window called on non-window\n");
						return;
					}

					int left = readInt();
					int top = readInt();
					int right = readInt();
					int bottom = readInt();

					ServerWindow *window = new ServerWindow(Rect(left, top, right, bottom), false);
					int windowId = idtab.registerAtom(window);

					((ServerWindow*) atom)->addChild(window);

					// Send a response
					sendResponse(CREATE_WINDOW, windowId, 1, transactionId);
				}

				break;

			case CREATE_BITMAP: {
				int width = readInt();
				int height = readInt();
				/// XXX need to specific color space
				Bitmap *bitmap = new Bitmap(Surface::kRGB24, width, height);
				int bitmapId = idtab.registerAtom(bitmap);

				// Send a response
				sendResponse(CREATE_WINDOW, bitmapId, 1, transactionId);
				break;
			}

			case GET_BOUNDS: {
				if (atom == NULL || atom->getType() != ATOM_SERVER_WINDOW) {
					printf("get bounds called on non-window\n");
					return;
				}

				Rect bounds = ((ServerWindow*) atom)->frame();
				sendResponse(GET_BOUNDS, id, 3, transactionId, bounds.width(), bounds.height());
				break;
			}

			default:
				printf("unknown request type %d\n", messageId);
		}
	} else if (messageClass == M_COMMAND) {
		switch (messageId) {
			case DESTROY_ATOM:
				if (atom != NULL) {
					if (atom->getType() == ATOM_SERVER_WINDOW)
						windowManager->closeWindow((ServerWindow*) atom);
					else
						delete atom;
				}

				break;

			case BEGIN_PAINT: {
				if (atom == NULL || atom->getType() != ATOM_SERVER_WINDOW) {
					printf("begin paint called on non-window\n");
					return;
				}

				((ServerWindow*) atom)->beginPaint();
				break;
			}

			case END_PAINT:
				if (atom == NULL || atom->getType() != ATOM_SERVER_WINDOW) {
					printf("begin paint called on non-window\n");
					return;
				}

				((ServerWindow*) atom)->endPaint();
				break;

			case SET_COLOR:
				if (atom == NULL || atom->getType() != ATOM_SERVER_WINDOW) {
					printf("begin paint called on non-window\n");
					return;
				}

				((ServerWindow*) atom)->getGC()->setColor(readInt());
				break;

			case STROKE_LINE: {
				if (atom == NULL || atom->getType() != ATOM_SERVER_WINDOW) {
					printf("begin paint called on non-window\n");
					return;
				}

				int left = readInt();
				int top = readInt();
				int right = readInt();
				int bottom = readInt();
				((ServerWindow*) atom)->getGC()->strokeLine(left, top, right, bottom);
				break;
			}

			case FILL_RECT: {
				if (atom == NULL || atom->getType() != ATOM_SERVER_WINDOW) {
					printf("begin paint called on non-window\n");
					return;
				}

				int left = readInt();
				int top = readInt();
				int right = readInt();
				int bottom = readInt();
				((ServerWindow*) atom)->getGC()->fillRect(left, top, right, bottom);
				break;
			}

			case LOCK_MOUSE_FOCUS:
				if (atom == NULL || atom->getType() != ATOM_SERVER_WINDOW) {
					printf("begin paint called on non-window\n");
					return;
				}

				((ServerWindow*) atom)->lockMouseFocus();
				break;

			case BLIT: {
				if (atom == NULL || atom->getType() != ATOM_SERVER_WINDOW) {
					printf("blit called on non-window\n");
					return;
				}

				Atom *bitmap = idtab.lookUpAtom(readInt());
				if (bitmap == NULL || bitmap->getType() == ATOM_BITMAP) {
					printf("blit called with invalid bitmap\n");
					return;
				}

				int sLeft = readInt();
				int sTop = readInt();
				int sRight = readInt();
				int sBottom = readInt();
				int dLeft = readInt();
				int dTop = readInt();
				int dRight = readInt();
				int dBottom = readInt();

				((ServerWindow*) atom)->getGC()->blit((Bitmap*) bitmap, sLeft, sTop, sRight, sBottom,
					dLeft, dTop, dRight, dBottom);

				break;
			}

			// XXX size of this is currently limited
			// XXX assumes RGB colorspace
			case SET_BITS: {
				if (atom == NULL || atom->getType() != ATOM_BITMAP) {
					printf("blit called on non-window\n");
					return;
				}

				Bitmap *bitmap = (Bitmap*) atom;

				char *c = (char*) bitmap->lockBits();
				for (int y = 0; y < bitmap->getHeight(); y++)
					memcpy(c + bitmap->getStride() * y, fReadBuffer + fDecodeOffset, bitmap->getWidth() * 3);


				break;
			}

			case PUSH_STATE:
				if (atom == NULL || atom->getType() != ATOM_SERVER_WINDOW) {
					printf("blit called on non-window\n");
					return;
				}

				((ServerWindow*)atom)->getGC()->pushState();
				break;

			case POP_STATE:
				if (atom == NULL || atom->getType() != ATOM_SERVER_WINDOW) {
					printf("blit called on non-window\n");
					return;
				}

				((ServerWindow*)atom)->getGC()->popState();
				break;

			case DRAW_STRING: {
				if (atom == NULL || atom->getType() != ATOM_SERVER_WINDOW) {
					printf("blit called on non-window\n");
					return;
				}

				int x = readInt();
				int y = readInt();
				int strlen = readInt();
				((ServerWindow*)atom)->getGC()->drawString(x, y, (const char*) fReadBuffer, strlen);
				break;
			}

			case SET_FONT: {
				if (atom == NULL || atom->getType() != ATOM_SERVER_WINDOW) {
					printf("blit called on non-window\n");
					return;
				}

				int strlen = readInt();
				((ServerWindow*)atom)->getGC()->setFont((const char*) fReadBuffer, strlen);
				break;
			}

			case SET_CURSOR:
				break;

			case INVALIDATE: {
				if (atom == NULL || atom->getType() != ATOM_SERVER_WINDOW) {
					printf("begin paint called on non-window\n");
					return;
				}

				int left = readInt();
				int top = readInt();
				int right = readInt();
				int bottom = readInt();
				((ServerWindow*) atom)->invalidate(Region(left, top, right, bottom));
				break;
			}

			default:
				printf("Unknown command id %d\n", messageId);
		}
	} else {
		printf("Unknown message class %d\n", messageClass);
	}
}

// Event:
// int length
// int class : 8
// int id : 24
// int windowid
// int param1
// int param2
// int param3
// int param4

void ServerConnection::sendResponse(int messageId, int windowId, int numParams, int param1, int param2,
	int param3, int param4)
{
	write(numParams * 4 + 12);
	write((M_RESPONSE << 24) | messageId);
	write(windowId);
	switch (numParams) {
		case 4:
			write(param1);
			write(param2);
			write(param3);
			write(param4);
			break;
		case 3:
			write(param1);
			write(param2);
			write(param3);
			break;
		case 2:
			write(param1);
			write(param2);
			break;
		case 1:
			write(param1);
			break;
	}
}

bool ServerConnection::fillBuffer(int size)
{
	/*int got;

	do {
		got = recv(fFD, (char*) fReadBuffer + fReadBufferLength, sizeof(fReadBuffer) - fReadBufferLength, 0);

		/// XXX need to check if there is an error reading from the socket

#if 0
		if (got < 0) {
			if (errno != EAGAIN) {
				lprintf("closing socket %p because of error %d %d\n", fFD, errno, got);
				fClosed = true;
				return;
			}

			break;
		}
#else
		if (got <= 0)
			break;
#endif

		fReadBufferLength += got;

	} while (fReadBufferLength < size);

	return fReadBufferLength >= size;*/

	return 0;
}

bool ServerConnection::readMessage()
{
	int messageLength;

	// First read the size of the message
	if (!fillBuffer(4))
		return false;	// Not enough data yet

	messageLength = (fReadBuffer[0] << 24) | (fReadBuffer[1] << 16)
		| (fReadBuffer[2] << 8) | fReadBuffer[3];
	if (!fillBuffer(messageLength))
		return false;	// Not enough data yet, try again

	fDecodeOffset = 4;
	processMessage();

	// Copy remaining data into beginning of buffer.
	memcpy(fReadBuffer, fReadBuffer + messageLength, fReadBufferLength - messageLength);
	fReadBufferLength -= messageLength;

	return true;
}

