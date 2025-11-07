//********************************************************************
//Author: Shariq Moghees
//Date: 11/5/2025
//Course: CMPE 311, Prof. Kidd, Fall 2025
//Filename: 311_project_2
//********************************************************************

#define TRUE 1
#define FALSE 0
#define PIN_LED1 2   // LED 1 connected to digital pin 2
#define PIN_LED2 3   // LED 2 connected to digital pin 3

unsigned long delayStart1, delayStart2;
int ledOn1, ledOn2;
unsigned long intervalLed[3] = {0, 1000, 500}; // index 1 is LED1 and  index 2 is LED2
int waiting4led = FALSE, waiting4interval = FALSE;
int interval;
int setLedNum = 1;

//tasks declaration
void taskLED1(void);
void taskLED2(void);
void taskSerialHandler(void);

//tasklist for the CE
void (*tasks[])(void) = {taskLED1, taskLED2, taskSerialHandler};
const int NUM_TASKS = sizeof(tasks) / sizeof(tasks[0]);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(PIN_LED1, OUTPUT);
  pinMode(PIN_LED2, OUTPUT);

  ledOn1 = FALSE;
  ledOn2 = FALSE;
  digitalWrite(PIN_LED1, LOW);
  digitalWrite(PIN_LED2, LOW);

  delayStart1 = millis();
  delayStart2 = millis();

  Serial.println("Cyclic Executive Good to go");
  Serial.print("Select LED and set interval (Format: <LED#> <interval_ms>): ");
  waiting4led = TRUE;
  waiting4interval = FALSE;
}

void loop() { 
  //the loop will never actually call any LED logic, it just cycles through the array
  //of pointers that point to different tasks that contain the subroutines
  for (int i = 0; i < NUM_TASKS; i++) { // round robin
    (*tasks[i])(); // dispatch the tasks once per major cycle
  }
}

//TASK SUBROUTINES (this is where the actual logic is for all the different tasks in the cyclic executive)

//Blink LED #1
void taskLED1(void) {
  if ((millis() - delayStart1) > intervalLed[1]) { // still using millis so we can keep LEDs blinking asynchronously
    ledOn1 = !ledOn1;
    digitalWrite(PIN_LED1, ledOn1);
    delayStart1 = millis();
  }
}

//Blink LED #2 (same type of logic as LED #1)
void taskLED2(void) {
  if ((millis() - delayStart2) > intervalLed[2]) {
    ledOn2 = !ledOn2;
    digitalWrite(PIN_LED2, ledOn2);
    delayStart2 = millis();
  }
}

//Serial Input Handler
void taskSerialHandler(void) {
  static String inputString = "";  // buffer to collect serial input

  while (Serial.available()) {
    char inChar = (char)Serial.read();
    // End of line, so process full input
    if (inChar == '\n') { //only trigger on newline to prevent duplicate reads
      inputString.trim();
      if (inputString.length() > 0) {
        // Split with a space to take in LED number and interval at the same time
        int spaceIndex = inputString.indexOf(' ');
        if (spaceIndex > 0) {
          int ledNum = inputString.substring(0, spaceIndex).toInt();
          int interval = inputString.substring(spaceIndex + 1).toInt();

          if (ledNum >= 1 && ledNum <= 2 && interval > 0) {
            intervalLed[ledNum] = interval;
            Serial.print("LED");
            Serial.print(ledNum);
            Serial.print(" interval set to ");
            Serial.print(interval);
            Serial.println(" ms"); //newline
          } else {
            Serial.println("Invalid entry. Use: <LED#> <interval_ms>");
          }
        } else {
          Serial.println("Format: <LED#> <interval_ms>");
        }
      }
      Serial.print("\nSelect LED (enter a valid number): "); // added newline for clean formatting
      inputString = "";
    } else {
      inputString += inChar; // keep collecting characters
    }
  }
}
