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

#ifndef __DEBUG_H
#define __DEBUG_H

extern void __assert_failed(int line, const char *file, const char *expr);
#define assert(x) { if (!(x)) {  __assert_failed(__LINE__, __FILE__, #x); } }
#define trespass(x) __assert_failed(__LINE__, __FILE__, x);

void addDebugCommand(const char *name, void (*callback)(int argc, const char **argv));

class Region;

void displayRegion(const Region &region, int color);

#endif

