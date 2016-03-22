#ifndef HEADER_MOTOR
#define HEADER_MOTOR

void motor_init();//zet DDR van motor en bumper.
void pwmklok();//zet pwm klok aan.
void stop();//zet de pwm klok uit en zet de compare registers op 0.
void vooruit();//zet de rijrichting op vooruit.
void achteruit();//zet de rijrichting op achteruit.
void rechtdoor(uint8_t);//snelheid = ("ingevoerde waarde" / 255 * 100%).
void links(uint8_t);//stuurt naar links. snelheid = ("ingevoerde waarde" / 255 * 100%).
void rechts(uint8_t);//stuurt naar rechts. snelheid = ("ingevoerde waarde" / 255 * 100%).

#endif
