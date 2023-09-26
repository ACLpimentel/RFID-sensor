#define pino_botao_le A1
#define pino_botao_gr A2
#define pino_uid A4
#define pino_del A3
#define buzzer 10


void setup() {

  pinMode(pino_botao_le, INPUT);
  pinMode(pino_botao_gr, INPUT);
  pinMode(pino_uid, INPUT);
  pinMode(pino_del, INPUT);
  Serial.begin(9600);
  Serial.print("begim");
  // put your setup code here, to run once:

}

void loop() {

  int test = digitalRead(pino_uid);
  Serial.print(test);
  // put your main code here, to run repeatedly:

}
