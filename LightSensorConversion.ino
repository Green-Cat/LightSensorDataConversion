#include <ESP8266WiFi.h>

String matrixausgabe_text  = " "; // Ausgabetext als globale Variable

volatile int matrixausgabe_index = 0;// aktuelle Position in Matrix

int brightness = 0 ;
double solar_irradiance = 0.0 ;
//--------------------------------------- HTTP-Get
int httpGET(String host, String cmd, String &antwort,int Port) {
  WiFiClient client; // Client über WiFi
  String text = "GET http://"+ host + cmd + " HTTP/1.1\r\n";
  text = text + "Host:" + host + "\r\n";
  text = text + "Connection:close\r\n\r\n";
  int ok = 1;
  if (ok) { // Netzwerkzugang vorhanden 
    ok = client.connect(host.c_str(),Port);// verbinde mit Client
    if (ok) {
      client.print(text);                 // sende an Client 
      for (int tout=1000;tout>0 && client.available()==0; tout--)
        delay(10);                         // und warte auf Antwort
      if (client.available() > 0)         // Anwort gesehen 
        while (client.available())         // und ausgewertet
          antwort = antwort + client.readStringUntil('\r');
      else ok = 0;
      client.stop(); 
      Serial.println(antwort);
    } 
  } 
  if (!ok) Serial.print(" no connection"); // Fehlermeldung
  return ok;
}


void setup(){ // Einmalige Initialisierung
  Serial.begin(115200);
  //------------ WLAN initialisieren 
  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  delay(100);
  Serial.print ("\nWLAN connect to:");
  Serial.print("Bdd");
  WiFi.begin("Bdd","vm180592");
  while (WiFi.status() != WL_CONNECTED) { // Warte bis Verbindung steht 
    delay(500); 
    Serial.print(".");
  };
  Serial.println ("\nconnected, meine IP:"+ WiFi.localIP().toString());
  matrixausgabe_text = " Meine IP:" + WiFi.localIP().toString();
  matrixausgabe_index=0;

}

void loop() { // Kontinuierliche Wiederholung 
  brightness = analogRead(0) ;
  if (( ( brightness ) < ( 15 ) ))
  {
    solar_irradiance = 0 ;
  }
  else
  {
    solar_irradiance = ( ( brightness / 1018.0 ) * 466 ) ;
  }

  { //Block------------------------------ sende Daten an Thingspeak (mit http GET) 
    Serial.println("\nThingspeak update ");
    String cmd = "/update?api_key="+ String("9KXRKQJV6VB82N1W");
    String host = "thingspeak.umwelt-campus.de";
    String antwort= " ";
    cmd = cmd +String("&field1="+String(solar_irradiance))+ "\n\r";
    httpGET(host,cmd,antwort,80);// und absenden 
  } // Blockende
  delay( 1000 );
}
