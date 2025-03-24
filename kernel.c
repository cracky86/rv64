#include <stdint.h>
#include <stddef.h>

#include "kernel.h"
#include "uart.h"
#include "strings.h"

void trap_handler(uint64_t cause, uint64_t epc, uint64_t tval) {  
  uintptr_t trap_return_pointer;
  print("\nTRAP encountered! Something went horribly wrong\n");
  print("Cause: ");
  printbyte((char)cause);
  print(" - ");
  print(exceptions[(int)cause]);
  print("\nEPC: ");
  print32((int)epc);
  print("\nmtval: ");
  print32((int)tval);
  print("\n");

  cvars[63] = 0xff;

  trap_return_pointer = ((cvars[0]<<24) | (cvars[1]<<16) | (cvars[2]<<8) | (cvars[3])) & 0x00000000ffffffff;
  
  void (*fptr)() = (void*)trap_return_pointer;
  fptr();
}

#include "wozmon.c"

void kmain(void) {
  print("init\n\n");

  char crash_flag = 0;
  for (int i=0; i<64; i++) {
    if (cvars[i]) {
      print("cvars not wiped, soft reboot or crash occured\n");
      crash_flag = 1;
      break;
    }
  }
  
  if (!crash_flag) {
    memset(cvars, 0x00, 64);
    
    for (int i=0; i<4; i++) {
      cvars[i] = (char)(((int)kmain >> ((3-i)*8))&0xff);
    }

    cvars[4] = 0x80;

    char address_byte;
    print("cvars are stored at ");
    for (int i=0; i<4; i++) {
      address_byte = (char)(((int)cvars >> ((3-i)*8))&0xff);
      printbyte(address_byte);
    }
    print("\n\n");
  } else {
    print("recovered from a crash\n\n");
  }

  char input_buffer[256];
  
  while(1) {
    input(input_buffer);
    print("\r\n");

    //process_run(input_buffer);
    process_command(input_buffer);
  }
  return;
}
