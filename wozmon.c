#include <stdint.h>
#include <stddef.h>

#define TOKEN_RUN 1
#define TOKEN_RANGE 2
#define TOKEN_BYTE 4
#define TOKEN_READ 8
#define TOKEN_WRITE 16
#define TOKEN_ADDRESS 32
#define TOKEN_ADDRESS_SHORT 64

#define TOKEN_LENGTH 32
#define MAX_TOKENS 20

void tokenize(char* input, char* split_command) {
  int split_index = 0;
  int token_index = 0;
  char flag = TOKEN_READ;
  char length = 0;
  char current_char;
      
  for (int i=0; *(input+i-1) || i==0; i++) {
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
    
    if (current_char != 0x20 && current_char != 0x00 && current_char != 0x2e) {
      split_command[token_index*TOKEN_LENGTH+split_index] = current_char;
      split_index++;
      length++;
    } else {
      if (length == 2) {
	flag |= TOKEN_BYTE;
      } else if (length == 8 || length == 9) {
	flag |= TOKEN_ADDRESS;
      } else if (length == 4 || length == 5) {
	flag |= TOKEN_ADDRESS_SHORT;
      }
      
      split_command[(TOKEN_LENGTH-1)+token_index*TOKEN_LENGTH] = flag;
      split_command[(TOKEN_LENGTH-2)+token_index*TOKEN_LENGTH] = length;

      flag = 8;
      length = 0;
      split_index = 0;
      token_index++;
    }
  }
  return;
}

void run(uintptr_t address) {
  void (*fptr)() = (void (*)())(uintptr_t)address;
  fptr();
  return;
}

void process_command(char* input) {
  char split_command[MAX_TOKENS * TOKEN_LENGTH];
  memset(split_command, 0x00, MAX_TOKENS * TOKEN_LENGTH);

  tokenize(input, split_command);

  uintptr_t address_start = 0;
  uintptr_t address_end = 0;
  uintptr_t temp_address;
  
  char bytes[9];
  char num_bytes = 0;

  char start_end = 1;
  char run_flag = 0;
  char read_write = 1;

  char current_char;
  char token_flag;
  
  // Set address_start, address_end, bytes, and other flags based on the tokenized input
  for (int token = 0; token<MAX_TOKENS; token++) {
    token_flag = split_command[(TOKEN_LENGTH-1)+token*TOKEN_LENGTH];
    temp_address = 0;

    if (token_flag & TOKEN_ADDRESS_SHORT) {
      temp_address = (cvars[4]<<24) | (cvars[5]<<16);
    }

    // Loop over the token and set appropriate flags and update temp_address
    for (int i = 0; *(split_command+token*TOKEN_LENGTH+i); i++) {
      current_char = *(split_command+token*TOKEN_LENGTH+i);
      if (token_flag & TOKEN_RUN) {
	run_flag = 1;
      }
      if (token_flag & TOKEN_WRITE) {
	read_write = 0;
      }
      if (token_flag & TOKEN_BYTE) {
	bytes[num_bytes] |= (char)char_to_nibble(current_char) << ((1-i)*4);
      }
      if ((token_flag & TOKEN_ADDRESS) && (i<8)) {
	temp_address |= (uintptr_t)char_to_nibble(current_char) << ((7-i)*4);
      }
      if ((token_flag & TOKEN_ADDRESS_SHORT) && (i<4)) {
	temp_address |= (uintptr_t)char_to_nibble(current_char) << ((3-i)*4);
      }

    }
    
    // If a byte token was present, increment the byte counter
    if (token_flag & TOKEN_BYTE) {
      num_bytes++;
    }

    // Set address_start or address_end depending on the state of start_end
    if (token_flag & (TOKEN_ADDRESS | TOKEN_ADDRESS_SHORT)) {
      if (start_end) {
	address_start = temp_address;
      } else {
	address_end = temp_address;
      }
    }
    if (token_flag & TOKEN_RANGE) {
      start_end = 0;
    }
    
  }

  // No address specified, show error and return
  if (!address_start) {
    print("No address specified\n");
    return;
  }

  // Jump to address_start and return
  if (run_flag) {
    run(address_start);
    return;
  }

  // Set address_end to address_start if unset
  if (!address_end) {
    address_end = address_start;
  }

  address_start &= 0x00000000ffffffffULL;
  address_end &= 0x00000000ffffffffULL;

  // If read_write is set, read memory between address_start and address_end
  if (read_write) {
    int byte_count = 0;
    char zero_flag = 1;

    char current_byte;

    char text_buffer[256];
    char text_buffer_length;

    for (uintptr_t sel_address = address_start; sel_address <= address_end; sel_address++) {
      current_byte = *(char*)sel_address;
      
      
      if (byte_count % 16 == 0) {
	if (byte_count > 15) {
	  if (zero_flag) {
	    putchar(0x20);
	    text_buffer[text_buffer_length+1] = 0x00;
	    print(text_buffer);
	    print("\n");
	  }
	} else {
	  print("ADDRESS   00 01 02 03   04 05 06 07   08 09 0a 0b   0c 0d 0e 0f\n");
	  print("---------------------------------------------------------------\n");
	  
	}

	if ((!*(uint64_t*)(sel_address&0xfffffff0)) && (!*(uint64_t*)((sel_address+8)&0xfffffff0))) {
	  if (zero_flag) {
	    print("*\n");
	  }
	  zero_flag = 0;
	  byte_count++;
	  continue;
	}
	zero_flag = 1;
	
	print32((int)sel_address);
	print(": ");
	printbyte(current_byte);
	putchar(0x20);
	text_buffer_length = 0;
      }
      if (!zero_flag) {
	byte_count++;
	continue;
      }
      if (current_byte >= 0x20 && current_byte <= 0x7e) {
	text_buffer[text_buffer_length] = current_byte;
      } else {
	text_buffer[text_buffer_length] = 0x2e;
      }
      text_buffer_length++;
      if (text_buffer_length == 1) {
	byte_count++;
	continue;
      }

      if (byte_count % 4 == 0) {
	print("| ");
	printbyte(current_byte);
	putchar(0x20);
      } else {
	printbyte(current_byte);
	putchar(0x20);
      }
      byte_count++;
    }
    if (zero_flag) {
      putchar(0x20);
      text_buffer[text_buffer_length+1] = 0x00;
      print(text_buffer);
    }
    print("\n\n");
  } else {
    // Write bytes to memory
    memcpy((char*)address_start, bytes, (int)num_bytes-1);
  }


  return;
}
