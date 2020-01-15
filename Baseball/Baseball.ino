#include <SPI.h>
class input_SR
{
  public:
    byte shift[2];
    
    int PL;

    void DataShiftIn() {
      digitalWrite(PL, LOW);
      delay(1);
      digitalWrite(PL, HIGH);
      shift[1] = SPI.transfer(0x00);
      shift[0] = SPI.transfer(0x00);
    }

    void updateStates() {
      DataShiftIn();
      for (int i = 0; i < 10 ; i++) {
        prevState[i]==currState[i];
        currState[i] = (i < 5) ?  !bitRead(shift[0], 7 - i) : !bitRead(shift[1], 7 - (i - 5));
      }
    }


  
    int currState [10];
    int prevState [10];
    input_SR(int _PL) :PL(_PL) {
      pinMode(PL, OUTPUT);
      SPI.setClockDivider(SPI_CLOCK_DIV128);
      SPI.setDataMode(SPI_MODE0);
      SPI.setBitOrder(MSBFIRST);
      SPI.begin();}
    int getPressType() {
      int time_ctr;
      for (int i = 0; i < 10; i++) {
        updateStates();
        time_ctr = 0;
        if(currState[i]==1 && prevState[i]==0){
        while (currState[i] == 1) {
          delay(1);
          time_ctr++;
          updateStates();
          if (currState[i] == 0) return (i + 1);
          if (time_ctr > 400) {return (i + 1) + 10;}
        }
      }
      }
      return 0;
    }
};


class hex_display
{ public:
    int segmentLatch;
    int segmentClock;
    int segmentData;

    hex_display( int _segmentLatch, int _segmentClock, int _segmentData):
      segmentLatch(_segmentLatch), segmentClock(_segmentClock), segmentData(_segmentData)
    {
      pinMode(segmentLatch, OUTPUT);
      pinMode(segmentClock, OUTPUT);
      pinMode(segmentData, OUTPUT);
    }

    void LatchData()
    {
    digitalWrite(segmentLatch, LOW);
    digitalWrite(segmentLatch, HIGH);
    }
    //Given a number, or '-', shifts it out to the display
   

//Given a number, or '-', shifts it out to the display
void postNumber(byte number, boolean decimal)
{
  //    -  A
  //   / / F/B
  //    -  G
  //   / / E/C
  //    -. D/DP

//#define a  1<<0
//#define b  1<<6
//#define c  1<<5
//#define d  1<<4
//#define e  1<<3
//#define f  1<<1
//#define g  1<<2
//#define dp 1<<7

  byte segments;

  switch (number)
  {
    case 1: segments = b | c; break;
    case 2: segments = a | b | d | e | g; break;
    case 3: segments = a | b | c | d | g; break;
    case 4: segments = f | g | b | c; break;
    case 5: segments = a | f | g | c | d; break;
    case 6: segments = a | f | g | e | c | d; break;
    case 7: segments = a | b | c; break;
    case 8: segments = a | b | c | d | e | f | g; break;
    case 9: segments = a | b | c | d | f | g; break;
    case 0: segments = a | b | c | d | e | f; break;
    case ' ': segments = 0; break;
    case 'c': segments = g | e | d; break;
    case '-': segments = g; break;
    case 'P': segments = e | f | a | b | g; break;
    case 'I': segments = b | c; break;
    case 'n': segments = e | a | c | f | b; break;
    case 'g': segments = a | f | g | b | c | d; break;
    case 'O': segments = a | b | c | d | e | f; break;
  }

  if (decimal) segments |= dp;

  //Clock these bits out to the drivers
  for (byte x = 0 ; x < 8 ; x++)
  {
    digitalWrite(segmentClock, LOW);
    digitalWrite(segmentData, segments & 1 << (7 - x));
    digitalWrite(segmentClock, HIGH); //Data transfers to the register on the rising edge of SRCK
    
  }
}
//Takes a number (-9,99) and displays on two 7-Segment Displays. Has boundary bhecks
void showNumber(float value)
{
  int negative = (value <0)? 1: 0;
  int number = abs(value); //Remove negative signs and any decimals

  //Serial.print("number: ");
  //Serial.println(number);
  if (negative) {
     if (number > 9) {number = 9;}
     postNumber(number, false);
     postNumber('-', false);
  }
  else {
     for (byte x = 0 ; x < 2 ; x++)
    {
      int remainder = number % 10;
      postNumber(remainder, false);
      number /= 10;
    }
  }
};



input_SR buttons(8);
hex_display hex(4,3,2);

void setup(){
  Serial.begin(9600);
 
  }
  int a1;
void loop(){

 
  a1 =buttons.getPressType();
  if(a1!=0){Serial.println(a1);}
  hex.showNumber(3,4);
  hex.LatchData();
  delay(1000);
  
  
  
  
  }



//
//
//
//class Scoreboard
//{
//    hex_display * hex_display;
//
//
//  public:
//
//
//    void updateLEDs(byte top, byte diamond) {
//      hex_display->shiftOut(top);
//      hex_display->shiftOut(diamond);
//    }
//
//    byte topLEDSaccum_SO(int first, int second)
//    {
//      if (first == 0 && second == 0) return 0b000000;
//      else if (first == 0 && second == 1) return 0b000001;
//      else if (first == 0 && second == 2) return 0b000011;
//      else if (first == 0 && second == 3) return 0b000111;
//
//      else if (first == 1 && second == 0) return 0b100000;
//      else if (first == 1 && second == 1) return 0b100100;
//      else if (first == 1 && second == 2) return 0b100110;
//      else if (first == 1 && second == 3) return 0b100111;
//
//
//      else if (first == 2 && second == 0) return 0b110000;
//      else if (first == 2 && second == 1) return 0b110100;
//      else if (first == 2 && second == 2) return 0b110110;
//      else if (first == 2 && second == 3) return 0b110111;
//
//      else if (first == 3 && second == 0) return 0b111000;
//      else if (first == 3 && second == 1) return 0b111100;
//      else if (first == 3 && second == 2) return 0b111110;
//      else return 0b111111;
//    }
//
//
//
//};
//
//int white = 8; // strike
//int yellow = 9; // hr
//int blue = 10; //triple
//int red = 11; //double
//int green = 12; //single
//
//int num_runs = 0;
//int add_runs = 0;
//int press_type = 0;
//
//Scoreboard myScoreboard(14, 15, 16);
//
//void setup() {
//  //Start Serial for debuging purposes
//  Serial.begin(9600);
//  //set pins to output because they are addressed in the main loop
//
//  pinMode(green, INPUT);
//  pinMode(red, INPUT);
//  pinMode(blue, INPUT);
//  pinMode(yellow, INPUT);
//  pinMode(white, INPUT);
//  Serial.println("Initializing");
//  {
//    Serial.println((String)"Number of Runs: " + num_runs);
//  }
//
//
//}
//
//// random flip
//
//
//byte round_SR;
//byte base_SR;
//int strikes = 0;
//int outs = 0;
//
//int button_curr [5] = {0, 0, 0, 0, 0};
//int button_prev [5] = {0, 0, 0, 0, 0};
//
//byte bases = 0b000;
//
//int press_code( int button_curr[5], int button_prev[5])
//{
//  int code = 0;
//  for (int i = 0; i < 5; i++) {
//    if (!button_prev[i] && button_curr[i])
//      code = i + 1;
//  }
//  return code;
//}
//
//void print_action(int press_type) {
//  switch (press_type) {
//    case 1: Serial.println("---\nSingle");
//
//      break;
//    case 2: Serial.println("---\nDouble");
//      break;
//    case 3: Serial.println("---\nTriple");
//      break;
//    case 4: Serial.println("---\nHome Run");
//      break;
//    case 5: Serial.println("---\nStrike");
//      break;
//    default:
//      break;
//  }
//}
//
//int enable_home(int en) {
//  return 8 * en;
//}
//
//
//int CountRunners(byte bases)
//{
//  return (bases == 0) ? 0 : (bases & 1) + CountRunners(bases >> 1);
//}
//
//void update_bases(byte& bases, int press_type, int& add_runs) {
//  byte new_bases = bases;
//  add_runs = 0;
//  switch (press_type) {
//    case 1: // Single
//      switch (bases) {
//        case 0b000:
//          new_bases = 0b100;
//          break;
//        case 0b001:
//          new_bases = 0b101;
//          break;
//        case 0b100: case 0b010:
//          new_bases = 0b110;
//          break;
//        default:
//          new_bases = 0b111;
//          break;
//      }
//      break;
//
//    case 2: // Double
//      switch (bases) {
//        case 0b000:
//          new_bases = 0b010;
//          break;
//        default:
//          new_bases = 0b011;
//          break;
//      }
//      break;
//
//    case 3: // Triple
//      new_bases = 0b001;
//      break;
//
//    case 4: // HomeRun
//      new_bases = 0b000;
//      break;
//  }
//  if (press_type != 0 && press_type != 5)
//    add_runs = CountRunners(bases) + 1 - CountRunners(new_bases);
//  bases = new_bases;
//}
//
//
//
//
//void loop() {
//
//
//
//  // Data gathering
//
//
//  button_curr[0] = !digitalRead(green);
//  button_curr[1] = !digitalRead(red);
//  button_curr[2] = !digitalRead(blue);
//  button_curr[3] = !digitalRead(yellow);
//  button_curr[4] = !digitalRead(white);
//
//  press_type = press_code(button_curr, button_prev);
//
//  print_action(press_type);
//
//  update_bases(bases, press_type, add_runs);
//  num_runs += add_runs;
//  if (add_runs != 0)
//  {
//    Serial.println((String)"Number of Runs: " + num_runs);
//  }
//
//
//  if (press_type != 0 && press_type != 5) {
//    myScoreboard.updateLEDs(myScoreboard.topLEDSaccum_SO(strikes, outs), bases + enable_home(0));
//    delay(750);
//  }
//
//  myScoreboard.updateLEDs(myScoreboard.topLEDSaccum_SO(strikes, outs), bases + enable_home(1));
//
//
//  for (int i = 0; i < 5; i++) {
//    button_prev[i] = button_curr[i];
//  }
//
//
//
//  delay(50);
//
//}
//
//
//
//
//
//
//
//
//void shiftOut(byte myDataOut) {
//  //internal function setup
//  int i = 0;
//  int pinState;
//
//  //clear everything out just in case to
//  //prepare shift register for bit shifting
//  digitalWrite(dataPin, 0);
//  digitalWrite(clockPin, 0);
//
//  for (i = 7; i >= 0; i--)  {
//    digitalWrite(clockPin, 0);
//    if ( myDataOut & (1 << i) ) {
//      pinState = 1;
//    }
//    else {
//      pinState = 0;
//    }
//
//    //Sets the pin to HIGH or LOW depending on pinState
//    digitalWrite(dataPin, pinState);
//    //register shifts bits on upstroke of clock pin
//    digitalWrite(clockPin, 1);
//    //zero the data pin after shift to prevent bleed through
//    digitalWrite(dataPin, 0);
//  }
//  //stop shifting
//  digitalWrite(clockPin, 0);
//}
