# LCD-Button-Shield-Menu-for-dfRobot-4xRelay-shield

This Arduino code had been written for the sole purpose of watering a wall plant throughout the day with a given set time frame.

The code has four menus: Period, ON-time, Start, and Toggle, where:
1. The Period menu item implements the user to set a time period in seconds by adding or subtracting numbers within five digits.
2. The ON-time menu item implements the user to set a time in seconds by adding or subtracting numbers within five digits. This time has to be lower or equal to the Period. It is the time that will run before the plant gets watered.
3. The Start menu item runs the system based on the times implemented, displayes the elapsed time, and toggles the status of the relays.
4. The Toggle menu item toggles the staus if the "Select" button s pressed, and lets the user choose the current relay (there are four.)

The Arduino itself has six buttons but we primarily use 5 of them. The "Left" buttton mostly exist back to the Menu page. The "Up" button either goes up the Menu line or adds relay. The "Down" button does the opposite of the Up button. The "Right" button either goes into a Menu item or shifts to the next digit (there are five digits for period and ON-time) to change the time. Finally, the "Select" button toggles the status of the relays.
