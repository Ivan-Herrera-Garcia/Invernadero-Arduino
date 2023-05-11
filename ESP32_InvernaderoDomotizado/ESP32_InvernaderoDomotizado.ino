// Creamos el servidor web usando WiFiServer
// Servidor Web
#include <WiFi.h>
//#include <HTTPClient.h> // Para trabajar con peticiones HTTP
#include <ArduinoJson.h> // para trabajar con objetos json
//const char* ssid     = "Megacable_2.4G_0398";
//const char* password = "xtnv5HYN";

// Incluimos a firebase
#include <FirebaseESP32.h>

// por si llegan a ser necesarias
#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>

  const char* ssid = "Megacable_2.4G_11CE";
  

  const char* password = "ArFx5LGh";

//const char* ssid = "ITLAlumnos";
//const char* password = "";

#define DATABASE_URL "invernaderodomotizadodb-default-rtdb.firebaseio.com"
// En caso de ser requerido definimos el email y contrasena
#define USER_EMAIL "proyectoesp32lamia@gmail.com"
#define USER_PASSWORD "Sistemasprogramables-1234"
// Creamos los objetos necesarios de FireBase, para el manejo de eventos
FirebaseData fbdo; // Manejo de datos
// Para la autenticacion
FirebaseAuth auth; // Se puede usar correo o la api key
FirebaseConfig config; // guardamos todas las config en este objeto
// Definimos bandera para cuando validemos si se ha hecho la conexion a la db
bool band = false;
// Definimos la API del proyecto
#define API_KEY "AIzaSyAth51Y0Nr1wSgHHRmaRnFUvjyLTu6Um18"

// Manejamos los millisegundos
unsigned long dataMillis = 0;
int contador = 0;
// Anadimos otra bandera para el proceso de leer, insertar datos
bool hagoAlgo = true;

// Vemos la parte web
WiFiServer server(80);

// Declaracion de variables globales
String estadoBomba = "";
String estadoBombilla = "";
String humedadSuelo = "";
String estadoPuerta = "Cerrada";
String nivelAgua = "";
String tempAgua = "";
String tempRelativa = "";
//   estadoBomba , estadoBombilla, humedadSUelo, estadoPuerta, nivelAgua, tempAgua, tempRelativa;
int nodo = 1; // Iniciamos el nodo/registro en el 1
unsigned long millis_tiempo;
// Procesamos los datos recibidos por Serial desde el arduino
void Procesar_Serial(){
  StaticJsonDocument <256> doc;
  deserializeJson(doc, Serial1); // Convierte de json a datos legibles
  // Leer variables
  estadoBomba = doc ["estadoBomba"].as<String>();
  estadoBombilla = doc ["estadoBombilla"].as<String>();
  humedadSuelo = doc ["humedadSuelo"].as<String>();
  estadoPuerta = doc ["estadoPuerta"].as<String>();
  nivelAgua = doc ["nivelAgua"].as<String>();
  tempAgua = doc ["tempAgua"].as<String>();
  tempRelativa = doc ["tempRelativa"].as<String>();

}
//----------------------------------------------------------------------------------------------------------------------
// Procesa los datos de la bomba
String procesarBomba(String parametro){
  StaticJsonDocument <256> doc;
  parametro = doc ["estadoBomba"].as<String>();
  return parametro;
}
//-----------------------------------------------------------------------------------------------------------------------
// procesa los datos de la bombilal
String procesarBombilla(String parametro){
  StaticJsonDocument <256> doc;
  parametro = doc ["estadoBombilla"].as<String>();
  return parametro;
}
//---------------------------------------------------------------------------------------------------------------------
// Procesa los datos de la humedad
String procesarHumedad(String parametro){
  StaticJsonDocument <256> doc;
  parametro = doc ["humedadSuelo"].as<String>();
  return parametro;
}
//---------------------------------------------------------------------------------------------------------------------
// Procesa los datos de la puerta
String procesarPuerta(String parametro){
  StaticJsonDocument <256> doc;
  parametro = doc ["estadoPuerta"].as<String>();
  return parametro;
}
//---------------------------------------------------------------------------------------------------------------------
// procesa los datos del nivel de agua
String procesarNivel(String parametro){
  StaticJsonDocument <256> doc;
  parametro = doc ["nivelAgua"].as<String>();
  return parametro;
}
//---------------------------------------------------------------------------------------------------------------------
// procesa los datos de la temperatura del agua
String procesarTempAgua(String parametro){
  StaticJsonDocument <256> doc;
  parametro = doc ["tempAgua"].as<String>();
  return parametro;
}
//---------------------------------------------------------------------------------------------------------------------
// procesa los datos de la temperatura relativa
String procesarTempRelativa(String parametro){
   StaticJsonDocument <256> doc;
  parametro = doc ["tempRelativa"].as<String>();
  return parametro;
}
//---------------------------------------------------------------------------------------------------------------------
// Inicializacion de componentes/configuraciones
void setup()
{
 // Se ejecuta una sola vez
    Serial.begin(115200); // Inicializamos el serial con 115200 baudios por que estamos usando esp32
  //   Serial2.begin(115200, SERIAL_8N1,rx2pin,tx2pin);
    delay(10);
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid); // Imprime la red

    WiFi.begin(ssid, password); // Busca la red wifi y pone la contrasena que le mandamos
// Mientras no se conecte a la red wifi, entonces imprimira puntitos
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
// Muestra la ip que suelta la red
    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP()); 
    
    server.begin(); // Inicia el servidor web
// Imprimimos el cliente de firebase  que estamos usando
Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

// ZONA IMPORTANTE DE CONFIGURACIONES DE FIREBASE
config.api_key = API_KEY; // Le mandamos la key de la app por la variable que definimos arriba
  
// Le mandamos las credenciales del correo
auth.user.email = USER_EMAIL;
auth.user.password = USER_PASSWORD;

// Le mandamos a config la url de la base de datos
config.database_url = DATABASE_URL;
// Le mandamos la clave secreta
config.signer.tokens.legacy_token = "23c2ANwtGUvjj4K0K9KV14LB3Xcd9TwxAD4sLpOD";// D
//config.signer.tokens.legacy_token = "23c2ANwtGUvjj4K0K9KV14LB3Xcd9TwxAD4sLpOD";// 
// Inicializamos el proceso dadas las configuraciones y parametros de autenticacion dados
Firebase.begin(&config, &auth);
// Le decimos que se reste reconectando a la red wifi
Firebase.reconnectWiFi(true);
while(Serial.read()>= 0){}
Serial1.begin(115200);   
}

int value = 0;

void loop(){
  
while (Serial1.available()){
 Serial.write(Serial.read()); // busca datos del serial
} 
//---------------------------------------------------------------------------------------------------------------------
// Mientras no se conecte a la base de datos entrara en el siguiente while  
while(!band)
  if(Firebase.ready())
  {
    Serial.println("OK, Conexion correcta a la base de datos");  
    band = true; // cambia la bandera y se sale del while
  } else {
    Serial.println("Algo salio mal, revise los parametros de la conexion");
  }
//Serial.print(char(Serial2.read());
// Si todo ha salido bien
// Manejamos el envio y lectura a la base de datos
hagoAlgo = true; // cambiamos la bandera para que mande datos a firebase
while(hagoAlgo){
// Mandamos datos a firebase
//                  instancia             nodo                          valor a mandar
Firebase.setString(fbdo,"/Sensores/"+String(nodo)+"/estadoBomba/",Serial1.print(estadoBomba));
//Serial1.print();
Firebase.setString(fbdo,"/Sensores/"+String(nodo)+"/estadoBobilla/",Serial1.print(estadoBombilla));
//Serial1.print();
Firebase.setString(fbdo,"/Sensores/"+String(nodo)+"/humedadSuelo/",Serial1.print(humedadSuelo));
//Serial1.print();
Firebase.setString(fbdo,"/Sensores/"+String(nodo)+"/estadoPuerta/",Serial1.print(estadoPuerta));
//Serial1.print();
Firebase.setString(fbdo,"/Sensores/"+String(nodo)+"/nivelAgua/",Serial1.print(nivelAgua));
//Serial1.print();
Firebase.setString(fbdo,"/Sensores/"+String(nodo)+"/tempAgua/",Serial1.print(tempAgua));
//Serial1.print();
Firebase.setString(fbdo,"/Sensores/"+String(nodo)+"/tempRelativa/",Serial1.print(tempRelativa));
hagoAlgo = false; // cambiamos la bandera para que salga del ciclo
}
nodo++; // incrementamos el valor del nodo
//---------------------------------------------------------------------------------------------------------------------
// Servidor web
  
 WiFiClient client = server.available();   // Esta en espera de nuevos clientes/navegadores

  if (client) {                             // si se conecta alguien
    Serial.println("New Client.");           // notificamos
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK"); // Estado de respuesta
            client.println("Content-type:text/html"); // decimos que interprete el texto como html
            client.println(); // Salto de linea
  client.print("<!DOCTYPE HTML>"); // le decimos que interprete lo siguiente como html
  client.print("<html>"); // iniciamos la etiqueta html
  client.print("<head>");
  client.print("<title>Invernadero Domotizado</title>");// Asignamos el nombre del titulo
  client.print("<meta name='viewport' content='width=device-width, initial-scale=1'>");
  // Mandamos a llamar a bootstrap para darle estilo a la pagina
  client.print("<link rel='stylesheet' href='https://cdn.jsdelivr.net/npm/bootstrap@4.0.0/dist/css/bootstrap.min.css' integrity = 'sha384-Gn5384xqQ1aoWXA+058RXPxPg6fy4IWvTNh0E263XmFcJlSAwiGgFAW/dAiS6JXm' crossorigin = 'anonymous'>"); // Mandamos a llamar a bootstrap
  // En efecto, funciona Bootstrap aqui
  client.print("</head>");
  client.print("<body>");// Iniciamos la etiqueta body, en donde estara todo el html en si
  // Creacion de las tablas
  client.print("<div class='container mt-3'>") ; // Iniciamos el contenedor
  client.print("<table class='table table-dark'>"); // Le damos estilo a la tabla
  client.print("<thead>"); // Abirmos el thead
  // Atributos de la tabla
  client.print("<th>Num.Registro</th>");
  client.print("<th>Bomba de agua</th>");
  client.print("<th>Bombilla</th>");
  client.print("<th>Humedad del suelo</th>");
  client.print("<th>Nivel del agua</th>");
  client.print("<th>Puerta</th>");
  client.print("<th>Temperatura del agua</th>");
  client.print("<th>Temperatura ambiente</th>");
  client.print("</thead>"); // Cerramos el thead
  //----------------------------------------------------------------------------------
  // Hasta aqui probamos la primer parte del codigo y funciona
// Hasta aqui todo bien
client.print("<tbody id='tbody1'></tbody>"); // Iniciamos el tbody
client.print("</table>"); // Cerramos la tabla
client.print("</div>");// Cerramos el div

// Invocamos los scripts necesarios
client.print("<script src='https://code.jquery.com/jquery-3.2.1.slim.min.js' integrity='sha384-KJ3o2DKtIkvYIK3UENzmM7KCkRr/rE9/Qpg6aAZGJwFDMVNA/GpGFF93hXpG5KkN' crossorigin='anonymous'></script>");
client.print("<script src='https://cdn.jsdelivr.net/npm/popper.js@1.12.9/dist/umd/popper.min.js' integrity='sha384-ApNbgh9B+Y1QKtv3Rn7W3mgPxhU9K/ScQsAP7hUibX39j7fakFPskvXusvfa0b4Q' crossorigin='anonymous'></script>");
client.print("<script src='https://cdn.jsdelivr.net/npm/bootstrap@4.0.0/dist/js/bootstrap.min.js' integrity='sha384-JZR6Spejh4U02d8jOt6vLEHfe/JQGiRRSQQxSfFWpi1MquVdAyjUar5+76PVCmYl' crossorigin='anonymous'></script>");  

// El script que se encarga de las operaciones en la talba
client.print("<script type='module'>");
client.print("var regNo = 0;"); // El contandor de registros
client.print("var tbody = document.getElementById('tbody1');"); // El tbody
// Anadimos la funcion para ingresar items a la tabla
client.print("function AddItemToTable(bomba,bombilla,humedad,nivelAgua,puerta,tempAgua,tempAmbiente){");
client.print("let trow = document.createElement('tr');"); // Creamos una fila
client.print("let td1 = document.createElement('td');"); // Creamos una columna
client.print("let td2 = document.createElement('td');"); // Creamos una columna
client.print("let td3 = document.createElement('td');"); // Creamos una columna
client.print("let td4 = document.createElement('td');"); // Creamos una columna
client.print("let td5 = document.createElement('td');"); // Creamos una columna
client.print("let td6 = document.createElement('td');"); // Creamos una columna
client.print("let td7 = document.createElement('td');"); // Creamos una columna
client.print("let td8 = document.createElement('td');"); // Creamos una columna
// Hacemos los innerHTML
client.print("td1.innerHTML = ++regNo;");
client.print("td2.innerHTML = bomba;");
client.print("td3.innerHTML = bombilla;");
client.print("td4.innerHTML = humedad;");
client.print("td5.innerHTML = nivelAgua;");
client.print("td6.innerHTML = puerta;");
client.print("td7.innerHTML = tempAgua;");
client.print("td8.innerHTML = tempAmbiente;");
// bomba,bombilla, humedadad, movimiento, puerta, tempAgua, tempAmbiente
client.print("trow.appendChild(td1);");
client.print("trow.appendChild(td2);");
client.print("trow.appendChild(td3);");
client.print("trow.appendChild(td4);");
client.print("trow.appendChild(td5);");
client.print("trow.appendChild(td6);");
client.print("trow.appendChild(td7);");
client.print("trow.appendChild(td8);");
// el append al renglon
client.print("tbody.appendChild(trow);");
client.print("}"); // Cerramos la funcion

// Importamos la funcion de firebase
client.print("import {initializeApp} from 'https://www.gstatic.com/firebasejs/9.14.0/firebase-app.js';");
client.print("const firebaseConfig = {");
client.print("apiKey: 'AIzaSyAth51Y0Nr1wSgHHRmaRnFUvjyLTu6Um18',"); // La api de la aplicacion de firebase
client.print("authDomain: 'invernaderodomotizadodb.firebaseapp.com',"); // el dominio en el que esta nuestra db
client.print("projectId: 'invernaderodomotizadodb',"); // El id del proyecto
client.print("storageBucket: 'invernaderodomotizadodb.appspot.com',"); // El almacenamiento
client.print("messagingSenderId: '658102291357',"); // El id del mensajero
client.print("appId: '1:658102291357:web:28a63e712da17358dffd74'");
client.print("};"); // Cerramos la config
// Hasta aqui hicimos la segunda prueba y funciona
// Iniciamos firebase y le mandamos la base de datos como referencia
client.print("const app = initializeApp(firebaseConfig);");
client.print("import { getDatabase, ref, child, onValue, get }"); // Importamos las funciones de firebase
client.print("from 'https://www.gstatic.com/firebasejs/9.14.0/firebase-database.js';"); // Importamos la base de datos

// Trabajamos con la base de datos
client.print("const db = getDatabase();");// Obtenemos el contexto
// Hacemos una funcion para anadir los items a la tabla
client.print("function AddAllItemsToTable(Sensores){");
client.print("regNo++;"); // Incrementamos el contador
client.print("tbody.innerHTML = '';"); // Limpiamos el tbody
client.print("Sensores.forEach(element => {"); // Iteramos sobre los elementos
client.print("AddItemToTable(element.bomba,element.bombilla,element.humedadSuelo,element.nivelAgua,element.puerta,element.tempAgua,element.tempAmbiente);"); // Anadimos el item a la tabla
client.print("})"); // Cerramos el foreach
client.print("}"); // Cerramos la funcion

// Hacemos una funcion para obtener los datos de la base de datos
client.print("function GetAllDataOnce(){");
client.print("const dbRef = ref(db);"); // Obtenemos la referencia de la base de datos
client.print("get(child(dbRef, 'Sensores')).then((snapshot) => {"); // Obtenemos los datos de la base de datos
client.print("var sensores = [];"); // Creamos un arreglo para los sensores
client.print("snapshot.forEach(childSnapshot => {"); // Iteramos sobre los elementos
client.print("sensores.push(childSnapshot.val());"); // Anadimos el valor al arreglo
client.print("});"); // Cerramos el foreach
client.print("AddAllItemsToTable(sensores);"); // Anadimos los items a la tabla
client.print("})"); // Cerramos el get
client.print("}"); // Cerramos la funcion
// Hacemos que se carge la tabla
client.print("window.onload = GetAllDataOnce();");
// Funcion para los datos en tiempo real
client.print("function GetAllDataRealTime(){");// abrimos funcion para obtener datos en tiempo real
client.print("const dbRef = ref(db, 'Sensores');"); // Obtenemos la referencia de la base de datos
client.print("onValue(dbRef, (snapshot) => {"); // Obtenemos los datos de la base de datos
client.print("var sensores = [];"); // Creamos un arreglo para los sensores
client.print("snapshot.forEach(childSnapshot => {"); // Iteramos sobre los elementos)")
client.print("sensores.push(childSnapshot.val());"); // Anadimos los elementos al arreglo
client.print("});"); // Cerramos el foreach
client.print("AddAllItemsToTable(sensores);"); // Anadimos los items a la tabla
client.print("})"); // Cerramos el onValue
client.print("}"); // Cerramos la funcion
// Hacemos que se carge la tabla
client.print("window.onload = GetAllDataRealTime();");
// Cerramos el script
// Fin del codigo en js
client.print("</script>");
// Fin del codigo en html
client.print("</body>");
client.print("</html>");

            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          } else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected."); // CUando el dispositivo abandone el sitio lo desconectamos del server
    delay(30000); // Actualizamos cada 30 segundos la base de datos y el server
  }
}
