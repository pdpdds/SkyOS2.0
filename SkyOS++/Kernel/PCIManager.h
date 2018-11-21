#pragma once
class PCIManager
{
public:
	PCIManager();
	~PCIManager();

	bool Initialize();
	bool InitPCI();	
};

