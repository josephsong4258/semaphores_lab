#include "dungeon_info.h"
#include "dungeon_settings.h"
#include <unistd.h> //gets pid
#include <stdio.h>

//should be responsible for calling fork and exec
// Once all characters have launched, call RunDungeon using the pid's of the character classes that you launched.
//  If something isn't set up right, you will likely see an appropriate error.
int main(void) {
  // PID Represents the process ID - a unique identtifier for each process
  pid_t barbarian_pid, wizard_pid, rogue_pid;
  RunDungeon(wizard_pid, rogue_pid, barbarian_pid);

  return 0;
  }