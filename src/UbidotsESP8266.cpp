/*
Copyright (c) 2017 Ubidots.

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPY RIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Made by: ----- María Carlina Hernández ---- Developer at Ubidots Inc 
         https://github.com/mariacarlinahernandez  
         ----- Jose Garcia ---- Developer at Ubidots Inc 
         https://github.com/jotathebest
*/

#include "UbidotsESP8266.h"

/***************************************************************************
CONSTRUCTOR
***************************************************************************/

/**
 * Constructor.
 */

Ubidots::Ubidots(char* token, const char* server) {
    _token = token;
    _server = server;
}

/***************************************************************************
FUNCTIONS TO MANAGE DATA
***************************************************************************/

/**
 * This function is to read the Ubidots' command via serial
 */
void Ubidots::readData() {

    int i = 0;
    while (Serial.available() > 0) {
      this->_command[i++] = (char) Serial.read();
    }

    i = 0;
    readServer();
    memset(this->_command, 0, sizeof(this->_command));
}

/**
 * This function establish the connection and send the request to the server
 */
uint8_t Ubidots::sendData() {   
    
    uint8_t max_retries = 0;
    uint8_t orderCode = checkCommand();

    if (orderCode > 0) {
        while (!_client.connected()){
            _client.connect(SERVER, PORT);
            max_retries++;
            if (max_retries>5) {
                Serial.println("ERROR");
                break;
            }

            if (_client.connected()) {
                if (orderCode == 1) {
                    Serial.println("OK, connected");
                    break;
                } 

                else if (orderCode == 2) {
                    _client.print(_request);
                    break;
                } 

                else {
                    break;
                }
            }
            delay(5000);
        } 
    }
    return orderCode;
}

/**
 * This function makes the request to the server
 */
void Ubidots::readServer() {

    uint8_t orderCode = sendData();

    if (orderCode > 1) {

        String response;
        int timeout = 0;

        while (!_client.available() && timeout < 5000) {
            timeout++;
            delay(1);
            if (timeout>=5000) {
                Serial.println(F("Error, max timeout reached. Send the request again"));
                _client.stop();
                break;
            }
        }

        while (_client.available()) {
            char c = _client.read();
            if(c == -1){
                Serial.println("ERROR");
                _client.stop();
                break;
            }
            response += c;
        }

        Serial.println(response);
        _client.stop();
    }
}

/**
 * This function verify the command sent and build the request to the Ubidots server
 */
uint8_t Ubidots::checkCommand() {

    char* command;
    char* response = "";
    int i = 0;
    uint8_t flag = 0;


    if ((strcmp(this->_command,"") == 0)) {
        return 0;
    }

    response = strtok(this->_command, "#");

    while (response != NULL) {

        if (i == 0) {
            if (strcmp(response,"init") != 0) {
                Serial.println("Wrong command. Verify the command and send it again.");
                return false;
            }
        }

        if (i == 1) {
            if (strcmp(response, "ONLINE?") == 0) {
                flag = 1;
            } else {
                _request = response;
                flag = 2;
            }
        }

        if (i == 2) {
            if (strcmp(response,"final") != 0) {
                Serial.println("Wrong command. Verify the command and send it again.");
                return 0;
            }
        }

        response = strtok(NULL, "#");
        i++;

    }
    return flag;
}

/***************************************************************************
AUXILIAR FUNCTIONS
***************************************************************************/

/**
 * This function is to set the WiFi connection
 * @arg ssid of the WiFi
 * @arg pass of the WiFi
 */
bool Ubidots::wifiConnection(char* ssid, char* pass) {
    WiFi.begin(ssid, pass);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    WiFi.setAutoReconnect(true);
    Serial.println(F("WiFi connected"));
    Serial.println(F("IP address: "));
    Serial.println(WiFi.localIP());
}