
#define trigPin 13
#define echoPin 12

#define ldrPin 0 // ldr analogic
#define ldrPin2 1 // ldr analogic

#define led 11 //on
#define led2 10 //off
#define led3 9  //light
#define led4 8 //system

const int default_light = 300;
const int minimum_light_limit = 80;
const int maximum_light_limit = 1000;
int light_limit_init = default_light;

const int default_distance = 100;
const int minimum_distance_limit = 5;
const int maximum_distance_limit = 150;
int distance_limit_init = default_distance;

const int default_timer = 2500;
const int minimum_timer = 500;
const int maximum_timer = 10000;
int timer_init = default_timer;

void passData(const String &title, const int &value);
void readSettings();

void setup() 
{ 
  Serial.begin (9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  pinMode(led, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);  
  pinMode(led4, OUTPUT);  
}

void loop() 
{
  long duration; 
  float distance = 0.0;
  
  int luminosity = 0;
  int light = 0;
  int light2 = 0;

  bool dis_ok = false;
  bool lum_ok = false;  
  
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);  
  digitalWrite(trigPin, LOW);

  light = analogRead(ldrPin);
  light2 = analogRead(ldrPin2);
  luminosity = (light+light2)/2;
   
  duration = (pulseIn(echoPin, HIGH)/2);
  distance = float(duration * 0.0343); 
 
  if (distance < distance_limit_init) 
  { 
    dis_ok = true;
    digitalWrite(led,HIGH); 
    digitalWrite(led2,LOW);    
    
  }else
  {
    dis_ok = false;
    digitalWrite(led,LOW);
    digitalWrite(led2,HIGH);    
  } 

   
 if(luminosity < light_limit_init)
  {
    lum_ok = true;
    digitalWrite(led3, HIGH); 

  }else 
  {
    lum_ok = false;
    digitalWrite(led3, LOW);
  }
  

  if(lum_ok && dis_ok) 
  {
    digitalWrite(led4, HIGH);  
    passData(String("ON#"),0);
    delayMicroseconds(100);
 
  }else
  {
    digitalWrite(led4, LOW);
    passData(String("OFF#"),0);
    delayMicroseconds(100);
  }
  
  delay(timer_init);
  passData(String("D#"),distance);
  passData(String("L#"),luminosity);
  readSettings();
}

void passData(const String &title, const int &value)
{  
  Serial.println(title+value);
  delay(250);
}

void readSettings()
{
 if (Serial.available() > 0)
  {    
    Serial.println("READING VALUES"); 
    int newDistance = Serial.parseInt();
    int newLight = Serial.parseInt();
    int newTimer = Serial.parseInt();

    Serial.println("SETTING VALUES"); 

    distance_limit_init = newDistance > minimum_distance_limit && newDistance < maximum_distance_limit ? newDistance : default_distance;
    light_limit_init = newLight > minimum_light_limit && newLight < maximum_light_limit ? newLight : default_light;
    timer_init = newTimer > minimum_timer && newTimer < maximum_timer ? newTimer : default_timer;
  
    Serial.println(String(distance_limit_init)+" - "+String(light_limit_init)+" - "+String(timer_init));    
  }
}
  
