#include <LittleFS.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>

const char* wifiSSID = "SCARA-Controller";  // WiFi network name
const char* wifiPassword = "123456789";     // WiFi password

// Global variables
String serialBuffer, webCommand;                                               // Incoming and processed data , ---
String lastSentData;                                                           // Last sent data to web
float PositionsX[100], PositionsY[100], PositionsZ[100], PositionsDelay[100];  // Position data (coordinates and delay)
float CodeX[1500], CodeY[1500], CodeZ[1500];                                   // Code coordinates (X, Y, Z)
char serialByte;                                                               // Received byte from serial input
int codeLineIndex = 0;                                                         // Code line index
int sendCodeCount = 1;                                                         // Current Code line sended to controller
int PositionCount = 0;                                                         // Number of stored Positions
int sendPositionCount = 1;                                                     // Current Position sended to controller
int PositionResendIndex = 0;                                                   // Position resend index (for web)
int settingResendIndex = 0;                                                    // setting resend index (for web)
bool isResendPositionActive = false;                                           // Resend Position status
bool isResendSettingActive = false;                                            // Resend setting status
bool movingByPosition = false;                                                 // Position movement status
bool movingByCode = false;                                                     // Code movement status


ESP8266WebServer webServer(80);

/**
        * Processes incoming serial data
        */
void processIncomingSerialData(String Data);

/**
        * Processes the "Ok" command
        */
void processOkCommand();

/**
        * Trims unnecessary data from the input buffer
        */
void trimDataBuffer();

/**
        * Handles incoming data request from the web server
        */
void handleReceiveDataRequest();

/**
        * Handles sending data to the web server
        */
void handleSendDataRequest();

/**
        * Processes the Code command
        * @param Code The Code command string
        */
void handleCode(String codeData);

/**
        * Processes the Position command (includes movement and delays)
        * @param PositionData The Position command string
        */
void handlePositions(String PositionData);

/**
        * Processes the setting command
        * @param settingData The setting command string
        */
void handleSetting(String settingData);

/**
        * Sends a Position to the Controller
        * @param lineIndex The Position index to be sent
        */
void sendPositionToController(int lineIndex);

/**
        * Sends a Code line to the Controller
        * @param lineIndex The Code line index to be sent
        */
void sendCodeToController(int lineIndex);

/**
        * Send data back to the web
        * @return The data of the resent Setting
        */
String getResentSettingData();

/**
        * Send data back to the web
        * @return The data of the resent Positions
        */
String getResentPositionsData();

/**
        * Extracts a numerical value from a Code & Positions string based on the given parameter
        * @param Data The command string
        * @param parameter The parameter to extract (X, Y, Z)
        * @return The extracted value
        */
float extractValueFromCode(String Data, String parameter);

/**
        * Saves a value to EEPROM
        * @param address The EEPROM address to save to
        * @param value The value to be saved
        */
void saveToEEPROM(int address, int value);

/**
        * Reads a value from EEPROM
        * @param address The EEPROM address to read from
        * @return The value read from EEPROM
        */
int readFromEEPROM(int address);

void setup() {
  Serial.begin(115200);
  EEPROM.begin(512);
  pinMode(LED_BUILTIN, OUTPUT);

  WiFi.softAP(wifiSSID, wifiPassword);  // Access Point

  Serial.println(WiFi.softAPIP());

  webServer.on("/", handleRoot);
  webServer.on("/Send", handleSendDataRequest);
  webServer.on("/Receive", handleReceiveDataRequest);
  webServer.begin();

  int defaultSettings[] = { 500, 0, 2, 500, 2, 500, 2, 500, 2, 300, 200 };
  for (int i = 0; i < sizeof(defaultSettings); i++) {
    saveToEEPROM(i, defaultSettings[i]);
  }
}

void loop() {
  webServer.handleClient();  // Handle incoming server requests

  if (Serial.available() > 0) {
    serialByte = Serial.read();
    serialBuffer += serialByte;
    digitalWrite(LED_BUILTIN, HIGH);

    if (serialByte == '\n') {
      webCommand = serialBuffer;
      processIncomingSerialData(serialBuffer);
      serialBuffer = "";
    }

    trimDataBuffer();
    delay(10);
  }
}

void processIncomingSerialData(String Data) {
  if (Data.indexOf("Code") != -1) {
    handleCode(Data);
  } else if (Data.indexOf("Positions") != -1) {
    handlePositions(Data);
  } else if (Data.indexOf("Setting") != -1) {
    handleSetting(Data);
  } else if (Data.indexOf("Ok") != -1) {
    processOkCommand();
  }
}

void processOkCommand() {
  if (movingByPosition == true) {
    if (sendPositionCount < PositionCount) {
      sendPositionCount++;
      sendPositionToController(sendPositionCount);
    } else {
      movingByPosition = false;
      sendPositionCount = 1;
      webCommand = "End positions move";
      if (Serial) {
        Serial.println("End");
      }
    }
  }
  if (movingByCode == true) {
    if (sendCodeCount < codeLineIndex) {
      sendCodeCount++;
      sendCodeToController(sendCodeCount);
    } else {
      movingByCode = false;
      sendCodeCount = 1;
      webCommand = "End code move";
      if (Serial) {
        Serial.println("End");
      }
    }
  }
}

void trimDataBuffer() {
  if (serialBuffer.length() > 200) {
    serialBuffer.remove(0, serialBuffer.length() - 200);  // Trim excess data
  }
}

void handleReceiveDataRequest() {
  webServer.sendHeader("Access-Control-Allow-Origin", "*");

  if (webServer.hasArg("code")) {
    String codeCom = webServer.arg("code");

    if (codeCom.indexOf("Pause") != -1) {
      if (Serial) {
        Serial.println("Code Pause");
      }

    } else if (codeCom.indexOf("pBack") != -1) {
      if (Serial) {
        Serial.println("Code pBack");
      }
    }

    if (codeCom.indexOf("EndSendingCode") != -1) {
      webCommand = "End";
    } else {
      handleCode(codeCom);
    }
    webServer.send(200, "text/plain", codeCom);

  } else if (webServer.hasArg("Positions")) {
    String positionCom = webServer.arg("Positions");

    if (positionCom.indexOf("Clear") != -1) {
      if (Serial) {
        Serial.println("Positions Clear");
      }

    } else if (positionCom.indexOf("Pause") != -1) {
      if (Serial) {
        Serial.println("Positions Pause");
      }

    } else if (positionCom.indexOf("pBack") != -1) {
      if (Serial) {
        Serial.println("Positions pBack");
      }
    }
    handlePositions(positionCom);
    webServer.send(200, "text/plain", positionCom);

  } else if (webServer.hasArg("move")) {
    String moveCom = webServer.arg("move");
    if (Serial) {
      Serial.println("Move = " + moveCom);
    }
    webServer.send(200, "text/plain", moveCom);

  } else if (webServer.hasArg("setting")) {
    String settingCom = webServer.arg("setting");

    handleSetting(settingCom);
    if (Serial) {
      Serial.println(settingCom);
    }
    webServer.send(200, "text/plain", settingCom);

  } else if (webServer.hasArg("BackEnd")) {
    String backEndCom = webServer.arg("BackEnd");

    if (backEndCom.indexOf("Refresh") != -1) {
      isResendSettingActive = true;
    }

    webServer.send(200, "text/plain", backEndCom);

  } else {
    webServer.send(400, "text/plain", "No command received.");
  }
}

void handleSendDataRequest() {
  webServer.sendHeader("Access-Control-Allow-Origin", "*");

  if (isResendSettingActive) {
    webCommand = getResentSettingData();
  } else if (isResendPositionActive) {
    webCommand = getResentPositionsData();
  }

  if (lastSentData != webCommand || webCommand == "End code move" || webCommand == "End positions move" || webCommand == "End") {
    lastSentData = webCommand;
    webServer.send(200, "text/plain", webCommand);
    webCommand = " ";
  } else {
    webServer.send(200, "text/plain", " ");
  }
}

void handleCode(String codeData) {
  if (codeData.indexOf("Run") != -1) {
    if (codeLineIndex > 0 && movingByCode != true) {
      movingByCode = true;
      sendCodeToController(sendCodeCount);
    }
  } else if (codeData.indexOf("Pause") != -1) {
    movingByCode = false;
  } else if (codeData.indexOf("pBack") != -1) {
    movingByCode = false;
    sendCodeCount = 1;
  } else if (codeData.indexOf("Send") != -1) {
    webCommand = "Send";
  } else if (codeData.indexOf("Clear") != -1) {
    codeLineIndex = 0;
    sendCodeCount = 1;
    movingByCode = false;
    for (int i = 0; i <= 1500; i++) {
      CodeX[i] = NAN;
      CodeY[i] = NAN;
      CodeZ[i] = NAN;
    }
  } else {
    codeLineIndex = extractValueFromCode(codeData, "(");
    CodeX[codeLineIndex] = extractValueFromCode(codeData, "X");
    CodeY[codeLineIndex] = extractValueFromCode(codeData, "Y");
    CodeZ[codeLineIndex] = extractValueFromCode(codeData, "Z");
  }
}

void handlePositions(String PositionData) {
  if (PositionData.indexOf("Run") != -1) {
    if (PositionCount > 0 && movingByPosition != true) {
      movingByPosition = true;
      sendPositionToController(sendPositionCount);
    }
  } else if (PositionData.indexOf("Pause") != -1) {
    movingByPosition = false;
  } else if (PositionData.indexOf("pBack") != -1) {
    movingByPosition = false;
    sendPositionCount = 1;
  } else if (PositionData.indexOf("Clear") != -1) {
    PositionCount = 0;
    sendPositionCount = 1;
    movingByPosition = false;
    for (int i = 0; i < 100; i++) {
      PositionsX[i] = NAN;
      PositionsY[i] = NAN;
      PositionsZ[i] = NAN;
      PositionsDelay[i] = NAN;
    }
  } else {
    PositionCount = extractValueFromCode(PositionData, "I");
    PositionsX[PositionCount] = extractValueFromCode(PositionData, "X");
    PositionsY[PositionCount] = extractValueFromCode(PositionData, "Y");
    PositionsZ[PositionCount] = extractValueFromCode(PositionData, "Z");
    PositionsDelay[PositionCount] = extractValueFromCode(PositionData, "T");
    if (Serial) {
      Serial.println("++");
    }
  }
}

void handleSetting(String settingData) {
  int address;
  int value;
  address = extractValueFromCode(settingData, "P");
  value = extractValueFromCode(settingData, "=");
  saveToEEPROM(address, value);
}

void sendPositionToController(int lineIndex) {
  char sendBuffer[50];
  if (lineIndex > 0 && PositionsDelay[lineIndex] > 0) {
    delay(PositionsDelay[lineIndex - 1]);
  }
  sprintf(sendBuffer, "Position I%d = X%.2f Y%.2f Z%.2f", lineIndex, PositionsX[lineIndex], PositionsY[lineIndex], PositionsZ[lineIndex]);
  if (Serial) {
    Serial.println(sendBuffer);
  }
}

void sendCodeToController(int lineIndex) {
  char sendBuffer[100];
  char tempBuffer[20];
  int offset = snprintf(sendBuffer, sizeof(sendBuffer), "Code I%d =", lineIndex);

  if (!isnan(CodeX[lineIndex])) {
    offset += snprintf(tempBuffer, sizeof(tempBuffer), " X%.2f", CodeX[lineIndex]);
    strncat(sendBuffer, tempBuffer, sizeof(sendBuffer) - offset - 1);
  }
  if (!isnan(CodeY[lineIndex])) {
    offset += snprintf(tempBuffer, sizeof(tempBuffer), " Y%.2f", CodeY[lineIndex]);
    strncat(sendBuffer, tempBuffer, sizeof(sendBuffer) - offset - 1);
  }
  if (!isnan(CodeZ[lineIndex])) {
    snprintf(tempBuffer, sizeof(tempBuffer), " Z%.2f", CodeZ[lineIndex]);
    strncat(sendBuffer, tempBuffer, sizeof(sendBuffer) - offset - 1);
  }
  if (Serial) {
    Serial.println(sendBuffer);
  }
}

String getResentSettingData() {
  char sendBuffer[40];
  if (settingResendIndex < 11) {
    settingResendIndex++;
    sprintf(sendBuffer, "Setting P%d =%d \n", (settingResendIndex - 1), readFromEEPROM(settingResendIndex - 1));
    return sendBuffer;
  } else {
    settingResendIndex = 0;
    isResendSettingActive = false;
    isResendPositionActive = true;
    return " ";
  }
}

String getResentPositionsData() {
  char buffer[50];
  if (PositionResendIndex < PositionCount) {
    PositionResendIndex++;
    sprintf(buffer, "Positions I%d = X%.2f Y%.2f Z%.2f T%.2f\n", PositionResendIndex, PositionsX[PositionResendIndex], PositionsY[PositionResendIndex], PositionsZ[PositionResendIndex], PositionsDelay[PositionResendIndex]);
    return buffer;
  } else {
    PositionResendIndex = 0;
    isResendPositionActive = false;
    return "Start";
  }
}

float extractValueFromCode(String Data, String parameter) {
  int index = Data.indexOf(parameter);
  if (index != -1) {
    return Data.substring(index + 1).toFloat();
  }
  return NAN;
}

void saveToEEPROM(int address, int value) {
  if (readFromEEPROM(address) == value) {
    return;
  } else {
    address = address * 2;
    EEPROM.write(address, value & 0xFF);             // LSB
    EEPROM.write(address + 1, (value >> 8) & 0xFF);  // MSB
    EEPROM.commit();
  }
}

int readFromEEPROM(int address) {
  address = address * 2;
  int lowByte = EEPROM.read(address);       // LSB
  int highByte = EEPROM.read(address + 1);  // MSB
  return (highByte << 8) | lowByte;         // MSB + LSB
}

void handleRoot() {
  if (!LittleFS.exists("/index.html")) {
    webServer.send(404, "text/plain", "File not found");
    Serial.println("File not found");
    return;
  }

  File file = LittleFS.open("/index.html", "r");
  webServer.streamFile(file, "text/html");
  file.close();
}
