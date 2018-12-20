#pragma once
#include "Screen.h"

class VirtualScreen : public Screen
{
public:
	VirtualScreen();
	~VirtualScreen();

	virtual void* lockBits();
	virtual void unlockBits();
	virtual int getStride() const;
	virtual Rasterizer *createRasterizer();
	virtual void setCursorPosition(int x, int y);
	virtual void setCursorShape(int width, int height, const char *colorBits, const char *maskBits);

};

