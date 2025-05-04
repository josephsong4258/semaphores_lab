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
sem_t *wizard_lever = NULL;

// Decodes the Caesar Cipher placed in the Barrier's spell field.
// The first character is the shift key - lets us know how many letters the alphabet should shift by
// Not too great with C so I used this resource to help me write the function https://www.scaler.com/topics/caesar-cipher-program-in-c/
void decode_caesar(const char *input, char *output){
	// First character is interpreted as it's ASCII numerical value - ex: 'T' =  84
	int shift = input[0];

	// Start at 1 since the 0th index is the shift key
	for (int i = 1; input[i] != '\0'; i++) {
		char ch = input[i];
		// ch - 'A' gives us the number order of ch ex: 'C' - 'A' will give us 2
		// Then we subtract the number order with shift will give us ch's caesar order
		// ex: [abc] shift of one -> [bcd].
		// We add by 26 and do the modulus operator so we properly wrap around when we get negative values
		// The shift can be greater than 26 since we're using the ASCII numerical value - we want to limit the range to [0, 25]
		// Can do this by rolling again with the modulus operator with (shift % 26)
		// Finally, + 'A' gives us the capital letter representation
		if (isupper(ch)) {
			output[i-1] = (ch - 'A' - (shift % 26) + 26) % 26 + 'A';
		}
		// Same but for lower case characters
		else if (islower(ch)) {
			output[i-1] = (ch - 'a' - (shift % 26) + 26) % 26 + 'a';
		}
		// Everything else stays the same
		else {
			output[i-1] = ch;
		}
	}
	// Cstrings must end with a null character
	output[strlen(input) - 1] = '\0';
}

void wizard_signal(int sig) {
	if (sig == DUNGEON_SIGNAL) {

    // Read encoded spell from barrier
	const char *encoded = dungeon->barrier.spell;

	// Decodes cipher and saves it to wizard's spell
	decode_caesar(encoded, dungeon->wizard.spell);

	sleep(SECONDS_TO_GUESS_BARRIER);
  }

    // Holds lever by waiting on semaphore
	if (sig == SEMAPHORE_SIGNAL) {
      sem_wait(wizard_lever);
	}
}
int main (void) {
	// Same stuff - taken from barbarian.c
  	int shm_fd = shm_open(dungeon_shm_name, 0_RDWR, 0666);
	if (shm_fd == -1) {
    	perror("Wizard failed to open dungeon");
    	exit(1);
	}
	dungeon = mmap(0, sizeof(struct Dungeon), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
  	if (dungeon == MAP_FAILED) {
    	perror("Wizard failed to map dungeon");
    	exit(1);
  }
    wizard_lever = sem_open(dungeon_lever_two, 0);
    if (wizard_lever == SEM_FAILED) {
      perror("Wizard failed to hold down lever");
      exit(1);
    }

    struct sigaction sa;
  	sa.sa_handler = wizard_signal;
  	sigemptyset(&sa.sa_mask);
  	sa.sa_flags = 0;

  	if (sigaction(DUNGEON_SIGNAL, &sa, 0) == -1) {
    	perror("Wizard failed to set up signal");
    	exit(1);
    }
    if (sigaction(SEMAPHORE_SIGNAL, &sa, 0) == -1) {
        perror("Wizard failed to set up semaphore signal");
        exit(1);
    }
   while (dungeon->running){
     pause();
    }

    sem_close(wizard_lever);
    munmap(dungeon, sizeof(struct Dungeon));
	return 0;
  }