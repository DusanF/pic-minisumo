# PIC mini sumo
Firmware for mini sumo robot created for bachelor's thesis

Robot in action, against competitor from CVUT:
https://youtu.be/-3ioMXq8Z8U

![alt text](https://github.com/DusanF/pic-minisumo/blob/main/sumo.jpg "Constructed mini sumo robot")

Contains MPLAB X IDE project.
Firmware is written in C for PIC16LF18456 MCU.

Robot was designed and created using affordable componnents, so it can be potentially used as educational tool. Total cost of hw for one unit is estimated to be around 20€. Nevertheless, it made it to the semifinals at international event "Robotic day" in Prague in 2019.

## Description
Program is realized as state machine. After reset begins initialzation of some variables, internal peripherals and GPIOs. After that, main loop is started, periodically checking conditions for change of state. If some condition is satisfied, state is changed and required actions are executed.

All sensors are read by interrupts. If any of the sensors changes state, appropriate flag is set.

Robot have line sensors located on bottom near front edge and IR reflective sensor of enemy presence in front. User button and timers are also generating interrupt requests.

### Possible states of state machine
![alt text](https://github.com/DusanF/pic-minisumo/blob/main/state_machine.png "state machine")

