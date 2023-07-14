/*****************************
  - Temperature Sensor [Header] -
*****************************/

#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
boolean isClear = true;


#define temperature 5
int Alarm = 4;
int relay = 3;
OneWire oneWire(temperature);

float DO;
double NH3;

DallasTemperature sensors(&oneWire);

float Celsius = 0;
float Fahrenheit = 0;

/***************************
  - pH Sensor [Header] -
***************************/

const int analogInPin = A0;
int sensorValue = 0;
unsigned long int avgValue;
float b;
int buf[10], temp;
float phValue;

/**************************
  DHT22 - Moisture Sensor
**************************/

#include <DHT22.h>
#define data 6  // D4 [DHT22 Port]

DHT22 dht22(data);
float t,h;

unsigned long currentTime = millis();
#define Timer (millis() - currentTime)
#define changeTime 5000  // เวลาที่ใช้ในการเปลี่ยนหน้าจอ 5 วินาที


/****************************
  - Main Start -
****************************/

void tempCompute() {
  sensors.requestTemperatures();

  Celsius = sensors.getTempCByIndex(0);
  Fahrenheit = sensors.toFahrenheit(Celsius);
}
void tempShow() {
  /****************************
    - Temperatures Loop Start -
  ****************************/
  lcd.setCursor(3, 0);
  lcd.print("Temperature");
  lcd.setCursor(0, 1);
  lcd.print("C:");
  lcd.print(Celsius);
  lcd.setCursor(9, 1);
  lcd.print("F:");
  lcd.print(Fahrenheit);
  /****************************
    - Temperatures Loop End -
  ****************************/
}
void pHCompute() {
  for (int i = 0; i < 10; i++) {
    buf[i] = analogRead(analogInPin);
    delay(10);
  }
  for (int i = 0; i < 9; i++) {
    for (int j = i + 1; j < 10; j++) {
      if (buf[i] > buf[j]) {
        temp = buf[i];
        buf[i] = buf[j];
        buf[j] = temp;
      }
    }
  }
  avgValue = 0;
  for (int i = 2; i < 8; i++)
    avgValue += buf[i];
  float pHVol = (float)avgValue * 5.0 / 1024 / 6;
  phValue = -5.70 * pHVol + 21.34;
}
void pHShow() {
  /************************
    - pH Loop Start -
  ************************/
  lcd.setCursor(1, 0);
  lcd.print("Potenz Hydrogen");
  lcd.setCursor(3, 1);
  lcd.print("pH = ");
  lcd.print(phValue);
  /***************************
    - pH Loop Ending -
  ***************************/
}
void dht22Compute() {
  t = dht22.getTemperature();
  h = dht22.getHumidity();
}
void dht22Show() {
  /***************************
    - DHT22 Loop Start
  ***************************/

  lcd.setCursor(0, 0);
  lcd.print("Temp & Humidity");
  lcd.setCursor(0, 1);
  lcd.print("T:");
  lcd.print(t);
  lcd.setCursor(9, 1);
  lcd.print("H:");
  lcd.print(h);

  /**************************
    - DHT22 Loop End
  **************************/
}

/****************************
  - Function For Condition -
****************************/

/************************
  - pH 0-14
  - ประมาณ 5-7 น้ำเปล่า
  - > 7 มีความบริสุทธิ์สูง เช่น น้ำกลั่น
  - < 5 มีความเป็นกรดสูง เช่น น้ำโค้ก น้ำอัดลม น้ำมะนาว
************************/

// มากกว่าเท่าไหร่ ถึงจะให้ Alarm()
#define phValuePeek 7 //<<--///แกค่า"pH"ตรงนี้

void Condition_pH(float phValue) {
  if (phValue > phValuePeek) {
    alarm();
  }
}

void alarm() {
  Serial.println(" Beep !!!!");
  digitalWrite(Alarm, HIGH);
}

/************************

 - DO System

*************************/
void ComputeDO() {
  if (Celsius >= 15 && Celsius <= 18) {
    DO = 9.75; // mg./L
  }
  else if (Celsius >= 18 && Celsius <= 20) {
    DO = 9.25; // mg./L
  }
  else if (Celsius >= 20 && Celsius <= 23) {
    DO = 8.85; // mg./L
  }
  else if (Celsius >= 23 && Celsius <= 25) {
    DO = 8.45; // mg./L
  }
  else if (Celsius >= 25 && Celsius <= 28) {
    DO = 8.05; // mg./L
  }
  else if (Celsius >= 28 && Celsius <= 30) {
    DO = 7.70; // mg./L
  }
  else if (Celsius >= 30 && Celsius <= 32) {
    DO = 7.30; // mg./L
  }
}

void Condition_DO() {
  if (DO >= 8.00) {
    offWaterPump();
  }
  else if (DO <= 4.00) {
    onWaterPump();
  }
}

void onWaterPump() {
  // สั่งเปิดปั้มน้ำ
}

void offWaterPump() {
 // สั่งปิดปั้มน้ำ
}

void DOShow() {
  lcd.setCursor(4, 0);
  lcd.print("DO Value");
  lcd.setCursor(3, 1);
  lcd.print("DO = ");
  lcd.print(DO);
}

/************************

 - NH3 System

*************************/
void ComputeNH3() {
  if (Celsius >= 20 && Celsius <= 25) {
    if (phValue >= 5 && phValue <= 6)
    {
      NH3 = 0.0;
    }
    else if (phValue >= 6 && phValue <= 7)
    {
      NH3 = 0.0002;
    }
    else if (phValue >= 7 && phValue <= 8)
    {
      NH3 = 0.002;
    }
    else if (phValue >= 8 && phValue <= 9)
    {
      NH3 = 0.0191;
    }
    else if (phValue >= 9 && phValue <= 9.5)
    {
      NH3 = 0.09;
    }
    else if (phValue >= 9.5)
    {
      NH3 = 0.112;
    }
  }
  else if (Celsius >= 25 && Celsius <= 28) {
    if (phValue >= 5 && phValue <= 6)
    {
      NH3 = 0.0;
    }
    else if (phValue >= 6 && phValue <= 7)
    {
      NH3 = 0.0003;
    }
    else if (phValue >= 7 && phValue <= 8)
    {
      NH3 = 0.0028;
    }
    else if (phValue >= 8 && phValue <= 9)
    {
      NH3 = 0.0269;
    }
    else if (phValue >= 9 && phValue <= 9.5)
    {
      NH3 = 0.124;
    }
    else if (phValue >= 9.5)
    {
      NH3 = 0.146;
    }
  }
  else if (Celsius >= 28 && Celsius <= 32) {
    if (phValue >= 5 && phValue <= 6)
    {
      NH3 = 0.0;
    }
    else if (phValue >= 6 && phValue <= 7)
    {
      NH3 = 0.0004;
    }
    else if (phValue >= 7 && phValue <= 8)
    {
      NH3 = 0.004;
    }
    else if (phValue >= 8 && phValue <= 9)
    {
      NH3 = 0.0124;
    }
    else if (phValue >= 9 && phValue <= 9.5)
    {
      NH3 = 0.181;
    }
    else if (phValue >= 9.5)
    {
      NH3 = 0.268;
    }
  }
  else if (Celsius >= 32 && Celsius <= 34) {
    if (phValue >= 5 && phValue <= 6)
    {
      NH3 = 0.0;
    }
    else if (phValue >= 6 && phValue <= 7)
    {
      NH3 = 0.0004;
    }
    else if (phValue >= 7 && phValue <= 8)
    {
      NH3 = 0.0124;
    }
    else if (phValue >= 8 && phValue <= 9)
    {
      NH3 = 0.0764;
    }
    else if (phValue >= 9 && phValue <= 9.5)
    {
      NH3 = 0.191;
    }
    else if (phValue >= 9.5)
    {
      NH3 = 0.336;
    }
  }
  else if (Celsius >= 34) {
    if (phValue >= 5 && phValue <= 6)
    {
      NH3 = 0.0;
    }
    else if (phValue >= 6 && phValue <= 7)
    {
      NH3 = 0.0005;
    }
    else if (phValue >= 7 && phValue <= 8)
    {
      NH3 = 0.0283;
    }
    else if (phValue >= 8 && phValue <= 9)
    {
      NH3 = 0.0885;
    }
    else if (phValue >= 9 && phValue <= 9.5)
    {
      NH3 = 0.269;
    }
    else if (phValue >= 9.5)
    {
      NH3 = 0.459;
    }
  }
}

void NH3Show() {
  lcd.setCursor(4, 0);
  lcd.print("NH3 Value");
  lcd.setCursor(3, 1);
  lcd.print("NH3 = ");
  lcd.print(NH3);
}

/****************************
  - Ending Code Document -
****************************/
void setup() {
  sensors.begin();
  Serial.begin(9600);
  pinMode(Alarm, OUTPUT);
  pinMode(relay, OUTPUT);
  digitalWrite(Alarm, LOW);
  digitalWrite(relay, LOW);
  delay(10);
  lcd.begin();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Welcome to");
  lcd.setCursor(2, 1);
  lcd.print("My project");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("By Friend Robot");
  delay(1500);
  lcd.clear();
  digitalWrite(Alarm, HIGH);
  delay(700);
  digitalWrite(Alarm, LOW);

  currentTime = millis();  // เริ่มต้น จับเวลา
  isClear=true;
}

void loop() {
  Serial.print("Time: ");
  Serial.println(Timer);
  /*************
  - เริ่มการเก็บค่า
  *************/
  tempCompute();
  pHCompute();
  dht22Compute();

  ComputeDO(); // หาค่า DO
  ComputeNH3(); // หาค่า NH3

  /*************
  - เริ่มต้นตรรกะ
  *************/
  Condition_pH(phValue); // ฟังชั่น ตรวจจับค่า pH

  Condition_DO(); // ฟังชั่น ตรวจจับค่า DO

  /*************
  - แสดงผล
  *************/
  if (Timer > (changeTime * 6.5 )) {
    currentTime = millis();  // Reset การจับเวลา
    isClear=true;
  }
  else if (Timer > (changeTime * 5 )) {
    // currentTime = millis();  // Reset การจับเวลา
    isClear=true;
  }
  else if (Timer > (changeTime * 3)) {
    isClear=true;
  }
  else if (Timer > (changeTime)) {
    isClear=true;
  }

  if (Timer > (changeTime * 6 )) {
    if (isClear){
      lcd.clear();
      isClear=false;
    }
    // Show NH3;
    NH3Show();
    delay(500);
  }
  else if (Timer > (changeTime * 4 )) {
    if (isClear){
      lcd.clear();
      isClear=false;
    }
    // Show DO;
    DOShow();
    delay(500);
  }
  else if (Timer > (changeTime * 2 )) {
    if (isClear){
      lcd.clear();
      isClear=false;
    }
    dht22Show();
    delay(500);
  } else if (Timer > (changeTime )) {
    if (isClear){
      lcd.clear();
      isClear=false;
    }
    pHShow();
    delay(500);
  }
  else if (Timer > 0) {
    if (isClear){
      lcd.clear();
      isClear=false;
    }
    tempShow();
    delay(500);
  }
}

/*****************************
  - Main End -
*****************************/