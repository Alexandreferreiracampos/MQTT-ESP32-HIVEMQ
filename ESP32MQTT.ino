#include <ssl_client.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <HTTPClient.h>


const char* ssid     = "Alexandre";
const char* password = "91906245";

char mqtt_host[] = "";
char clientid[] = "testeesp32";
char username[] = "Smart-home";
char passwd[] = "91906245";
char topicname[] = "smart-home";
char topicsub[] = "smarthome";

int led = 2;

const int mqtt_port = 8883;

hw_timer_t *timer = NULL;

void IRAM_ATTR resetModule(){
    ets_printf("(watchdog) reiniciar\n"); //imprime no log
    ESP.restart(); //reinicia o chip
}

HTTPClient http;
WiFiClientSecure clientmqtt;
PubSubClient client(clientmqtt);

const char* mosq_cert =  \
     "-----BEGIN CERTIFICATE-----\n" \
     "MIIDSjCCAjKgAwIBAgIQRK+wgNajJ7qJMDmGLvhAazANBgkqhkiG9w0BAQUFADA/\n" \
     "MSQwIgYDVQQKExtEaWdpdGFsIFNpZ25hdHVyZSBUcnVzdCBDby4xFzAVBgNVBAMT\n" \
     "DkRTVCBSb290IENBIFgzMB4XDTAwMDkzMDIxMTIxOVoXDTIxMDkzMDE0MDExNVow\n" \
     "PzEkMCIGA1UEChMbRGlnaXRhbCBTaWduYXR1cmUgVHJ1c3QgQ28uMRcwFQYDVQQD\n" \
     "Ew5EU1QgUm9vdCBDQSBYMzCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEB\n" \
     "AN+v6ZdQCINXtMxiZfaQguzH0yxrMMpb7NnDfcdAwRgUi+DoM3ZJKuM/IUmTrE4O\n" \
     "rz5Iy2Xu/NMhD2XSKtkyj4zl93ewEnu1lcCJo6m67XMuegwGMoOifooUMM0RoOEq\n" \
     "OLl5CjH9UL2AZd+3UWODyOKIYepLYYHsUmu5ouJLGiifSKOeDNoJjj4XLh7dIN9b\n" \
     "xiqKqy69cK3FCxolkHRyxXtqqzTWMIn/5WgTe1QLyNau7Fqckh49ZLOMxt+/yUFw\n" \
     "7BZy1SbsOFU5Q9D8/RhcQPGX69Wam40dutolucbY38EVAjqr2m7xPi71XAicPNaD\n" \
     "aeQQmxkqtilX4+U9m5/wAl0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNV\n" \
     "HQ8BAf8EBAMCAQYwHQYDVR0OBBYEFMSnsaR7LHH62+FLkHX/xBVghYkQMA0GCSqG\n" \
     "SIb3DQEBBQUAA4IBAQCjGiybFwBcqR7uKGY3Or+Dxz9LwwmglSBd49lZRNI+DT69\n" \
     "ikugdB/OEIKcdBodfpga3csTS7MgROSR6cz8faXbauX+5v3gTt23ADq1cEmv8uXr\n" \
     "AvHRAosZy5Q6XkjEGB5YGV8eAlrwDPGxrancWYaLbumR9YbK+rlmM6pZW87ipxZz\n" \
     "R8srzJmwN0jP41ZL9c8PDHIyh8bwRLtTcm1D9SZImlJnt1ir/md2cXjbDaJWFBM5\n" \
     "JDGFoqgCWjBH4d1QB7wCCZAA62RjYJsWvIjJEubSfZGL+T0yjWW06XyxV3bqxbYo\n" \
     "Ob8VZRzI9neWagqNdwvYkQsEjgfbKbYK7p2CNTUQ\n" \
     "-----END CERTIFICATE-----\n";

void setup()
{
    Serial.begin(115200);
    delay(10);

    pinMode(led, OUTPUT);
    digitalWrite(led, LOW);

    // We start by connecting to a WiFi network

    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

  

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    clientmqtt.setCACert(mosq_cert);
    client.setServer(mqtt_host, mqtt_port);
    client.setCallback(callback);

    while(!client.connected()){
      Serial.println("Conectando ao Broker");
      if(client.connect(clientid, username, passwd)){
        Serial.println("Conectado com o Broker");
        }else{
          Serial.println("Falha ao conectar");
          Serial.println(client.state());
          delay(2000);
          }
      }
      
      client.subscribe(topicsub);

      long tme = millis();

    timer = timerBegin(0, 80, true); //timerID 0, div 80
    //timer, callback, interrupção de borda
    timerAttachInterrupt(timer, &resetModule, true);
    //timer, tempo (us), repetição
    timerAlarmWrite(timer, 15000000, true); //igual a 15 segundos
    timerAlarmEnable(timer); //habilita a interrupção 
}

void smarthomeGet(String parametro){
  
      digitalWrite(led, HIGH);
      http.begin("http://192.168.0."+parametro);
      delay(80);
      http.GET();
      delay(80);
      http.end();
      timerWrite(timer, 0);
      digitalWrite(led, LOW);
      delay(80);
      ESP.restart();
  }

void callback(char* topic, byte* payload, unsigned int length) {
   
   Serial.print("Mensagem do Topic");
   Serial.println(topic);
   Serial.print("Message:");
   String message;
   for(int i = 0; i < length; i++){
    
    message = message + (char) payload[i];
    }
    
    if(message == "luzSala"){
        smarthomeGet("200/rele6");
    }
    if(message == "ledSala"){
        smarthomeGet("200/rele5");
    }
    if(message == "luzGaragem"){
        smarthomeGet("200/rele3");
    }
    if(message == "arandela"){
        smarthomeGet("200/rele4");
    }
    if(message == "ledFachada"){
        smarthomeGet("200/rele1");
    }
    if(message == "portao"){
        smarthomeGet("200/relea");
    }
    if(message == "luzQuarto"){
        smarthomeGet("100/rele4");
    }
    if(message == "LedQuarto"){
        smarthomeGet("100/fade");
    }
    if(message == "luzCozinha"){
        smarthomeGet("53/Controle?Rele1=on");
    }
    if(message == "luzGormet"){
        smarthomeGet("200/relee");
    }
    if(message == "ledGormet"){
        smarthomeGet("200/relef");
    }
    if(message == "arandelasInterior"){
        smarthomeGet("200/releg");
    }
    if(message == "piscina"){
        smarthomeGet("200/releh");
    }
    if(message == "som"){
        smarthomeGet("200/relej");
    }
       
  }

void loop()
{
   
   Serial.print(".");
   delay(200);
   client.loop();
   timerWrite(timer, 0); //reseta o temporizador (alimenta o watchdog) 
}
