#include <Servo.h>

// Define pins
const int trigPin = 2;
const int echoPin = 3;
const int greenLed = 4;
const int redLed = 5;
const int laserPin = 6;
const int buzzerPin = 7;
const int radarServoPin = 9;
const int laserServoPin = 10;

// Variables
Servo radarServo;
Servo laserServo;
long duration;
int distance;
int radarPos = 0;
bool targetLocked = false;
unsigned long previousMillis = 0;
int ledState = LOW;

// Variable to store last valid distance (to filter out zero readings)
int lastValidDistance = 0; 

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  // LEDs and buzzer setup
  pinMode(greenLed, OUTPUT);
  pinMode(redLed, OUTPUT);
  pinMode(laserPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  radarServo.attach(radarServoPin);
  laserServo.attach(laserServoPin);

  // Initial Steady State
  digitalWrite(greenLed, HIGH);
  digitalWrite(redLed, LOW);
  digitalWrite(laserPin, LOW);
  digitalWrite(buzzerPin, LOW);
  
  laserServo.write(90); 
  Serial.begin(9600);
}

void loop() {
  if (!targetLocked) {
    scanArea();
  } else {
    engageTarget();
  }
}

void scanArea() {
  // Green LED ON while scanning
  digitalWrite(greenLed, HIGH);
  digitalWrite(redLed, LOW);
  digitalWrite(laserPin, LOW);

  // Scanning from 0 to 180 degrees
  for (int i = 0; i <= 180; i++) {
    radarServo.write(i);
    delay(30);
    distance = calculateDistance();
    
    // Send data to Processing (Angle, Distance)
    Serial.print(i);
    Serial.print(",");
    Serial.print(distance);
    Serial.println(".");
    
    // Target detected condition (Range < 20cm)
    if (distance > 0 && distance < 20) { 
      targetLocked = true;
      radarPos = i;
      return; 
    }
  }

  // Scanning back from 180 to 0 degrees
  for (int i = 180; i >= 0; i--) {
    radarServo.write(i);
    delay(30);
    distance = calculateDistance();
    
    // Send data to Processing
    Serial.print(i);
    Serial.print(",");
    Serial.print(distance);
    Serial.println(".");

    // Target detected condition
    if (distance > 0 && distance < 20) {
      targetLocked = true;
      radarPos = i;
      return;
    }
  }
}

void engageTarget() {
  // Turn off scanning light
  digitalWrite(greenLed, LOW); 
  
  // >>> PHASE 1: WARNING (تحذير قبل التوجيه) <<<
  // Duration: 1.5 Seconds (تم التقليل لثانية ونصف)
  unsigned long startWarning = millis();
  
  while (millis() - startWarning < 1500) {
    // 1. Check if target left (Escape opportunity)
    distance = calculateDistance();
    if (distance > 20) {
        delay(50); 
        distance = calculateDistance(); 
        if (distance > 20 && distance != 0) {
           resetSystem(); // Target left, return to scanning
           return;
        }
    }
    
    // 2. Warning Alarm (Siren sound + Red Blink)
    // Buzzer works here as a WARNING SIREN
    digitalWrite(redLed, HIGH);
    tone(buzzerPin, 700); // Constant tone (Siren beep)
    delay(200);
    
    digitalWrite(redLed, LOW);
    noTone(buzzerPin);
    delay(200);
  }

  // >>> PHASE 2: ENGAGEMENT (الهجوم) <<<
  // Target didn't leave, now we attack
  
  laserServo.write(radarPos);   // Move laser to target NOW
  digitalWrite(laserPin, HIGH); // Turn Laser ON
  
  int missedReadings = 0; 
  
  // Continuous Fire Loop
  while (true) {
    distance = calculateDistance();
    
    // Check if target is still there (with error tolerance)
    if (distance > 20) {
       missedReadings++; 
    } else {
       missedReadings = 0; 
    }

    if (missedReadings > 5) {
       break; // Target eliminated/left
    }

    // Independent Red LED Control (Fast blink for action)
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= 100) { 
      previousMillis = currentMillis;
      ledState = !ledState;
      digitalWrite(redLed, ledState);
    }

    // Firing Sound Effect (Pew Pew)
    // Buzzer works here as GUNFIRE
    for (int k = 1000; k > 500; k -= 100) { 
        tone(buzzerPin, k);
        delay(10);
    }
    noTone(buzzerPin);
    delay(50); 
  }

  // Target lost: Reset system
  digitalWrite(laserPin, LOW); 
  resetSystem(); 
}

void resetSystem() {
  targetLocked = false;
  digitalWrite(greenLed, HIGH); 
  digitalWrite(redLed, LOW);
  digitalWrite(laserPin, LOW);
  noTone(buzzerPin);
  laserServo.write(90);
}

// Anti-Glitch Distance Calculation Function
int calculateDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  
  int currentDist = duration * 0.034 / 2;
  
  if (currentDist == 0) {
    return lastValidDistance;
  } else {
    lastValidDistance = currentDist;
    return currentDist;
  }
}
