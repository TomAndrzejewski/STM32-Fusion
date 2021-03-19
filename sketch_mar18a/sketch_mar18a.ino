// Motor encoder output pulse per rotation (change as required)
// -------------------- !!! CHANGE TO PROPER NUMBER !!! ----------------------
#define ENC_COUNT_REV 1
 
// Encoder output to Arduino Interrupt pin
#define ENC_1_IN_1 2 
#define ENC_1_IN_2 3
#define ENC_2_IN_1 20
#define ENC_2_IN_2 20
 
// Engines driver PWM steering pins
#define PWM_1 6 
#define PWM_2 7

// Przyciski zmieniajace kierunek obrotu silnikow
#define SWITCH_DIR_1 18
#define SWITCH_DIR_2 19

// Diody LED sygnalizujace odczyt kierunku obrotu silnikow z enkoderow
#define ENC_1_DIR_INDICATOR 36
#define ENC_2_DIR_INDICATOR 34

// Wyjscia do sterownika silnikow, informujace o tym, jaki ma byc kierunek obrotu kazdego silnika
#define MOTOR_1_DIR 32
#define MOTOR_2_DIR 30


// Encoder turning FORWARD
#define FORWARD 1
// Encoder turning BACKWARD
#define BACKWARD 0

// Analog pin for potentiometer
int speedIndicator_1 = 0;
int speedIndicator_2 = 0;
 
// Pulse count from encoder
volatile long encoder_1_Value_1 = 0;
volatile long encoder_1_Value_2 = 0;
volatile long encoder_1_Value = 0;
volatile long encoder_2_Value_1 = 0;
volatile long encoder_2_Value_2 = 0;
volatile long encoder_2_Value = 0;
 
// One-second interval for measurements
int interval = 1000;
 
// Counters for milliseconds during interval
long previousMillis = 0;
long updateEncoder_1_1_Millis = 0;
long updateEncoder_1_2_Millis = 0;
long currentMillis = 0;
 
// Variable for RPM measuerment
int rpm_1 = 0;
int rpm_2 = 0;
 
// Variable for PWM motor speed output
int motorPwm_1 = 0;
int motorPwm_2 = 0;

// Variable for direction of engines read from encoders
int motorDir_1 = 0;
int motorDir_2 = 0;
 
void setup()
{
  // Setup Serial Monitor
  Serial.begin(57600); 
  
  // Set encoders as input with internal pullup  
  pinMode(ENC_1_IN_1, INPUT_PULLUP); 
  pinMode(ENC_1_IN_2, INPUT_PULLUP); 
  pinMode(ENC_2_IN_1, INPUT_PULLUP); 
  pinMode(ENC_2_IN_2, INPUT_PULLUP); 

  // Set switches as input with internal pullup
  pinMode(SWITCH_DIR_1, INPUT_PULLUP);
  pinMode(SWITCH_DIR_2, INPUT_PULLUP);
  
  // Set PWM, DIR connections as outputs
  pinMode(PWM_1, OUTPUT);
  pinMode(PWM_2, OUTPUT);
  pinMode(MOTOR_1_DIR, OUTPUT);
  pinMode(MOTOR_2_DIR, OUTPUT);
  pinMode(ENC_1_DIR_INDICATOR, OUTPUT);
  pinMode(ENC_2_DIR_INDICATOR, OUTPUT);

  // Set interrupts of encoders
  attachInterrupt(digitalPinToInterrupt(ENC_1_IN_1), updateEncoder_1_1, RISING);
  attachInterrupt(digitalPinToInterrupt(ENC_1_IN_2), updateEncoder_1_2, RISING);
  attachInterrupt(digitalPinToInterrupt(ENC_2_IN_1), updateEncoder_2_1, RISING);
  attachInterrupt(digitalPinToInterrupt(ENC_2_IN_2), updateEncoder_2_2, RISING);

  // Set interrupts of switches
  attachInterrupt(digitalPinToInterrupt(SWITCH_DIR_1), changeDir_1, RISING);
  attachInterrupt(digitalPinToInterrupt(SWITCH_DIR_2), changeDir_2, RISING);

  // Set initial values for all outputs
  analogWrite(PWM_1, 0);
  analogWrite(PWM_2, 0);
  digitalWrite(MOTOR_1_DIR, LOW);
  digitalWrite(MOTOR_2_DIR, LOW);
  digitalWrite(ENC_1_DIR_INDICATOR, LOW);
  digitalWrite(ENC_2_DIR_INDICATOR, LOW);
  
  // Setup initial values for timer
  previousMillis = millis();
}
 
void loop()
{
  
  // Control motor with potentiometer
  motorPwm_1 = map(analogRead(speedIndicator_1), 0, 1023, 0, 255);
  motorPwm_2 = map(analogRead(speedIndicator_2), 0, 1023, 0, 255);
    
  // Write PWM to controller
  analogWrite(PWM_1, motorPwm_1);
  analogWrite(PWM_2, motorPwm_2);
  
  currentMillis = millis();


  // Update RPM value every second
  if (currentMillis - previousMillis > interval) {
    previousMillis = currentMillis;
 
 
    // Calculate RPM
    encoder_1_Value = encoder_1_Value_1 + encoder_1_Value_2;
    encoder_1_Value = encoder_1_Value_1 + encoder_1_Value_2;
    
    rpm_1 = (float)(encoder_1_Value * 60 / ENC_COUNT_REV);
    rpm_2 = (float)(encoder_1_Value * 60 / ENC_COUNT_REV);

    // Check direction of encoders
    if (encoder_1_Value_1 > encoder_1_Value_2)    // --------- CHECK IF IT IS RIGHT!!! -----------
    {
      motorDir_1 = FORWARD;
    }
    else
    {
      motorDir_2 = BACKWARD;
    }
    
 
    // Only update display when there is a reading
    if (motorPwm_1 > 0 || rpm_1 > 0 || motorPwm_2 > 0 || rpm_2 > 0) 
    {
      // Engine 1
      Serial.print("ENGINE 1:");
      Serial.print('\n');
      Serial.print("PWM VALUE: ");
      Serial.print(motorPwm_1);
      Serial.print('\t');
      Serial.print(" PULSES: ");
      Serial.print(encoder_1_Value);
      Serial.print('\t');
      Serial.print(" DIRECTION: ");
      if (motorDir_1 == FORWARD)
        Serial.print("Forward");
      else
        Serial.print("Backward");
      Serial.print('\t');
      Serial.print(" SPEED: ");
      Serial.print(rpm_1);
      Serial.println(" RPM");

      // Engine 2
      Serial.print("ENGINE 2:");
      Serial.print('\n');
      Serial.print("PWM VALUE: ");
      Serial.print(motorPwm_2);
      Serial.print('\t');
      Serial.print(" PULSES: ");
      Serial.print(encoder_2_Value);
      Serial.print('\t');
      Serial.print(" DIRECTION: ");
      if (motorDir_2 == FORWARD)
        Serial.print("Forward");
      else
        Serial.print("Backward");
      Serial.print('\t');
      Serial.print(" SPEED: ");
      Serial.print(rpm_2);
      Serial.println(" RPM");
    }

    // Reset encoder ticks
    encoder_1_Value_1 = 0;
    encoder_1_Value_2 = 0;
    encoder_2_Value_1 = 0;
    encoder_2_Value_2 = 0; 
  }
}
 
void updateEncoder_1_1()
{
  // Increment value for each pulse from encoder
  encoder_1_Value_1++;
}

void updateEncoder_1_2()
{
  // Increment value for each pulse from encoder
  encoder_1_Value_2++;
}

void updateEncoder_2_1()
{
  // Increment value for each pulse from encoder
  encoder_1_Value_1++;
}

void updateEncoder_2_2()
{
  // Increment value for each pulse from encoder
  encoder_1_Value_2++;
}

void changeDir_1()
{
  // Sprawdzenie poprzedniego kierunku i zmiana go na przeciwny
  if (digitalRead(MOTOR_1_DIR) == HIGH)   // --------- CHECK WHAT TURN IS "HIGH" OUTPUT!!! ---------
  {
    digitalWrite(MOTOR_1_DIR, LOW);
    digitalWrite(ENC_1_DIR_INDICATOR, LOW);
  }
  else
  {
    digitalWrite(MOTOR_1_DIR, HIGH);
    digitalWrite(ENC_1_DIR_INDICATOR, HIGH);
  }
}

void changeDir_2()
{
  // Sprawdzenie poprzedniego kierunku i zmiana go na przeciwny
  if (digitalRead(MOTOR_2_DIR) == HIGH)   // --------- CHECK WHAT TURN IS "HIGH" OUTPUT!!! ---------
  {
    digitalWrite(MOTOR_2_DIR, LOW);
    digitalWrite(ENC_2_DIR_INDICATOR, LOW);
  }
  else
  {
    digitalWrite(MOTOR_2_DIR, HIGH);
    digitalWrite(ENC_2_DIR_INDICATOR, HIGH);
  }
}
