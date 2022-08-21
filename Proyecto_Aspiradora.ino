#include <WiFi.h>
#include <UltraDistSensor.h>


//Pines para el motor 1
#define vel1 13 //enable1
#define forward_m1 12 //in1
#define reverse_m1 11 //in2
//Pines para el motor 2
#define vel2 8 //enable2
#define forward_m2 10//in3
#define reverse_m2 9 //in4

//Pines para el motor 3 (aspiradora)
#define vel3 29 //enable2
#define forward_m3 31//in3
#define reverse_m3 30 //in4

#define button_enc 4 //botón para encender y apagar motores
#define Trig1 24 //Pin del trigger
#define Echo1 23 //Pin del Echo
#define Trig2 25 //Pin del trigger
#define Echo2 26 //Pin del Echo

#define led 7

//Configuraciones para el PWM
const int frequency = 1000;
const int pwm_channel = 0;
const int pwm_channel_2 = 0;
const int resolution = 8;
int dutyCycle = 125; //Velocidad de los motores de 0 a 255
int dutyCycle_2 = 255;



UltraDistSensor ultra1;
UltraDistSensor ultra2;
int distance1;
int distance2;

int val = 0; //val se emplea para almacenar el estado del boton
int state = 0; // 0 motores apagados, mientras que 1 motores encendidos.
int old_val = 0; // almacena el antiguo valor de val

const char* ssid = "WIFI";
const char* pass = "password";

enum {encendido, avance, obstaculo} estado ;

void IRAM_ATTR Encendido(){
  val= digitalRead(button_enc); // lee el estado del Boton
  if ((val == HIGH) && (old_val == LOW)){
  state=1-state;
  delay(10);
  }
  old_val = val; // valor antiguo 
  if (state==1){
   estado=avance; // envía el estado avance
  }
  else{
    estado=encendido; // envía el estado encendido
    }
}

void setup() {
  pinMode(vel1, OUTPUT);
  pinMode(forward_m1, OUTPUT);
  pinMode(reverse_m1, OUTPUT);
  pinMode(vel2, OUTPUT);
  pinMode(forward_m2, OUTPUT);
  pinMode(reverse_m2, OUTPUT);
  pinMode(vel3, OUTPUT);
  pinMode(forward_m3, OUTPUT);
  pinMode(reverse_m3, OUTPUT);
  pinMode(button_enc, INPUT);
  pinMode(led, OUTPUT);
  
  ledcSetup(pwm_channel, frequency, resolution);
  ledcAttachPin(vel1, pwm_channel);
  ledcAttachPin(vel2, pwm_channel);
  ledcAttachPin(vel3, pwm_channel_2);
  ledcWrite(pwm_channel, dutyCycle);
  ledcWrite(pwm_channel_2, dutyCycle_2);

  ultra1.attach(Trig1,Echo1);
  ultra2.attach(Trig2,Echo2);
  Serial.begin(115200);
  estado=encendido;
  attachInterrupt(digitalPinToInterrupt(button_enc),Encendido, RISING); //Interrupción del botón encendido/avance
  WiFi.begin(ssid, pass);
  delay(300);
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi Connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
}

void loop() {
  switch(estado){
    case encendido:
    Serial.println("Encendido");
    //Todos los motores apagados.
    digitalWrite(led, HIGH);
    digitalWrite(forward_m1, LOW);
    digitalWrite(reverse_m1, LOW);
    digitalWrite(forward_m2, LOW);
    digitalWrite(reverse_m2, LOW);
    digitalWrite(forward_m3, LOW);
    digitalWrite(reverse_m3, LOW);
        break;
    
    case avance:
    Serial.println("Avance");
    digitalWrite(led, HIGH);
    delay(1000);
    digitalWrite(led, LOW);
    delay(1000);
    
    //Enciende primer motor
    digitalWrite(forward_m1, HIGH);
    digitalWrite(reverse_m1, LOW);
    //Enciende segundo motor
    digitalWrite(forward_m2, HIGH);
    digitalWrite(reverse_m2, LOW);
    //Enciende el motor de la aspiradora
    digitalWrite(forward_m3, HIGH);
    digitalWrite(reverse_m3, LOW);
    distance1=ultra1.distanceInCm(); //Calcula la distancia en que se encuentre un objeto en cm
    distance2=ultra2.distanceInCm(); //Calcula la distancia en que se encuentre un objeto en cm
    //Serial.println(distance);
    if (distance1<10 || distance2<10){
      //Si un obstáculo se encuentra a una distancia menor a 10cm se detienen los motores y pasa al estado obstáculo.
      digitalWrite(forward_m1, LOW);
      digitalWrite(reverse_m1, LOW);
      digitalWrite(forward_m2, LOW);
      digitalWrite(reverse_m2, LOW);
      estado=obstaculo;
        break; 
      }

    
    case obstaculo:
    Serial.println("Obstáculo");
    digitalWrite(led, HIGH);
    delay(300);
    digitalWrite(led, LOW);
    delay(300);
    //Se enciende los motores de las ruedas en retroceso.
    digitalWrite(forward_m1, LOW);
    digitalWrite(reverse_m1, HIGH);
    digitalWrite(forward_m2, LOW);
    digitalWrite(reverse_m2, HIGH);
    delay(1000); //Por un segundo
    //Se enciende el motor de la rueda izquierda para girar a la derecha
    digitalWrite(forward_m1, LOW);
    digitalWrite(reverse_m1, LOW);
    digitalWrite(forward_m2, LOW);
    digitalWrite(reverse_m2, HIGH);
    delay(2000); //Por dos segundo
    estado=avance; //Cambio a estado de avance
        break;

    }

}
