/*******************************************************************************
 * Copyright (c) 2014 IBM Corp.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution. 
 *
 * The Eclipse Public License is available at 
 *   http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at 
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Ian Craggs - initial contribution
 *******************************************************************************/

#define MQTTCLIENT_QOS2 1

#include <SPI.h>
#include <WiFi.h>
#include <WifiIPStack.h>
#include <Countdown.h>
#include <MQTTClient.h>
#include <string.h>
//#include <aJSON.h>

// SHARP LCD LIBRALIES
// Include application, user and local libraries
#include "SPI.h"
#include "OneMsTaskTimer.h"
#include "LCD_SharpBoosterPack_SPI.h"

// LCD variables
// Variables
LCD_SharpBoosterPack_SPI myScreen;
uint8_t myOrientation = 0;
uint16_t myCount = 0;


// your network name also called SSID
//char ssid[] = "SungwonGawon2_5G";
char ssid[] = "NetweeN";
// your network password
char password[] = "car391133";
char printbuf[100];
int arrivedcount = 0;



WifiIPStack ipstack;
MQTT::Client<WifiIPStack, Countdown> client = MQTT::Client<WifiIPStack, Countdown>(ipstack);

const char* topic1 = "hansung/iot/lab/rm229/temperature";
const char* topic2 = "hansung/iot/lab/rm229/humidity";
const char* SubscribedTopic1 = "hansung/iot/lab2/+/device";
const char* SubscribedTopic2 = "hansung/iot/lab/rm230/device";


void setup()
{
  // LED 
  pinMode(RED_LED, OUTPUT);     
  // LCD
  myScreen.begin();
  myScreen.clearBuffer();
  myScreen.setFont(0);
  myScreen.setOrientation(3);
  myScreen.text(10, 10, "cc3200 launch");
  myScreen.flush();
  delay(2000);
  myScreen.clear();

  
  Serial.begin(9600);
  // attempt to connect to Wifi network:
  Serial.print("Attempting to connect to Network named: ");
  // print the network name (SSID);
  Serial.println(ssid); 
  // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
  WiFi.begin(ssid, password);
  while ( WiFi.status() != WL_CONNECTED) {
    // print dots while we wait to connect
    Serial.print(".");
    delay(300);
  }
  
  Serial.println("\nYou're connected to the network");
  Serial.println("Waiting for an ip address");
  
  while (WiFi.localIP() == INADDR_NONE) {
    // print dots while we wait for an ip addresss
    Serial.print(".");
    delay(300);
  }

  Serial.println("\nIP Address obtained");
  // We are connected and have an IP address.
  Serial.println(WiFi.localIP());
  
  Serial.println("MQTT Hello example");
  connect();
}

void loop()
{
  static int loopCounter = 0;
  char buf[100];  // character buffer for subscribing topics
  myScreen.clearBuffer();
 // myScreen.setFont(0);
 // myScreen.setOrientation(3);
  myScreen.text(10, 10, "CC3200 IoT");
  myScreen.text(10, 20, "Running...");
  myScreen.text(10, 30, String(loopCounter));
  
  // connect
  if (!client.isConnected())
    connect();
  MQTT::Message message;
  arrivedcount = 0;

  // Send and receive QoS 0 message
  
  sprintf(buf, "data : %d",loopCounter);
 // Serial.println(buf);
  message.qos = MQTT::QOS1;
  message.retained = false;
  message.dup = false;
  message.payload = (void*)buf;
  message.payloadlen = strlen(buf)+1;
  
  // publish 1st topic
  int rc = client.publish(topic1, message);
  if (rc == 0) {
   Serial.println("publish1 success..");
   Serial.println(loopCounter);
    }
    else
    Serial.println("publish1 error");
  // publish 2nd toptic  
  rc = client.publish(topic2, message);
  if (rc == 0) {
   Serial.println("publish2 success..");
   Serial.println(loopCounter);
    }
    else
    Serial.println("publish2 error");
    
  //while (arrivedcount == 0)
  //  client.yield(1000);
        
  // Send and receive QoS 1 message
 /* sprintf(buf, "Hello World!  QoS 1 message");
  Serial.println(buf);
  message.qos = MQTT::QOS1;
  message.payloadlen = strlen(buf)+1;
  rc = client.publish(topic, message);
  //while (arrivedcount == 1)
   // client.yield(1000);
        
  // Send and receive QoS 2 message
 /* sprintf(buf, "Hello World!  QoS 2 message");
  Serial.println(buf);
  message.qos = MQTT::QOS2;
  message.payloadlen = strlen(buf)+1;
  rc = client.publish(topic, message);
  while (arrivedcount == 2)
    client.yield(1000);*/
  
  for (uint8_t i=0; i<20; i++) delay(100);
  loopCounter++;
  myScreen.flush();
}


/*************************************************************/
/************* sub-routine ***********************************/
/*************************************************************/
void blink(char LED)
{
 digitalWrite(LED, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);               // wait for a second
  digitalWrite(LED, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);     // wait for a second
}

void messageArrived1(MQTT::MessageData& md)
{
  MQTT::Message &message = md.message;
  //payload = (char*)malloc(10*sizeof(char));
  char*  payload;
  char str[] = "on" ;
  /*sprintf(printbuf, "Message1: arrived %d times: qos %d, retained %d, dup %d, packetid %d\n", 
		++arrivedcount, message.qos, message.retained, message.dup, message.id);
  Serial.print(printbuf);
  sprintf(printbuf, "Payload %s\n", (char*)message.payload);
  Serial.println(printbuf); */
  payload = (char*)message.payload;
  //Serial.println(payload);
  if(!strcmp(payload,str))
   blink(RED_LED);
  
  myScreen.text(10,40,"msg1 arrived:");
  myScreen.text(10,50,payload);
  //aJsonObject* msg = aJson.parse(payload);
  //aJsonObject* name;
 // name =aJson.getObjectItem(msg, "name");
  //Serial.println(name->valuestring);
  // clean up payload pointer
  for (char i=0;i<20;i++)
  *(payload + i) = NULL;  
}

void messageArrived2(MQTT::MessageData& md)
{
   MQTT::Message &message = md.message;
  //payload = (char*)malloc(10*sizeof(char));
  char*  payload;
  char str[] = "on2" ;
  /*sprintf(printbuf, "Message1: arrived %d times: qos %d, retained %d, dup %d, packetid %d\n", 
		++arrivedcount, message.qos, message.retained, message.dup, message.id);
  Serial.print(printbuf);
  sprintf(printbuf, "Payload %s\n", (char*)message.payload);
  Serial.println(printbuf); */
  payload = (char*)message.payload;
  //Serial.println(payload);
  if(!strcmp(payload,str))
   blink(RED_LED);
  
  myScreen.text(10,60,"msg2 arrived:");
  myScreen.text(10,70,payload);
  //aJsonObject* msg = aJson.parse(payload);
  //aJsonObject* name;
 // name =aJson.getObjectItem(msg, "name");
  //Serial.println(name->valuestring);
  // clean up payload pointer
  for (char i=0;i<20;i++)
  *(payload + i) = NULL;
}

void connect()
{
  char hostname[] = "iot.eclipse.org";
  int port = 1883;
  sprintf(printbuf, "Connecting to %s:%d\n", hostname, port);
  Serial.print(printbuf);
  int rc = ipstack.connect(hostname, port);

  if (rc != 1)
  {
    sprintf(printbuf, "rc from TCP connect is %d\n", rc);
    Serial.print(printbuf);
  } 
  Serial.println("MQTT connecting");
  MQTTPacket_connectData data = MQTTPacket_connectData_initializer;       
  data.MQTTVersion = 3;
  data.clientID.cstring = (char*)"energia-sample";
  rc = client.connect(data);

  if (rc != 0)
  {
    sprintf(printbuf, "rc from MQTT connect is %d\n", rc);
    Serial.print(printbuf);
  }
  Serial.println("MQTT connected");
  
  rc = client.subscribe(SubscribedTopic1, MQTT::QOS0, messageArrived1);     
  if (rc != 0)
  {
    sprintf(printbuf, "rc from MQTT subscribe is %d\n", rc);
    Serial.println(printbuf);
  }
  Serial.println("MQTT subscribed 1");
  
  rc = client.subscribe(SubscribedTopic2, MQTT::QOS0, messageArrived2); 
  if (rc != 0)
  {
    sprintf(printbuf, "rc from MQTT subscribe is %d\n", rc);
    Serial.println(printbuf);
  }
  Serial.println("MQTT subscribed 2");
  
}


