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

#include <math.h>
#include "RoundWindow.h"

RoundWindow::RoundWindow(int diameter)
	:	ServerWindow(Rect(0, 0, diameter, diameter))
{
	Region circleRegion;
	int offset;
	int radius = diameter / 2;

	for (int i = 0; i < diameter; i++) {
		offset =(int) sqrt(pow(radius, 2) - pow(i - radius, 2));

		circleRegion |= Region(radius - offset, i, radius + offset, i);
	}

	setBoundingRegion(circleRegion);
}

bool RoundWindow::isDraggable() const
{
	return true;
}

void RoundWindow::paint(GraphicsContext &context)
{
	context.setColor(0xff00ff);		// Yellow
	context.fillRect(0, 0, 100, 100);
}
