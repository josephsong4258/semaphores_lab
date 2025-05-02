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
#include <stdbool.h>
#include <math.h>

struct Dungeon *dungeon;

// Attempts to guess the correct lock pick angle.
// Makes a guess by writing a float value into dungeon->rogue.pick
// After each guess, the dungeon responds by updating :
// dungeon->trap.direction = 'u', 'd', or '-' (up, down, correct respectively)
// dungeon->trap.locked = true or false (false means success)
// Use binary search to find the angle
void rogue_signal(int sig) {
	// lowest possible angle is 0.0 degrees
	float low = 0.0;
	// highest possible angle is defined in dungeon settings
	float high = MAX_PICK_ANGLE;
	float guess;

	// sleep uses seconds while usleep works on microseconds. 1 sec = 1,000,000 microseconds
	int time = 0;
	int total = SECONDS_TO_PICK * 1000000;

	while (time < total) {
		// Guess set to midpoint between low and high
		guess = (low + high) / 2.0;

		// Write guess into shared memory
		dungeon->rogue.pick = guess;

		// Wait X microseconds before checking for hint
		usleep(TIME_BETWEEN_ROGUE_TICKS);
		time += TIME_BETWEEN_ROGUE_TICKS;

		// Read the hints from dungeon
		// Guess was too low - need to shift the bottom of the search range up
		if (dungeon->trap.direction == 'u') {
			low = guess;
		}
		// Guess was too high - need to shift the top of search range down
		if (dungeon->trap.direction == 'd') {
			high = guess;
		}
		// Guess is correct
		if (dungeon->trap.direction == '-' && dungeon->trap.locked == false)

	}
}


int main (void) {
	// Same stuff - taken from barbarian.c
  	int shm_fd = shm_open(dungeon_shm_name, 0_RDWR, 0666);
	if (shm_fd == -1) {
    	perror("Rogue failed to open dungeon");
    	exit(1);
	}
	dungeon = mmap(0, sizeof(struct Dungeon), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
  	if (dungeon == MAP_FAILED) {
    	perror("Rogue failed to map dungeon");
    	exit(1);
  }
	struct sigaction sa;
  	sa.sa_handler = rogue_signal;
  	sigemptyset(&sa.sa_mask);
  	sa.sa_flags = 0;

  	if (sigaction(DUNGEON_SIGNAL, &sa, 0) == -1) {
    	perror("Rogue failed to set up signal");
    	exit(1);
    }

   while (1){
     pause();
    }

	return 0;
  }