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
#include <semaphore.h>



// Pointer to shared memory dungeon
struct Dungeon *dungeon;

sem_t *barbarian_lever = NULL;

//Runs when the dungeon signals it's the Barbarians turn
// It is successful if the barbarian's attack equals the enemy's healtth
void barb_attack(int sig){
	if (sig == DUNGEON_SIGNAL) {
  		// Copy enemy's health value to barbarian's attack attribute to ensure success
  		dungeon->barbarian.attack = dungeon -> enemy.health;

  		// Sleep for amount of time it takes barbarian to attack
  		sleep(SECONDS_TO_ATTACK);
	}
  // Holds lever by waiting on semaphore
	if (sig == SEMAPHORE_SIGNAL) {
    sem_wait(barbarian_lever);
	}

}

int main(void) {

  // https://man7.org/linux/man-pages/man3/shm_open.3.html
  // shm_open gives a file descriptor that represents a chunk of shared memory managed by the OS
  // First parameter specifies name of shared-memory object - we got it from dungeon_info.h
  // Next parameter is if it hasn't been created yet(O_CREAT), or if it's open for reading/writing(O_RDWR)
  // Third parameter - 0666 seems to be the default value. Just tells us Owners, groups, and others have R/W permission
  int shm_fd = shm_open(dungeon_shm_name, O_RDWR, 0666);
  // If successful, shm_open returns a file descriptor(non-negative integer)
  // On failure, returns -1
  if (shm_fd == -1) {
    perror("Barbarian failed to open dungeon");
    exit(1);
  }

  // https://man7.org/linux/man-pages/man2/mmap.2.html
  // mmap maps the shared memory object into the process's virtual memory
  // Leave the first parameter as 0 so the OS decides where to choose the address
  // 2nd parameter is the size of dungeon since that's what we want to map
  // 3rd parameter read/write permissions
  // MAP_SHARED - updates to the mapping are visible to other processes in the region
  // file descriptor is shm_fd
  // represents the offset - don't need one, so 0
  dungeon = mmap(0, sizeof(struct Dungeon), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
  // if mapping is successful, mmap returns a pointer to the mapped area.
  // If not successful, mmap returns the value MAP_FAILED( equal to (void *) -1)
  if (dungeon == MAP_FAILED) {
    perror("Barbarian failed to map dungeon");
    exit(1);
  }

  barbarian_lever = sem_open(dungeon_lever_one, 0);
  if (barbarian_lever == SEM_FAILED) {
      perror("Barbarian failed to press down lever");
      exit(1);
    }

  // https://pubs.opengroup.org/onlinepubs/007904875/functions/sigaction.html
  // sigaction invokes a change in action taken by a process when they receive a specific signal
  struct sigaction sa;

  // sa_handler is the function that will run when the signal is caught
  // Set it to the function we want to run when DUNGEON_SIGNAL arrives
  sa.sa_handler = barb_attack;
  // sa_mask defines any signals we want to block while inside the handler
  // We call sigemptyset to start with no blocked signals
  sigemptyset(&sa.sa_mask);
  // sa_flags sets any special behavior (we don't need any special behavior, so set to 0)
  sa.sa_flags = 0;

  // If sigaction is successful it returns 0, otherwise -1 is returned
  if (sigaction(DUNGEON_SIGNAL, &sa, 0) == -1) {
    perror("Barbarian failed to set up signal");
    exit(1);
    }
  if (sigaction(SEMAPHORE_SIGNAL, &sa, 0) == -1) {
    perror("Barbarian failed to set up signal");
    exit(1);
  }

    // Waits and does nothing until signal arrives.
    // Needed so process stays alive for the duration of the dungeon game
    // A signal from dungeon will "wake up" the process again
  while (dungeon->running){
     pause();
    }
  sem_close(barbarian_lever);
  munmap(dungeon, sizeof(struct Dungeon));

  return 0;
}