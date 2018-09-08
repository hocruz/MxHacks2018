#include <LiquidCrystal_I2C.h>
#include <DS1302B.h>
#include <EEPROM.h>
#include <SPI.h>
#include <MFRC522.h>

#include <String.h>
#define RST_PIN         9       
#define SS_PIN          10      
LiquidCrystal_I2C lcd(0x27, 20, 4);

byte PIRPin1 = 2;     //agregar pirpin 2, estos estan en los pines 2,16,17
byte PIRPin2 = 17;     //agregar pirpin 2, estos estan en los pines 2,16,17

byte pinledbuz1=8;
byte pinledbuz2=14;
      
byte PinBotPan1=15;  
byte PinBotPan2=16;
byte AlarmaChofer;
byte AlarmaUsuario;

byte BandTarj=0;
MFRC522 mfrc522(SS_PIN, RST_PIN);   

uint8_t CE_PIN   = 7;
uint8_t IO_PIN   = 6;
uint8_t SCLK_PIN = 5;


byte buffer[32];
String Simp1;
byte dia;
byte mes;
int agno;
byte hora;
byte minuto;
String Fecha;
String Horas1,NvecesUso;
boolean band = false;
int PasajeMoo=0;
DS1302 rtc(CE_PIN, IO_PIN, SCLK_PIN);

String CadenaAux,CadenaAuxS;
float CostoPasaje;
float Saldo;
float tiempotarjeta;
byte block;
byte len;
float cambio;
int tipodeusuario;   // 1.- ordinario    2.- estudiante 3.- adulto mayor

     

MFRC522::MIFARE_Key key;
MFRC522::StatusCode status;
//int pirState = LOW;// de inicio no hay movimiento
int val = 0;                  // estado del pin
int PasajeroFr,PasajeroTr; 
int Direccion = 0;
int II,JJ;
String recibir1;
String IdAutobus;
String SSIDAutobus;
String PASSWORDAutobus;
String HOSTDatos;
String PUERTOServidor;
String TIEMPOPoleo;
String FECHAAutobus;
String HORAAutobus;
String COSTOPasaje;
String s, s1;
int Valor;
int tpoleo,tpoleo2;
boolean tiempodepoleo=false;
void setup() {
  lcd.begin();
  lcd.backlight();
  Serial.begin(9600);
  SPI.begin();               // Inicializa SPI bus
  mfrc522.PCD_Init();        // Inicializa Tarjeta Rfid 
  letrero();
  PasajeMoo=0;
   pinMode(PIRPin1, INPUT);
   pinMode(PIRPin2, INPUT);
   
   pinMode(pinledbuz1,OUTPUT);
   pinMode(pinledbuz2,OUTPUT);
   
   pinMode(PinBotPan1,INPUT); //Puerta Frente
   pinMode(PinBotPan2,INPUT); //Puerta Trasera
   
   digitalWrite(pinledbuz1, LOW);
   digitalWrite(pinledbuz2, LOW);
   
   digitalWrite(PIRPin1, LOW);
   digitalWrite(PIRPin2, LOW);
   
   
   PasajeroFr=0;
   PasajeroTr=0;
   Valor=0;

    
  IdAutobus="";
  char caracter;
  for (int i = 1; i<(1+EEPROM.read(0)); i++)
   {
   caracter=EEPROM.read(i);
   IdAutobus=IdAutobus+caracter;
   }
  //lcd.setCursor(6,2);
  //lcd.print(IdAutobus);
  
  SSIDAutobus="";
  for (int i = 9; i<(9+EEPROM.read(8)); i++)
   {
   caracter=EEPROM.read(i);
   SSIDAutobus=SSIDAutobus+caracter;
   }
 
  PASSWORDAutobus="";
  for (int i = 22; i<(22+EEPROM.read(21)); i++)
   {
   caracter=EEPROM.read(i);
   PASSWORDAutobus=PASSWORDAutobus+caracter;
   }
  
  if ((EEPROM.read(69)==49)&&(EEPROM.read(70)==49))
  {
   HOSTDatos="";
   for (int i = 38; i<(38+EEPROM.read(37)); i++)
   {
   caracter=EEPROM.read(i);
   HOSTDatos=HOSTDatos+caracter;
   }
  
  PUERTOServidor="";
  for (int i = 63; i<(63+EEPROM.read(62)); i++)
   {
   caracter=EEPROM.read(i);
   PUERTOServidor=PUERTOServidor+caracter;
   }
  
  TIEMPOPoleo="";
  for (int i = 67; i<(67+EEPROM.read(66)); i++)
   {
   caracter=EEPROM.read(i);
   TIEMPOPoleo=TIEMPOPoleo+caracter;
   }
  tpoleo=TIEMPOPoleo.toInt(); 
  }




  
  COSTOPasaje="";
  for (int i = 72; i<(72+EEPROM.read(71)); i++)
   {
   caracter=EEPROM.read(i);
   COSTOPasaje=COSTOPasaje+caracter;
   } 

   rtc.halt(false); 
   rtc.writeProtect(false);
   FechayHora();
  tiempodepoleo=false;
  tpoleo2=minuto;

}


void letrero(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(F("TARJETA  INTELIGENTE"));
  lcd.setCursor(6, 2);
  lcd.print(F("I  N  E"));
  
  //lcd.setCursor(6,2);
  //lcd.print(IdAutobus);
  
  
 }


void letrero3(){
  lcd.setCursor(0, 1); lcd.print("                    ");
  lcd.setCursor(0, 2); lcd.print("                    ");
  lcd.setCursor(0, 3); lcd.print("                    ");
  lcd.setCursor(0, 4); lcd.print("                    ");
  
  lcd.setCursor(0, 0); lcd.print("Contacto: OBED CRUZ ");
  lcd.setCursor(0, 1); lcd.print("Tel: 782 167 0044");
  lcd.setCursor(0, 2); lcd.print("Tipo Sangre : O+");
  lcd.setCursor(0, 3); lcd.print("Alergico: Penicilina");
  delay(5500);
  lcd.setCursor(0, 1); lcd.print("                    ");
  lcd.setCursor(0, 2); lcd.print("                    ");
  lcd.setCursor(0, 3); lcd.print("                    ");
  lcd.setCursor(0, 4); lcd.print("                    ");
  
  lcd.setCursor(0, 0); lcd.print("Hopsital Registrado");
  lcd.setCursor(0, 1); lcd.print("ISSSTE");
  lcd.setCursor(0, 2); lcd.print("Enfermedad Cronica");
  lcd.setCursor(0, 3); lcd.print("Diabetico");
  delay(5500);
  lcd.setCursor(0, 1); lcd.print("                    ");
  lcd.setCursor(0, 2); lcd.print("                    ");
  lcd.setCursor(0, 3); lcd.print("                    ");
  lcd.setCursor(0, 4); lcd.print("                    ");
  letrero();
 }


void FechayHora()
{
 
 Time tiempo;
 tiempo = rtc.getTime();
 dia=tiempo.date;
 mes=tiempo.mon;
 agno=tiempo.year;
 hora=tiempo.hour;
 minuto=tiempo.min;
 
 Fecha="";
 Fecha=dia;
 
 if (Fecha.length()==1) Fecha="0"+Fecha;
 
 Fecha=Fecha+"/";
 String Paso="";
 Paso=mes;
 if (Paso.length()==1) Paso="0"+Paso;
  
 Fecha=Fecha+Paso+"/"; 
 Fecha=Fecha+agno;
 Horas1="";
 Paso="";
 Horas1=hora;
 
 if (Horas1.length()==1) Horas1="0"+Horas1;
  
 Horas1=Horas1+":";
 Paso="";
 Paso=minuto;
 if (Paso.length()==1)
  Paso="0"+Paso;
 
 Horas1=Horas1+Paso; 

 if ((EEPROM.read(69)==49)&&(EEPROM.read(70)==49)){
   int difer=minuto-tpoleo2;
 if (difer>=tpoleo){
 tiempodepoleo=true;tpoleo2=minuto;}
  else if (difer<0){
     int difer2=59-tpoleo2;
     difer2=difer2+minuto;
     if (difer2>=tpoleo){
 tiempodepoleo=true;tpoleo2=minuto;}
     
    }   
 }    
 if (digitalRead(PinBotPan1) == LOW) AlarmaUsuario=1; else AlarmaUsuario=0;
 if (digitalRead(PinBotPan2) == LOW) AlarmaChofer=1; else AlarmaChofer=0;
}

void ErrorLectura(){
    BandTarj=0;
    lcd.clear();
    lcd.setCursor(7, 0);
    lcd.print(F("ERROR "));
    lcd.setCursor(9, 1);
    lcd.print(F("DE "));
    lcd.setCursor(6, 2);
    lcd.print(F("LECTURA "));
}

void loop() {
  
      
     
  detectatarjeta();
  if (band) {
    letrero3();
  //  LecturaTarjetayCobrar();
    band = false;
  } 
  
   
  
   
   val = digitalRead(PIRPin1);
   if (val == HIGH) PasajeroFr=PasajeroFr+1;
   
   val = digitalRead(PIRPin2);
   if (val == HIGH) PasajeroTr=PasajeroTr+1;
    
}






void detectatarjeta() {
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;
  MFRC522::StatusCode status;
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  band = true;
  //si();
  Serial.println(F("tarjeta detectada"));
}

void sonbajo(){
  digitalWrite(pinledbuz1, LOW);
  digitalWrite(pinledbuz2, LOW);
}

void sonalto(){
  digitalWrite(pinledbuz1, HIGH);
  digitalWrite(pinledbuz2, HIGH);
}

void no() 
  {
   for (byte ty=0;ty<4;ty++){
   delay(150);
   sonalto();
   delay(500);
   sonbajo();}
  }


void si() {

   for (byte ty=0;ty<2;ty++){
   delay(150);
   sonalto();
   delay(100);
   sonbajo();}
   
}



void leercampo() {
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid)); 
  if (status != MFRC522::STATUS_OK) {
    ErrorLectura();
    no();
    letrero();
    return;
  }

  status = mfrc522.MIFARE_Read(block, buffer, &len);
  if (status != MFRC522::STATUS_OK) {
    ErrorLectura();
    no();
    letrero();
    
  }
  
  CadenaAux = "";
  for (uint8_t i = 0; i < 16; i++)
  {
    if (buffer[i] != 32)
    {
      if (buffer[i] != 46) {
        CadenaAux = CadenaAux + (buffer[i] - 48);

      }
      else if (buffer[i] == 46) {
        CadenaAux = CadenaAux + ".";
      }
    }
  }
 
}


void leercampo3() {
  
  for (byte r=0;r<32;r++)
   buffer[r]=32;
   
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid)); 
  if (status != MFRC522::STATUS_OK) {
    ErrorLectura();
    no();
    letrero();
    return;
  }
  len=18;
  status = mfrc522.MIFARE_Read(block, buffer, &len);
  if (status != MFRC522::STATUS_OK) {
    ErrorLectura();
    no();
    letrero();
  }
  
}

void EscribirBloque(){
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    ErrorLectura();
    no();
    letrero();
    return;
  }
  // Escribimos block
  status = mfrc522.MIFARE_Write(block, buffer, len);
  if (status != MFRC522::STATUS_OK) {
    ErrorLectura();
    no();
    letrero();
    return;
  } 
   
}

void escrituraAtarjeta() {
  char g;
 
  String Cadena = "";
  Cadena = "";
  Cadena = cambio;
  for (byte i = 0; i < (Cadena.length()); i++) {
    g = Cadena.charAt(i);
    buffer[i] = g;
  }
 
  
  for (byte i = Cadena.length(); i < 16; i++) buffer[i] = ' ';     // pad with spaces

  block = 26;
  len = 16;
  EscribirBloque();
  if (BandTarj==1) {
    for (byte i = 0; i < (IdAutobus.length()); i++) {
     g = IdAutobus.charAt(i);
     buffer[i] = g;
    }
    
    for (byte i = IdAutobus.length(); i < 16; i++) buffer[i] = ' ';     // pad with spaces
    block = 40;
    len = 16;
    EscribirBloque();
if (BandTarj==1) {
    for (byte i = 0; i < (Fecha.length()); i++) {
     g = Fecha.charAt(i);
     buffer[i] = g;
    }
   
    for (byte i = Fecha.length(); i < 16; i++) buffer[i] = ' ';     // pad with spaces
    block = 42;
    len = 16;
    EscribirBloque();
if (BandTarj==1) {
    for (byte i = 0; i < (Horas1.length()); i++) {
     g = Horas1.charAt(i);
     buffer[i] = g;
    }
    
    for (byte i = Horas1.length(); i < 16; i++) buffer[i] = ' ';     // pad with spaces
    block = 44;
    len = 16;
    EscribirBloque();
if (BandTarj==1) {
    for (byte i = 0; i < (NvecesUso.length()); i++) {
     g = NvecesUso.charAt(i);
     buffer[i] = g;
    }
    
    for (byte i = NvecesUso.length(); i < 16; i++) buffer[i] = ' ';     // pad with spaces
    block = 46;
    len = 16;
    EscribirBloque();
if (BandTarj==1) {
    lcd.clear();
    lcd.setCursor(2, 0);
    lcd.print(CadenaAuxS);
    lcd.setCursor(0, 1);
    lcd.print(F("SALDO: $"));
    lcd.print(Saldo);
    lcd.setCursor(0, 2);
    lcd.print(F("CAMBIO: $"));
    lcd.print(cambio);
    lcd.setCursor(5, 3);
    lcd.print(F("Pasaje - $ "));
    lcd.setCursor(15,3);
    lcd.print(CostoPasaje);
    PasajeMoo=PasajeMoo+1;

    
    si();
    delay(150);
    letrero();
}}}}}

}



void LecturaTarjetayCobrar() {
    byte cad[16];
    
  BandTarj=1;  
  // Se lee el numero de identificacion de la tarjeta que debera ser igual a la clave de la tarjeta
  
  for (byte j = len; j < 16; j++) cad[j] = 32;
  for (byte i = 0; i < mfrc522.uid.size; i++) {
   cad[i]=mfrc522.uid.uidByte[i];
  }  

  block = 46;
  len = 18; 
  leercampo3();
  if (BandTarj==1) {
  CadenaAux="";
  char kij1;
  for(byte g=0;g<16;g++){
  kij1=buffer[g];
   if (kij1 != 32) CadenaAux=CadenaAux+kij1;
  } 
  int ty=CadenaAux.toFloat();
  ty=ty+1;
  NvecesUso=ty;
 
  block = 4;
  len = 18; 
  leercampo3();
  if (BandTarj==1) {
  CadenaAux="";
  char kij;
  for(byte g=0;g<16;g++){
  kij=buffer[g];
   if (kij != " ") CadenaAux=CadenaAux+kij;
 }
  CadenaAuxS=CadenaAux; //Nombre

  block = 8;
  len = 18; 
  leercampo();
  tipodeusuario = CadenaAux.toFloat();
  
  block = 26;
  len = 18; 
  leercampo();
  if (BandTarj == 1) {
  Saldo = CadenaAux.toFloat();
  CostoPasaje = COSTOPasaje.toFloat();



//Aqui leer el tipo de usuario si este es igual a 10 entonces
//IdAutobus 
//PasajeroFr
//PasajeMoo
//COSTOPasaje
//AlarmaUsuario
//AlarmaChofer
//agno
//mes
//dia
//hora
//minuto
    // PasajeroTr=0; //Agregar a web
    // PasajeroFr=0;
    // PasajeMoo=0;
     


  Serial.print(F("Tipo Usuario:"));
  Serial.println(tipodeusuario);
  //tipodeusuario=1;
  if (Saldo >= CostoPasaje) {
      if (tipodeusuario == -48){
        Serial.println("entre");      
        cambio = Saldo - CostoPasaje;
        escrituraAtarjeta();
      }
      else{
            if (tipodeusuario == 5){
             //aqui enviar datos a la tarjeta 
             lcd.clear();
             lcd.setCursor(4, 1);
             lcd.print(F("INFORMACION"));
          lcd.setCursor(3, 2);
          lcd.print(F("PROPORCIONADA"));
          si();
          letrero(); 
            }
           
        }  
      
    }
   else {
          lcd.clear();
          lcd.setCursor(7, 1);
          lcd.print(F("SALDO"));
          lcd.setCursor(4, 2);
          lcd.print(F("INSUFICIENTE"));
          no();
          letrero();
    
        }
 

  }
  }
  }
    
    mfrc522.PICC_HaltA(); 
  mfrc522.PCD_StopCrypto1(); 
  }               

