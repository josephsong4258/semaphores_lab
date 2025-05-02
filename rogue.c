#include "dungeon_info.h"
#include "dungeon_settings.h"
#include <unistd.h> //gets pid
#include <stdio.h>

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