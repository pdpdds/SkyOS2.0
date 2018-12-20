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

#ifndef __PROTOCOL_H
#define __PROTOCOL_H

enum CommandId {
	DESTROY_ATOM = 0,
	BEGIN_PAINT,
	END_PAINT,
	SET_COLOR,
	STROKE_LINE,
	FILL_RECT,
	LOCK_MOUSE_FOCUS,
	BLIT,
	SET_BITS,
	DRAW_STRING,
	SET_FONT,
	SET_CURSOR,
	PUSH_STATE,
	POP_STATE,
	INVALIDATE
};

enum RequestId {
	CREATE_WINDOW = 0,
	CREATE_BITMAP,
	GET_BOUNDS,
};

enum MessageClass {
	M_REQUEST = 0,
	M_RESPONSE,
	M_COMMAND,
	M_EVENT
};

enum EventType {
	EVT_MOUSE_BUTTON,
	EVT_MOUSE_MOVED,
	EVT_MOUSE_EXIT,
	EVT_PAINT,
	EVT_KEY_DOWN,
	EVT_KEY_UP,
	EVT_RESIZED,
	EVT_GOT_FOCUS,
	EVT_LOST_FOCUS
};

#endif
