// #include <DallasTemperature.h>
// #include <OneWire.h>
#include <GravityTDS.h>

GravityTDS gravityTds;

// pinout untuk masing-masing sensor
// A0 Po (PH)
// A1 To (Temp-Ph : Temp)
// A2 Tur (Turbidity : Percentage)
// A3 Tds (Tds : PPM)
const int AnalogPh = A0;
const int AnalogTemp = A1;         
const int AnalogTurb = A2;   
const int AnalogTDS = A3;        

// OneWire oneWire(AnalogTe);
// DallasTemperature sensors(&oneWire); 

void setup(){
  // baud rate maksimum u/ connect dgn esp8266
  Serial.begin(9600); 

  // set-up tds
  gravityTds.setPin(AnalogTDS);
  gravityTds.setAref(5.0); // ref voltage device
  gravityTds.setAdcRange(1024); // resolusi data
  gravityTds.begin(); // init

  // set-up Ph
  pinMode(AnalogPh,INPUT);
}

void loop(){
  // Serial.print("PH: ");
  // Serial.print(get_ph_reading());
  // Serial.print(" Temp: ");
  // Serial.print(get_temp_reading());
  // Serial.print(" TDS: ");
  // Serial.print(get_tds_reading());
  // Serial.print(" TUR: ");
  // Serial.println(get_turb_reading());
  Serial.print(get_ph_reading());
  Serial.print("#");
  Serial.print(get_temp_reading());
  Serial.print("#");
  Serial.print(get_tds_reading());
  Serial.print("#");
  Serial.println(get_turb_reading());
  delay(5000);
}

// https://cimpleo.com/blog/arduino-ph-meter-using-ph-4502c/
// kalibrasi : https://raaflahar.medium.com/ph-4502c-sensor-diymore-how-to-use-and-calibrate-using-arduino-uno-r3-3afc2b96631
float phCalibration4 = 3.1; 
float phCalibration7 = 16.6;
float get_ph_reading(){
  int analog_ph_reading = analogRead(AnalogPh); // deklarasikan analog_ph_reading
  float TeganganPh = 5 / 1024.0 * analog_ph_reading; // deklarasikan TeganganPh

  // hitung nilai pH menggunakan tegangan yang diukur
  float pH = 7.0 + ((2.5 - TeganganPh) / (phCalibration7 - phCalibration4)) * (7.0 - 4.0);
  return pH;
}

float get_temp_reading(){
  // implementasi sensor 
  float analog_temp = analogRead(AnalogTemp);
  float voltage = analog_temp * (5.0 / 1024.0);
  return voltage;
}
// https://wiki.dfrobot.com/Turbidity_sensor_SKU__SEN0189
float get_turb_reading(){
  // pakai mode analog
  // rumus : baca pin * (voltase device (3.3 esp, 5.0 ardduino) / 1024.0)
  // voltage = analogRead(pin) * (5.0 / 1024.0)
  // karena mau baca persentase, anggap saja
  return 100.0 - analogRead(AnalogTurb) * (5.0 / 1024.0);
}

float get_tds_reading(){
  // gravityTds.setTemperature(get_temp_reading());
  // gravityTds.setTemperature(24.0);
  gravityTds.update();
  float tdsValue = gravityTds.getTdsValue();
  return tdsValue;
}
