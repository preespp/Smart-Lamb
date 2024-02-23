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
const long timeIntervalNeoPixel= 100; //checking Neo Pixel Color every 0.1 sec
unsigned long previousTimelcd= millis();
unsigned long previousTimetemp= millis();
unsigned long previousTimesensor= millis();
const long timeInterval= 100;

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
    if (temperatureF > 70) {
      
    redColor = 2 ;
    greenColor = 5 ;
    blueColor = 3;
    }
    
    if (temperatureF > 60 && temperatureF <= 70 ) {
      
    redColor = 5 ;
    greenColor = 2;
    blueColor = 3;
    }
    
    if (temperatureF < 60 ) {
      
    redColor = 3;
    greenColor = 2;
    blueColor = 5;
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
  
  float temperatureC = (voltage - 0.5) * 100 ;
  
  float temperatureF = (temperatureC * 9.0 / 5.0) + 32.0;
  
  Serial.print(temperatureF); 
  Serial.println(" degrees F");
  
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
  // set threshold distance to activate LEDs
  distanceThreshold = 265;
  
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
  if (cm > distanceThreshold) {
    setColor(redColor*0,redColor*0,redColor*0);
  }
  if (cm <= distanceThreshold && cm > distanceThreshold - 20) {
	  setColor(redColor*1,greenColor*1,blueColor*1);
  }
  if (cm <= distanceThreshold - 20 && cm > distanceThreshold - 40) {
	  setColor(redColor*4,greenColor*4,blueColor*4);
  }
  if (cm <= distanceThreshold - 40 && cm > distanceThreshold - 60) {
	  setColor(redColor*7,greenColor*7,blueColor*7);
  }
  if (cm <= distanceThreshold - 60 && cm > distanceThreshold - 80) {
	  setColor(redColor*10,greenColor*10,blueColor*10);
  }
  if (cm <= distanceThreshold - 80 && cm > distanceThreshold - 100) {
	  setColor(redColor*13,greenColor*13,blueColor*13);
  }
  if (cm <= distanceThreshold - 100 && cm > distanceThreshold - 120) {
	  setColor(redColor*16,greenColor*16,blueColor*16);
  }
  if (cm <= distanceThreshold - 120 && cm > distanceThreshold - 140) {
	  setColor(redColor*19,greenColor*19,blueColor*19);
  }
  if (cm <= distanceThreshold - 140 && cm > distanceThreshold - 160) {
	  setColor(redColor*22,greenColor*22,blueColor*22);
  }
  if (cm <= distanceThreshold - 160 && cm > distanceThreshold - 180) {
	  setColor(redColor*25,greenColor*25,blueColor*25);
  }
  if (cm <= distanceThreshold - 180 && cm > distanceThreshold - 200) {
	  setColor(redColor*28,greenColor*28,blueColor*28);
  }
  if (cm <= distanceThreshold - 200 && cm > distanceThreshold - 220) {
	  setColor(redColor*31,greenColor*31,blueColor*31);
  }
  if (cm <= distanceThreshold - 220 && cm > distanceThreshold - 240) {
	  setColor(redColor*34,greenColor*34,blueColor*34);
  }
  if (cm <= distanceThreshold - 240 && cm > distanceThreshold - 260) {
	  setColor(redColor*37,greenColor*37,blueColor*37);
  }
  if (cm <= distanceThreshold - 260) {
	  setColor(redColor*40,greenColor*40,blueColor*40);
  }
  
  return cm;
}