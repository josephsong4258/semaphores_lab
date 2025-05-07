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
#include <semaphore.h>


struct Dungeon *dungeon;
sem_t *lever1;
sem_t *lever2;

// Attempts to guess the correct lock pick angle.
// Makes a guess by writing a float value into dungeon->rogue.pick
// After each guess, the dungeon responds by updating :
// dungeon->trap.direction = 'u', 'd', or '-' (up, down, correct respectively)
// dungeon->trap.locked = true or false (false means success)
// Use binary search to find the angle
void rogue_signal(int sig) {
    if (sig == DUNGEON_SIGNAL) {
		// lowest possible angle is 0.0 degrees
		float low = 0.0;
		// highest possible angle is defined in dungeon settings
		float high = MAX_PICK_ANGLE;

		// sleep uses seconds while usleep works on microseconds. 1 sec = 1,000,000 microseconds
		int time = 0;
		int total = SECONDS_TO_PICK * 1000000;

		while (time < total && dungeon->running) {

			// Guess set to midpoint between low and high
			float guess = (low + high) / 2.0f;

			// Write guess into shared memory
			dungeon->rogue.pick = guess;

			// Wait for guess to register
			usleep(TIME_BETWEEN_ROGUE_TICKS);
			time += TIME_BETWEEN_ROGUE_TICKS;

			// Guess is correct
			if (dungeon->trap.direction == '-' && dungeon->trap.locked == false) {
				// loop breaks iff the total time is reached or when the lock is successfully picked
				break;
			}

			// Read the hints from dungeon
			// Guess was too low - need to shift the bottom of the search range up
			// Sometimes the pick stays stuck at an angle - even after multiple loops
			// I'll try to force it to move a direction by using the function nextafterf
			if (dungeon->trap.direction == 'u') {
				if (guess == low) {
					low = nextafterf(guess, high);
				}
				else {
					low = guess;
				}
			}
			// Guess was too high - need to shift the top of search range down
			// Apply the same fix if it gets stuck
			else if (dungeon->trap.direction == 'd') {
				if (guess == high) {
					high = nextafterf(guess, low);
					}
				else {
					high = guess;
					}
		}
	}
  }
	//Barb and Wizard have executed sem_wait() on their levers which holds the door open so the Rogue can enter
	if (sig == SEMAPHORE_SIGNAL) {
		printf("Rogue has entered the treasure room\n");
		// Read exactly 4 treasure characters - dungeon writes one at a time
		int count = 0;
		while (count < 4 && dungeon->running) {
			char c = dungeon->treasure[count];
			if (c != '\0') {
				// Copy each letter into spoils
				dungeon->spoils[count] = c;
				printf("Rogue got treasure letter %c\n", c);
				count++;
			}
		}
		printf("Rogue has collected all 4 treasures\n");
		//Release both semaphores
		sem_post(lever1);
		sem_post(lever2);
		printf("Both levers have been released\n");
	}
}


int main (void) {
	// Same stuff - taken from barbarian.c
  	int shm_fd = shm_open(dungeon_shm_name, O_RDWR, 0666);
	if (shm_fd < 0) {
    	printf("Rogue process running without shared memory\n");
	 	return 0;
	}
	dungeon = mmap(0, sizeof(struct Dungeon), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
	lever1 = sem_open(dungeon_lever_one, 0);
	lever2 = sem_open(dungeon_lever_two, 0);

	struct sigaction sa;
  	sa.sa_handler = rogue_signal;
  	sigemptyset(&sa.sa_mask);
  	sa.sa_flags = SA_NODEFER;
    sigaction(DUNGEON_SIGNAL, &sa, NULL);
    sigaction(SEMAPHORE_SIGNAL, &sa, NULL);

    while (dungeon->running){
    	pause();
    }
	sem_close(lever1);
	sem_close(lever2);
	munmap(dungeon, sizeof(struct Dungeon));
	return 0;
 }
