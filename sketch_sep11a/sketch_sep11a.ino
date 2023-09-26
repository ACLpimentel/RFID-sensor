//Programa: Leitura e gravacao de cartoes RFID
//Autor: MakerHero
#include <Adafruit_Sensor.h>
#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>
#include <TimeOut.h>

//Pinos Reset e SS módulo MFRC522
#define SS_PIN 9
#define RST_PIN 8
MFRC522 mfrc522(SS_PIN, RST_PIN);

LiquidCrystal_I2C lcd(0x3F, 16, 2);

// pino dos botões
#define pino_botao_le A0
#define pino_botao_gr A1
#define pino_uid A3
#define pino_del A4

#define buzzer 1


// teclado de membrana
#include <Keypad.h>  // Biblioteca do codigo

const byte LINHAS = 5;   // Linhas do teclado
const byte COLUNAS = 4;  // Colunas do teclado

const char TECLAS_MATRIZ[LINHAS][COLUNAS] = {  // Matriz de caracteres (mapeamento do teclado)
  { 'f', 'e', '#', '*' },
  { '1', '2', '3', 'u' },
  { '4', '5', '6', 'd' },
  { '7', '8', '9', 's' },
  { 'l', '0', 'r', 'n' }
};

const byte PINOS_LINHAS[LINHAS] = { 47, 49, 45, 43, 41 };  // Pinos de conexao com as linhas do teclado
const byte PINOS_COLUNAS[COLUNAS] = { 46, 44, 42, 40 };    // Pinos de conexao com as colunas do teclado

Keypad teclado_personalizado = Keypad(makeKeymap(TECLAS_MATRIZ), PINOS_LINHAS, PINOS_COLUNAS, LINHAS, COLUNAS);  // Inicia teclado


MFRC522::MIFARE_Key key;

void setup() {
  pinMode(pino_botao_le, INPUT);
  pinMode(pino_botao_gr, INPUT);
  Serial.begin(9600);  //Inicia a serial
  SPI.begin();         //Inicia  comunicação via SPI
  mfrc522.PCD_Init();  //Inicia MFRC522

  pinMode(buzzer, OUTPUT);
  digitalWrite(buzzer, LOW);


  //Inicializa o LCD 16x2
  lcd.init();
  lcd.backlight();
  mensageminicial();

  //Prepara chave - padrao de fabrica = FFFFFFFFFFFFh
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;
}
bool lin = 0;
bool col = 0;
int timeFun;

bool execut;

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

    mensagem_inicial_cartao();
    execut = 1;
    timeFun = millis();
    while (!mfrc522.PICC_IsNewCardPresent() && execut) {
      Serial.println("inside is card present");
      if ((millis() - timeFun) >= 100000) {
        execut = 0;
      }
      delay(100);
    }

    if (execut) {
      modo_leitura();
    }
    {
      mensageminicial();
    }
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

    mensagem_inicial_cartao();
    execut = 1;
    timeFun = millis();
    while (!mfrc522.PICC_IsNewCardPresent() && execut) {
      Serial.println(execut);
      Serial.println("inside is card present");
      if ((millis() - timeFun) >= 100000) {
        execut = 0;
      }
      delay(100);
    }

    if (execut) {
      modo_gravacao();
    }
    {
      mensageminicial();
    }
  }

  //Verifica se o botao modo UID foi pressionado
  int modo_uid = digitalRead(pino_uid);
  if (modo_uid != 0) {
    lcd.clear();
    Serial.println("Modo leitura UID");
    lcd.setCursor(4, 0);
    lcd.print("Modo UID");
    lcd.setCursor(3, 1);
    lcd.print("selecionado");
    while (digitalRead(pino_uid) == 1) {}
    delay(3000);

    mensagem_inicial_cartao();
    execut = 1;
    timeFun = millis();
    while (!mfrc522.PICC_IsNewCardPresent() && execut) {
      Serial.println("inside is card present");
      if ((millis() - timeFun) >= 100000) {
        execut = 0;
      }
      delay(100);
    }

    if (execut) {
      modo_cod();
    }
    {
      mensageminicial();
    }
  }

  //Verifica se o botao modo apagar foi pressionado
  int modo_del = digitalRead(pino_del);
  if (modo_del != 0) {
    lcd.clear();
    Serial.println("Modo limpar cartao");
    lcd.setCursor(3, 0);
    lcd.print("Modo apagar");
    lcd.setCursor(3, 1);
    lcd.print("selecionado");
    while (digitalRead(pino_uid) == 1) {}
    delay(3000);

    mensagem_inicial_cartao();
    execut = 1;
    timeFun = millis();
    while (!mfrc522.PICC_IsNewCardPresent() && execut) {
      Serial.println("inside is card present");
      if ((millis() - timeFun) >= 100000) {
        execut = 0;
      }
      delay(100);
    }

    if (execut) {
      modo_apagar();
    }
    {
      mensageminicial();
    }
  }
}

void mensageminicial()  // tela de seleção
{
  Serial.println("Selecione o modo leitura ou gravacao...");
  Serial.println();
  lcd.clear();
  lcd.print("Selecione o modo:");
  lcd.setCursor(0, 1);
  lcd.print("ler/grav/del/UID");
}

void mensagem_inicial_cartao() {
  Serial.println("Aproxime o seu cartao do leitor...");
  lcd.clear();
  lcd.print(" Aproxime o seu");
  lcd.setCursor(0, 1);
  lcd.print("cartao do leitor");
}

void modo_leitura() {
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
  int n = 0;

  //testa a presença de caracter de quebra de linha no inicio do bloco
  if (buffer[0] == 10) {
    n = 1;
  } else {
    n = 0;
  }
  Serial.println(n);
  for (byte i = n; i < 16; i++) // valor de n se adapta para não mostrar caracter de quebra de linha
  {
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
  if (buffer[0] == 10) {
    n = 1;
  } else {
    n = 0;
  }
  Serial.println(n);
  Serial.print(buffer[0]);
  for (byte i = n; i < 16; i++) // valor de n se adapta para não mostrar caracter de quebra de linha
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
  //Aguarda cartao ser detectado

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
  Serial.print(F("Tipo do PICC: "));
  byte piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
  Serial.println(mfrc522.PICC_GetTypeName(piccType));

  byte buffer[34];
  byte block;
  byte status, len;


  Serial.setTimeout(20000L);
  Serial.println(F("Digite o codigo, em seguida o caractere #"));
  lcd.clear();
  lcd.print("Digite codigo ");
  lcd.setCursor(0, 1);
  lcd.print("+ Ent p/ gravar");
  delay(1500);
  lcd.clear();
  lcd.print("codico + ent");


  String codi = "";

  len = 0;
  int x = 0;
  char last; // guarda ultimo caracter inserido
  Serial.print(" letras in");
  do {
      Serial.print("out letras ");

      // mostra na tela valor do codigo
      lcd.setCursor(0, 1);
      lcd.print(codi);
      delay(80); //

    char leitura_teclas = teclado_personalizado.getKey();
    if (leitura_teclas) {  // Se alguma tecla foi pressionada


      buffer[x] = leitura_teclas;  // Imprime a tecla pressionada na porta serial

      Serial.println(leitura_teclas);
      Serial.println(buffer[x]);
      codi += char(buffer[x]);
      last = char(buffer[x]);
      x++;
      len++;
      
    }
  } while (len < 17 && buffer[x - 1] != 35); // 16 é numero maximo de caracter mostrado por linha, 35 é o equivalnte de # em decimal
  Serial.print("letras out");

  buffer[x - 1] = " "; // remove a # 

  bool empty;
  Serial.print(len);

  // testa se nome foi inserido
  if (len > 1) {
    empty = true;
  } else {
    empty = false;
  }  // "/n               " é o valor atribuido ao bloco caso nada seja inserido

  for (byte i = len; i < 20; i++) buffer[i] = ' ';

  block = 4;
  //Serial.println(F("Autenticacao usando chave A..."));
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A,
                                    block, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("PCD_Authenticate() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    delay(1000);
  }

  //Grava no bloco 4
  //
  if (empty && last == "#") {
    status = mfrc522.MIFARE_Write(block, buffer, 16);
    if (status != MFRC522::STATUS_OK && empty) {
      Serial.print(F("MIFARE_Write() failed: "));
      Serial.println(mfrc522.GetStatusCodeName(status));
      delay(1000);
    }
  }

  block = 5;
  //Serial.println(F("Authenticating using key A..."));
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A,
                                    block, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("PCD_Authenticate() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    delay(1000);
  }

  //Grava no bloco 5
  if (empty && last == "#") {
    status = mfrc522.MIFARE_Write(block, &buffer[16], 16);
    if (status != MFRC522::STATUS_OK) {
      Serial.print(F("MIFARE_Write() failed: "));
      Serial.println(mfrc522.GetStatusCodeName(status));
      delay(1000);
    }
  }

  Serial.println(F("Digite o sobrenome,em seguida o caractere #"));
  lcd.clear();
  lcd.print("Digite nome + #");
  lcd.setCursor(0, 1);
  lcd.print("p/ gravar-max:16");

  len = Serial.readBytesUntil('#', (char *)buffer, 16);
  for (byte i = len; i < 17; i++) buffer[i] = ' ';
  Serial.print(len);
  Serial.print(buffer[0]);



  // testa se sobrenome foi inserido
  if (len > 1) {
    empty = true;
  } else {
    empty = false;
  }  // "/n               " é o valor atribuido ao bloco caso nada seja inserido

  block = 1;
  //Serial.println(F("Autenticacao usando chave A..."));
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A,
                                    block, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("PCD_Authenticate() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    delay(1000);
  }

  //Grava no bloco 1
  //Executa se novo sobrenome tnha sido inserido
  if (empty) {
    status = mfrc522.MIFARE_Write(block, buffer, 16);
    if (status != MFRC522::STATUS_OK) {
      Serial.print(F("MIFARE_Write() failed: "));
      Serial.println(mfrc522.GetStatusCodeName(status));
      delay(1000);
    }
  }

  block = 2;
  //Serial.println(F("Autenticacao usando chave A..."));
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A,
                                    block, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("PCD_Authenticate() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    delay(500);
  }

  //Grava no bloco 2
  //Executa se novo sobrenome tenha sido inserido
  if (empty) {
    status = mfrc522.MIFARE_Write(block, &buffer[16], 16);
    if (status != MFRC522::STATUS_OK) {
      Serial.print(F("MIFARE_Write() failed: "));
      Serial.println(mfrc522.GetStatusCodeName(status));
      delay(500);
    } else {
      Serial.println(F("Dados gravados com sucesso!"));
      lcd.clear();
      lcd.print("Gravacao OK!");
    }
  }



  mfrc522.PICC_HaltA();       // Halt PICC
  mfrc522.PCD_StopCrypto1();  // Stop encryption on PCD
  delay(5000);
  mensageminicial();
}

void modo_apagar() {

  //Aguarda cartao ser detectado


  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }

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
  Serial.print(F("Tipo do PICC: "));
  byte piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
  Serial.println(mfrc522.PICC_GetTypeName(piccType));

  Serial.println("Apagando dados cartao");
  lcd.clear();
  lcd.print(" Apagando dados");
  lcd.setCursor(2, 1);
  lcd.print("do cartao...");

  byte buffer[34];
  byte block;
  byte status, len;

  len = Serial.readBytesUntil('#', (char *)buffer, 20);
  Serial.print(len);

  for (byte i = len; i < 20; i++) buffer[i] = ' ';

  block = 4;
  //Serial.println(F("Autenticacao usando chave A..."));
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A,
                                    block, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("PCD_Authenticate() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    delay(500);
  }

  //Grava no bloco 4
  //
  status = mfrc522.MIFARE_Write(block, buffer, 16);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("MIFARE_Write() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    delay(500);
  }


  block = 5;
  Serial.println(F("Authenticating using key A..."));
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A,
                                    block, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("PCD_Authenticate() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    delay(500);
  }

  //Grava no bloco 5

  status = mfrc522.MIFARE_Write(block, &buffer[16], 16);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("MIFARE_Write() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    delay(500);
  }


  len = Serial.readBytesUntil('#', (char *)buffer, 30);
  for (byte i = len; i < 30; i++) buffer[i] = ' ';
  Serial.print(len);
  Serial.print(buffer[0]);

  block = 1;
  Serial.println(F("Autenticacao usando chave A..."));
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A,
                                    block, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("PCD_Authenticate() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    delay(500);
  }

  //Grava no bloco 1
  //Atribui valor vasio
  status = mfrc522.MIFARE_Write(block, buffer, 16);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("MIFARE_Write() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    delay(500);
  }


  block = 2;
  //Serial.println(F("Autenticacao usando chave A..."));
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A,
                                    block, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("PCD_Authenticate() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    delay(500);
  }

  //Grava no bloco 2
  //Atribui valor vasio
  status = mfrc522.MIFARE_Write(block, &buffer[16], 16);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("MIFARE_Write() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    delay(500);
  } else {
    Serial.println(F("Dados gravados com sucesso!"));
    lcd.clear();
    lcd.print("Dados apagados!");
  }

  mfrc522.PICC_HaltA();       // Halt PICC
  mfrc522.PCD_StopCrypto1();  // Stop encryption on PCD
  delay(5000);
  mensageminicial();
}

void modo_cod() {

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
  lcd.setCursor(1, 1);
  lcd.print(conteudo);
  delay(8000);

  mensageminicial();
}
