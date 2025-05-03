#include "dungeon_info.h"
#include "dungeon_settings.h"
#include <unistd.h> //gets pid
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <signal.h>
#include <semaphore.h>

//should be responsible for calling fork and exec
// Once all characters have launched, call RunDungeon using the pid's of the character classes that you launched.
//  If something isn't set up right, you will likely see an appropriate error
// After the characters reach the end of the dungeon, the door to the treasure room is opened by two levers(Semaphores) that are held by the Barbarian and Wizard
// while the Rogue retrieves the treasure,
int main(void) {
	struct Dungeon *dungeon;
	// To create shared memory 3 functions must be used - shm_open, ftruncate, and mmap
	// Create shared memory for the Dungeon struct - this will be "consumed" by all other processes
	// Note the O_CREAT flag compared to others that only have read/write
	int shm_fd = shm_open(dungeon_shm_name, 0_CREAT | 0_RDWR, 0666);
	if (shm_fd == -1) {
		perror("Game failed to open dungeon);
		exit(1);
	}
	// Set the size of shared memory object to Dungeon struct
	if (ftruncate(shm_fd, sizeof(struct Dungeon)) == -1) {
		perror("Game failed to truncate dungeon");
		exit(1);
	}
	dungeon = mmap(0, sizeof(struct Dungeon), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0)
	if (dungeon == MAP_FAILED) {
    	perror("Game failed to map dungeon");
    	exit(1);
	}


	dungeon->running = true;

	// Initialize and open a named semaphore
	sem_t *lever1 = sem_open(dungeon_lever_one, 0_CREATE, 0666, 0);
	sem_t *lever2 = sem_open(dungeon_lever_one, 0_CREATE, 0666, 0);
	if (lever1 == SEM_FAILED || lever2 == SEM_FAILED) {
		perror("Game failed to open semaphore");
		exit(1);
	}
	// PID Represents the process ID - a unique identifier for each process
    pid_t barbarian_pid, wizard_pid, rogue_pid;
    RunDungeon(wizard_pid, rogue_pid, barbarian_pid);

  return 0;
  }