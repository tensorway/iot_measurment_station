#define PUMP 32
#define LED_BUILTIN 2


// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin PUMP as an output.
  pinMode(PUMP, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  delay(1000);
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(PUMP, HIGH);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)

  delay(1000);                       // wait for a second
  digitalWrite(PUMP, LOW);    // turn the LED off by making the voltage LOW
  digitalWrite(LED_BUILTIN, LOW);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
}
