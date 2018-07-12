//Código de simulação da Tomada inteligente versão 2
//para testar diretamente no arduino
//LINK DE ACESSO |||| RESULTADO dentro das aspas ""
//  192.168.0.30:1000              |||| "IoT By Diovane Soligo" - (string)
//  192.168.0.30:1000/voltagem     |||| "220" - (voltagem exemplo) (int)
//  192.168.0.30:1000/corrente     |||| "10.5" - (corrente exemplo) (float)
//  192.168.0.30:1000/potencia     |||| "865.20" - (watts exemplo) (float)
//  192.168.0.30:1000/umidade      |||| "77" - (% vindo do sensor DHT11) (int)
//  192.168.0.30:1000/temperatura  |||| "12" - (ºC vindo do sensor DHT11) (int)
//  192.168.0.30:1000/ligar        |||| liga o rele de carga, liga led (void)
//  192.168.0.30:1000/desligar     |||| desliga o rele de carga, desliga led (void)
//  192.168.0.30:1000/status       |||| "on" se o rele estiver ligado ou "off" se desligado (string)
//  192.168.0.30:1000/hello        |||| "OKDSV2ENCPH@NTOM?DSV2?30" - (identificador da tomada, PREFIXO ? VERSÃO DE CONTROLE ? ID "ÚNICO" DO DISPOSITIVO PARA CADASTRO) (array[])
//  192.168.0.30:1000/"outro link" |||| "404?Ops! ERRO :(" - (array[] string)

#include <SPI.h>
#include <Ethernet_v2.h>//utilizado o ethernet shield v2 italiano
//caso troque o ethernet shield por outro modelo basta trocar biblioteca e configuração do setup
#include "DHT.h"

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
#if defined(WIZ550io_WITH_MACADDRESS) // Use assigned MAC address of WIZ550io
;
#else
byte mac[] = {0x90, 0xA2, 0xDA, 0x10, 0x6D, 0x9D};
#endif  
IPAddress ip(192,168,0,30);
EthernetServer server(1000);

#define DHTPIN A1 //SENSOR UMIDADE E TEMPERATURA porta A1
#define DHTTYPE DHT11 // DHT 11
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  dht.begin();
#if defined(WIZ550io_WITH_MACADDRESS)
  Ethernet.begin(ip);
#else
  Ethernet.begin(mac, ip);
#endif  
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
  
  iniciaLedsIndicadores();
  //led vermelho, referente a status de conexão externas, pisca quando alguem conectar
  //led amarelo, representa o rele de carga, se ligado ou desligado
}

void loop() {
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    
    boolean currentLineIsBlank = true;
    String linha;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        linha.concat(c);
        if (c == '\n' && currentLineIsBlank) {
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/javascript");
          client.println("Access-Control-Allow-Origin: *");
          client.println();
          int posicao = 5;
          boolean X = true;
          while (X == true) {//pega o comando recebido
            if (linha.substring(posicao, posicao + 1) == " ") {
              X = false;
            }
            posicao++;
          }
          String comando = linha.substring(5, posicao - 1);
          if (comando != "") {
            int v;
            float c;
            if (comando == "voltagem" or comando == "corrente" or comando == "potencia") {
              c = readCurrent();
              v = readVoltage();
            }
            if (comando == "umidade") {
              client.println(int(dht.readHumidity()));
            } else if (comando == "temperatura") {
              client.println(int(dht.readTemperature()));
            } else if (comando == "hello") {
              client.println("OKDSV2ENCPH@NTOM?DSV2?30");//PREFIXO "não alteravel" ? VERSÃO DE CONTROLE ? ID "ÚNICO" DO DISPOSITIVO PARA CADASTRO
            } else if (comando == "voltagem") {
              client.println(v);
            } else if (comando == "corrente") {
              client.println(c);
            } else if (comando == "potencia") {
              client.println(v * c);
            } else if (comando == "ligar") {
              digitalWrite(7, HIGH);
            } else if (comando == "desligar") {
              digitalWrite(7, LOW);
            } else if (comando == "status") {
              if (digitalRead(7) == HIGH) {
                client.println("on");
              } else {
                client.println("off");
              }
            } else {
              client.println("404?Ops! ERRO :( ");
            }
          } else {
            client.println("IoT By Diovane Soligo");
          }
          break; // fim da transmissão
        }
        if (c == '\n') {
          currentLineIsBlank = true;
        } else if (c != '\r') {
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    sinalizaAcesso();
  }
  client.flush();
}
//SENSOR TENSAO
//Simulador não possui o sensor conectado, porém mantido código
int readVoltage() {
  float val = 0.00;
  float media = 0.00;
  for (int i = 1; i <= 1000; i++) {
    int valor = analogRead(A3);
    if ((valor > 100) & (valor < 650)) {
      val = ((analogRead(A3) * 5.000) / 1024) * 53;
    }
    if ((valor > 651) & (valor < 1023)) {
      val = ((analogRead(A3) * 5.000) / 1024) * 49;
    }
    media += val;
    delay(1);
  }
  media /= 1000;
  return int(media);
}
/*************************************/
float readCurrent() {//código retidado, pois o simulador não possui o sensor corrente
  return 10.3;
}
/************************************/
void iniciaLedsIndicadores(){
  pinMode(A0,OUTPUT);//led vermelho
  pinMode(7,OUTPUT);//led amarelo
  digitalWrite(A0,LOW);
  digitalWrite(7,LOW);
  //sinaliza inicio
  int i=0;
  while(i<15){
    digitalWrite(A0,HIGH);
    digitalWrite(7,HIGH);
    delay(100);
    digitalWrite(A0,LOW);
    digitalWrite(7,LOW);
    delay(100);
    i++;
    }
  }
 /*****************************/
 void sinalizaAcesso(){
  int i=0;
  while(i<5){
    digitalWrite(A0,HIGH);
    delay(40);
    digitalWrite(A0,LOW); 
    delay(40); 
    i++;
  }
  }
