#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);

const char* ssid     = "Arduino Wifi";      // WIFI SSID  
const char* password = "************";      // WIFI Password 
String APIKEY = "e876574712e8cd87bad58bc226831414"; // Get free Default API key from openweathermap.org
String CityID = "1732811"; //Kluang  ...find yours...why openwheathermap not stick with standardised poscode instead..???

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 28800;       //My,Sg GMT = +8 UTC*60*60 , GREENWICH GMT = 0 UTC +(60*60)= 0 
const int   daylightOffset_sec = 0;	 // ???
char buffer[80];
int cursorPosition=0;

WiFiClient client;
char servername[]="api.openweathermap.org";  // remote server we will connect to
String result;

int  counter = 60;
String weatherDescription ="";
String weatherLocation = "";
String Country;
float Temperature;
float Humidity;
float Pressure;

void setup() {
	
  lcd.begin();
  Serial.begin(115200);
  lcd.setCursor(0,1);
  lcd.print("     Connecting");  
  Serial.println("Connecting");
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    lcd.setCursor(cursorPosition,2); 
    lcd.print(".");
    cursorPosition++;
  }
  lcd.clear();
  lcd.setCursor(0,1);
  lcd.print("     Connected!");
  Serial.println("Connected");
  delay(1000);
  
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
 
  unsigned timeout = 5000;
  unsigned start = millis();
  while (!time(nullptr))
  {
    Serial.print(".");
    delay(1000);
  }
  delay(1000);

}

void printLocalTime()
{
  time_t rawtime;
  struct tm * timeinfo;

  time (&rawtime);
  timeinfo = localtime (&rawtime);
  strftime (buffer,80,"    %I:%M:   %p    %A ",timeinfo);
  Serial.println(buffer);
  lcd.print(buffer);
  
}

void printLocalTimeOnly()
{
  time_t rawtime;
  struct tm * timeinfo;

  time (&rawtime);
  timeinfo = localtime (&rawtime);
  strftime (buffer,80,"%I:%M %p",timeinfo);
  lcd.print(buffer);
  
}

void printDateOnly()
{
  time_t rawtime;
  struct tm * timeinfo;

  time (&rawtime);
  timeinfo = localtime (&rawtime);
  strftime (buffer,80,"%d.%m.%Y",timeinfo);
  lcd.print(buffer);
  
}

void printSecOnly()
{
  time_t rawtime;
  struct tm * timeinfo;

  time (&rawtime);
  timeinfo = localtime (&rawtime);
  strftime (buffer,80,"%S",timeinfo);
  lcd.print(buffer);
  
}


void loop() {
	
    if(counter == 60) //Get new data every 10 minutes
    {
      counter = 0;
      displayGettingData();
      delay(1000);
      getWeatherData();
	  
    }else
    { 
      lcd.clear();
      counter++;
	    
            pageOne();			 // display 15 sec of Time with sec update ..me have not know loop yet
	    pageOne();
	    pageOne();
     	    pageOne();
	    pageOne();
	    pageOne();
            pageOne();
	    pageOne();
	    pageOne();
	    pageOne();
	    pageOne();
	    pageOne();
            pageOne();
	    pageOne();
	    pageOne();
	    
      for (int positionCounter = 0; positionCounter < 10; positionCounter++) {
      lcd.scrollDisplayLeft();
      delay(150);
       }
      displayWeather(weatherLocation,weatherDescription);
      lcd.setCursor(12,0);
      printLocalTimeOnly();
      //scrollDescription(weatherDescription);
      displayConditions(Temperature,Humidity,Pressure);
	    
	    pageTwo();                       // display 10 sec of Time with min update on pageTwo
	    pageTwo();
	    pageTwo();
	    pageTwo();
	    pageTwo();
      	    pageTwo();
      	    pageTwo();
            pageTwo();
            pageTwo();
            pageTwo();
	    
      for (int positionCounter = 0; positionCounter < 10; positionCounter++) {
      lcd.scrollDisplayRight();
      delay(150);
       }
    }
}

void getWeatherData() //client function to send/receive GET request data.
{
  if (client.connect(servername, 80)) {  //starts client connection, checks for connection
    client.println("GET /data/2.5/weather?id="+CityID+"&units=metric&APPID="+APIKEY);
    client.println("Host: api.openweathermap.org");
    client.println("User-Agent: ArduinoWiFi/1.1");
    client.println("Connection: close");
    client.println();
  } 
  else {
    Serial.println("connection failed"); //error message if no client connect
    Serial.println();
  }

  while(client.connected() && !client.available()) delay(1); //waits for data
  while (client.connected() || client.available()) { //connected or data available
    char c = client.read(); //gets byte from ethernet buffer
      result = result+c;
    }

  client.stop(); //stop client
  result.replace('[', ' ');
  result.replace(']', ' ');
  Serial.println(result);

char jsonArray [result.length()+1];
result.toCharArray(jsonArray,sizeof(jsonArray));
jsonArray[result.length() + 1] = '\0';

StaticJsonBuffer<1024> json_buf;
JsonObject &root = json_buf.parseObject(jsonArray);
if (!root.success())
{
  Serial.println("parseObject() failed");
}

String location     = root["name"];
String country      = root["sys"]["country"];
float  temperature  = root["main"]["temp"];
float  humidity     = root["main"]["humidity"];
String weather      = root["weather"]["main"];
String description  = root["weather"]["description"];
float  pressure     = root["main"]["pressure"];

weatherDescription = description;
weatherLocation = location;
Country = country;
Temperature = temperature;
Humidity = humidity;
Pressure = pressure;

}

void displayWeather(String location,String description)
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(location);
  lcd.print(",");
  lcd.print(Country);
  lcd.setCursor(0,1);
  lcd.print(description);
}
void displayConditions(float Temperature,float Humidity, float Pressure)
{
 //lcd.clear();
 lcd.setCursor(0,2);
 lcd.print("T: "); 
 lcd.print(Temperature,0);
 lcd.print((char)223);
 lcd.print("C ");
 
 //Printing Humidity
 lcd.print("H: ");
 lcd.print(Humidity,0);
 lcd.print(" %");
 
 //Printing Pressure
 lcd.setCursor(0,3);
 lcd.print("P: ");
 lcd.print(Pressure,0);
 lcd.print(" hPa");
}
void pageOne()
{
    lcd.setCursor(1,1);
    printLocalTime();
    lcd.setCursor(11,1);
    printSecOnly();
    lcd.setCursor(10,3);
    printDateOnly();
    delay(1000);
}
void pageTwo()
{
    lcd.setCursor(12,0);
    printLocalTimeOnly();
    delay(1000);
}
void displayGettingData()
{
  lcd.clear();
  lcd.setCursor(4,1);
  lcd.print("Getting data");
  delay (1000);
  lcd.clear();
}
/* 			Not know how to scroll just description yet...the whole page scrolled
void scrollDescription(String description)
{
  lcd.setCursor(0,1);
  lcd.print(description);
  for (int positionCounter = 0; positionCounter < 20; positionCounter++) {
  lcd.scrollDisplayLeft();
  delay(150);}
}
*/
