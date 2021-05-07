#include <Keypad.h>
#include <Keyboard.h>
#include <Wire.h>

//Status variables
String commandQueue;
bool pauseMode;
bool regularInputMode;
bool loopMode;

//set up command keypad
const byte ROWS1 = 4; //four rows
const byte COLS1 = 4; //four columns
char commandKeys[ROWS1][COLS1] = {
	{'1','2','3','>'},
	{'4','5','6','<'},
	{'7','8','9','C'},
	{'m','0','c','p'}
};

/*
 * contolKeys key:
 * 0, 1, 2, 3, 4, 5, 6, 7, 8, 9         time delays
 * >                                    run
 * <                                    delete last charcter
 * C                                    Clear all characters / stop cycle mode
 * c                                    cycle mode toggle
 * m                                    regular controller toggle
 * y                                    pause mode
 */
byte colPins1[COLS1] = {7, 6, 5, 4}; //connect to the row pinouts of the keypad
byte rowPins1[ROWS1] = {11, 10, 9, 8}; //connect to the column pinouts of the keypad
Keypad controlKeypad = Keypad( makeKeymap(commandKeys), rowPins1, colPins1, ROWS1, COLS1 );

void setup(){
	regularInputMode = false;
	pauseMode = false;

	Wire.begin();
	pinMode(2, INPUT_PULLUP);
	pinMode(LED_BUILTIN, OUTPUT);
	pinMode(12, OUTPUT);  
	Keyboard.begin();
}

void loop(){
	char key = controlKeypad.getKey();
	if (key != NO_KEY){
		switch(key)
		{
			case '0':
				commandQueue.concat(key);
				break;

			case '1':
				commandQueue.concat(key);
				break;

			case '2':
				commandQueue.concat(key);
				break;

			case '3':
				commandQueue.concat(key);
				break;

			case '4':
				commandQueue.concat(key);
				break;

			case '5':
				commandQueue.concat(key);
				break;

			case '6':
				commandQueue.concat(key);
				break;

			case '7':
				commandQueue.concat(key);
				break;

			case '8':
				commandQueue.concat(key);
				break;

			case '9':
				commandQueue.concat(key);
				break;

			case '>':
				executeCommandQueue();
				commandQueue = "";
				break;

			case '<':
				if(commandQueue.length() > 0){
					commandQueue.remove(commandQueue.length() - 1);
				}
				break;

			case 'C':
				commandQueue = "";
				break;

			case 'm':
				if(regularInputMode){
					regularInputMode = false;
				} else {
					regularInputMode = true;
				}
				break;

			case 'p':
				if(pauseMode){
					pauseMode = false;
				} else {
					pauseMode = true;
				}
				break;

			case 'c':
				if(loopMode){
					loopMode = false;
				} else {
					loopMode = true;
				}
				break;

		}
	}

	if(regularInputMode){
		//clear up and put in input mode queue
		commandQueue = "";
		pauseMode = false;
		loopMode = false;
		regularInputLoop();
	}

	Wire.requestFrom(8, 1);    // request 1 bytes from slave device #8
	while (Wire.available()) { // slave may send less than requested
		char key = Wire.read(); // receive a byte as character
		if(key != '!')
		{
			commandQueue.concat(key);         // print the character
		}
	}
}

void executeCommandQueue(){
	digitalWrite(12,HIGH);
	if(pauseMode){
		Keyboard.press('o');
		delay(50);
		Keyboard.release('o');
		//push start button
	}

	int currentPos = 0;
	int queueLength = commandQueue.length();
	String singleCommandBlock = "";

	//Read through commandQueue and execute commands
	while(currentPos != queueLength){

		if(isDigit(commandQueue.charAt(currentPos))){
			int timeUnitsPerSingleCommand = 0;
			if(isDigit(commandQueue.charAt(currentPos+1))){
				timeUnitsPerSingleCommand = 10*(commandQueue.charAt(currentPos) - '0') + (commandQueue.charAt(currentPos+1) - '0');
				currentPos++;
			}
			else{
				timeUnitsPerSingleCommand = (commandQueue.charAt(currentPos) - '0');
			}
			executeCommand(singleCommandBlock, timeUnitsPerSingleCommand);
			singleCommandBlock = "";
		} else {
			singleCommandBlock.concat(commandQueue.charAt(currentPos));  
		}
		currentPos++;
	}


	if(pauseMode){
		Keyboard.press('o');
		delay(50);
		Keyboard.release('o');  
	}
	digitalWrite(12,LOW);
}

int potPin = 2;

int getUnitLength(){
	int retVal;
	retVal = analogRead(2);

	if(retVal < 200){
		retVal = 200;
	}
	else if(retVal < 400){
		retVal = 400;
	}
	else if(retVal < 600){
		retVal = 600;
	}
	else if(retVal < 800){
		retVal = 800;
	}
	else{
		retVal = 1000;
	}

	return retVal;
}

int getMashesPerTimeUnit(){
	int retVal;
	retVal = 100;
	return retVal;
}

void executeCommand(String &singleCommandBlock, int timeUnits){
	int commandLength = singleCommandBlock.length();
	int unitLength = getUnitLength();
	int totalCommandLength = ((unitLength)*(timeUnits));
	//scan for whether the command containts a mash character #
	bool containsMash = false;

	for(int i = 0; i < commandLength; i++){
		if(commandQueue.charAt(i) == '#'){
			containsMash = true;
		}
	}

	containsMash = false;

	if(containsMash){
		//byte mashesPUT = getMashesPerTimeUnit();
	} else {
		for(int i =0; i < singleCommandBlock.length(); i++){
			Keyboard.press(singleCommandBlock.charAt(i));
		}
		delay(totalCommandLength);
		for(int i =0; i < singleCommandBlock.length(); i++){
			Keyboard.release(singleCommandBlock.charAt(i));
		}
	}
	delay(20);

}

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

void regularInputLoop(){
	Wire.beginTransmission(8);
	Wire.write('r');
	Wire.endTransmission(8);
	delay(50);

	/*
	 *  Only Accept input after all buttons on command side aren't being pressed
	 */
	bool keyIsBeingPressed = false;
	do{
		Wire.requestFrom(8,2);
		if(Wire.read() != 0){
			keyIsBeingPressed = true;
		}else{
			keyIsBeingPressed = false;
		}
		if(Wire.read() != 0){
			keyIsBeingPressed = true;
		}

	} while (keyIsBeingPressed);

	Wire.requestFrom(8,2);
	byte previousInputs[2];

	previousInputs[0] = Wire.read();
	previousInputs[1] = Wire.read();
	byte currentInputs[2] = {0,0};
	while(controlKeypad.getKey() != 'm'){  
		Wire.requestFrom(8,2);
		currentInputs[0] = Wire.read();
		currentInputs[1] = Wire.read();
		byte firstTwoRowDifference = currentInputs[0] ^ previousInputs[0];
		if(firstTwoRowDifference != 0){
			updateKeyboardRI(currentInputs[0], firstTwoRowDifference, true);
		}
		byte secondTwoRowDifference = currentInputs[1] ^ previousInputs[1];
		if(secondTwoRowDifference != 0){
			updateKeyboardRI(currentInputs[1], secondTwoRowDifference, false);
		}
		previousInputs[0] = currentInputs[0];
		previousInputs[1] = currentInputs[1];
	}

	//release all Keyboard
	Keyboard.releaseAll();

	Wire.beginTransmission(8);
	Wire.write('n');
	Wire.endTransmission(8);
	delay(50);
	regularInputMode = false;
}

void updateKeyboardRI(byte currentInputs, byte changes, bool topRows){
	if(topRows){
		if( (changes & leftShoulder) == leftShoulder)
		{
			if (currentInputs & leftShoulder == leftShoulder){
				Keyboard.press('q');
			} else {
				Keyboard.release('q');
			}
		}
		if( (changes & upButton) == upButton ){
			if ((currentInputs & upButton) == upButton){
				Keyboard.press('u');
			} else {
				Keyboard.release('u');
			}
		}
		if( (changes & rightShoulder) == rightShoulder) {
			if ((currentInputs & rightShoulder) == rightShoulder){
				Keyboard.press('e');
			} else {
				Keyboard.release('e');
			}
		}
		if( (changes & aButton) == aButton){
			if ((currentInputs & aButton) == aButton){
				Keyboard.press('a');
			} else {
				Keyboard.release('a');
			}
		}
		if( (changes & leftButton) == leftButton){
			if ((currentInputs & leftButton) == leftButton){
				Keyboard.press('l');
			} else {
				Keyboard.release('l');
			}
		}
		if( (changes & rightButton) == rightButton){
			if ((currentInputs & rightButton) == rightButton){
				Keyboard.press('r');
			} else {
				Keyboard.release('r');
			}
		}
		if( (changes & bButton) == bButton){
			if ((currentInputs & bButton) == bButton){
				Keyboard.press('b');
			} else {
				Keyboard.release('b');
			}
		}
	}else{
		if( (changes & selectButton) == selectButton){
			if ((currentInputs & selectButton) == selectButton){
				Keyboard.press('s');
			} else {
				Keyboard.release('s');
			}
		}
		if( (changes & downButton) == downButton ){
			if ((currentInputs & downButton) == downButton){
				Keyboard.press('d');
			} else {
				Keyboard.release('d');
			}
		}
		if( (changes & startButton) == startButton) {
			if ((currentInputs & startButton) == startButton){
				Keyboard.press('o');
			} else {
				Keyboard.release('o');
			}
		}
		if( (changes & xButton) == xButton){
			if ((currentInputs & xButton) == xButton){
				Keyboard.press('x');
			} else {
				Keyboard.release('x');
			}
		}
		if( (changes & hotkeyButton) == hotkeyButton){
			if ((currentInputs & hotkeyButton) == hotkeyButton){
				Keyboard.press('k');
			} else {
				Keyboard.release('k');
			}
		}
		if( (changes & yButton) == yButton){
			if ((currentInputs & yButton) == yButton){
				Keyboard.press('y');
			} else {
				Keyboard.release('y');
			}
		}
	}
}
