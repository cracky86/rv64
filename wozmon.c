#include <stdint.h>
#include <stddef.h>

#define TOKEN_RUN 1
#define TOKEN_RANGE 2
#define TOKEN_BYTE 4
#define TOKEN_READ 8
#define TOKEN_WRITE 16
#define TOKEN_ADDRESS 32

void tokenize(char* input, char* split_command) {
  int split_index = 0;
  int token_index = 0;
  char flag = 8;
  char length = 0;
  char current_char;
  for (int i=0; *(input+i+(32*token_index)); i++) {
    current_char = *(input+i);

    switch (current_char) {
    case 82:
      flag &= 0xf7;
      flag |= TOKEN_RUN;
      break;
    case 58:
      flag &= 0xef;
      flag |= TOKEN_WRITE;
      break;
    case 46:
      flag |= TOKEN_RANGE;
      break;
    }
    
    
    if (current_char != 0x20) {
      split_command[token_index*32+split_index] = current_char;
      split_index++;
      length++;
    } else {
      if (length == 2) {
	flag |= TOKEN_BYTE;
      } else if (length == 8) {
	flag |= TOKEN_ADDRESS;
      }
      printbyte(flag);
      split_command[31+token_index*32] = flag;
      split_command[30+token_index*32] = length;
      flag = 8;
      length = 0;
      split_index = 0;
      token_index++;
    }
  }
  if (length == 2) {
    flag |= TOKEN_BYTE;
  } else if (length == 8 || length == 9) {
    flag |= TOKEN_ADDRESS;
  }
  split_command[31+token_index*32] = flag;
  split_command[30+token_index*32] = length;
  return;
}

void process_command(char* input) {
  char split_command[192];
  memset(split_command, 0x00, 192);
  tokenize(input, split_command);
  uintptr_t address_start = 0;
  uintptr_t address_end = 0;
  char byte = 0;

  char run_flag = 0;
  char read_write = 1;

  char current_char;
  for (int token = 0; token<6; token++) {
    for (int i = 0; *(split_command+token*32+i); i++) {
      current_char = *(split_command+token*32+i);
      if ((split_command[31+token*32] & TOKEN_RUN)) {
	run_flag = 1;
      }
      if ((split_command[31+token*32] & TOKEN_WRITE)) {
	read_write = 0;
      }
      if ((split_command[31+token*32] & TOKEN_BYTE)) {
	byte |= (char)char_to_nibble(current_char) << ((1-i)*4);
      }
      if ((split_command[31+token*32] & TOKEN_ADDRESS) && (i<8)) {
	address_start |= (uintptr_t)char_to_nibble(current_char) << ((7-i)*4);
      }
      if ((split_command[31+token*32] & TOKEN_RANGE)) {
	if (i<8) {
	  address_start |= (uintptr_t)char_to_nibble(current_char) << ((7-i)*4);
	} else if (i>8) {
	  address_end |= (uintptr_t)char_to_nibble(current_char) << ((16-i)*4);
	}
      }
    }
  }

  if (run_flag) {
    run(address_start);
    return;
  }

  if (!address_end) {
    address_end = address_start;
  }
  
  if (read_write) {
    int byte_count = 0;
    for (uintptr_t sel_address = address_start; sel_address <= address_end; sel_address++) {
      if (byte_count % 16 == 0) {
	
	if (byte_count > 15) {
	  print("\n");
	} else {
	  print("ADDRESS   0        4        8       12\n");
	  print("---------------------------------------------\n");
	  
	}
	print32((int)sel_address);
	print(": ");
	printbyte(*(char*)sel_address);
      } else if (byte_count % 4 == 0) {
	print(" ");
	printbyte(*(char*)sel_address);
      } else {
	printbyte(*(char*)sel_address);
      }
      byte_count++;
    }
    print("\n");
  } else {
    memset((char*)address_start, byte, address_end-address_start);
  }


  return;
}
