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

#include "NoisyWindow.h"

NoisyWindow::NoisyWindow(const Rect &frame, Surface *bitmap)
	:	ServerWindow(frame),
		fCurrent(0),
		fIter(0),
		fBitmap(bitmap)
{
}


unsigned long seed = 0xdeadbeef;

unsigned long
p_rand()
{
    unsigned int next = seed;
    int result;

    next *= 1103515245;
    next += 12345;
    result = (unsigned int) (next / 65536) % 2048;

    next *= 1103515245;
    next += 12345;
    result <<= 11;
    result ^= (unsigned int) (next / 65536) % 1024;

    next *= 1103515245;
    next += 12345;
    result <<= 10;
    result ^= (unsigned int) (next / 65536) % 1024;

    seed = next;
    return result;
}

void NoisyWindow::draw()
{
	GraphicsContext *context = getGC();

	if (fIter-- <= 0) {
		fCurrent = p_rand() % 4;
		fIter = p_rand() % 15000;
	}

	Rect &fr = frame();
	fr.offsetTo(0, 0);

	long width = fr.right - fr.left;
	long height = fr.bottom - fr.top;
	context->setColor(p_rand() & 0xffffff);
	switch (fCurrent) {
		case 0:
			context->strokeLine(
				p_rand() % width + fr.left,
				p_rand() % height + fr.top,
				p_rand() % width + fr.left,
				p_rand() % height + fr.top);
			break;

		case 1:
			context->fillRect(
				p_rand() % width + fr.left,
				p_rand() % height + fr.top,
				p_rand() % width + fr.left,
				p_rand() % height + fr.top);
			break;


		case 2:
			context->blit(fBitmap, p_rand() % width + fr.left,
				p_rand() % height + fr.top);
			break;


		case 3: {
			Rect displayRect(p_rand() % width + fr.left,
				p_rand() % height + fr.top, p_rand() % width + fr.left,
				p_rand() % height + fr.top);
			context->blit(fBitmap, 0, 0, fBitmap->getWidth() - 1,
				fBitmap->getHeight() - 1, displayRect.left, displayRect.top,
				displayRect.right, displayRect.bottom);
			break;
		}
	}
}
