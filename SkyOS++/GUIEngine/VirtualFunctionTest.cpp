#include "VirtualFunctionTest.h"


char* szClassName = "VirtualFunctionTest";
char* szClassName2 = "VirtualFunctionTest2";

VirtualFunctionTest::VirtualFunctionTest()
{
}


VirtualFunctionTest::~VirtualFunctionTest()
{
}

char* VirtualFunctionTest::GetStringName()
{
	return szClassName;
}

bool VirtualFunctionTest::IsSpaceShip()
{
	return false;
}

VirtualFunctionTest2::VirtualFunctionTest2()
{
}


VirtualFunctionTest2::~VirtualFunctionTest2()
{
}

char* VirtualFunctionTest2::GetStringName()
{
	return szClassName2;
}

bool VirtualFunctionTest2::IsSpaceShip()
{
	return true;
}