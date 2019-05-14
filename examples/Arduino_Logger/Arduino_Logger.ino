namespace {
  const char * TOKEN = "<token>";
  const int MEASURE = 0;
  const int SEND_MEASUREMENTS = 1;
  const int READ_ROOM_DATA = 2;
  const int NUM_STATES = 3;
  const int CUARTO1_PIN = 2;
  const int CUARTO2_PIN = 3;
  const int CUARTO3_PIN = 4;
  const int CUARTO4_PIN = 5;
  const int CUARTO5_PIN = 6;
  const int CUARTO6_PIN = 7;
  const int CUARTO7_PIN = 8;
  const int CUARTO8_PIN = 9;
}

int currentPowerConsumption = 0;

int currentState = 0;

bool waitingForResponse = false;
bool waitingForRoomData = false;

char telemetryUnitResponse[100];

void setup() {
  Serial.begin(115200);

  pinMode(2, OUTPUT);
  digitalWrite(2, LOW);

  pinMode(3, OUTPUT);
  digitalWrite(3, LOW);

  pinMode(4, OUTPUT);
  digitalWrite(4, LOW);

  pinMode(5, OUTPUT);
  digitalWrite(5, LOW);

  pinMode(6, OUTPUT);
  digitalWrite(6, LOW);

  pinMode(7, OUTPUT);
  digitalWrite(7, LOW);

  pinMode(8, OUTPUT);
  digitalWrite(8, LOW);

  pinMode(9, OUTPUT);
  digitalWrite(9, LOW);

  delay(10000);
}

void loop() {
  if (!waitingForResponse) {
    switch (currentState) {
      case MEASURE:
        measure();
        break;
      case SEND_MEASUREMENTS:
        sendMeasurements();
        waitingForResponse = true;
        break;
      case READ_ROOM_DATA:
        requestRoomData();
        waitingForResponse = true;
        waitingForRoomData = true;
        break;
    }
    currentState = (currentState + 1)%NUM_STATES;
  } else {
    readResponseData();
  }
}

void measure() {

}

void sendMeasurements() {
  char* command = (char *) malloc(sizeof(char) * 128);
  char varInString[10];
  dtostrf(currentPowerConsumption, 4, 2, varInString);

  sprintf(command, "init#POST|%s|consumo:%s|end#final", TOKEN, varInString);
  Serial.print(command);

  free(command);

  waitingForResponse = true;
}

void requestRoomData() {
  char* command = (char *) malloc(sizeof(char) * 128);

  sprintf(command, "init#GET|%s|cuarto1,cuarto2,cuarto3,cuarto4,cuarto5,cuarto6,cuarto7,cuarto8|end#final", TOKEN);
  Serial.print(command);

  free(command);

  waitingForResponse = true;
  waitingForRoomData = true;
}

void readResponseData() {
  int i = 0;
  int dotCounter = 0;
  while (Serial.available() > 0) {
    char letter = (char) Serial.read();
    if (letter == '.') {
      dotCounter++;
    }
    if (3 < dotCounter) {
      delay(5000);
      currentState = MEASURE;
      break;
    }
    telemetryUnitResponse[i++] = letter;
    waitingForResponse = false;
  }

  if (!waitingForResponse && waitingForRoomData) {
    delay(200);
    char* response = strtok(telemetryUnitResponse, " ");

    if (strncmp(response,"OK", 2) == 0) {
      int i = 2;
      while (i < 10) {
        response = strtok(NULL, " ");
        if (strncmp(response, "0", 1) == 0) {
          digitalWrite(i, LOW);
        } else {
          digitalWrite(i, HIGH);
        }
        i++;
      }
    }
    waitingForRoomData = false;
  }

  delay(1000);
}
