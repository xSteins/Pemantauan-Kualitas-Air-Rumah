# Proyek IOT - Pemantauan Kualitas Air
## Prequisite (Hardware)
Proyek ini membutuhkan beberapa perangkat arduino sebagai berikut :
- [ ] Arduino Mega 2560 / Equivalent
- [ ] NodeMCU ESP8266
- [ ] Level shifter 5v -> 3.3v (Optional)
- [ ] Breadboard
- [ ] [**DS18B20 Arduino Temperature sensor**](https://wiki.dfrobot.com/Waterproof_DS18B20_Digital_Temperature_Sensor__SKU_DFR0198_) 
- [ ] [**gravity arduino turbidity sensor**](https://wiki.dfrobot.com/Turbidity_sensor_SKU__SEN0189) 
- [ ] [PH-4502C Sensor](https://raaflahar.medium.com/ph-4502c-sensor-diymore-how-to-use-and-calibrate-using-arduino-uno-r3-3afc2b96631) 
- [ ] [Arduino Analog TDS Sensor](https://wiki.dfrobot.com/Gravity__Analog_TDS_Sensor___Meter_For_Arduino_SKU__SEN0244) 
- [ ] Pull-up resistor 4k7 untuk sensor DS18B20

## Prequisite (Software)
Software ini buat koding & upload program ke devicenya, dan menjalankan local websocket server :
- [ ] Arduino IDE
- [ ] Library OneWire, DallasTemperature, PubSubClient, dan driver pendukung device espnya
- [ ] NodeJS
- [ ] Mosquitto (Windows) untuk testing MQTT

## Hasil Testing
### Tampilan UI Awal
Pada tahap berikut belum ada implementasi logging / pemilihan profil (scale up)
![Connected State](Documentation/connected-state.png)
![ConnectedState(2)](Documentation/connected-state-live.png)

### Tampilan UI Final
Tahap yang terbaru sudah memiliki laman untuk memilih device / toren yang ingin dipantau.
Selain itu, sudah ada logging dan analisis sederhana yang disimpulkan dari data keempat sensor

- Halaman pemilihan profil / device
![Profile Selection](https://github.com/xSteins/Pemantauan-Kualitas-Air-Rumah/blob/ef2f322e445d7a805625927331951b85ffeffe98/Documentation/updated-profile-selection.png)
- Halaman dashboard saat data dari broker masih kosong
![Dashboard Empty](https://github.com/xSteins/Pemantauan-Kualitas-Air-Rumah/blob/ef2f322e445d7a805625927331951b85ffeffe98/Documentation/dashboard-inactive.png)
