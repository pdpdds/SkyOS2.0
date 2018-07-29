#pragma once

class VirtualFunctionBase
{
public:
	VirtualFunctionBase(){}
	virtual ~VirtualFunctionBase(){}

	virtual char* GetStringName() = 0;
	virtual bool IsSpaceShip() = 0;
};

class VirtualFunctionTest : public VirtualFunctionBase
{
public:
	VirtualFunctionTest();
	virtual ~VirtualFunctionTest();

	virtual char* GetStringName() override;
	virtual bool IsSpaceShip() override;
};

class VirtualFunctionTest2 : public VirtualFunctionBase
{
public:
	VirtualFunctionTest2();
	virtual ~VirtualFunctionTest2();

	virtual char* GetStringName() override;
	virtual bool IsSpaceShip() override;
};

