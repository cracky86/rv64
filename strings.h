#ifndef __STRINGS_H__
#define __STRINGS_H__

// Sets a contingous range of bytes
void memset(char* ptr, char byte, int length) {
  while (length) {
    length--;
    *(ptr+length) = byte;
  }
  return;
}

// Copy number of bytes from source to destination
void memcpy(char* ptr_to, char* ptr_from, int length) {
  while (length) {
    length--;
    *(ptr_to+length) = *(ptr_from+length);
  }
  return;
}

// Prints a string to uart console
void print(char* str) {
  for (int i=0; *(str+i); i++) {
    putchar(*(str+i));
  }
  return;
}

// Print byte as hex value
void printbyte(char byte) {
  char nibble = byte >> 4 & 0x0f; // Process most significant nibble

  if (nibble < 10) {
    putchar(nibble+48);
  } else {
    putchar(nibble+87);
  }
  
  nibble = byte & 0x0f; // Process last significant nibble

  if (nibble < 10) {
    putchar(nibble+48);
  } else {
    putchar(nibble+87);
  }
  return;
}

// Converts an integer to its text representation
void int_to_string(int number, char* out) {
  // If number is single digit, just output it
  if (number / 10 == 0) {
    *out = (char)number+48;
    *(out+1) = 0x00;
    return;
  }

  // Repeatedly divide by 10 to obtain number of places
  int count = 0;
  int multiplier = 1;
  int number_copy = number;
  while (number_copy) {
    number_copy /= 10;
    multiplier *= 10;
    count++;
  }

  int count_position = 0;
  for (int i=count-1; 0; i--) {
    multiplier /= 10;
    *(out+count_position) = (char)((number / multiplier) % 10 + 48);
    count_position++;
  }
  *(out+count_position) = 0x00;
  return;
}

// Simple routine for taking user input via uart console, only works with 1 byte long characters
void input(char* input) {
  char current_char;
  int len = 0;
  
  while (1) {
    getchar(&current_char);

    // Exit when user presses enter
    if (current_char == 0x0d) {
      input[len] = 0x00;
      break;
    }

    // Backspace
    if (current_char == 0x7f) {
      // Do not attempt backspacing if input is already empty
      if (!len) { continue; }

      putchar(0x08);
      putchar(0x20);
      putchar(0x08);
      input[len-1] = 0x00;
      len--;
    } else {
      // Any other character gets appended to the buffer
      putchar(current_char);
      input[len] = current_char;
      len++;
    }
  }
}

// Converts a text character representing a nibble (0-F) and outputs its corresponding value
char char_to_nibble(char nibble) {
  if (nibble >= 48 && nibble <= 57) {
    return nibble-48;
  } else if (nibble >= 97 && nibble <= 102) {
    return nibble-87;
  } else {
    return 0;
  }
}

// Prints a 32 bit value as its hex representation
void print32(int value) {
  char current_byte;
  for (int i=0; i<4; i++) {
    current_byte = (char)((value >> ((3-i)*8))&0xff);
    printbyte(current_byte);
  }
  return;
}
#endif
