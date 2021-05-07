#include <Keypad.h>
#include <Wire.h>

//set up command keypad
const byte ROWS1 = 4; //four rows
const byte COLS1 = 4; //four columns
char commandKeys[ROWS1][COLS1] = {
    {'q', 'u', 'e', 'a'},
    {'l', ' ', 'r', 'b'},
    {'s', 'd', 'o', 'x'},
    {'*', ' ', 'k', 'y'}
};

/*
   contolKeys key:
   u, l, d, r,          directions
   a, b, x, y,          face buttons
   q, e                 shoulder buttons l and r
   s                    select (as in share on ps4)
   o                    start (as in options on ps4)
   k                    hotkey enable
#                    mash command
' '                  currently unused
*/


/* Save any active inputs in 2 bytes, with the top two rows as the
 * LSB running top left to bottom right as byte 0, and the bottom two
 * rows as byte 1
 *
 */

const byte regularInputModeStatusPin = 11;

const byte leftShoulder = 1;
const byte upButton = 2;
const byte rightShoulder = 4;
const byte aButton = 8;
const byte leftButton = 16;
const byte rightButton = 64;
const byte bButton = 128;

const byte selectButton = 1;
const byte downButton = 2;
const byte startButton = 4;
const byte xButton = 8;
//ignoring mutator
const byte hotkeyButton = 64;
const byte yButton = 128;

byte colPins2[COLS1] = {6, 5, 4, 3}; //connect to the row pinouts of the keypad
byte rowPins2[ROWS1] = {10, 9, 8, 7}; //connect to the column pinouts of the keypad
Keypad commandKeypad = Keypad( makeKeymap(commandKeys), rowPins2, colPins2, ROWS1, COLS1 );

bool regularInputMode = false;

void setup() {
    pinMode(regularInputModeStatusPin, OUTPUT);
    Wire.begin(8);
    Wire.onRequest(request);
    Wire.onReceive(recieve);
    commandKeypad.setHoldTime(0);
}

void loop() {
    delay(100);
}

void request() {
    if (regularInputMode) {
        byte toSend[2] = {0,0};
        commandKeypad.getKeys();
        for (int i=0; i<LIST_MAX; i++){   // Scan the whole key list.
            if(commandKeypad.key[i].kstate != IDLE && commandKeypad.key[i].kstate != RELEASED){
                switch (commandKeypad.key[i].kchar) {
                    case 'q':
                        toSend[0] += leftShoulder;
                        break;
                    case 'u':
                        toSend[0] += upButton;
                        break;
                    case 'e':
                        toSend[0] += rightShoulder;
                        break;
                    case 'a':
                        toSend[0] += aButton;
                        break;
                    case 'l':
                        toSend[0] += leftButton;
                        break;
                    case 'r':
                        toSend[0] += rightButton;
                        break;
                    case 'b':
                        toSend[0] += bButton;
                        break;
                    case 's':
                        toSend[1] += selectButton;
                        break;
                    case 'd':
                        toSend[1] += downButton;
                        break;
                    case 'o':
                        toSend[1] += startButton;
                        break;
                    case 'x':
                        toSend[1] += xButton;
                        break;
                    case 'k':
                        toSend[1] += hotkeyButton;
                        break;
                    case 'y':
                        toSend[1] += yButton;
                        break;
                }
            }
        }
        Wire.write(toSend, 2);
    } else {
        char key = commandKeypad.getKey();
        if (key != NO_KEY && key != ' ') {
            Wire.write(key);
        }
        else {
            Wire.write('!');
        }
    }
}

void recieve(int numBytes){
    char readVal = Wire.read();
    if(readVal == 'r'){
        regularInputMode = true;
        digitalWrite(regularInputModeStatusPin, HIGH);
    } else if (readVal == 'n'){
        regularInputMode = false;
        digitalWrite(regularInputModeStatusPin, LOW);
    }
}
