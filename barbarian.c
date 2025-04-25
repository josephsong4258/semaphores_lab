#include "dungeon_info.h"
#include "dungeon_settings.h"
#include <unistd.h> //gets pid
#include <stdio.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <fcntl.h> //O_RDWR

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
  // https://www.geeksforgeeks.org/posix-shared-memory-api/
  // shm_open gives a file descriptor that represents a chunk of shared memory managed by the OS
  // First parameter specifies name of shared-memory object - we got it from dungeon_info.h
  // Next parameter is if it hasn't been created yet(O_CREAT), or if it's open for reading/writing(O_RDWR)
  // Third parameter - 0666 seems to be the default value. Just tells us Owners, groups, and others have R/W permission
  int shm_fd = shm_open(dungeon_shm_name, O_RDWR 0666);

  // mmap maps the shared memory object into the process's own address space
  // Leave the first parameter as NULL so the OS decides where to choose the address
  // 2nd parameter is the size of dungeon since that's what we want to map
  dungeon = mmap(NULL, sizeof(struct Dungeon)
  return 0;
      }