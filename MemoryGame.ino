/**
 * 1. Connect to power
 * 2. Green LED will turn on when game is prepared.
 * 3. Push Main button (Without color assigned)
 * 4. All LEDs will turn on and one LED will kick off the game
 * 5. Enjoy the game!
 * 6. If you push wrong Color button during the game, LEDs will blink few times and you have to reset the game using Main button.
 * 
 * Tip: If you want to set up difficulty of the game, hold Main button in any phase for a while until all LEDs start blinking rappidly.
 * LEDs will show currently selected level of difficulty in binary code. Increase difficulty using Red button and decrease it using Yellow.
 * Hold Main button again to confirm your selection.
 */




const char colors[4] = {'r', 'g', 'b', 'y'};

const int redLedPin = 2;
const int greenLedPin = 3;
const int blueLedPin = 4;
const int yellowLedPin = 5;

const int redButtonPin = 6;
const int greenButtonPin = 7;
const int blueButtonPin = 8;
const int yellowButtonPin = 9;
const int newGameButtonPin = 10;

const int maxGameLength = 100;
char game[maxGameLength];
int currentLevelColorIndex = -1;
int currentLevel = -1;
int pushedButtonPin = -1;

unsigned long lastButtonChangeTime = 0;
int difficulty = 1;
const int difficultySpaceSize = 9;
const int gameDifficulties[difficultySpaceSize] = {1000, 900, 800, 700, 600, 500, 400, 300, 200};

const int prepared_mode = 0;
const int game_mode = 1;
const int difficulty_mode = 2;

int currentMode;

void setup() {
    Serial.begin(9600);
    pinMode(redLedPin, OUTPUT);
    pinMode(greenLedPin, OUTPUT);
    pinMode(blueLedPin, OUTPUT);
    pinMode(yellowLedPin, OUTPUT);
    pinMode(redButtonPin, INPUT);
    pinMode(greenButtonPin, INPUT);
    pinMode(blueButtonPin, INPUT);
    pinMode(yellowButtonPin, INPUT);
    pinMode(newGameButtonPin, INPUT);
    currentMode = prepared_mode;
}

void loop() {
  if((millis() - lastButtonChangeTime) < 100) {
    return;
  }
  switch (currentMode) {
    case prepared_mode:
      digitalWrite(greenLedPin, HIGH);
      break;
    case difficulty_mode:
      displayCurrentDifficulty();
      break;
  }
  if (pushedButtonPin == -1) {
    checkButtonPush();
  }
  else {
    checkButtonRelease();
  }
}

void checkButtonPush() {
  if (digitalRead(newGameButtonPin) == HIGH) {
    pushButton(newGameButtonPin);
  }
  else if (digitalRead(redButtonPin) == HIGH) {
    pushButton(redButtonPin);
  }
  else if (digitalRead(greenButtonPin) == HIGH) {
    pushButton(greenButtonPin);
  }
  else if (digitalRead(blueButtonPin) == HIGH) {
    pushButton(blueButtonPin);
  }
  else if (digitalRead(yellowButtonPin) == HIGH) {
    pushButton(yellowButtonPin);
  }
}

void pushButton(int buttonPin) {
  pushedButtonPin = buttonPin;
  lastButtonChangeTime = millis();
  Serial.println("Pushed button: " + String(pushedButtonPin));
  if(currentMode == game_mode) {
    if (pushedButtonPin != newGameButtonPin) {
      digitalWrite(getLedPinByButtonPin(buttonPin), HIGH);
    }
  }
}

void checkButtonRelease() {
  if (digitalRead(pushedButtonPin) == LOW) {
    lastButtonChangeTime = millis();
    switch (currentMode) {
      case prepared_mode:
        if (pushedButtonPin == newGameButtonPin) {
          blinkAllColors(1000, 1);
          resetGame();
          playLevel();
          currentMode = game_mode;
        }
        break;
      case game_mode:
        if (pushedButtonPin == newGameButtonPin) {
          blinkAllColors(1000, 1);
          resetGame();
          playLevel();
        } else {
          releaseColorButton(pushedButtonPin);
        }
        break;
      case difficulty_mode:
        if (pushedButtonPin == redButtonPin) {
          if (difficulty < difficultySpaceSize) {
            difficulty += 1;
          }
        }
        else if(pushedButtonPin == yellowButtonPin) {
          if (difficulty > 1) {
            difficulty -= 1;
          }
        }
        break; 
    }
    pushedButtonPin = -1;
  } else {
  if(pushedButtonPin == newGameButtonPin and (millis() - lastButtonChangeTime) > 1000) {
    blinkAllColors(150, 5);
    if (currentMode == game_mode) {
      currentMode = difficulty_mode;
    } else {
      currentMode = game_mode;
    }
  }
  }
}

void releaseColorButton(int buttonPin) {
  digitalWrite(getLedPinByButtonPin(buttonPin), LOW);
  char buttonColor = getButtonColor(buttonPin);
  if (buttonColor == game[currentLevelColorIndex]) {
    currentLevelColorIndex += 1;
    if (currentLevelColorIndex > currentLevel) {
      delay(1000);
      nextLevel();
      playLevel();
    }
  }
  else {
    gameLost(buttonColor);
  }
}



void resetGame() {
  game[0] = getRandomColor();
  for (int i = 1; i < maxGameLength; i++) {
    game[i] = '-';
  }
  currentLevel = 0;
  currentLevelColorIndex = 0;
}

void nextLevel() {
  currentLevel += 1;
  currentLevelColorIndex = 0;
  game[currentLevel] = getRandomColor();
}

void playLevel() {
  Serial.println("Level #" + String(currentLevel + 1));
  displayLevel();
}

void gameLost(char pushedColor) { 
  Serial.println("Game lost :( You pushed color " + String(pushedColor) + " but you should push " + String(game[currentLevelColorIndex]));
  blinkAllColors(250, 3);
}

char getRandomColor() {
  int randomNum = random(0, 4);
  return colors[randomNum];
}



void displayLevel() {
  int gameSpeed = gameDifficulties[difficulty - 1];
  for (int i = 0; i <= currentLevel; i++) {
    blinkLed(game[i], gameSpeed);
    if (i != currentLevel) {
      delay(gameSpeed);
    }
  }
}

int getLedPinByColor(char color) {
  switch (color) {
    case 'r':
      return redLedPin;
     case 'g':
      return greenLedPin;
     case 'b':
      return blueLedPin;
     case 'y':
      return yellowLedPin;
  }
}

int getLedPinByButtonPin(int buttonPin) {
  switch (buttonPin) {
    case redButtonPin:
      return redLedPin;
     case greenButtonPin:
      return greenLedPin;
     case blueButtonPin:
      return blueLedPin;
     case yellowButtonPin:
      return yellowLedPin;
  }
}

char getButtonColor(int buttonPin) {
  switch (buttonPin) {
    case redButtonPin:
      return 'r';
    case greenButtonPin:
      return 'g';
    case blueButtonPin:
      return 'b';
    case yellowButtonPin:
      return 'y';
  }
}

void blinkAllColors(int blink_delay, int repeat) {
  for (int i = 0; i < repeat; i++) {
    digitalWrite(redLedPin, HIGH);
    digitalWrite(greenLedPin, HIGH);
    digitalWrite(blueLedPin, HIGH);
    digitalWrite(yellowLedPin, HIGH);
    delay(blink_delay);
    digitalWrite(redLedPin, LOW);
    digitalWrite(greenLedPin, LOW);
    digitalWrite(blueLedPin, LOW);
    digitalWrite(yellowLedPin, LOW);
    if (i + 1 != repeat) {
      delay(blink_delay);
    }
  }
}

void blinkLed(char color, int blink_delay) {
  int ledPin = getLedPinByColor(color);
  digitalWrite(ledPin, HIGH);
  delay(blink_delay);
  digitalWrite(ledPin, LOW);
}

void displayCurrentDifficulty() {
  digitalWrite(yellowLedPin, bitRead(difficulty, 0));
  digitalWrite(blueLedPin, bitRead(difficulty, 1));
  digitalWrite(greenLedPin, bitRead(difficulty, 2));
  digitalWrite(redLedPin, bitRead(difficulty, 3));
}

