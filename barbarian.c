#include "dungeon_info.h"
#include "dungeon_settings.h"
#include <unistd.h> //gets pid
#include <stdio.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <fcntl.h> //O_RDWR
#include <sys/stat.h> // 0666
#include <stdlib.h>



// Pointer to shared memory dungeon
struct Dungeon *dungeon;

//Runs when the dungeon signals it's the Barbarians turn
// It is successful if the barbarian's attack equals the enemy's healtth
void barb_attack(int sig){
  // Copy enemy's health value to barbarian's attack attribute to ensure success
  dungeon->barbarian.attack = dungeon -> enemy.health;

  // Sleep for amount of time it takes barbarian to attack
  sleep(SECONDS_TO_ATTACK);
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
    perror("Failed to open dungeon");
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
    perror("Failed to map dungeon");
    exit(1);
  }
  return 0;
      }