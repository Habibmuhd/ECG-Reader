#include <Adafruit_GFX.h> // Core graphics library
#include <MCUFRIEND_kbv.h> // Hardware-specific library
#include <SPI.h>

#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0
#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin

#define BLACK 0x0000
#define BLUE 0x001F
#define RED 0xF800
#define GREEN 0x07E0
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define YELLOW 0xFFE0
#define WHITE 0xFFFF

MCUFRIEND_kbv tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

//for graphing
int y1[315]; //Build an array with 312 y values
int x1;
unsigned long interval = 35; // 35ms wait
unsigned long previousgraphmillis = 0;
unsigned long delta = 0;

//for bpm calculation

int threshold = 600;
int counter = 0;
int bpm = 0;
long offset = 0;
long int previousMillis = 0;
long int timediff;
int middlevalue = 0;
int lastvalue = 0;
float BPM;


void setup()
{
    Serial.begin(9600);
    Serial.println(F("Using Adafruit 2.8\" TFT Breakout Board Pinout"));

    Serial.print("TFT size is ");
    Serial.print(tft.width());
    Serial.print("x");
    Serial.println(tft.height());

    tft.reset();

    uint16_t identifier = tft.readID();

    tft.begin(identifier);
    tft.setRotation(1);
    tft.fillScreen(BLACK); //  clear screen

    tft.drawRect(2, 2, 315, 130, MAGENTA);
    tft.drawLine(1, 2, 1, 130, MAGENTA);
    tft.drawLine(315, 2, 315, 130, MAGENTA); //make the borders thicker

    tft.setTextColor(WHITE);
    tft.setTextSize(2);
    tft.setCursor(80, 10);
    tft.println("   BitPulse");
   

    tft.drawLine(3, 30, tft.width() - 5, 30, MAGENTA); //line below bit pulse
    tft.drawLine(3, 31, tft.width() - 5, 31, MAGENTA); //line below bit pulse
    tft.drawLine(3, 32, tft.width() - 5, 32, MAGENTA); //line below bit pulse
    tft.drawLine(3, 128, tft.width() - 5, 128, MAGENTA);
    tft.drawLine(3, 129, tft.width() - 5, 129, MAGENTA);
    tft.drawLine(3, 130, tft.width() - 5, 130, MAGENTA);

    tft.setTextColor(YELLOW);
    tft.setTextSize(2);
    tft.setCursor(7, 40);
    tft.println(" PULSE ");

    tft.drawLine(90, 30, 90, 130, MAGENTA); // make line separating bpm & brady thicker
    tft.drawLine(91, 30, 91, 130, MAGENTA);
    tft.drawLine(92, 30, 92, 130, MAGENTA);  
  
  tft.setCursor(7, 100);
    tft.setTextColor(YELLOW);
    tft.print(" BPM ");

    tft.setCursor(95, 40);
    tft.setTextSize(2);
    tft.setTextColor(YELLOW);
    tft.println(" STATUS: ");

    tft.setCursor(95, 70);
    tft.setTextSize(2);
    tft.setTextColor(YELLOW);
    tft.println(" BRADYCARDIA: ");

    tft.setCursor(95, 100);
    tft.setTextSize(2);
    tft.setTextColor(YELLOW);
    tft.println(" TACHYCARDIA: ");

    
   
}

void loop()
{

    tft.drawLine(1, 130, 1, 235, MAGENTA);//lines beside graphing. need to loop
    tft.drawLine(2, 130, 2, 235, MAGENTA);
    tft.drawLine(315, 130, 315, 235, MAGENTA);
    tft.drawLine(316, 130, 316, 235, MAGENTA);
    tft.drawLine(2, 235, 316, 235, MAGENTA);
    tft.drawLine(2, 236, 316, 236, MAGENTA);
     
   
      while (x1 < 315)
      
     {       
        y1[x1] = analogRead(A5);
            Serial.println(x1); // print the results to the Serial Monitor:
            Serial.println(y1[x1]);
            y1[x1] = map(y1[x1], 0, 1023, 0, -100); //range from 0 to -250 to get a positive graph,small graph so reduce to 100
            tft.drawLine(x1 + 2, y1[x1] + 200, (x1 + 1), y1[x1 + 1] + 200, GREEN); //change 240 to a higher value to bring graph down
             tft.fillRect(x1 + 5, 133, 10, 102, BLACK);
            x1++;
      
      int currentvalue = analogRead(A5); //Reads the sensor input

                if ((currentvalue > middlevalue) && (currentvalue >= threshold))  //If the sensor value obtained is greater than the middle value
    
               {
                  lastvalue = middlevalue;
                  middlevalue = currentvalue; 

               }
         
          delta = millis() - offset;   //delta is difference in time interval
        
        if (delta >= 0 && delta < 10000)
              
        {
      
            if ((currentvalue < middlevalue) && (middlevalue >= threshold) && (middlevalue > lastvalue)) //from line 120
                
            {
              counter ++ ;
              middlevalue=0;
              lastvalue=0;
            }
        }
            
        if (delta >= 10000) 
        
        {
         int bpm =  (counter * 6); // Compute bpm
         Serial.print("bpm ");
         float BPM = ( bpm * 1);
        

           tft.fillRect (4,70, 86, 20 , BLACK); // FOR PULSE REFRESH SCREEN
          tft.setCursor(20, 70);
          tft.setTextSize(2);
          tft.setTextColor(WHITE);
          tft.print(BPM , 2 );

          counter = 0;
          offset = millis(); // Reset Counter

                if (bpm < 60)

                {
 

                  tft.fillRect (254,69, 50, 20 , BLACK); // FOR BRADY
                  tft.setCursor(255, 70);
                  tft.setTextSize(2);
                  tft.setTextColor(RED);
                  tft.println(" YES ");

                 tft.fillRect (254,99, 50, 20 , BLACK); // FOR TACHY
                  tft.setCursor(255, 100);
                  tft.setTextSize(2);
                  tft.setTextColor(GREEN);
                  tft.println(" NO ");

                  tft.fillRect (194,39, 110, 20 , BLACK); // FOR STATUS
                  tft.setCursor(195, 40); // for STATUS
                  tft.setTextSize(2);
                  tft.setTextColor(RED);
                  tft.println("SEEK HELP ");
                }
  
                if (bpm > 100)

                {

                  tft.fillRect (254,99, 50, 20 , BLACK); // FOR TACHY
                  tft.setCursor(255, 100);
                  tft.setTextSize(2);
                  tft.setTextColor(RED);
                  tft.println(" YES ");

                  tft.fillRect (254,69, 50, 20 , BLACK); // FOR BRADY
                  tft.setCursor(255, 70);
                  tft.setTextSize(2);
                  tft.setTextColor(GREEN);
                  tft.println(" NO ");
                  
                  tft.fillRect (194,39, 110, 20 , BLACK); // FOR STATUS
                  tft.setCursor(195, 40); // for STATUS
                  tft.setTextSize(2);
                  tft.setTextColor(RED);
                  tft.println("SEEK HELP ");
                  
                }

                if (bpm >= 60 && bpm <= 100)

                {
                  tft.fillRect (254,69, 50, 20 , BLACK); // FOR BRADY
                  tft.setCursor(255, 70);
                  tft.setTextSize(2);
                  tft.setTextColor(GREEN);
                  tft.println(" NO ");
                  
                  tft.fillRect (254,99, 50, 20 , BLACK); // FOR TACHY
                  tft.setCursor(255, 100);
                  tft.setTextSize(2);
                  tft.setTextColor(GREEN);
                  tft.println(" NO ");

                  tft.fillRect (194,39, 110, 20 , BLACK); // FOR STATUS
                  tft.setCursor(210, 40); // for STATUS
                  tft.setTextSize(2);
                  tft.setTextColor(GREEN);
                  tft.println("FINE ");
                }
    
                offset = millis();
                
        }
      delay (35);
      }
          x1 = 0;
    }
