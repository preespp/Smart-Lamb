#include <Adafruit_NeoPixel.h>

#include <LiquidCrystal_I2C.h>

#define PIN 2	 // input pin Neopixel is attached to

#define NUMPIXELS 18 // number of neopixels in strip

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

LiquidCrystal_I2C lcd(0x27,16,2);

const int trigPin = 9;  

const int echoPin = 10; 

const int sensorPin_temp = A0;

float duration, distance;  

// for multi-tread
unsigned long previousTimeNeoPixel= millis();
const long timeIntervalNeoPixel= 500; //checking Neo Pixel Color every 0.1 sec
unsigned long previousTimelcd= millis();
unsigned long previousTimetemp= millis();
unsigned long previousTimesensor= millis();
const long timeInterval= 1000;

int redColor = 0;
int greenColor = 0;
int blueColor = 0;

float temperatureF = 0;

void setup() {
  // Initialize the NeoPixel library.
  pixels.begin();
  
  // Initialize LCD.
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Temperature=");
  lcd.setCursor(12,0);
  lcd.print(temperatureF);
  lcd.setCursor(15,0);
  lcd.print("F");
  
  // Initialize Ultrasonic Sensor
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Initialize serial and neopixel output.
  pinMode(2, OUTPUT);
  Serial.begin(9600);
}

void loop(){

  unsigned long currentTime = millis();

  int reading = analogRead(sensorPin_temp);

  int length;

  // task receive temp input // task change color based on temperature
  if(currentTime - previousTimetemp > timeInterval){
    
  	previousTimetemp = currentTime;
    
    temperatureF = temp_en(reading);
  }

  // task ultrasonic input
  if(currentTime - previousTimesensor > timeInterval){
    
  	previousTimesensor = currentTime;
    
    // need to get color from temperature at the same time
    if (temperatureF > 85) {
    // light blue to make users feel cooler in the hot atmosphere
      
    redColor = 17 ;
    greenColor = 21 ;
    blueColor = 23;
    }
    
    if (temperatureF > 55 && temperatureF <= 85 ) {
    
    // warm white in warm weather
    redColor = 24 ;
    greenColor = 24;
    blueColor = 24;
    }
    
    if (temperatureF <= 55 ) {
    // red tone in the cold weather
      
    redColor = 25;
    greenColor = 16;
    blueColor = 12;
    }
    
    length = dis_sensor(redColor,greenColor,blueColor);
  }

  // task neo pixel
  if(currentTime - previousTimeNeoPixel > timeIntervalNeoPixel) {
    
    previousTimeNeoPixel = currentTime;
    
    show_color();
    }
  
  // task lcd show temp
  if(currentTime - previousTimelcd > timeInterval) {
    
  	previousTimelcd = currentTime;

    //print temperature
    lcd.setCursor(0,0);
    lcd.print("Temperature=");
    lcd.setCursor(12,0);
    lcd.print("   ");
    lcd.setCursor(12,0);
    lcd.print(temperatureF);
    lcd.setCursor(15,0);
    lcd.print("F");
    
    //print distance
    lcd.setCursor(0,1);
    lcd.print("Distance = ");
    lcd.setCursor(11,1);
    lcd.print("     ");

    if (length < 10){ // 1 digits
      lcd.setCursor(13,1);
      lcd.print(length);
    }

    if (length >= 10 && length < 100){  // 2 digits
      lcd.setCursor(12,1);
      lcd.print(length);
    }

    if (length >= 100){ // 3 digits
      lcd.setCursor(11,1);
      lcd.print(length);
    }

    lcd.setCursor(14,1);
    lcd.print("cm");
  }

}

//#######################//
// All Functions in loop //
//#######################//

// receive temperature from tmp36 sensor
float temp_en(int reading){  
  
  float voltage = reading * 5.0;
  
  voltage /= 1024.0; 
  
  float temperatureC = (voltage - 0.5) * 50 ; // test tmp36 with actual room tempeture and adjust equation
  // to make it relevant with room temperature (recommended to use more accurate sensor)
  
  float temperatureF = (temperatureC * 9.0 / 5.0) + 32.0;
  
  Serial.print(temperatureF); 
  Serial.println(" degrees F");
  Serial.print(temperatureC); 
  Serial.println(" degrees C");
  
  return temperatureF;
}

// setColor()
// picks random values to set for RGB
void setColor(int r,int g,int b){
  
  redColor = r;
  
  greenColor = g;
  
  blueColor = b;
}

// show color
void show_color(){
  
	for (int i=0; i < NUMPIXELS; i++) {
    
    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
    pixels.setPixelColor(i, pixels.Color(redColor, greenColor, blueColor));
    // This sends the updated pixel color to the hardware.
    pixels.show();
  }
}


int distanceThreshold = 0;

float cm = 0;

float inches = 0;

// Ultrasonic sensor
long readUltrasonicDistance(int triggerPin, int echoPin){
  
  // Clear the trigger
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  
  // Sets the trigger pin to HIGH state for 10 microseconds
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  
  // Reads the echo pin, and returns the sound wave travel time in microseconds
  float thisduration = pulseIn(echoPin, HIGH);
  return thisduration;
}

int dis_sensor(int redColor,int greenColor,int blueColor)
{
  // set threshold distance to activate neopixel
  // changable + I set it low for the convinence of demonstration 
  distanceThreshold = 82;
  
  // measure the ping time in cm
  long duration = readUltrasonicDistance(trigPin, echoPin);
  cm = 0.0343 * duration / 2;
  
  // convert to inches by dividing by 2.54
  inches = (cm / 2.54);
  
  Serial.print(cm);
  Serial.print("cm, ");
  Serial.print(inches);
  Serial.println("in");

  // condition for setting different levels of brightness
  // feel free to change distance or uncomment any condition
  if (cm > distanceThreshold) {
    setColor(redColor*0,redColor*0,redColor*0);
  }
  if (cm <= distanceThreshold && cm > distanceThreshold - 7) {
	  setColor(redColor*1,greenColor*1,blueColor*1);
  }
  if (cm <= distanceThreshold - 7 && cm > distanceThreshold - 14) {
	  setColor(redColor*2,greenColor*2,blueColor*2);
  }
  if (cm <= distanceThreshold - 14 && cm > distanceThreshold - 21) {
	  setColor(redColor*3,greenColor*3,blueColor*3);
  }
  if (cm <= distanceThreshold - 21 && cm > distanceThreshold - 28) {
	  setColor(redColor*4,greenColor*4,blueColor*4);
  }
  if (cm <= distanceThreshold - 28 && cm > distanceThreshold - 35) {
	  setColor(redColor*5,greenColor*5,blueColor*5);
  }
  if (cm <= distanceThreshold - 35 && cm > distanceThreshold - 42) {
	  setColor(redColor*6,greenColor*6,blueColor*6);
  }
  if (cm <= distanceThreshold - 42 && cm > distanceThreshold - 49) {
	  setColor(redColor*6,greenColor*6,blueColor*6);
  }
  if (cm <= distanceThreshold - 49 && cm > distanceThreshold - 56) {
	  setColor(redColor*7,greenColor*7,blueColor*7);
  }
  if (cm <= distanceThreshold - 56 && cm > distanceThreshold - 63) {
	  setColor(redColor*8,greenColor*8,blueColor*8);
  }
  if (cm <= distanceThreshold - 63 && cm > distanceThreshold - 70) {
	  setColor(redColor*9,greenColor*9,blueColor*9);
  }
  if (cm <= distanceThreshold - 70 && cm > distanceThreshold - 77) {
	  setColor(redColor*10,greenColor*10,blueColor*10);
  }
  //if (cm <= distanceThreshold - 220 && cm > distanceThreshold - 240) {
	//  setColor(redColor*34,greenColor*34,blueColor*34);
  //}
  //if (cm <= distanceThreshold - 240 && cm > distanceThreshold - 260) {
	//  setColor(redColor*37,greenColor*37,blueColor*37);
  //}
  if (cm <= distanceThreshold - 77) {
	  setColor(redColor*11,greenColor*11,blueColor*11);
  }
  
  return cm;
}
