# Wireless Windshield Defroster
The solution is rather quite simple, in order to control the warm air fan, a relay can be used to either complete the circuit to the source or disconnect the fan. As for the spatial distancing, the relay could be controlled by an impulse from a wireless module.

For the relay I used the Songle SRD-05VDC-SL-C module, for the wireless, the ESP8266 Wi-Fi module, and an Arduino Uno to manage them all.

_Fritzing diagram of the project_
![fritzing diagram](https://github.com/DudasDorian/wireless-windshield-defroster/blob/main/fritzing_project.png)

## Hardware considerations
In the image above, one can observe the connections necessary for the project to work properly. The Wi-Fi module needs a 3.3V source at its VCC and Enable pins, while the relay needs to be powered from a 5V source; both components also require a grounding connection. The 0 and 1 pins on the Arduino Uno board represent the RX (Receive) and TX (Transmit) and are used as the channel of communication with the ESP8266 module. Through pin number 3, the relay is controlled at the whim of the Wi-Fi module. The relay intermediates a circuit from the battery to the heating fan. A wire from the cable had to be inserted and screwed into the relay NO (Normally Open) and COM (Common) output pins.

## Software considerations
The ESP8266 Wi-Fi module hosts a webpage at IP address 192.68.4.1 on port 80, from where it can be accessed functionally. There are two buttons, ON and OFF, which redirect to a seemingly identical page, but which can be distinguished operationally. They control the relay wire by writing either LOW or HIGH signals on the relay pin. For an even better user experience, the Arduino makes use of the Timer Interrupt to count to a set number of seconds. It was set to activate at a frequency of 1Hz, and it increments a numeric variable, should the relay be on, which is restrained by a boolean variable. If the numeric count reaches the set amount, the relay is turned off automatically. The numeric variable is also reset once a pivotal moment is reached.
