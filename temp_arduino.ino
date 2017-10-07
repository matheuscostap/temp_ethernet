#include <SPI.h>
#include <Ethernet.h>

//Configurações de rede
byte mac[] = {0xDE,0xAD,0xBE,0xEF,0xFE,0xED}; //mac address
IPAddress ip(192,168,1,12); //endereço ip
IPAddress subnet(255,255,255,0); //mascara de subrede
IPAddress gateway(192,168,1,1); //gateway

//Inicializa o servidor web na porta 80
EthernetServer server(80);

//LM35
const int LM35 = 0;
float temperatura = 0;

void setup() {
  Serial.begin(9600);
  
  //Inicializa a interface de rede
  Ethernet.begin(mac,ip,gateway,subnet);
  server.begin();
}

void loop() {
  
  //Aguarda conexao do browser
  EthernetClient client = server.available();
  if(client){
    Serial.println("Novo cliente");
    
    //Uma requisição HTTP termina com uma linha em branco
    boolean currentLineIsBlank = false;

    while(client.connected()){
      if(client.available()){
        char c = client.read();
        Serial.write(c);
        //Se chegou ao fim da linha (e recebeu um CR) e a linha está
        //em branco a requisição HTTP terminou
        //ja é possível enviar uma resposta

        if(c == '\n' && currentLineIsBlank){

          while(client.available()){
             Serial.println(c);
             //Serial.write(client.read());
          }
          
          Serial.println("**************   Enviando Resposta   **************");
          //Enviando um response header http
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: application/json");
          client.println("Server: ArduinoUNO");
          client.println("Connection: close");
          client.println("");
          
          //JSON
          loadJSON(client);
          break;
        }

        if(c == '\n'){
          //começando uma nova linha
          currentLineIsBlank = true;
        }else if (c != '\r'){
          //recebeu um caractere na linha atual
          currentLineIsBlank = false;
        }
      }
    }

    //dando tempo do browse receber os dados
    delay(1);

    //fecha a conexao
    client.stop();
  }
}//loop



//JSON que será retornado ao acessar
void loadJSON(EthernetClient client){
  client.print("{");
  client.print("\"temperatura\":\"");
  client.print(lerTemperatura());
  client.print("\"");
  client.print("}");
}



//Leitura do sensor
float lerTemperatura(){
  temperatura = (float(analogRead(LM35))*5/(1023))/0.01;
  return temperatura;
}
