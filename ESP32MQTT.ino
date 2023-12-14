#include <ssl_client.h>
#include <WiFiClientSecure.h>

/*
 *  This sketch sends data via HTTP GET requests to data.sparkfun.com service.
 *
 *  You need to get streamId and privateKey at data.sparkfun.com and paste them
 *  below. Or just customize this script to talk to other HTTP servers.
 *
 */

#include <PubSubClient.h>


const char* ssid     = "Alexandre";
const char* password = "91906245";

char mqtt_host[] = "";
char clientid[] = "testeesp32";
char username[] = "Smart-home";
char passwd[] = "91906245";
char topicname[] = "rele1";
char topicsub[] = "relestat";

int led = 2;

const int mqtt_port = 8883;

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
      client.publish(topicname, "teste mqtt");
      client.subscribe(topicsub);
}

void callback(char* topic, byte* payload, unsigned int length) {
   Serial.print("Mensagem do Topic");
   Serial.println(topic);
   Serial.print("Message:");
   String message;
   char* message2;
   for(int i = 0; i < length; i++){

    message = message + (char) payload[i];
    client.publish(topicname, message2);
    
    }
    if(message == "power"){
      digitalWrite(led, !digitalRead(led));
      if(digitalRead(led) == HIGH){
        message2="led1:1";
        }else{
          message2="led1:0";
          }
      client.publish(topicname, message2);
      }
  }

void loop()
{
   client.loop();
}
