#include <Keypad.h>
#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <MFRC522.h>

#define I2C_ADDR 0x3F
#define RST_PIN 9    //Pin 9 para el reset del RC522
#define SS_PIN 10   //Pin 10 para el SS (SDA) del RC522

const byte pinFilas[] = {7, 6, 5, 4};
const byte pinColumnas[] = {A0, A1, A2, A3};
const char teclas[4][4] = {{'1', '2', '3', 'A'}, 
                           {'4', '5', '6', 'B'}, 
                           {'7', '8', '9', 'C'}, 
                           {'*', '0', '#', 'D'}};

const byte numeroUsuarios = 1;
const byte usuarios[numeroUsuarios][4] = {{0x62, 0x16, 0xD1, 0x01}}; // TNE Jorge Verdugo

const char clave[12] = {'*', '2', 'B', '5', 'C', '8', 'A', '*', '3', 'D', '0', '#'};

 /* Funcion de configuracion de pines del modulo LCD/I2C 
    (Direccion,en,rw,rs,d4,d5,d6,d7,backlight,polaridad)*/
Keypad teclado = Keypad(makeKeymap(teclas), pinFilas, pinColumnas, 4, 4);
MFRC522 rfid(SS_PIN, RST_PIN); //Creamos el objeto para el RC522
LiquidCrystal_I2C lcd(I2C_ADDR, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

byte ActualUID[4];


byte smile[8] = {
  0b00000000,
  0b00001010,
  0b00001010,
  0b00001010,
  0b00000000,
  0b00010001,
  0b00001110,
  0b00000000,
};

byte angry[8] = {
  0b00010001,
  0b00001010,
  0b00001010,
  0b00001010,
  0b00000000,
  0b00001110,
  0b00010001,
  0b00000000,
};

int paraCentrar(String frase) {
  if (frase.length() <= 16 && frase.length() > 0) {
    if (frase.length() % 2 == 0) {
      return (8 - frase.length() / 2);
    } else {
      return (8 - (frase.length() + 1) / 2); //owo
    }
  }
  return 16;
}
                     
void setup() {
  lcd.begin(16, 2);
  lcd.home();
  mostrarInicio();
  Serial.begin(9600); //Iniciamos la comunicación  serial
  SPI.begin();        //Iniciamos el Bus SPI
  rfid.PCD_Init(); // Iniciamos  el MFRC522
  Serial.println("Lectura del UID");
}

  void mostrarInicio() {
    lcd.clear();
    lcd.print("Escriba clave o");
    lcd.setCursor(2, 1);
    lcd.print("acerque NFC");
  }
  
int largo = 0;
void loop() {
  char pulsacion = teclado.getKey();

  lcd.home();
  if (pulsacion) {
    largo++;
    lcd.clear();
    lcd.print("Ingresando clave");
    lcd.setCursor(2, 1);
    for (int i=0; i<largo; i++) {
      lcd.setCursor(2 + i, 1);
      lcd.print("*");
    }
    if (largo >= 12) {
      largo = 0; 
      lcd.clear();
      lcd.print("La clave fue ingresada");
      delay(5000);
      mostrarInicio();
    }
  }

  // Revisamos si hay nuevas tarjetas  presentes
  if (rfid.PICC_IsNewCardPresent()) {  
    // Seleccionamos una tarjeta
    if (rfid.PICC_ReadCardSerial()) {
      // Enviamos serialemente su UID
      Serial.print("Card UID: ");
      for (byte i = 0; i < rfid.uid.size; i++) {
        //Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
        Serial.print(rfid.uid.uidByte[i], HEX);
        // ActualUID[i] = rfid.uid.uidByte[i];   
      } 
      Serial.println();
      //revisarUsuarios(ActualUID);
      // Terminamos la lectura de la tarjeta  actual
      rfid.PICC_HaltA();         
    }      
  }
  
}

boolean esLaClave() {
  
}

boolean compararUids(byte uid1[],byte uid2[]) {
  if(uid1[0] != uid2[0])return(false);
  if(uid1[1] != uid2[1])return(false);
  if(uid1[2] != uid2[2])return(false);
  if(uid1[3] != uid2[3])return(false);
  return(true);
}

void revisarUsuarios(byte uid[]) {
  for (byte i = 0; i < numeroUsuarios; i++) {
    if (compararUids(uid, usuarios[i])) {
      lcd.clear();
      lcd.createChar (0, smile);
      lcd.setCursor(3, 0);
      lcd.print("Bienvenido");
      lcd.setCursor(0, 1);
      lcd.print("Jorge Verdugo");
      lcd.setCursor(14, 1);
      lcd.write((byte)0);
      delay(5000);
    } else {
      lcd.clear();
      lcd.createChar (1, angry);
      lcd.setCursor(5, 0);
      lcd.print("ACCESO");
      lcd.setCursor(4, 1);
      lcd.print("DENEGADO");
      lcd.setCursor(13, 1);
      lcd.write((byte)1);
      delay(5000);
    }
  } 
}
