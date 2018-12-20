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

#ifndef TITLEDWINDOW_H
#define TITLEDWINDOW_H

#include "ServerWindow.h"

#define TITLE_HEIGHT 20
#define BORDER_WIDTH 3

class TitledWindow : public ServerWindow {
public:
	TitledWindow(const Rect&);
	virtual void paint(GraphicsContext&);
};

#endif
