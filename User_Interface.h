#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H

//we should regulate input to the OLED based on ADC readings
void displayMenu();
void goDown(int joystickVal);
void goUp(int joystickVal);
void buttonClick() ;

#endif //USER_INTERFACE_H