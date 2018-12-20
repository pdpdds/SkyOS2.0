#include "VirtualScreen.h"
#include "Rasterizer24bpp.h"
#include "BasicStruct.h"

#define VIRTUAL_SCREEN_WIDTH 1024
#define VIRTUAL_SCREEN_HEIGHT 768
#define INPUT_WINDOW_HEIGHT 20


VirtualScreen::VirtualScreen()
	: Screen(Surface::kRGB24, VIRTUAL_SCREEN_WIDTH, VIRTUAL_SCREEN_HEIGHT)
{
}


extern BootParams bootParams;

VirtualScreen::~VirtualScreen()
{
}

void* VirtualScreen::lockBits()
{
	return (void*)bootParams.framebuffer_addr;
}

void VirtualScreen::unlockBits()
{

}

int VirtualScreen::getStride() const
{
	return VIRTUAL_SCREEN_WIDTH * 3;
}

Rasterizer* VirtualScreen::createRasterizer()
{
	return new Rasterizer24bpp(this);
}

void VirtualScreen::setCursorPosition(int x, int y)
{

}

void VirtualScreen::setCursorShape(int width, int height, const char *colorBits, const char *maskBits)
{

}
