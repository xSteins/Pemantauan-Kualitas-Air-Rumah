#include <ESP8266WiFi.h>
#include <PubSubClient.h>
// ini untuk koneksi internet esp dan brokernya
const char* ssid = "INI-HARUS-DIISI";
const char* password = "INI-HARUS-DIISI";
const char* mqtt_server = "broker.mqtt-dashboard.com"; // pakai hivemind free server
const int mqtt_port = 1883; // port untuk mqtt

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(9600);  //baud rate 9600 sama kyk arduino
  WiFi.begin(ssid, password);

  // tunggu sampai terhubung ke wifi
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("connected to wifi");

  // inisialisasi koneksi mqtt
  client.setServer(mqtt_server, mqtt_port);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // membaca data dari arduino mega (misalnya: "7.20#1.63#667.07#98.44")
  if (Serial.available()) {
    String data = Serial.readStringUntil('\n');  // membaca data serial
    parseAndSendData(data);  // memproses dan mengirim data ke mqtt
  } 
  else {
    // tes data dummy
    // String data = "7.20#25.60#150#98.44";
    // parseAndSendData(data);  // memproses dan mengirim data ke mqtt
    Serial.println("Gagal membaca");
  }
  delay(5000); // delay 5 detik
}

void reconnect() {
  // loop sampai terkoneksi kembali ke mqtt
  while (!client.connected()) {
    if (client.connect("ESP8266Client")) {
      Serial.println("connected to mqtt server");
    } else {
      delay(5000);
    }
  }
}

void parseAndSendData(String data) {
  // memisahkan data berdasarkan simbol '#'
  int firstHash = data.indexOf('#');
  int secondHash = data.indexOf('#', firstHash + 1);
  int thirdHash = data.indexOf('#', secondHash + 1);

  String ph = data.substring(0, firstHash);
  String temp = data.substring(firstHash + 1, secondHash);
  String turbidity  = data.substring(secondHash + 1, thirdHash);
  String tds = data.substring(thirdHash + 1);

  // mengirimkan data ke topik mqtt yang sesuai
  boolean ph_sent = client.publish("RALVIN01PH", ph.c_str());
  boolean temp_sent = client.publish("RALVIN01TEMP", temp.c_str());
  boolean tds_sent = client.publish("RALVIN01TDS", tds.c_str());
  boolean turb_sent = client.publish("RALVIN01TURBIDITY", turbidity.c_str());

  // cek apakah semua data berhasil dikirim
  if (ph_sent && temp_sent && tds_sent && turb_sent) {
    Serial.println("Data berhasil dikirim");
  } else {
    Serial.println("Data gagal dikirim");
  }
}

