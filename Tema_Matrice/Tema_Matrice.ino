#include "LedControl.h" //  need the library
#include <LiquidCrystal.h>
#include <time.h>
#include <EEPROM.h>

#define CLK_PIN 11
#define MAX7219_1 12
#define LOAD_PIN 10
#define NR_DRIVER 1
#define POT_PIN A0
#define BUTTON_PIN 8
#define INITIAL_BOARDLENGHT 4
#define LIMIT_LINE 6
#define START_INDEX 0
#define SCREEN_DIMENSION 9
#define SPEED_INCREASE_NUMBER 5
#define LENGHT_DECREASE_NUMBER 10
#define MARGIN 8
#define START 0
#define SPEED_STEP 100

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
 int highScore;
 int playerPosition;
 int boardLenght; // the lenght of the catch block, automatically gets shorter every 10 points
 int score;
 long long lastMove;
 int playScreen [SCREEN_DIMENSION][SCREEN_DIMENSION];
 int SPEED;
 

 bool startScreen;
 bool firstTime;
void setup()
{
  // the zero refers to the MAX7219 number, it is zero for 1 chip
  
  lc.shutdown(0, false); // turn off power saving, enables display
  lc.setIntensity(0, 2); // sets brightness (0~15 possible values)
  lc.clearDisplay(0);// clear screen  
  pinMode(BUTTON_PIN, INPUT);
  
  ballLine = playerPosition = score = 0;
  srand( time(NULL) );
  ballColumn = rand() % LIMIT_LINE + 1;
 
  SPEED = 1000;
  for (int i = START; i < SCREEN_DIMENSION; i++)
  for (int j = START; j < SCREEN_DIMENSION; j++)
   playScreen[i][j] = 0;

   
  
   pinMode(PWM, OUTPUT);
   analogWrite(PWM, 95);
   
   lcd.begin (16, 2) ;
   lcd.clear();
   lcd.setCursor(0, 0);
   
  boardLenght = INITIAL_BOARDLENGHT;
  startScreen = true;
  firstTime = true;
  highScore = EEPROM.read(START_INDEX); 
}


void reviveBall () 
   {
      ballLine = 0;
      ballColumn = rand() % LIMIT_LINE + 1;
   }
        
void ballCatched () 
   {
      score++;
      if (score % SPEED_INCREASE_NUMBER == 0) SPEED -= SPEED_STEP;
      if (score % LENGHT_DECREASE_NUMBER == 0) boardLenght--;
      reviveBall();
      
    }
    
void moveBall()
   {
        ballLine++;    
   }   


   
void endGame()
  {
    if (score > highScore ) 
    {
      highScore = score; 
      EEPROM.write (0, score);
    }
    lc.clearDisplay(0);
    
    startScreen = true;
    initialiseScreenAndValues();
   
  }
  
void setPlayerOnMap (int playerPosition)
  {
    playScreen[LIMIT_LINE][playerPosition + boardLenght] = playScreen[LIMIT_LINE][playerPosition] = 1; //setez marginile cutiei 
    int i;
    for (i = playerPosition; i <= playerPosition + boardLenght; i++)
    playScreen[LIMIT_LINE+1][i] = 1; //setez fundul cutiei
   }
  
void setPlayerPosition()
  {
    int sensorValue = analogRead(POT_PIN);
    playerPosition = map (sensorValue, 0, 1025, 0, MARGIN-boardLenght);
    
  }

void clearMap()
  {
    for (int i = START; i < MARGIN; i++)
    for (int j = START; j < MARGIN; j++)
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
      {  
        if (firstTime == true)
        { 
          lc.clearDisplay(0);
          lcd.setCursor(0,0);
          lcd.print((String)"Last Score : " + score); 
          lcd.setCursor(0,1);
          lcd.print("Press the button to continue");   
          firstTime = false;
        }         
        if (digitalRead(BUTTON_PIN) == HIGH) //if the reset button is pressed, the game starts 
        { 
          initialiseScreenAndValues();
          startScreen = false;
          firstTime = true;
          score = 0;
        }
             
      }
    else
      {
        clearMap();
        setPlayerPosition(); // maps the input to the x-axis coordonates 
        setPlayerOnMap(playerPosition); // implements the logic for the crate based on the playerposition
         
        if (millis() - lastMove >= SPEED)
        { 
           // speed sets up the time when the ball is droping 1 unit. Accelerates every 5  points
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
      int i;
      for (i = START; i < MARGIN; i++)
        {
          lc.setRow(0, i, character[i]);
        }
    }


    
void draw ()
    {
      lc.clearDisplay(0);
    for (int row = START; row < MARGIN; row++)
      for (int col = START; col < MARGIN; col++)
         if (playScreen[col][row] == 1)
            lc.setLed(0, col, row, true);
    }
