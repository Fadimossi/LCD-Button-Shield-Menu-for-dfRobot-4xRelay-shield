# LCD-Button-Shield-Menu-for-dfRobot-4xRelay-shield

This Arduino code has been written for the sole purpose of running a timer. For this specific project, the goal is to implement the Arduino computer to water a wall plant constantly throughout the day. Other examples this code could be used for include turning on/off lights, music, and toasters. 

The code has four menus: Period, ON-time, Start, and Toggle, where:
1. The Period menu item implements the user to set a time period in seconds by adding or subtracting numbers within five digits.
2. The ON-time menu item implements the user to set a time in seconds by adding or subtracting numbers within five digits. This time has to be lower or equal to the Period. It is the time that will run before the plant gets watered.
3. The Start menu item runs the system based on the times implemented, displays the elapsed time, and toggles the status of the relays.
4. The Toggle menu item toggles the status if the "Select" button is pressed, and lets the user choose the current relay (there are four.)

The Arduino itself has six buttons but we primarily use 5 of them. The "Left" button mostly exist back to the Menu page. The "Up" button either goes up the Menu line or adds relay. The "Down" button does the opposite of the "Up" button. The "Right" button either goes into a Menu item or shifts to the next digit (there are five digits for Period and ON-time) to change the time. Finally, the "Select" button toggles the status of the relays.

Reference: 
Paul SS, https://www.instructables.com/id/Arduino-Uno-Menu-Template/
