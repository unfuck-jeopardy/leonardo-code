/**
 * Author: Sergej "disasmwinnie" Schmidt
 * License: MIT license 
 */

#include <Keyboard.h>

#define LED_RED 4       // D1
#define LED_YELLOW 5    // D3
#define LED_GREEN 6     // D2
#define LED_BLUE 7      // D4
#define LED_WHITE_D5 8  // D5
#define LED_WHITE_D6 9  // D6

#define BUZZER_IN_0 13  // Player A/RED
#define BUZZER_IN_1 12  // Player B/GREEN
#define BUZZER_IN_2 11  // Player C/YELLOW
#define BUZZER_IN_3 10  // Player D/BLUE

// Hardware is built for four players only.
#define PLAYER_COUNT  4

struct player_io {
  uint8_t led;
  uint8_t buzzer;
  char key_press;
};

struct player_io all_players[PLAYER_COUNT];
struct player_io *current_player = NULL;

void setup() {
  Keyboard.begin();
  // Player A
  all_players[0].led = LED_RED;
  all_players[0].buzzer = BUZZER_IN_0;
  all_players[0].key_press = 'A';
  // Player B
  all_players[1].led = LED_GREEN;
  all_players[1].buzzer = BUZZER_IN_1;
  all_players[1].key_press = 'B';
  // Player C
  all_players[2].led = LED_YELLOW;
  all_players[2].buzzer = BUZZER_IN_2;
  all_players[2].key_press = 'C';
  // Player D
  all_players[3].led = LED_BLUE;
  all_players[3].buzzer = BUZZER_IN_3;
  all_players[3].key_press = 'D';
  
  // initialize IO PINs
  for(uint8_t player_i = 0; player_i < PLAYER_COUNT; player_i++) {
    pinMode(all_players[player_i].buzzer, INPUT_PULLUP);
    pinMode(all_players[player_i].led, OUTPUT);
  }
}

/** 
 *  Check which player pressed the button. In cases nobody, return NULL.
 */
struct player_io *who_pressed() {
  for(uint8_t player_i = 0; player_i < PLAYER_COUNT; player_i++) {
    // Pull-down resistor: in case buzzer pressed, the PIN is '0'
    if(digitalRead(all_players[player_i].buzzer) == 0) {
      return &all_players[player_i];
    }
  }
  return NULL;
}

/**
 * Helper function to turn on/off the status LEDs.
 */
void status_leds(uint8_t high_or_low) {
  digitalWrite(LED_WHITE_D5, high_or_low);
  digitalWrite(LED_WHITE_D6, high_or_low);
}

/**
 * Helper function to realize LED blinking in the loop() function.
 * It just turns off everything and the loop() has to take care of the
 * logic to enable to proper LEDs depending on state.
 */
void turn_off_all_leds() {
  status_leds(LOW);
  for(uint8_t player_i = 0; player_i < PLAYER_COUNT; player_i++) {
    digitalWrite(all_players[player_i].led, LOW);
  }
}


void loop() {
  // Change status only if the player changed. Keep it in a tmp var for the moment.
  struct player_io *tmp_player = who_pressed();

  /*
   * While who_pressed() function returns the current buzzer state, there is a case which
   * remains unhandled when iteration over all players from the beginning: when in current loop()-
   * run a player with a lesser ID presses buzzer, he/she wins the next round. For example if 
   * player C, index 2 in the all_players array, presses first, the next iteration can be won by
   * player A or B. One had to rely on the key press of C to happen soon enough and the GUI will
   * catch it. That is not enough for such high stakes games like Hacker-Jeopardy.
   * 
   * Therefore the following two cases must be checked to ensure no further issues arise.
   */
  // Only allow checking for new player if the current_player was NOT set previously.
  if(current_player == NULL) {
    status_leds(HIGH);
    current_player = tmp_player;
  // If in this run the player can be unset, but only if the previous round a buzzer WAS set.
  } else if(current_player != NULL && tmp_player == NULL) {
    current_player = tmp_player;
  } else {
    digitalWrite(current_player->led, HIGH);
    Keyboard.press(current_player->key_press);
    Keyboard.releaseAll();
  }
  delay(100);
  turn_off_all_leds();
  delay(100);
}
