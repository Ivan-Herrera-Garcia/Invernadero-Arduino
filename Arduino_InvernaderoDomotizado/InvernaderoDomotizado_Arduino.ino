// SISTEMAS PROGRAMABLES
// INVERNADERO DOMOTIZADO  | FUNCIONES QUE CARGAMOS EN EL ARDUINO.
//-----------------------------------------------------------------------
// Mandamos a llamar las librerias necesarias
#include <OneWire.h>
#include <SPI.h>
#include <MFRC522.h>
#include <DHT.h>                
#include <DallasTemperature.h>

// Zona de definicion de variables globales
// las variables con #define se pueden usar en cualquier parte del archivo/clase
// Definimos el pin digital donde se conecta el sensor
#define DHTPIN 2
// Dependiendo del tipo de sensor
#define DHTTYPE DHT11
#define RST_PIN  9    //Pin 9 para el reset del RC522
#define SS_PIN  53  //Pin 10 para el SS (SDA) del RC522
MFRC522 mfrc522(SS_PIN, RST_PIN); ///Creamos el objeto para el RC522
#define sensor A0 //SE DEFINE
#define sensor A1 //SE DEFINE
OneWire ourWire(4);   //Se establece el pin 4  como bus OneWire
// Inicializamos el sensor dth pasandole como parametros el pin y el tipo que es DHT11
DHT dht(DHTPIN, DHTTYPE);
DallasTemperature sensors(&ourWire); //Se declara una variable u objeto para nuestro sensor
const byte pinBuzzer = 3; //PIN PWM
byte salida = 10; //Probar con 25
// En el setup estan los elementos que se ejecutan una sola vez
void setup() {
// Ponemos en modo salida los pines 7 y 8 y 12 los cuales estaran dando valores
pinMode(7, OUTPUT);
pinMode(6,OUTPUT);
pinMode(12, OUTPUT);
pinMode(sensor, INPUT);
// Inicializamos el serial begin en 115200 baudios porque estamos trabajando con arduino mega
Serial.begin(115200); 
dht.begin(); // Inicializamos el sensor dht
sensors.begin();   //Se inicia el sensor
SPI.begin();        //Iniciamos el Bus SPI
  mfrc522.PCD_Init(); // Iniciamos el MFRC522
}

// Los siguientes parametros son para almacenar los usuarios
// que tienen accesso a la hora de pasar sus tarjetas
// Como tal no lee el usuario, es mera referencia para nosotros, lo que lee son los bytes que hay
// dentro del arreglo
byte ActualUID[7]; //almacenará el código del Tag leído
byte ivan[7]= {0x05, 0x8B, 0x4C, 0x54, 0xB0, 0x02, 0x00} ; //código del usuario 1
byte pedro[7]= {0x82,0x87,0x14,0xFA,0x9E,0x47,0xC5} ; //código del usuario 2
byte raul[7]={0x05,0x8A,0xE8,0x98,0xB4,0x62,0x00};
byte oscar[7] = {0x05,0x86,0x5D,0xCD,0xD7,0x72,0x00};
// Zona de banderas para detener distintos loops
bool mostrar = true;
bool bombaagua = false;
bool bombilla = false;
// Variables guardar los datos que lanzen los sensores
int valorHumedad;
float h;
 float t;
float f;
float hif;
float hic;
float temp;

// En el loop el codigo se ejecutara n veces hasta que se detenga el proceso de arduino
void loop() { 
  sensores(); // mandamos a llamar a sensores
      Serial.println("Control de acceso:"); // Le pedira al usuario que pase la tarjeta
  // Revisamos si hay nuevas tarjetas  presentes
  if ( mfrc522.PICC_IsNewCardPresent()) 
        {  
      //Seleccionamos una tarjeta
            if ( mfrc522.PICC_ReadCardSerial()) 
            {
                  // Enviamos serialemente su UID
                  Serial.print(F("Card UID:"));
                  for (byte i = 0; i < mfrc522.uid.size; i++) {
                          Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
                          Serial.print(mfrc522.uid.uidByte[i], HEX);   
                          ActualUID[i]=mfrc522.uid.uidByte[i];          
                  } 
                  Serial.print("     ");                 
                  //comparamos los UID para determinar si es uno de nuestros usuarios  
                  if(compareArray(ActualUID,pedro)) {
                    Serial.println("Acceso concedido...");
                    sensores();
                    versensores();
                    mostrar = true;
                    delay(10000);                     

                    }
                  // En este caso de todos los posibles arreglos solo tenemos dada de alta
                  // la tarjeta de ivan
                  if(compareArray(ActualUID,ivan)) {
                    Serial.println("Acceso concedido...");
                    sensores();
                    versensores();
                    mostrar = true;
                    delay(10000);                     

                    } 
    
                  mfrc522.PICC_HaltA();   
  } }
    sensores();
delay(100);
}

//Función para comparar dos vectores
 boolean compareArray(byte array1[],byte array2[])
{
  if(array1[0] != array2[0])return(false);
  if(array1[1] != array2[1])return(false);
  if(array1[2] != array2[2])return(false);
  if(array1[3] != array2[3])return(false);
  return(true);
}

// Creamos un metodo para ir viendo los valores de los sensores, estos seran visibles luego de que
// se haya concedido el acceso
void versensores() {

// Imprimimos la intensidad de luz
Serial.print("Light Intensity:");
// Leemos el valor de la intensidad de luz, el cual se encuentra en el pin analogo 2
Serial.print(int(Light(analogRead(2)))); //pin A2
Serial.println(" Lux"); // EL nombre del sensor como tal
Serial.print("Humedad: "); // El siguiente valor a leer es el de la humedad
  Serial.print(h); // imprimimos el valor de la humedad relativa
  Serial.print(" %\t");
  Serial.println("Temperatura: ");
  Serial.print(t); // imprimimos el valor en grados centigrados
  Serial.print(" *C ");
  Serial.print(f); // Imprimimos el valor de los grados pero en farenheit
  Serial.print(" *F\t");
  Serial.println("Índice de calor: ");
  Serial.print(hic); // Imprimimos el indice de calor en grados centigrados
  Serial.print(" *C ");
  Serial.print(hif); // Mandamos a imprimir el indice de calor en grados farenheit
  Serial.println(" *F");
   Serial.print("Temperatura de agua= ");//SENSOR DE TEMPERATURA DE AGUA
  Serial.print(temp); // Imprime el valor de la tempAgua en grados centigrados
  Serial.println(" C");
  Serial.print("Humedad de suelo: ");//SENSOR DE HUMEDAD DE SUELO
  Serial.print(valorHumedad); // Imprime el valor de la humedad del suelo
  Serial.println("%"); 
  Serial.println("Water level Sensor Value: "); //SENSOR DE NIVEL DE AGUA
  Serial.println(analogRead(A1)); //Se lee el valor de A1 para el sensor de nivel de agua
}
// Metodo para la lectura de valores de los sensores
void sensores() {
  // Si la luz es menor igual a 15 entonces se enciende la bombilla led
if (int(Light(analogRead(2))) <= 15) {
      bombilla=true; // Cambiamos el estado de la bandera para que encienda la bombilla
              Serial.print("Estado de bombilla:");
      Serial.println(bombilla);
  digitalWrite(7,LOW); // Apaga la bombilla
} else {
  // Si no apaga la bombilla
  bombilla = false;
  // Muestra el estado de la bombilla
        Serial.print("Estado de bombilla:");
        Serial.println(bombilla);
  digitalWrite(7,HIGH);  // Enciende la bombilla
  }
  // Leemos la humedad relativa
  h = dht.readHumidity();
  // Leemos la temperatura en grados centígrados (por defecto)
  t = dht.readTemperature();
  // Leemos la temperatura en grados Fahreheit
  f = dht.readTemperature(true);
 
  // Comprobamos si ha habido algún error en la lectura
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Error obteniendo los datos del sensor DHT11");
    return;
  }
 
  // Calcular el índice de calor en Fahreheit
  hif = dht.computeHeatIndex(f, h);
  // Calcular el índice de calor en grados centígrados
  hic = dht.computeHeatIndex(t, h, false);
  sensors.requestTemperatures();   //Se envía el comando para leer la temperatura
  temp= sensors.getTempCByIndex(0); //Se obtiene la temperatura en ºC;
  valorHumedad = map(analogRead(A0), 0, 1023, 100, 0); // lee el valor de la humedad
  // si la humedad es menor igual a 50 entonces el la bomba se apaga
  if (valorHumedad <= 50) {
    bombaagua=false;
            Serial.print("Estado de bomba de agua:");

    Serial.println(bombaagua);
  digitalWrite(12,LOW);
 } else {
  // Si no , entonces la bomba se enciende
    digitalWrite(12,HIGH);
    bombaagua=true; // cambia el estado de la bandera
      // Muestra el estado actual
            Serial.print("Estado de bomba de agua:");
    Serial.println(bombaagua);
  }
//Comparacion de si el nivel del agua es igual o menor a 50 se activara el buzzer como alerta de sonido
  if(analogRead(A4) <= 50) { 
  analogWrite(pinBuzzer, salida);} // Manda a leer los valores del pinBuzzer y cambia la salida segun la bandera 
  }

// Metodo de tipo Double para leer llos valores del luxometro
double Light (int RawADC0)
{
double Vout=RawADC0*0.0048828125; // Instanciamos el objeto
int lux=500/(10*((5-Vout)/Vout));// usamos la sig formula para convertir los valores a algo mas digerible
//int lux=(2500/Vout-500)/10;
return lux; // Regiresamos el valor del luxometro
}
