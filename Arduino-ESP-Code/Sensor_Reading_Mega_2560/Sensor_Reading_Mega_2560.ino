#include <DallasTemperature.h>
#include <OneWire.h>
#include <GravityTDS.h>

GravityTDS gravityTds;

// pinout untuk masing-masing sensor
const int AnalogPh = A0;
const int AnalogTurb = A1;   
const int AnalogTDS = A2;        
// pinout digital u/ temperatur
#define ONE_WIRE_BUS 24
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire); 


void setup(){
  // baud rate maksimum u/ connect dgn esp8266
  Serial.begin(9600); 

  // set-up tds
  gravityTds.setPin(AnalogTDS);
  gravityTds.setAref(5.0); // ref voltage device
  gravityTds.setAdcRange(1024); // resolusi data
  gravityTds.begin(); // init

  // init sensor temperatur
  sensors.begin();
  // untuk debugging onewire
  // int numberOfDevices = sensors.getDeviceCount();
  // Serial.print(numberOfDevices, DEC);
  // Serial.println(" devices.");
}


void loop(){
  // Untuk kirim data ke ESP pakai # sebagai separator value
  get_ph_reading();
  Serial.print("#");
  Serial.print(get_temp_reading());
  Serial.print("#");
  get_tds_reading();
  Serial.print("#");
  get_turb_reading();
  Serial.println();
  delay(5000);
}

// https://cimpleo.com/blog/arduino-ph-meter-using-ph-4502c/
// kalibrasi : https://raaflahar.medium.com/ph-4502c-sensor-diymore-how-to-use-and-calibrate-using-arduino-uno-r3-3afc2b96631
float valPh4 = 3.50;
float valPh7 = 3.00;
void get_ph_reading(){
  // pinout analog (P0 di PCB)
  int analog_ph_reading = analogRead(AnalogPh);
  // hitung kompensasi voltase dgn reading analognya
  float TeganganPh = 5 / 1024.0 * analog_ph_reading; 
  // Serial.print(" ADC pH : ");
  // Serial.println(TeganganPh);
  // hitung nilai pH menggunakan tegangan yang diukur
  float pH_Step = (valPh4 - valPh7) / 3;
  float pH = 7.0 + ((valPh7-TeganganPh) / pH_Step);
  Serial.print(pH);
}

float get_temp_reading(){
  // request temperatur
  sensors.requestTemperatures();
  // return temperatur dalam bentuk celsius
  return sensors.getTempCByIndex(0);
}
// https://wiki.dfrobot.com/Turbidity_sensor_SKU__SEN0189
void get_turb_reading(){
  int analog_turb_reading = analogRead(AnalogTurb);
  // validasi jika sensor gagal
  if (analog_turb_reading <= 0) { 
    Serial.print("-503");
    return;
  }

  // pakai mode analog
  // rumus : baca pin * (voltase device (3.3 esp, 5.0 ardduino) / 1024.0)
  // voltage = analogRead(pin) * (5.0 / 1024.0)
  // karena mau baca persentase, anggap saja
  float turb_reading = 100.0 - analogRead(AnalogTurb) * (5.0 / 1024.0);
  Serial.print(turb_reading);
}

void get_tds_reading(){
  float temp = get_temp_reading();
  if (temp == -503) { 
    // set error code juga kalo suhu tidak bisa dibaca
    Serial.print("-503");
    return;
  }
  gravityTds.setTemperature(temp);
  gravityTds.update();

  float tdsValue = gravityTds.getTdsValue();
  if (tdsValue <= 0) { // validasi jika TDS gagal baca
    Serial.print("-503");
    return;
  }
  Serial.print(tdsValue);
}
