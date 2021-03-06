#ifndef INPUT_SR_H
#define INPUT_SR_H

#include <SPI.h>
#include <Wire.h>
#include "buzzer.h"
class input_SR
{
  public:
    byte shift_1;
    byte shift_2;
    byte activeButtonCode;

    buzzer * buzzer_inst;
    
    int t1_LED;
    int t2_LED;


    int pressType;
    //int pressHold;

    int buttonPressed;
    int buttonPressedTeam;

    int PL;
    int t1pin;
    int t2pin;
    
    void debugPress(int ignore_none){
      if (buttonPressed != 0) {
        Serial.println("-----------------------------");
        Serial.print("Press Type: ");
        Serial.println(pressType);
//        Serial.print("Press hold: ");
//        Serial.println(pressHold);

        Serial.print("Button Pressed: ");
        Serial.println(buttonPressed);
        Serial.print("Button Pressed Team: ");
        Serial.println(buttonPressedTeam);
        Serial.println("-----------------------------");
      }
    }
    void DataShiftIn() {
      digitalWrite(PL, LOW);
      delay(1);
      digitalWrite(PL, HIGH);
      shift_1 = SPI.transfer(0x00);
      shift_2 = SPI.transfer(0x00);
    }

    void init() {
      pressType = 0;
    
      buttonPressed = 0;
      buttonPressedTeam = 0;

      set_LED_brightness(1, 100);
      set_LED_brightness(2, 100);
    }

    input_SR(int _PL, int _t1pin, int _t2pin) : PL(_PL), t1pin(_t1pin), t2pin(_t2pin)  {
      pinMode(PL, OUTPUT);
      pinMode(t1pin, OUTPUT);
      pinMode(t2pin, OUTPUT);
      buzzer_inst = new buzzer(9);
      SPI.setClockDivider(SPI_CLOCK_DIV128);
      SPI.setDataMode(SPI_MODE0);
      SPI.setBitOrder(MSBFIRST);
      SPI.begin();
      init();
    }

    void set_LED_brightness(int team, float amt){
      if(amt>100) amt =100;
      else if(amt<0) amt =0;
      amt = 2.55*amt;
      
      if(team == 1) analogWrite(t1pin, amt);
      else if (team ==2) analogWrite(t2pin, amt);
    }

    void set_LED_brightness(float amt){
      if(amt>100) amt =100;
      else if(amt<0) amt =0;
      amt = 2.55*amt;
      
      if(buttonPressedTeam == 1) analogWrite(t1pin, amt);
      else if (buttonPressedTeam ==2) analogWrite(t2pin, amt);
    }
    int currButtonPressed(){
      if (buttonPressedTeam == 1) return !bitRead(shift_1, 8 - buttonPressed);
      else if (buttonPressedTeam == 2) return !bitRead(shift_2, 8 - buttonPressed);
      else return -1;
    }

    bool timeThresholdMet( unsigned long time_pressed, unsigned long time_threshold){
      //Serial.println(((millis() - time_pressed) > time_threshold));
      return ((millis() - time_pressed) >= time_threshold);
    }

    bool timeThresholdMet( unsigned long time_pressed, unsigned long time_threshold_begin, unsigned long time_threshold_end ){
      //Serial.println((((millis() - time_pressed) > time_threshold_begin) && ((millis() - time_pressed) < time_threshold_end)));
      return (((millis() - time_pressed) >= time_threshold_begin) && ((millis() - time_pressed) <= time_threshold_end));
    }
    void extractButtonInfo(){
      if (shift_1 <= 0b01111000) {
        buttonPressedTeam = 1;
        Serial.println("Debug");
        buttonPressed = 1;
      }
      else if (shift_1 <= 0b10111000) {
        buttonPressedTeam = 1;
        buttonPressed = 2;
      }
      else if (shift_1 <= 0b11011000) {
        buttonPressedTeam = 1;
        buttonPressed = 3;
      }
      else if (shift_1 <= 0b11101000) {
        buttonPressedTeam = 1;
        buttonPressed = 4;
      }
      else if (shift_1 <= 0b11110000) {
        buttonPressedTeam = 1;
        buttonPressed = 5;
      }
      else if (shift_2 <= 0b01111000) {
        buttonPressedTeam = 2;
        buttonPressed = 1;
      }
      else if (shift_2 <= 0b10111000) {
        buttonPressedTeam = 2;
        buttonPressed = 2;
      }
      else if (shift_2 <= 0b11011000) {
        buttonPressedTeam = 2;
        buttonPressed = 3;
      }
      else if (shift_2 <= 0b11101000) {
        buttonPressedTeam = 2;
        buttonPressed = 4;
      }
      else if (shift_2 <= 0b11110000) {
        buttonPressedTeam = 2;
        buttonPressed = 5;
      }
      else {
        buttonPressedTeam = 0;
        buttonPressed = 0;
      }
      if (buttonPressedTeam == 1) activeButtonCode = shift_1;
      else if (buttonPressedTeam == 2) activeButtonCode = shift_2;
      else activeButtonCode = 0;
    }

    void getPressType() {
      DataShiftIn();
      bool done = false;
      extractButtonInfo();
      unsigned long time_pressed;
      int short_threshold = 400;
      int long_threshold=1800;
      pressType = 0;
      set_LED_brightness(100);
      time_pressed = millis();
      if (buttonPressedTeam != 0){
        pressType = 1;
        while (currButtonPressed()){
          
          if(timeThresholdMet(time_pressed, long_threshold+300)){
               set_LED_brightness(100); 
              pressType = 3;}
          else if(timeThresholdMet(time_pressed, long_threshold+200)){
               set_LED_brightness(0);
              pressType = 3;}
          else if(timeThresholdMet(time_pressed, long_threshold+100)){
               set_LED_brightness(100); 
              pressType = 3;}
          else if (timeThresholdMet(time_pressed, long_threshold)){
              set_LED_brightness(0); 
             pressType = 3;}
          else if (timeThresholdMet(time_pressed, short_threshold+200)){
              set_LED_brightness(100); 
             pressType = 2;}
          else if (timeThresholdMet(time_pressed, short_threshold)){
             set_LED_brightness(0); 
            pressType = 2; }

          if (timeThresholdMet(time_pressed, short_threshold, short_threshold+40)){
            buzzer_inst->toneOn();
          }
          if (timeThresholdMet(time_pressed, long_threshold, long_threshold+40)){
            buzzer_inst->toneOn();
          }
          if (timeThresholdMet(time_pressed, long_threshold+100, long_threshold+140)){
            buzzer_inst->toneOn();
          }
          else buzzer_inst->toneOff();
            
          DataShiftIn();
        }
      }
       set_LED_brightness(1,100);
       set_LED_brightness(2,100);
      //debugPress(0);
    }
};

#endif
