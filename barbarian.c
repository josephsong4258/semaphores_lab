#include "dungeon_info.h"
#include "dungeon_settings.h"
#include <unistd.h> //gets pid
#include <stdio.h>
#include <signal.h>

// Pointer to shared memory dungeon
struct Dungeon *dungeon;

//Runs when the dungeon signals it's the Barbarians turn
// It is successful if the barbarian's attack equals the enemy's healtth
void barb_attack(int sig){
  // Copy enemy's health value to barbarian's attack attribute to ensure success
  dungeon->barbarian.attack = dungeon -> enemy.health;

  // Sleep for amount of time it takes barbarian to attack
  sleep(SECONDS_TO_ATTACK)

int main(void) {
  return 0;
      }