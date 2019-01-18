#pragma once

struct _BootParams;
bool InitKernelSystem(_BootParams* param, unsigned int kernelPageDirectory);
void StartNativeSystem(void* param);

