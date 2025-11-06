//********************************************************************
//Author: Shariq Moghees
//Date: 11/5/2025
//Course: CMPE 311, Prof. Kidd, Fall 2025
//Filename: 311_project_2.ino
//********************************************************************

#define TRUE 1
#define FALSE 0
#define PIN_LED2 2
#define PIN_LED3 3

unsigned long delayStart2, delayStart3;
int ledOn2, ledOn3;
unsigned long intervalLed[14] = {1000}; //ms interval for each LED index
int waiting4led =FALSE, waiting4interval = FALSE;
int interval;
int setLedNum = 1;

//tasks declaration
void taskLED2(void);
void taskLED3(void);
void taskSerialHandler(void);

//tasklist for the cyclic executive table
void (*tasks[])(void) = {taskLED2, taskLED3, taskSerialHandler};
const int NUM_TASKS = sizeof(tasks)/sizeof(tasks[0]);

void setup() {
  // put your setup code here, to run once:
  //initialize
  Serial.begin(9600);
  pinMode(PIN_LED2, OUTPUT);
  pinMode(PIN_LED3, OUTPUT);

  ledOn2 = FALSE;
  ledOn3 = FALSE;
  digitalWrite(PIN_LED2, LOW);
  digitalWrite(PIN_LED3, LOW);

  delayStart2 = millis();
  delayStart3 = millis();
  intervalLed[2] = 1000;
  intervalLed[3] = 500;

  Serial.println("Cyclic Executtive Good to go");
  Serial.print("Select LED (enter a valid number): ");
  waiting4led = TRUE;
  waiting4interval = FALSE;
}

void loop() { 
//the loop will never actually call any LED logic, it just cycles through the array
// of pointers that point to different tasks that contain the subroutines
  // put your main code here, to run repeatedly:
  for(int i =0; i < NUM_TASKS; i++){ //round robin, move to the next index task pointer after every iteration
    (*tasks[i])(); //dispatch the tasks once per major cycle
  }
}

//TASK SUBROUTINES (this is where the actual logic is for all the different task in the cyclic executive)

//Blink LED # 2
void taskLED2(void){
  if((millis() - delayStart2) > intervalLed[2]){ //still using millis so we can keep the LEDs blinking asynchronously
    ledOn2 = !ledOn2;
    digitalWrite(PIN_LED2, ledOn2);
    delayStart2 = millis();
  }
}

//Blink LED # 3 //same tpye of logic as LED # 2
void taskLED3(void){
  if((millis() - delayStart3) > intervalLed[3]){
    ledOn3 = !ledOn3;
    digitalWrite(PIN_LED3, ledOn3);
    delayStart3 = millis();
  }
}

//Serial Input Handler
void taskSerialHandler(void){
  if (Serial.available() != 0) {
    if (waiting4led == TRUE) {
      setLedNum = Serial.parseInt();
      Serial.print("Specify an interval (ms): ");
      waiting4led = FALSE;
      waiting4interval = TRUE;
    }else if (waiting4interval == TRUE) {
      interval = Serial.parseInt();
      intervalLed[setLedNum] = interval;
      Serial.print("LED");
      Serial.print(setLedNum);
      Serial.print(" interval set to ");
      Serial.print(interval);
      Serial.println(" ms");
      Serial.print("Select LED (enter a valid number): ");
      waiting4interval = FALSE;
      waiting4led = TRUE;
    }else {
      Serial.println("INVALID INPUT: neither LED# nor interval state");
    }
  }
}
