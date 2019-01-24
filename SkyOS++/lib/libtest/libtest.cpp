// libtest.cpp: 콘솔 응용 프로그램의 진입점을 정의합니다.
//

#include "stdafx.h"
#include "libdis.h"

int main()
{
	x86_insn_t curr_inst;
	x86_invariant_t inv;
	char line[80];
	int pos = 0, n = 0;
	int size = 0;
	HANDLE address = GetModuleHandle(NULL);
	unsigned char* buf = (unsigned char*)address;
	x86_init(opt_none, NULL, NULL);	
	//size = x86_disasm((unsigned char*)address, 0x1000, 0, i, &curr_inst);
	int fileSize = 0x100;

	while ( pos < fileSize) {
      /* disassemble address */
      size = x86_disasm(buf, fileSize, 0, pos, &curr_inst);
      if ( size ) {
         /* print instruction */
         x86_format_insn(&curr_inst, line, 80, intel_syntax);
         printf("%s\n", line);
         pos += size;
      } else {
         printf("Invalid instruction\n");
         pos++;
      }
   }

	x86_cleanup();

    return 0;
}

