// 
// Copyright 1998-2012 Jeff Bush
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

#ifndef _MEMORY_LAYOUT_H
#define _MEMORY_LAYOUT_H

#define KERNEL_HEAP_FRAME_COUNT 12800

const unsigned int kUserBase = 0x08000000;
const unsigned int kUserTop = 0xbfffffff;
const unsigned int kKernelBase = 0x01600000;
const unsigned int kKernelDataBase = 0x02000000;
const unsigned int kKernelDataTop = 0x03000000;
const unsigned int kBootStackBase = 0xc0100000;
const unsigned int kBootStackTop = 0xc0103fff;
const unsigned int kHeapBase = 0xc0104000;
const unsigned int kHeapTop = kHeapBase + KERNEL_HEAP_FRAME_COUNT * 4096;
const unsigned int kIOAreaBase = 0xe4000000;
const unsigned int kIOAreaTop = 0xe7ffffff;
const unsigned int kKernelTop = 0xffffffff;
const unsigned int kAddressSpaceTop = 0xffffffff;

#endif
