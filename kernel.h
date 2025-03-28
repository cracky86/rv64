#ifndef __OS_H__
#define __OS_H__

int chksum = 0xffffffff;

char *exceptions[] = {
  "Instruction address misaligned",
  "Instruction access fault",
  "Illegal instruction",
  "Breakpoint",
  "Load address misaligned",
  "Load access fault",
  "Store/AMO address misaligned",
  "Store/AMO access fault",
  "Environment call from V/U mode",
  "Environment call from H/S mode",
  "Environment call from VS mode",
  "Environment call from M mode",
  "Instruction page fault",
  "Load page fault",
  "Unknown error",
  "Store/AMO page fault",
  "Unknown error",
  "Unknown error",
  "Software check",
  "Unknown error",
  "Instruction guest page fault",
  "Load guest page fault",
  "Virtual instruction",
  "Store/AMO guest page fault"};

char cvars[64];

/*
  Common variable (cvar) allocation:

  0-3: trap return pointer
  4-5: short address msb
*/

#endif
