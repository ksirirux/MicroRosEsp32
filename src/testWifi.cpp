#include <Arduino.h>
#include <micro_ros_platformio.h>

#include <WiFi.h>

#include <rcl/rcl.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>

#if defined(MICRO_ROS_TRANSPORT_ARDUINO_WIFI)
#endif

IPAddress myIp;
WiFiServer server(80);
WiFiClient client;
String header;
unsigned long previousTime = 0;
unsigned long currentTime = millis();
long timeoutTime = 2000;

// Auxiliar variables to store the current output state
String output26State = "off";
String output27State = "off";

// Assign output variables to GPIO pins
const int output26 = 26;
const int output27 = 27;
void showWebServer();
void renderLedControl();


void htmlHeader();
void htmlFooter();
void renderDataPage();


String getValue(String data, char separator, int index);

void setup()
{
    pinMode(output26, OUTPUT);
    pinMode(output27, OUTPUT);
    // Set outputs to LOW
    digitalWrite(output26, LOW);
    digitalWrite(output27, LOW);
    Serial.begin(57600);
    IPAddress agent_ip(192, 168, 1, 35);
    size_t agent_port = 8888;
    char ssid[] = "tawechai 2.4";
    char psk[] = "0819732784";

    set_microros_wifi_transports(ssid, psk, agent_ip, agent_port);
    Serial.printf("Connect to =>ssid:%s \n", ssid);
    myIp = WiFi.localIP();
    Serial.println(myIp);
    server.begin();
}

void loop()
{
    showWebServer();
}

enum MyEnum
{
    MAINPAGE,
    REMOTE,
    MAP
};

void showWebServer()
{
    client = server.available();
    if (client)
    {
        currentTime = millis();
        previousTime = currentTime;
        String currentLine = "";
        while (client.connected() && currentTime - previousTime < timeoutTime)
        {
            currentTime = millis();
            if (client.available())
            {
                char c = client.read();
                Serial.write(c);
                header += c;
                if (c == '\n')
                {
                    if (currentLine.length() == 0)
                    {
                        //renderLedControl();
                        String page = getValue(header,'/',1);
                        if (page.indexOf("data")>=0){
                            renderDataPage();
                        }else if(page.indexOf("Control")){
                            renderLedControl();
                        }
                        break;
                    }
                    else
                    { // if you got a newline, then clear currentLine
                        currentLine = "";
                    }
                }
                else if (c != '\r')
                {                     // if you got anything else but a carriage return character,
                    currentLine += c; // add it to the end of the currentLine
                }
            }
        }
        // Clear the header variable
        header = "";
        // Close the connection
        client.stop();
        Serial.println("Client disconnected.");
        Serial.println("");
    }
}

void htmlHeader(){
    client.println("HTTP/1.1 200 OK");
    client.println("Content-type:text/html");
    client.println("Connection: close");
    client.println();

    client.println("<!DOCTYPE html><html>");
    client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
    client.println("<link rel=\"icon\" href=\"data:,\">");
    client.println("<meta charset=\"UTF-8\">");
    client.println("<meta http-equiv=\"refresh\" content=\"5\" >");
    //Bootstrap
    client.println("<link rel=\"stylesheet\" href=\"https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/css/bootstrap.min.css\" integrity=\"sha384-ggOyR0iXCbMQv3Xipma34MD+dH/1fQ784/j6cY/iJTQUOhcWr7x9JvoRxT2MZw1T\" crossorigin=\"anonymous\">");
    client.println("<link rel=\"stylesheet\" href=\"https://use.fontawesome.com/releases/v5.6.3/css/all.css\" integrity=\"sha384-UHRtZLI+pbxtHCWp1t77Bi1L4ZtiqrqD80Kn4Z8NTSRyMA2Fd33n5dQ8lWUE00s/\" crossorigin=\"anonymous\">");
    client.println("<script src=\"https://code.jquery.com/jquery-3.3.1.slim.min.js\" integrity=\"sha384-q8i/X+965DzO0rT7abK41JStQIAqVgRVzpbzo5smXKp4YfRvH+8abtTE1Pi6jizo\" crossorigin=\"anonymous\"></script>");
    client.println("<script src=\"https://cdnjs.cloudflare.com/ajax/libs/popper.js/1.14.7/umd/popper.min.js\" integrity=\"sha384-UO2eT0CpHqdSJQ6hJty5KVphtPhzWj9WO1clHTMGa3JDZwrnQq4sF86dIHNDz0W1\" crossorigin=\"anonymous\"></script>");
    client.println("<script src=\"https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/js/bootstrap.min.js\" integrity=\"sha384-JjSmVgyd0p3pXB1rRibZUAYoIIy6OrQ6VrjIEaFf/nJGzIxFDsf4x0xIM+B07jRM\" crossorigin=\"anonymous\"></script>");

    //CSS
    client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
    client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
    client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
    client.println(".button2 {background-color: #555555;}</style></head>");

    client.println("<body>");

}
void htmlFooter(){
    client.println("</body></html>");

    client.println();
}

void renderLedControl()
{
    htmlHeader();
    // turns the GPIOs on and off
    if (header.indexOf("GET /26/on") >= 0)
    {
        Serial.println("GPIO 26 on");
        output26State = "on";
        digitalWrite(output26, HIGH);
    }
    else if (header.indexOf("GET /26/off") >= 0)
    {
        Serial.println("GPIO 26 off");
        output26State = "off";
        digitalWrite(output26, LOW);
    }
    else if (header.indexOf("GET /27/on") >= 0)
    {
        Serial.println("GPIO 27 on");
        output27State = "on";
        digitalWrite(output27, HIGH);
    }
    else if (header.indexOf("GET /27/off") >= 0)
    {
        Serial.println("GPIO 27 off");
        output27State = "off";
        digitalWrite(output27, LOW);
    }

    // Display the HTML web page
    
    // CSS to style the on/off buttons
    
    // Web Page Heading
    client.println("<h1>Robot Remote Control Server</h1>");

    // Display current state, and ON/OFF buttons for GPIO 26
    client.println("<p>GPIO 26 - State " + output26State + "</p>");
    // If the output26State is off, it displays the ON button
    if (output26State == "off")
    {
        client.println("<p><a href=\"/26/on\"><button class=\"button\">ON</button></a></p>");
    }
    else
    {
        client.println("<p><a href=\"/26/off\"><button class=\"button button2\">OFF</button></a></p>");
    }

    // Display current state, and ON/OFF buttons for GPIO 27
    client.println("<p>GPIO 27 - State " + output27State + "</p>");
    // If the output27State is off, it displays the ON button
    if (output27State == "off")
    {
        client.println("<p><a href=\"/27/on\"><button class=\"button\">ON</button></a></p>");
    }
    else
    {
        client.println("<p><a href=\"/27/off\"><button class=\"button button2\">OFF</button></a></p>");
    }
    
    htmlFooter();
    
}

void renderDataPage(){
    htmlHeader();
    client.println("<p><h1>Data Page</h1></p>");
    client.println("<p>ddddd</p>");

    htmlFooter();
}


String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }
    String str = found > index ? data.substring(strIndex[0], strIndex[1]) : "" ;
   
  return str;
}
