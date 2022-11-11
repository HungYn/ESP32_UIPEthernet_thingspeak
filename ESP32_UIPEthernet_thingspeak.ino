// UIPEthernet.h  https://github.com/UIPEthernet/UIPEthernet
// PubSubClient.h  https://github.com/knolleary/pubsubclient
//增加RAM https://andrologiciels.wordpress.com/arduino/electronique/ram/

//http://coolaf.com/  增加TalkBack 命令
//http://api.thingspeak.com/talkbacks/47145/commands.json?api_key=OSZC0RZD8PZFWLZ3
//command_string=TURN_ON

//更新頻道並執行下一個 TalkBack 命令
//http://api.thingspeak.com/update
//field1=4&field2=61&field3=35&field4=31&field5=0&api_key=A3SK4EBBQREXHD19&talkback_key=OSZC0RZD8PZFWLZ3

//列出所有 TalkBack 命令查看
//https://api.thingspeak.com/talkbacks/47145/commands.json?api_key=OSZC0RZD8PZFWLZ3


//#include <avr/pgmspace.h>

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 20, 4);

#include <UIPEthernet.h>
//#include <PubSubClient.h>
#define STATIC 1 // 設置為 1 以禁用 DHCP

// Update these with values suitable for your network.
uint8_t mac[6] = {0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02};
IPAddress ip(192, 168, 0, 128);
IPAddress dnss(192, 168, 0, 1);// the dns server ip
IPAddress gateway(192, 168, 0, 1);// the router's gateway address:
IPAddress subnet(255, 255, 255, 0);// the subnet:


//https://thingspeak.com/channels
unsigned long myChannelNumber = 1623046;
unsigned long myTalkBackID = 47145;
const char * myTalkBackKey = "OSZC0RZD8PZFWLZ3";
const char * myWriteAPIKey = "A3SK4EBBQREXHD19";


// Initialize values for ThingSpeak updates
int number1 = 0;
int number2 = random(1, 4); //1,2,3
//int number3 = random(0, 100);
int number4 = 0;
int number5 = 0;
int number6 = 0;

EthernetClient client;


unsigned long  previous_millis_A = 0; //初始化以前的millis
unsigned long  interval_A = 15 * 1000; //間隔15秒執行


#include "DHT.h"
#define DHTPIN 33      //讀取DHT11 Data
#define DHTTYPE DHT11 //選用DHT11
DHT dht(DHTPIN, DHTTYPE); // Initialize DHT sensor
float h;
float t;

int led1 = 25;
int led2 = 26;
int led3 = 27;
//int led4 = 6;


void setup()
{

  Serial.begin(115200);

#if STATIC
  Ethernet.begin(mac, ip);
  //    Ethernet.begin(mac, ip, dnss, gateway, subnet);
#else
  if (Ethernet.begin(mac) == 0) {
    //    Serial.println(F("Failed to configure Ethernet using DHCP"));
  }
#endif

  Serial.print(F("localIP: "));
  Serial.println(Ethernet.localIP());


  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  //  pinMode(led4, OUTPUT);


  //----------啟動DHT----------
  dht.begin();//啟動DHT

  //------lcd--------
  lcd.init(); // initialize LCD
  //  lcd.backlight(); // 開啟背光
  lcd.setCursor(0, 0);
  lcd.print(F("IP:"));
  lcd.print(Ethernet.localIP());
  lcd.setCursor(0, 1);
  lcd.print(F("Mask:"));
  lcd.print(Ethernet.subnetMask());
  lcd.setCursor(0, 2);
  lcd.print(F("gw:"));
  lcd.print(Ethernet.gatewayIP());
  lcd.setCursor(0, 3);
  lcd.print(F("dns:"));
  lcd.print(Ethernet.dnsServerIP());
  delay(1500);
  lcd.setCursor(0, 1);
  lcd.print(F("                    "));
  lcd.setCursor(0, 2);
  lcd.print(F("                    "));
  lcd.setCursor(0, 3);
  lcd.print(F("                    "));

}

void loop()
{

  // Make the POST to ThingSpeak
  if (millis() - previous_millis_A > interval_A) {


    h = dht.readHumidity();//讀取濕度
    t = dht.readTemperature();//讀取攝氏溫度
    //  float f = dht.readTemperature(true);//讀取華氏溫度

    Serial.print(F("濕度： "));
    Serial.print(h);
    Serial.print(F("%  溫度："));
    Serial.print(t);
    Serial.println(F("°C "));
    lcd.setCursor(0, 1);
    lcd.print(F("Temperature:"));
    lcd.print(t);

    lcd.setCursor(0, 2);
    lcd.print(F("Humidity:"));
    lcd.print(h);


    // Create the message body for the POST out of the values
    String postMessage =  String("field1=") + String(number1) +
                          String("&field2=") + String(t) +
                          String("&field3=") + String(h) +
                          String("&field4=") + String(number4) +
                          String("&field5=") + String(number5) +
                          String("&field6=") + String(number6) +
                          String("&api_key=") + String(myWriteAPIKey) +
                          String("&talkback_key=") + String(myTalkBackKey);

    String newCommand = String();

    //送出TalkBack指令開燈
    httpRequest();

    int x = httpPOST(postMessage, newCommand);



    // Check the result
    if (x == 200) {
      Serial.println(F("檢查隊列"));
      // Check for a command returned from TalkBack
      if (newCommand.length() != 0) {

        Serial.println(F("隊列中的最新命令"));


        if (newCommand.indexOf("1_ON") != -1) {
          digitalWrite(led1, HIGH);
          number4 = 1;
          lcd.setCursor(0, 3);
          lcd.print(F("LED1:Y"));
        }

        if (newCommand.indexOf("1_OFF") != -1) {
          digitalWrite(led1, LOW);
          number4 = 0;
          lcd.setCursor(0, 3);
          lcd.print(F("LED1:N"));
        }

        if (newCommand.indexOf("2_ON") != -1) {
          digitalWrite(led2, HIGH);
          number5 = 1;
          lcd.setCursor(7, 3);
          lcd.print(F("LED2:Y"));
        }

        if (newCommand.indexOf("2_OFF") != -1) {
          digitalWrite(led2, LOW);
          number5 = 0;
          lcd.setCursor(7, 3);
          lcd.print(F("LED2:N"));
        }

        if (newCommand.indexOf("3_ON") != -1) {
          digitalWrite(led3, HIGH);
          number6 = 1;
          lcd.setCursor(14, 3);
          lcd.print(F("LED3:Y"));
        }

        if (newCommand.indexOf("3_OFF") != -1) {
          digitalWrite(led3, LOW);
          number6 = 0;
          lcd.setCursor(14, 3);
          lcd.print(F("LED3:N"));
        }

        Serial.print(newCommand);



      }
      else {
        Serial.println(F("沒什麼新的"));
      }

    }
    else {
      Serial.print(F("問題檢查隊列。 HTTP 錯誤代碼" ) );
      Serial.println(String(x));

    }

    // Confirm code works by changing values
    number1++;
    if (number1 > 99) {
      number1 = 0;
    }
    number2 = random(1, 4);//1,2,3
    //    number3 = random(0, 100);
    //    number4 = random(0, 100);



  }
}


int httpPOST(String postMessage, String &response) {
  client.stop();
  bool connectSuccess = false;
  connectSuccess = client.connect("api.thingspeak.com", 80);
  previous_millis_A = millis();
  //  Serial.print(F("DD"));
  // Serial.println(connectSuccess);

  if (!connectSuccess) {
    return -301;
  }

  client.println("POST /update HTTP/1.1");
  client.println("Host: api.thingspeak.com");
  client.println("Content-Type: application/x-www-form-urlencoded");
  client.println("Content-Length: " + String(postMessage.length()));
  client.println();
  client.println(postMessage);

  long startWaitForResponseAt = millis();
  while (client.available() == 0 && millis() - startWaitForResponseAt < 5000) {
    delay(100);
  }

  if (client.available() == 0) {
    return -304; // Didn't get server response in time
  }

  if (!client.find(const_cast<char *>("HTTP/1.1"))) {
    return -303; // Couldn't parse response (didn't find HTTP/1.1)
  }

  int status = client.parseInt();
  if (status != 200) {
    return status;
  }

  if (!client.find(const_cast<char *>("\n\r\n"))) {
    return -303;
  }

  String tempString = String(client.readString());
  response = tempString;

  return status;
}

void httpRequest() {
  // close any connection before send a new request.
  // This will free the socket on the WiFi shield
  client.stop();

  String postMessagea = "";
  postMessagea = "command_string=OFF";
  Serial.print(F("亂數"));
  Serial.println(number2);

  if (number4 == 0 && number2 == 1) {
    postMessagea = "command_string=1_ON";
  }
  else if (number4 == 1 && number2 == 1) {
    postMessagea = "command_string=1_OFF";
  }

  else if (number5 == 0 && number2 == 2) {
    postMessagea = "command_string=2_ON";
  }
  else if (number5 == 1 && number2 == 2) {
    postMessagea = "command_string=2_OFF";
  }

  else if (number6 == 0 && number2 == 3) {
    postMessagea = "command_string=3_ON";
  }
  else if (number6 == 1 && number2 == 3) {
    postMessagea = "command_string=3_OFF";
  }

  // if there's a successful connection:
  if (client.connect("api.thingspeak.com", 80)) {
    Serial.println(F("連接..."));
    // send the HTTP GET request:
    client.println("POST /talkbacks/47145/commands.json?api_key=OSZC0RZD8PZFWLZ3 HTTP/1.1");
    client.println("Host: api.thingspeak.com");
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.println("Content-Length: " + String(postMessagea.length()));
    client.println();
    client.println(postMessagea);

    previous_millis_A = millis();

  } else {
    // if you couldn't make a connection:
    Serial.println(F("連接失敗"));

    previous_millis_A = millis();

  }
}
