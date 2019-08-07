/***************************************************************************************
    Name    : LCD Button Shield Menu for dfRobot 4xRelay shield
    Author  : Fadi Mossi, Ricardo Toledo-Crow
    Created : 8/5/19
    Version : 1.0
    Notes   : Code to operate a dfRobot 4x relay shield as timer switches. This code 
          is based on Paul Siewert's 'LCD Button Shield Menu' June 14, 2016
    License : This program is free software. You can redistribute it and/or modify
              it under the terms of the GNU General Public License as published by
              the Free Software Foundation, either version 3 of the License, or
              (at your option) any later version.
              This program is distributed in the hope that it will be useful,
              but WITHOUT ANY WARRANTY; without even the implied warranty of
              MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
              GNU General Public License for more details.
 ***************************************************************************************/

String menuItems[] = {"Set Period", "Set ON time", "Start", "Toggle"};

// Navigation button variables
int readKey;
int savedDistance = 0;
// Menu control variables
int menuPage = 0;
int maxMenuPages = round(((sizeof(menuItems) / sizeof(String)) / 2) + .5);
int cursorPosition = 0;
// Timer variables
int Period = 20; // set starting period (10 min) in seconds
int tTime = 10; // set initial On-time (1 min) in seconds. Time<=Period
// Relay variables
int Relay = 1; // Current relay: 1, 2, 3, 4
bool relayStatus[4] = {false, false, false, false};
// Relay pins on shield must be wired D4->Relay1 or will conflict with LCD
byte relayPin[4] = {4, 7, 8, 10}; //D4>RELAY1,D7>RELAY2,D8>RELAY3,D10>RELAY4
bool Run = 0;

// Creates 3 custom characters for the menu display
byte downArrow[8] = {
  0b00100, //   *
  0b00100, //   *
  0b00100, //   *
  0b00100, //   *
  0b00100, //   *
  0b10101, // * * *
  0b01110, //  ***
  0b00100  //   *
};
byte upArrow[8] = {
  0b00100, //   *
  0b01110, //  ***
  0b10101, // * * *
  0b00100, //   *
  0b00100, //   *
  0b00100, //   *
  0b00100, //   *
  0b00100  //   *
};
byte menuCursor[8] = {
  B01000, //  *
  B00100, //   *
  B00010, //    *
  B00001, //     *
  B00010, //    *
  B00100, //   *
  B01000, //  *
  B00000  //
};

// include the library code:
#include <Wire.h>
#include <Adafruit_RGBLCDShield.h>
#include <utility/Adafruit_MCP23017.h>

Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

void setup() {
  Serial.begin(9600); // Initializes serial communication  
  lcd.begin(16, 2);   // Initializes and clears the LCD screen
  lcd.clear();
  // Creates the byte for the 3 custom characters
  lcd.createChar(0, menuCursor);
  lcd.createChar(1, upArrow);
  lcd.createChar(2, downArrow);
  // initialize pins etc
  for (uint8_t i = 0; i < 4; i++) pinMode(relayPin[i], OUTPUT);
  for (uint8_t j = 0; j < 4; j++) digitalWrite(relayPin[j], LOW);
}

void loop() {
  mainMenuDraw();
  drawCursor();
  operateMainMenu();
}

// When called, this function will erase the current cursor and redraw it based on the cursorPosition and menuPage variables.
void drawCursor() {
  for (int x = 0; x < 2; x++) {     // Erases current cursor
    lcd.setCursor(0, x);
    lcd.print(" ");
  }

  // The menu is set up to be progressive (menuPage 0 = Item 1 & Item 2, menuPage 1 = Item 2 & Item 3, menuPage 2 = Item 3 & Item 4), so
  // in order to determine where the cursor should be you need to see if you are at an odd or even menu page and an odd or even cursor position.
  if (menuPage % 2 == 0) {
    if (cursorPosition % 2 == 0) {  // If the menu page is even and the cursor position is even that means the cursor should be on line 1
      lcd.setCursor(0, 0);
      lcd.write(byte(0));
    }
    if (cursorPosition % 2 != 0) {  // If the menu page is even and the cursor position is odd that means the cursor should be on line 2
      lcd.setCursor(0, 1);
      lcd.write(byte(0));
    }
  }
  if (menuPage % 2 != 0) {
    if (cursorPosition % 2 == 0) {  // If the menu page is odd and the cursor position is even that means the cursor should be on line 2
      lcd.setCursor(0, 1);
      lcd.write(byte(0));
    }
    if (cursorPosition % 2 != 0) {  // If the menu page is odd and the cursor position is odd that means the cursor should be on line 1
      lcd.setCursor(0, 0);
      lcd.write(byte(0));
    }
  }
}

// If there are common usage instructions on more than 1 of your menu items you can call this function from the sub
// menus to make things a little more simplified. If you don't have common instructions or verbage on multiple menus
// I would just delete this void. You must also delete the drawInstructions()function calls from your sub menu functions.
void drawInstructions() {
  lcd.setCursor(0, 1); // Set cursor to the bottom line
  lcd.print("Use ");
  lcd.write(byte(1)); // Up arrow
  lcd.print("/");
  lcd.write(byte(2)); // Down arrow
  lcd.print(" buttons");
}

// This function will generate the menu items that can fit on the screen. They will change as you scroll through your menu. Up and down arrows will indicate your current menu position.
void mainMenuDraw() {
  //Serial.println(menuPage);
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print(menuItems[menuPage]);
  lcd.setCursor(1, 1);
  lcd.print(menuItems[menuPage + 1]);
  if (menuPage == 0) {
    lcd.setCursor(15, 1);
    lcd.write(byte(2));
  } else if (menuPage > 0 and menuPage < maxMenuPages) {
    lcd.setCursor(15, 1);
    lcd.write(byte(2));
    lcd.setCursor(15, 0);
    lcd.write(byte(1));
  } else if (menuPage == maxMenuPages) {
    lcd.setCursor(15, 0);
    lcd.write(byte(1));
  }
}

void operateMainMenu() {
  int activeButton = 0;
  while (activeButton == 0) {
    uint8_t button = lcd.readButtons(); // for adafruit lcd
    switch (button) {
      case 0: // When button returns as 0 there is no action taken
        //Serial.println("case 0");
        break;
      case BUTTON_RIGHT:  // this will take us into the function when pressed
        button = 0;
        switch (cursorPosition) { // The case that is selected here is dependent on which menu page you are on and where the cursor is.
          case 0:
            setPeriod();
            break;
          case 1:
            setOnTime();
            break;
          case 2:
            setStart();
            break;
          case 3:
            setToggle();
            break;
        }
        activeButton = 1;
        mainMenuDraw();
        drawCursor();
        break;
      case BUTTON_UP: // This case will go up the menu
        button = 0;
        if (menuPage == 0) {
          cursorPosition = cursorPosition - 1;
          cursorPosition = constrain(cursorPosition, 0, ((sizeof(menuItems) / sizeof(String)) - 1));
        }
        if (menuPage % 2 == 0 and cursorPosition % 2 == 0) {
          menuPage = menuPage - 1;
          menuPage = constrain(menuPage, 0, maxMenuPages);
        }
        if (menuPage % 2 != 0 and cursorPosition % 2 != 0) {
          menuPage = menuPage - 1;
          menuPage = constrain(menuPage, 0, maxMenuPages);
        }
        cursorPosition = cursorPosition - 1;
        cursorPosition = constrain(cursorPosition, 0, ((sizeof(menuItems) / sizeof(String)) - 1));
        mainMenuDraw();
        drawCursor();
        activeButton = 1;
        break;
      case BUTTON_DOWN: // This case will go down the menu
        button = 0;
        if (menuPage % 2 == 0 and cursorPosition % 2 != 0) {
          menuPage = menuPage + 1;
          menuPage = constrain(menuPage, 0, maxMenuPages);
        }
        if (menuPage % 2 != 0 and cursorPosition % 2 == 0) {
          menuPage = menuPage + 1;
          menuPage = constrain(menuPage, 0, maxMenuPages);
        }
        cursorPosition = cursorPosition + 1;
        cursorPosition = constrain(cursorPosition, 0, ((sizeof(menuItems) / sizeof(String)) - 1));
        mainMenuDraw();
        drawCursor();
        activeButton = 1;
        break;
    }
  }
}

void setPeriod() { // sets the timer period to a max of 99,999 seconds
  int activeButton = 0;
  int flag = 0;
  int posIndx = 0; // index to the current digit of the Period string
  char segPeriod[20];
  
  sprintf(segPeriod,"%05d",Period); // Stores the Period as a string in setPeriod
// output to serial for debugging    
//  for (int m = 0; m < 20; m = m + 1) {
//    Serial.println(segPeriod[m]);
//  }
//  Serial.println(atoi(segPeriod)); // Prints and converts the Period segment from ASCII to integer 
//  Serial.print("size ...");
//  Serial.println(sizeof(segPeriod));

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("-Period:        ");

  while (activeButton == 0) {
    uint8_t button = lcd.readButtons(); // for adafruit lcd

    switch (button) {
      case BUTTON_LEFT:  // This case will exit the setPeriod function
        button = 0;
        activeButton = 1;
        Period = atoi(segPeriod); // converts the Period string to integer
        Serial.println(Period);
        break;
      case BUTTON_UP:  // This case will add 1 to the current digit in the Period string
        segPeriod[posIndx] = segPeriod[posIndx] + 1;
        if (segPeriod[posIndx]>'9') {
          segPeriod[posIndx] = '0';
        }
        break;
      case BUTTON_DOWN:  // This case will subtract 1 from the curent digit in the Period string
        segPeriod[posIndx] = segPeriod[posIndx] - 1;
        if (segPeriod[posIndx]<'0') {
          segPeriod[posIndx] = '9';
        }
        break;
      case BUTTON_RIGHT:  // This case will advance the cursor to the next Period digit to the right
        posIndx = posIndx + 1;
        if (posIndx > 4) {
          posIndx = 0;
        }
        break;
    }
    delay(50); // this delay helps with button 'bounce'. We really need to implement a debounce.
    // Locates the position and writes and displays the period
    lcd.setCursor(9, 0);
    lcd.print(segPeriod);
    lcd.setCursor(9 + posIndx, 0); // this code blinks the current digit in the string
    if (flag) {
      lcd.print(" ");
      flag = 0;
    }
    else {
      flag = 1;
      lcd.print(segPeriod[posIndx]);
    }
    delay(50);
  }
}

void setOnTime() { // sets the timer ON-time. ON-time should be <= Period
  int activeButton = 0;
  int flag = 0;
  int posIndx = 0; // index to the current digit of the ON-time string
  char segTime[20];

  sprintf(segTime, "%05d", tTime); // Stores the ON-time as a string in segTime

  // output to serial for debugging
  //  for (int m = 0; m < 20; m = m + 1) {
  //    Serial.println(segTime[m]);
  //  }
  //  Serial.println(atoi(segTime)); // Prints and converts the time segment from ASCII to integer
  //  Serial.print("size ...");
  //  Serial.println(sizeof(segTime));

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("-Time:          ");

  while (activeButton == 0) {
    uint8_t button = lcd.readButtons(); // for adafruit lcd

    switch (button) {
      case BUTTON_LEFT:  // This case will exit the setOnTime function
        button = 0;
        activeButton = 1;
        tTime = atoi(segTime); // converts the ON-time string to integer
        //Serial.println(tTime);
        break;
      case BUTTON_UP:  // This case will add 1 to the current digit in the ON-time string
        segTime[posIndx] = segTime[posIndx] + 1;
        if (segTime[posIndx] > '9') {
          segTime[posIndx] = '0';
        }
        break;
      case BUTTON_DOWN:  // This case will subtract 1 from the curent digit in the ON-time string
        segTime[posIndx] = segTime[posIndx] - 1;
        if (segTime[posIndx] < '0') {
          segTime[posIndx] = '9';
        }
        break;
      case BUTTON_RIGHT:  // This case will advance the cursor to the next ON-time digit to the right
        posIndx = posIndx + 1;
        if (posIndx > 4) { // we have a max of 5 digits or 99,999 seconds
          posIndx = 0;
        }
        break;
    }
    delay(50); // this delay helps with button 'bounce'. We really need to implement a debounce.
    // Locates the position and writes and displays the time
    lcd.setCursor(7, 0);
    lcd.print(segTime);
    lcd.setCursor(7 + posIndx, 0);
    if (flag) { // this code blinks the current digit in the string
      lcd.print(" ");
      flag = 0;
    }
    else {
      flag = 1;
      lcd.print(segTime[posIndx]);
    }
    delay(50);
  }
}

void setStart() { // Starts and runs the counter and displays progress and status of relays
  int activeButton = 0;
  lcd.clear();

  //Converts seconds to milliseconds
  long int onTime = tTime * 1000.00;
  long int onPeriod = Period * 1000.00;
  unsigned long nowtime = millis(); // set starting time
  float nexttime = 0;
  float prevtime = 0;

  // Locates the position and writes and displays "Running:" on the first line
  lcd.setCursor(0, 0); lcd.print("Running:        ");
  
  while (activeButton == 0) {
    uint8_t button = lcd.readButtons(); // for adafruit lcd

    prevtime = nexttime;
    nexttime = (millis() - nowtime) % (long int)(onPeriod);
    if (nexttime - prevtime <=  0) { // just started period so toggle
      relayStatus[Relay-1] = Toggle(Relay);
      // Serial.println(String(prevtime) + ',' + String(nexttime) + ',' + String(onPeriod) + ',' + String(onTime));
    }
    if (nexttime >= onTime && prevtime < onTime) { // just crossed the cycle so toggle
      relayStatus[Relay-1] = Toggle(Relay);
      // Serial.println(String(prevtime) + ',' + String(nexttime) + ',' + String(onPeriod) + ',' + String(onTime));
    }

    // Locates the position and writes and displays the "Running time" in seconds on the first line, and the "Relay" and "Status" on the second
    lcd.setCursor(9,0); lcd.print("       "); lcd.setCursor(9,0); lcd.print(int(nexttime / 1000));
    lcd.setCursor(0, 1); lcd.print("Relay:"); lcd.print(Relay); lcd.print(" Status:"); lcd.print(relayStatus[Relay-1]);

    switch (button) {
      case BUTTON_LEFT:  // This case will exit the setStart function
        button = 0;
        activeButton = 1;
        break;
      case BUTTON_UP:  // This case will change the relay to the next one if the "up" button is pressed
        Relay = (Relay + 1);
        if (Relay == 5) Relay = 1;
        break;
      case BUTTON_DOWN: // This case will change the relay to the previous one if the "down" button is pressed
        Relay = (Relay - 1);
        if (Relay == 0) Relay = 4;
        break;
      case BUTTON_SELECT: // This case will toggle the status if the "select" button is pressed
        button = 0;
        relayStatus[Relay-1] = Toggle(Relay);
        break;
    }
    delay(75);
  }
}

// Toggle relay function 
  bool Toggle(uint8_t rindex) {
  
  bool val = digitalRead(relayPin[rindex - 1]);
  val = !val;
  digitalWrite(relayPin[rindex - 1], val);
  return val;
}

void setToggle() { // toggle current relay
  int activeButton = 0;

  lcd.clear();

  while (activeButton == 0) {
    uint8_t button = lcd.readButtons(); // for adafruit lcd

    // Locates the position and writes and displays the "Current Relay" on the first line, and the "Status" on the second
    lcd.setCursor(0, 0);
    lcd.print("Current Relay: ");
    lcd.print(Relay);
    lcd.setCursor(0, 1);
    lcd.print("Status: ");
    lcd.print(relayStatus[Relay-1]);
    
    switch (button) {
      case BUTTON_LEFT:  // This case will execute if the "left" button is pressed
        button = 0;
        activeButton = 1;
        break;
      case BUTTON_UP:  // This case will change the relay to the next one if the "up" button is pressed
        Relay = (Relay + 1);
        if (Relay == 5) Relay = 1;
        break;
      case BUTTON_DOWN: // This case will change the relay to the previous one if the "down" button is pressed
        Relay = (Relay - 1);
        if (Relay == 0) Relay = 4;
        break;
      case BUTTON_SELECT: // This case will toggle the status is the "select" button is pressed
        button = 0;
        relayStatus[Relay-1] = Toggle(Relay);
        break;
    }
    delay(75);
  }
}
