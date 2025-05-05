#include "dungeon_info.h"
#include "dungeon_settings.h"
#include <unistd.h> //gets pid
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <signal.h>
#include <semaphore.h>
#include <stdlib.h>

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
	int shm_fd = shm_open(dungeon_shm_name, O_CREAT | O_RDWR, 0666);
	if (shm_fd == -1) {
		perror("Game failed to open dungeon");
		exit(1);
	}
	// Set the size of shared memory object to Dungeon struct
	if (ftruncate(shm_fd, sizeof(struct Dungeon)) == -1) {
		perror("Game failed to truncate dungeon");
		exit(1);
	}
	dungeon = mmap(0, sizeof(struct Dungeon), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
	if (dungeon == MAP_FAILED) {
    	perror("Game failed to map dungeon");
    	exit(1);
	}


	dungeon->running = true;

	// Initialize and open a named semaphore
	sem_t *lever1 = sem_open(dungeon_lever_one, O_CREAT, 0666, 0);
	sem_t *lever2 = sem_open(dungeon_lever_two, O_CREAT, 0666, 0);
	if (lever1 == SEM_FAILED || lever2 == SEM_FAILED) {
		perror("Game failed to open semaphore");
		exit(1);
	}

	// fork() creates a new process by duplicating the current process
	// In the child process - fork returns 0, in the parent process - child's PID
	pid_t barbarian_pid = fork();
	if (barbarian_pid == 0) {
		// execl replaces the current process image with a new program
		// chose execl here because we know the exact path ("./barbarian) and no extra arguments are needed
		// ./barbarian represents the path to the executable file we want to run - 2nd parameter is mostly arbitrary - it is the processes' new name
		// to play it safe we'll just name it barbarian
		execl("./barbarian", "barbarian", NULL);
		perror("Barbarian failed to fork");
		exit(1);
	}

	pid_t wizard_pid = fork();
	if (wizard_pid == 0) {
		execl("./wizard", "wizard", NULL);
		perror("Wizard failed to fork");
		exit(1);
	}

	pid_t rogue_pid = fork();
	if (rogue_pid == 0) {
		execl("./rogue", "rogue", NULL);
		perror("Rogue failed to fork");
		exit(1);
	}
	// PID Represents the process ID - a unique identifier for each process
    RunDungeon(wizard_pid, rogue_pid, barbarian_pid);

	dungeon->running = false;

	// Cushion so children can clean up
	sleep(3);

	// Clean up shared memory/semaphores
	sem_close(lever1);
	sem_close(lever2);
	sem_unlink(dungeon_lever_one);
	sem_unlink(dungeon_lever_two);
	munmap(dungeon, sizeof(struct Dungeon));
	shm_unlink(dungeon_shm_name);

  	return 0;
  }