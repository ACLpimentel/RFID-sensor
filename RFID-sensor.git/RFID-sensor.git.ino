
#include <SPI.h>
#include <MFRC522.h>


#define SS_PIN 9
#define RST_PIN 8
#define buzzer 4
#define lock 5

MFRC522  mfrc522(SS_PIN, RST_PIN);

void rfid_func();

void setup() {
  // put your setup code here, to run once:

  pinMode(buzzer, OUTPUT);
  pinMode(lock,OUTPUT);

  digitalWrite(buzzer ,LOW);
  digitalWrite(lock ,LOW);

  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();

  Serial.println("Trava eletronica");
  Serial.println("aproxime as tags do leitor...");
  Serial.println();

}

void loop() {
  // put your main code here, to run repeatedly:

  rfid_func();

}

void rfid_func()
{
  if(!mfrc522.PICC_IsNewCardPresent()) return;

  if(!mfrc522.PICC_ReadCardSerial()) return;

  Serial.print("UID da tag: ");
  String conteudo= "";
  byte letra;
  for (byte i =0; i < mfrc522.uid.size; i++)
  {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    conteudo.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    conteudo.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.print("Mensagem: ");
  conteudo.toUpperCase();

  if (conteudo.substring(1) == "0C A6 22 49" 
  
  )
  {
    Serial.print("Acesso liberado!");
    Serial.println();

    //Aciona buzzer 1 vez
    digitalWrite(buzzer,HIGH);
    delay(600);
    digitalWrite(buzzer,LOW);
    delay(150);

    //libera trava
    digitalWrite(lock,HIGH);
    delay(3000);
    digitalWrite(lock,LOW);
  
  } // end if

  else
  {
    Serial.println("Acesso Negado: ");
    Serial.println();

    // Aciona buzzer 2 vezes
    digitalWrite(buzzer,HIGH);
    delay(150);
    digitalWrite(buzzer,LOW);
    delay(150);
    digitalWrite(buzzer,HIGH);
    delay(150);
    digitalWrite(buzzer,LOW);
    delay(150);
  } // end else

} // end rfid_func
