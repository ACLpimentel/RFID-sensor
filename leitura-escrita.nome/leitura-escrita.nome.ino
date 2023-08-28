//Programa: Leitura e gravacao de cartoes RFID
//Autor: MakerHero
#include <Adafruit_Sensor.h>
#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>
#include <locale.h>

//Pinos Reset e SS módulo MFRC522
#define SS_PIN 9
#define RST_PIN 8
MFRC522 mfrc522(SS_PIN, RST_PIN);

LiquidCrystal_I2C lcd(0x3F, 16, 2);

#define pino_botao_le A4
#define pino_botao_gr A2
#define pino_uid A3

#define buzzer 4

MFRC522::MIFARE_Key key;

void setup() {
  pinMode(pino_botao_le, INPUT);
  pinMode(pino_botao_gr, INPUT);
  Serial.begin(9600);  //Inicia a serial
  SPI.begin();         //Inicia  comunicação via SPI
  mfrc522.PCD_Init();  //Inicia MFRC522

  pinMode(buzzer, OUTPUT);
  digitalWrite(buzzer, LOW);

  dump_byte_array(key.keyByte, MFRC522::MF_KEY_SIZE);

  //Inicializa o LCD 16x2
  lcd.init();
  lcd.backlight();
  mensageminicial();

  //Prepara chave - padrao de fabrica = FFFFFFFFFFFFh
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;
}

void loop() {
  //Verifica se o botao modo leitura foi pressionado
  int modo_le = digitalRead(pino_botao_le);  // modo_le recebe valor do botão de leitura
  if (modo_le != 0)                          // se for verdadeiro o modo leitura inicia
  {
    lcd.clear();
    Serial.println("Modo leitura selecionado");
    lcd.setCursor(2, 0);
    lcd.print("Modo leitura");
    lcd.setCursor(3, 1);
    lcd.print("selecionado");
    while (digitalRead(pino_botao_le) == 1) {}  // grante que o codigo posterior so é executado depois de o botão ser solto
    delay(3000);
    modo_leitura();
  }
  //Verifica se o botao modo gravacao foi pressionado
  int modo_gr = digitalRead(pino_botao_gr);
  if (modo_gr != 0) {
    lcd.clear();
    Serial.println("Modo gravacao selecionado");
    lcd.setCursor(2, 0);
    lcd.print("Modo gravacao");
    lcd.setCursor(3, 1);
    lcd.print("selecionado");
    while (digitalRead(pino_botao_gr) == 1) {}
    delay(3000);
    modo_gravacao();
  }

  int modo_uid = digitalRead(pino_uid);
  if (modo_uid != 0) {
    lcd.clear();
    Serial.println("Modo leitura UID");
    lcd.setCursor(2, 0);
    lcd.print("Modo UID");
    lcd.setCursor(3, 1);
    lcd.print("selecionado");
    while (digitalRead(pino_uid) == 1) {}
    delay(3000);
    modo_cod();
  }
}
void mensageminicial()  // tela de seleção
{
  Serial.println("nSelecione o modo leitura ou gravacao...");
  Serial.println();
  lcd.clear();
  lcd.print("Selecione o modo");
  lcd.setCursor(0, 1);
  lcd.print("ler/gravar/UID");
}

void mensagem_inicial_cartao() {
  Serial.println("Aproxime o seu cartao do leitor...");
  lcd.clear();
  lcd.print(" Aproxime o seu");
  lcd.setCursor(0, 1);
  lcd.print("cartao do leitor");
}

void modo_leitura() {
  mensagem_inicial_cartao();
  //Aguarda cartao
  while (!mfrc522.PICC_IsNewCardPresent()) {
    delay(100);
  }

  // enquanto " !mfrc522.PICC_ReadCardSerial()" for true o codigo não seguirá em frente
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;  // volta para o local do codigo onde o bloco de codigo foi chamado
  }

  digitalWrite(buzzer, HIGH);
  delay(200);
  digitalWrite(buzzer, LOW);

  del_data();

  //Mostra UID na serial
  Serial.print("UID da tag : ");
  String conteudo = "";  // variavel que recebe uid
  byte letra;
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    conteudo.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    conteudo.concat(String(mfrc522.uid.uidByte[i], HEX));
    // variavel conteudo armazena valor do UID
  }
  Serial.println();

  //Obtem os dados do setor 1, bloco 4 = Nome
  byte sector = 1;
  byte blockAddr = 4;
  byte trailerBlock = 7;
  byte status;
  byte buffer[18];
  byte size = sizeof(buffer);

  //Autenticacao usando chave A
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A,
                                    trailerBlock, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("PCD_Authenticate() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  status = mfrc522.MIFARE_Read(blockAddr, buffer, &size);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("MIFARE_Read() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
  }
  //Mostra os dados do nome no Serial Monitor e LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  for (byte i = 1; i < 16; i++) {
    Serial.print(char(buffer[i]));
    lcd.write(char(buffer[i]));
  }
  Serial.println();

  //Obtem os dados do setor 0, bloco 1 = Sobrenome
  sector = 0;
  blockAddr = 1;
  trailerBlock = 3;

  //Autenticacao usando chave A
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A,
                                    trailerBlock, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("PCD_Authenticate() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  status = mfrc522.MIFARE_Read(blockAddr, buffer, &size);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("MIFARE_Read() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
  }
  //Mostra os dados do sobrenome no Serial Monitor e LCD
  lcd.setCursor(0, 1);
  for (byte i = 1; i < 16; i++)  // !!!! CHANGE THE VALUE OF i FROM 0 TO 1
  {
    Serial.print(char(buffer[i]));
    lcd.write(char(buffer[i]));
  }
  Serial.println();

  // Halt PICC
  mfrc522.PICC_HaltA();
  // Stop encryption on PCD
  mfrc522.PCD_StopCrypto1();
  delay(3000);

  mensageminicial();
}

void modo_gravacao() {
  mensagem_inicial_cartao();
  //Aguarda cartao ser detectado
  while (!mfrc522.PICC_IsNewCardPresent()) {
    delay(100);
  }
  if (!mfrc522.PICC_ReadCardSerial()) return;

  digitalWrite(buzzer, HIGH);
  delay(200);
  digitalWrite(buzzer, LOW);

  //Mostra UID na serial
  Serial.print(F("UID do Cartao: "));  //Dump UID
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    // conversão de base 10 em hexa
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
  }
  //Mostra o tipo do cartao
  Serial.print(F("nTipo do PICC: "));
  byte piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
  Serial.println(mfrc522.PICC_GetTypeName(piccType));

  byte buffer[34];
  byte block;
  byte status, len;

  Serial.setTimeout(20000L);
  Serial.println(F("Digite o sobrenome,em seguida o caractere #"));
  lcd.clear();
  lcd.print("Digite o sobreno");
  lcd.setCursor(0, 1);
  lcd.print("me + #");
  len = Serial.readBytesUntil('#', (char *)buffer, 30);
  for (byte i = len; i < 30; i++) buffer[i] = ' ';

  block = 1;
  //Serial.println(F("Autenticacao usando chave A..."));
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A,
                                    block, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("PCD_Authenticate() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  //Grava no bloco 1
  status = mfrc522.MIFARE_Write(block, buffer, 16);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("MIFARE_Write() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  block = 2;
  //Serial.println(F("Autenticacao usando chave A..."));
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A,
                                    block, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("PCD_Authenticate() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  //Grava no bloco 2
  status = mfrc522.MIFARE_Write(block, &buffer[16], 16);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("MIFARE_Write() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  Serial.println(F("Digite o nome, em seguida o caractere #"));
  lcd.clear();
  lcd.print("Digite o nome e");
  lcd.setCursor(0, 1);
  lcd.print("em seguida #");
  len = Serial.readBytesUntil('#', (char *)buffer, 20);
  for (byte i = len; i < 20; i++) buffer[i] = ' ';

  block = 4;
  //Serial.println(F("Autenticacao usando chave A..."));
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A,
                                    block, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("PCD_Authenticate() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  //Grava no bloco 4
  status = mfrc522.MIFARE_Write(block, buffer, 16);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("MIFARE_Write() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  block = 5;
  //Serial.println(F("Authenticating using key A..."));
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A,
                                    block, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("PCD_Authenticate() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  //Grava no bloco 5
  status = mfrc522.MIFARE_Write(block, &buffer[16], 16);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("MIFARE_Write() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    //return;
  } else {
    Serial.println(F("Dados gravados com sucesso!"));
    lcd.clear();
    lcd.print("Gravacao OK!");
  }

  mfrc522.PICC_HaltA();       // Halt PICC
  mfrc522.PCD_StopCrypto1();  // Stop encryption on PCD
  delay(5000);
  mensageminicial();
}

void modo_cod() {
  mensagem_inicial_cartao();
  //Aguarda cartao
  while (!mfrc522.PICC_IsNewCardPresent()) {
    delay(100);
  }

  // enquanto " !mfrc522.PICC_ReadCardSerial()" for true o codigo não seguirá em frente
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;  // volta para o local do codigo onde o bloco de codigo foi chamado
  }

  digitalWrite(buzzer, HIGH);
  delay(200);
  digitalWrite(buzzer, LOW);

  //Mostra UID na serial
  Serial.print("UID da tag : ");
  String conteudo = "";  // variavel que recebe uid
  byte letra;
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    conteudo.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    conteudo.concat(String(mfrc522.uid.uidByte[i], HEX));
    // variavel conteudo armazena valor do UID
  }
  Serial.println("####### fim conversao");
  Serial.println();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Numero do cartao:");
  lcd.setCursor(0, 1);
  lcd.print(conteudo);
  delay(5000);

  mensageminicial();
}

void del_data() {

  
  dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
  
  MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
  Serial.println(mfrc522.PICC_GetTypeName(piccType));

  // Check for compatibility
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI
      && piccType != MFRC522::PICC_TYPE_MIFARE_1K
      && piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    Serial.println(F("This sample only works with MIFARE Classic cards."));
    return;
  }

  // In this sample we use the second sector,
  // that is: sector #1, covering block #4 up to and including block #7

  for (byte sector_var = 0; sector_var < 16; sector_var++) {
      for (byte block_var = 1; block_var < 5; block_var)
      {
          byte sector = sector_var;
          byte blockAddr = block_var;
          byte dataBlock[] = {
            0x00, 0x00, 0x00, 0x00,  //  1,  2,   3,  4,
            0x00, 0x00, 0x00, 0x00,  //  5,  6,   7,  8,
            0x00, 0x00, 0x00, 0x00,  //  9, 10, 255, 12,
            0x00, 0x00, 0x00, 0x00   // 13, 14,  15, 16
          };
          byte trailerBlock = 7;
          MFRC522::StatusCode status;
          byte buffer[18];
          byte size = sizeof(buffer);

          // Authenticate using key A
          Serial.println(F("Authenticating using key A..."));
          status = (MFRC522::StatusCode)mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
          if (status != MFRC522::STATUS_OK) {
            Serial.print(F("PCD_Authenticate() failed: "));
            Serial.println(mfrc522.GetStatusCodeName(status));
            return;
          }

          // Show the whole sector as it currently is
          Serial.println(F("Current data in sector:"));
          mfrc522.PICC_DumpMifareClassicSectorToSerial(&(mfrc522.uid), &key, sector);  // display sored values
          Serial.println();

          // Read data from the block
          Serial.print(F("Reading data from block "));
          Serial.print(blockAddr);
          Serial.println(F(" ..."));
          status = (MFRC522::StatusCode)mfrc522.MIFARE_Read(blockAddr, buffer, &size);
          if (status != MFRC522::STATUS_OK) {
            Serial.print(F("MIFARE_Read() failed: "));
            Serial.println(mfrc522.GetStatusCodeName(status));
          }
          Serial.print(F("Data in block "));
          Serial.print(blockAddr);
          Serial.println(F(":"));
          dump_byte_array(buffer, 16);
          Serial.println();
          Serial.println();

          // Autehticate with key B
          Serial.println(F("Authenticating again using key B..."));
          status = (MFRC522::StatusCode)mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, trailerBlock, &key, &(mfrc522.uid));
          if (status != MFRC522::STATUS_OK) {
            Serial.print(F("PCD_Authenticate() failed: "));
            Serial.println(mfrc522.GetStatusCodeName(status));
            return;
          }

          // Write data to the block
          Serial.print(F("Writing data into block "));
          Serial.print(blockAddr);
          Serial.println(F(" ..."));
          dump_byte_array(dataBlock, 16);
          Serial.println();
          status = (MFRC522::StatusCode)mfrc522.MIFARE_Write(blockAddr, dataBlock, 16);
          if (status != MFRC522::STATUS_OK) {
            Serial.print(F("MIFARE_Write() failed: "));
            Serial.println(mfrc522.GetStatusCodeName(status));
          }
          Serial.println();

          // Read data from the block (again, should now be what we have written)
          Serial.print(F("Reading data from block "));
          Serial.print(blockAddr);
          Serial.println(F(" ..."));
          status = (MFRC522::StatusCode)mfrc522.MIFARE_Read(blockAddr, buffer, &size);
          if (status != MFRC522::STATUS_OK) {
            Serial.print(F("MIFARE_Read() failed: "));
            Serial.println(mfrc522.GetStatusCodeName(status));
          }
          Serial.print(F("Data in block "));
          Serial.print(blockAddr);
          Serial.println(F(":"));
          dump_byte_array(buffer, 16);
          Serial.println();

          // Check that data in block is what we have written
          // by counting the number of bytes that are equal
          Serial.println(F("Checking result..."));
          byte count = 0;
          for (byte i = 0; i < 16; i++) {
            // Compare buffer (= what we've read) with dataBlock (= what we've written)
            if (buffer[i] == dataBlock[i])
              count++;
          }
          Serial.print(F("Number of bytes that match = "));
          Serial.println(count);
          if (count == 16) {
            Serial.println(F("Success :-)"));
          } else {
            Serial.println(F("Failure, no match :-("));
            Serial.println(F("  perhaps the write didn't work properly..."));
          }
          Serial.println();

          // Dump the sector data
          Serial.println(F("Current data in sector:"));
          mfrc522.PICC_DumpMifareClassicSectorToSerial(&(mfrc522.uid), &key, sector);
          Serial.println();

          // Halt PICC
          mfrc522.PICC_HaltA();
          // Stop encryption on PCD
          mfrc522.PCD_StopCrypto1();
        }
  }
}