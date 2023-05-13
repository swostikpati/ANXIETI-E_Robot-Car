#define SENS1_PIN A4
#define SENS2_PIN A5

int SENS1_val;
int SENS2_val;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(SENS1_PIN,INPUT);
 pinMode(SENS2_PIN,INPUT);

}

void loop() {
  // put your main code here, to run repeatedly:
  
 SENS1_val=digitalRead(SENS1_PIN);
 SENS2_val=digitalRead(SENS2_PIN);

 Serial.println(SENS1_val);

}
