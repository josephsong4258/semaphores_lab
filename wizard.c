#include "dungeon_info.h"
#include "dungeon_settings.h"
#include <stddef.h>
#include <unistd.h> //gets pid
#include <stdio.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <fcntl.h> //O_RDWR
#include <sys/stat.h> // 0666
#include <stdlib.h>

struct Dungeon *dungeon;

// Decodes the Caesar Cipher placed in the Barrier's spell field.
// The first character is the shift key - lets us know how many letters the alphabet should shift by
// Not too great with C so I used this resource to help me write the function https://www.scaler.com/topics/caesar-cipher-program-in-c/
void decode_caesar(const char *input, char *output){
	int shift = input[0];

	// Start at 1 since the 0th index is the shift key
	for (int i = 1; input[i] != '\0'; i++) {
		char ch = input[i];
		// ch - 'A' gives us the number order of ch ex: 'C' - 'A' will give us 2
		// Then we subtract the number order with shift will give us ch's caesar order
		// ex: [abc] shift of one -> [bcd].
		// We add by 26 and do the modulus operator so we properly wrap around when we get negative values
		// Finally, + 'A' gives us the capital letter representation
		if (isupper(ch)) {
			output[i-1] = (ch - 'A' - shift + 26) % 26 + 'A';
		}
		else if (islower(ch)) {
			output[i-1] = (ch - 'a' - shift + 26) % 26 + 'a';
int main (void) {
  return 0;
  }