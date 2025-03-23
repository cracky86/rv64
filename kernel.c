#include <stdint.h>
#include <stddef.h>

#include "kernel.h"
#include "uart.h"
#include "strings.h"

void run(uintptr_t address) {
  void (*fptr)() = (void (*)())(uintptr_t)address;
  fptr();
  return;
}

void test(void) {
  print("Hello World!\n");
  return;
}

void trap_handler(uint64_t cause, uint64_t epc, uint64_t tval) {  
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
  
  while (1) {}
  return;
}

#include "wozmon.c"

void kmain(void) {
  print("init\n\n");
  
  // Output the address of the test function and execute it
  char address_byte;
  void (*fptr)() = &test;
  print("0x");
  for (int i=0; i<4; i++) {
    address_byte = (char)(((uintptr_t)fptr >> ((3-i)*8))&0xff);
    printbyte(address_byte);
  }
  print("\n\n");  

  char input_buffer[256];
  
  while(1) {
    input(input_buffer);
    print("\r\n");

    //process_run(input_buffer);
    process_command(input_buffer);
  }
  return;
}
