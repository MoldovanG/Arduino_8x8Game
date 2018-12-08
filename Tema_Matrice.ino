#include "LedControl.h" //  need the library
#include <LiquidCrystal.h>
#include <time.h>

#define CLK_PIN 11
#define MAX7219_1 12
#define LOAD_PIN 10
#define NR_DRIVER 1
#define POT_PIN A0
#define BUTTON_PIN 8
#define INITIAL_BOARDLENGHT 4
#define LIMIT_LINE 6

#define RS 2
#define E 3
#define D4 4
#define D5 5
#define D6 6
#define D7 7
#define PWM 9

LiquidCrystal lcd (RS,E,D4,D5,D6,D7);
LedControl lc = LedControl( MAX7219_1, CLK_PIN, LOAD_PIN , NR_DRIVER ); //DIN, CLK, LOAD, No. DRIVER
 
// pin 12 is connected to the MAX7219 pin 1
// pin 11 is connected to the CLK pin 13
// pin 10 is connected to LOAD pin 12
// 1 as we are only using 1 MAX7219

 int ballLine, ballColumn;

 int playerPosition;
 int boardLenght; // lungimea navetei de prindere se modifica pentru a creste dificultatea
 int score;
 long long lastMove;
 int playScreen [9][9];
 int SPEED;
 
 byte number_0_Sad[] = { B00100100,B00100100,B00000000,B00011000,B00000000,B00011000,B00100100,B01000010};
 byte number_1[] = { B00000000,B00011000,B00111000,B01111000,B00011000,B00011000,B00011000,B00011000};
 byte number_2[] =   { B00111110,B00111110,B00000110,B00111110,B00110000,B00110000,B00111110,B00111110};
 byte number_3[] =  { B00000000,B00111100,B00111100,B00001100,B00111100,B00001100,B00111100,B00111100};
 byte number_4 [] = { B01100110,B01100110,B01100110,B01111110,B00000110,B00000110,B00000110,B00000110};
 byte number_5 [] = { B00000000,B01111110,B01111110,B01100000,B01111110,B00000110,B01111110,B01111110};
 byte number_6 [] = { B00111110,B00111110,B00110000,B00110000,B00111110,B00111110,B00110110,B00111110};
 byte number_7 [] = { B00011110,B00011110,B00000110,B00000110,B00011111,B00000110,B00000110,B00000110};
 byte number_8 [] = { B00111110,B00100010,B00100010,B00111110,B00111110,B00100010,B00100010,B00111110};
 byte number_9 [] = { B01111000,B01001000,B01001000,B01111000,B00001000,B00001000,B00001000,B00001000};
 byte number_10 [] = { B00100000,B01101110,B10101010,B00101010,B00101010,B00101110,B00000000,B00000000};
 byte numberG_10_Smile [] = { B00100100,B00100100,B00000000,B00011000,B00000000,B01000010,B00100100,B00011000};
 byte numberG_10_Amazed [] = { B00100100,B00100100,B00000000,B00011000,B00000000,B00011000,B00100100,B00011000};

 bool startScreen;
void setup()
{
  // the zero refers to the MAX7219 number, it is zero for 1 chip
  
  lc.shutdown(0 , false); // turn off power saving, enables display
  lc.setIntensity(0 , 2); // sets brightness (0~15 possible values)
  lc.clearDisplay(0);// clear screen  
  pinMode(BUTTON_PIN, INPUT);
  
  ballLine = playerPosition = score = 0;
  srand( time(NULL) );
  ballColumn = rand() % LIMIT_LINE + 1;
 
  SPEED = 1000;
  for (int i = 0 ; i < 8 ; i++)
  for (int j = 0 ; j<9 ; j++)
   playScreen[i][j] = 0;

   lcd.begin (16 , 2) ;
  lcd.clear();
  lcd.setCursor(0 , 0);
  
   pinMode(PWM , OUTPUT);
   analogWrite(PWM , 99);

  boardLenght = INITIAL_BOARDLENGHT;
  startScreen = true;
}




  void reviveBall () {
        ballLine=0;
        ballColumn = rand() % LIMIT_LINE + 1;
     }
        
  void ballCatched () {
      score++;
      if (score % 5 == 0) SPEED -= 100;
      if (score % 10 == 0) boardLenght--;
      reviveBall();
      
    }
    
  void moveBall(){
        ballLine++;    
    }   

   void pause (int time)
   {
    int lastUpdate=millis();
    while (millis()-lastUpdate<time);
      
   }
   
  void endGame()
  {
    lc.clearDisplay(0);
    switch (score){
    case 0: {
                  printByte(number_0_Sad);
                  break;
               }
    case 1: {
                printByte(number_1);
                 break;
               }
    case 2: {
                printByte(number_2);
                 break;
               }
    case 3 : {
                printByte(number_3);
                 break;
               }
    case 4 : {
                printByte(number_4);
                 break;
               }
    case 5 : {
                printByte(number_5);
                 break;
               }
    case 6 : {
                printByte(number_6);
                 break;
               }
    case 7 : {
                printByte(number_7);
                 break;
               }
    case 8 : {
                printByte(number_8);
                 break;
               }
    case 9 : {
                printByte(number_9);
                 break;
               }
    case 10 : {
                printByte(number_10);
                 break;
               }
                                                
    
    default :{
      int counter = 0;
      while (counter < 5) {
      printByte(numberG_10_Smile);
      
      pause(500);
     
      lc.clearDisplay(0);
      printByte(numberG_10_Amazed);
      
      pause(500);
 
      counter++;
        }
      }
    }
    startScreen = true;
    initialiseScreenAndValues();
    pause(1000);
   
  }
  
void setPlayerOnMap (int playerPosition)
  {
    playScreen[LIMIT_LINE][playerPosition + boardLenght] = playScreen[LIMIT_LINE][playerPosition] = 1; //setez marginile cutiei 
    int i;
    for (i = playerPosition ; i <= playerPosition + boardLenght ; i++)
    playScreen[LIMIT_LINE+1][i] = 1; //setez fundul cutiei
   
  }
  
void setPlayerPosition()
  {
  int sensorValue = analogRead(POT_PIN);
    playerPosition = map (sensorValue, 0, 1025, 0, 8-boardLenght);
    
  }

void clearMap()
  {
    for (int i = 0 ; i < 8 ; i++)
    for (int j = 0 ; j < 8 ; j++)
    playScreen[i][j] = 0;
  }
  
void initialiseScreenAndValues ()
  {
    clearMap();
    lc.clearDisplay(0);
    reviveBall();
    boardLenght = INITIAL_BOARDLENGHT;   
  }
void loop()
  {
    if (startScreen == true)
      { lc.clearDisplay(0);
        lcd.setCursor(0,0);
        lcd.print("Your last score :" + score ); 
        lcd.setCursor(0,1);
        lcd.print("Press the button to continue");      
        if (digitalRead(BUTTON_PIN) == HIGH)
        { initialiseScreenAndValues();
          startScreen=false;
         score = 0;
        }
        //meniul de start screen, asteapta sa fie implementat      
      }
    else
      {
      clearMap();
      setPlayerPosition();
      setPlayerOnMap(playerPosition);
       
      if (millis() - lastMove >= SPEED){
        lastMove = millis();
        
        moveBall();
        if (ballLine == LIMIT_LINE && (ballColumn > playerPosition && ballColumn < playerPosition + boardLenght))
            ballCatched();
        else if (ballLine >= LIMIT_LINE) endGame();
      }
      playScreen[ballLine][ballColumn] = 1;
      draw();
      }
  }
  
void printByte (byte character [])
    {
      int i = 0;
      for (i = 0 ; i < 8 ; i++)
        {
          lc.setRow(0, i, character[i]);
        }
    }


    
void draw ()
    {
      lc.clearDisplay(0);
    for (int row = 0; row < 8 ; row++)
      for (int col = 0; col < 8 ; col++)
         if (playScreen[col][row] == 1)
            lc.setLed(0, col, row, true);
    }
