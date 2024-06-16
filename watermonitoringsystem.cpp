#include <DallasTemperature.h>
#include <OneWire.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);  

//declaring differennt pins connected to aurdino for different purposes
const int trigPin = 9;              // Connecting the trig pin of the ultrasonic sensor to digital pin 9
const int echoPin = 10;            // Connecting the echo pin of the ultrasonic sensor to digital pin 10
const int qualityPin  = 14;        // Connecting with the actual pin A0 connected to your turbidity sensor
const int waterTempPin= 2;         // Connecting  yellow wire to Pin2 on Arduino
const int relayPin = 5;            //connecting the real pin that will handle the water pump
const int manualPin=3;             //its the manual pin that will allow the user to manually control water pump
const int LEDpin=12;               //vecause we want to display to usr when ever the motor is ON by turning on the LED

 //declaring this variable which will help us understanding and also setting the realy module
 bool pumpOn = false;

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(waterTempPin);
 
// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(LEDpin,OUTPUT);
  pinMode(relayPin,OUTPUT);
  pinMode(manualPin, INPUT_PULLUP);
   /*In the context of a push button, which is a simple mechanical switch,
   connecting it to the pin with INPUT_PULLUP configuration has a specific purpose.
   When the button is not pressed, the pin is effectively pulled up to Vcc through the internal resistor, so the pin reads as HIGH. 
   When the button is pressed, it connects the pin to ground, and the pin reads LOW.*/
  sensors.begin();

}

void loop() {

  float duration, distance, water_level, water_cent, size, voltage,water_temp_C,water_temp_F,size_tank;
  float turbidityValue,high_level,low_level,medium_level;

//setting the length of container , that would be different for different sizes,
//in our case as we are using a small container so chossed the size according to our container in cm
  size=15.14;
  size_tank=10;
  high_level=0.75*size_tank;
  medium_level=0.5*size_tank;
  low_level=0.1*size_tank;
  
  //TRIGGERING/INITIATING DIFFERENT SENSOR SATTACHED/

  // Trigger the ultrasonic sensor
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(5);
  digitalWrite(trigPin, LOW);

    //MEASURING WATER LEVEL/

  // Measure the duration of the echo pulse
  duration = pulseIn(echoPin, HIGH);
  
  // Calculate the distance in centimeters
  //its the distance of sensor(top of container) and water surface
  distance = (duration * 0.0343) / 2;
  
  //water level will be tank level minus distnace 
  water_level=size_tank-(distance-5.14);

  //calculating the percentage based on tha sizeof container and amount of water in conytainer
  //it will be different for differenet sizes
  water_cent=(water_level/size_tank)*100;

  //WATER QUALITY SENSOR/

  // Reading the analog value from the turbidity sensor
  //The analogRead() function in Arduino reads this analog voltage and returns a digital value ranging from 0 to 1023.
  turbidityValue = analogRead(qualityPin);

  //CALCULATING THE VOLTAGE W.R.T TO TURBIDITY VALUE OBTAINED FROM SENSOR/

  // Converting the analog reading to voltage
  //Converting the raw analog reading to voltage allows for a standardized representation of the sensor output.
  //as the voltage generated from the water will tell  us what quality of our water is
  voltage = turbidityValue * (5.0 / 1024.0); 

  //TEMPERATURE SENSOR/

  // Calling sensors.requestTemperatures() to issue a global temperature and Requests to all devices on the bus
  sensors.requestTemperatures();

  //MEASURING WATER TEMPERATURE/

  water_temp_C=sensors.getTempCByIndex(0);
  water_temp_F=sensors.getTempFByIndex(0);

  //CATERING WITH WATER PUMP/

  // Check if water level is below 10% or manual button is pressed
  Serial.println(pumpOn);          //printing thison screen for ur ease to check th status of variable 
  if (water_level <= high_level || digitalRead(manualPin) == LOW) {
     // Turn on the water pump through the relay
     if(water_level >= low_level || water_level <= low_level)
     {
    digitalWrite(relayPin, HIGH);
    digitalWrite(LEDpin, HIGH);
    //turning on the LED light when the motor is on to give a physical signal to authority that motor is ON
    lcd.setCursor(0, 0);
    lcd.print("Low Level,PumpON!");
    Serial.println("Water pump is ON");
    Serial.println("manualpin");
    Serial.println(digitalRead(manualPin));
    pumpOn = true;
     

    //the status ofpumpOn should get chnaged if this condiotn works, so checking its value to ensure that its changed
    Serial.println(pumpOn);
     }
     else{
      digitalWrite(relayPin, HIGH);
    digitalWrite(LEDpin, HIGH);
    //turning on the LED light when the motor is on to give a physical signal to authority that motor is ON
    lcd.setCursor(0, 0);
    lcd.print("Low Level,PumpON!");
    Serial.println("Water pump is ON");
    Serial.println("manualpin");
    Serial.println(digitalRead(manualPin));
    pumpOn = true;
     }
  } 
  else if ((water_level >= high_level && digitalRead(manualPin) == HIGH))
   {  // Turn off the water pump through the relay
    Serial.println(pumpOn);
    digitalWrite(relayPin, LOW);
    digitalWrite(LEDpin, LOW);
    Serial.println("manualpin");
    Serial.println(digitalRead(manualPin));
    Serial.println("Water pump is OFF");
    pumpOn = false;
     //the status ofpumpOn should get chnaged if this condiotn works, that can be indicated by LED turing OFF
     /* when the water level reaches the upper limit than it will automatically turn off the motor,
    and will display the water level on lcd screen*/
  }
         
 
  //DISPLAYING INSTANCES ON SERIAL MONITOR/

  //for our own instance so we get each changing attribute at serial monitor
  // Print the distance on the serial monitor for clear monitoring
 
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
  Serial.print("Water Level in cm:");
  Serial.print( water_level);
  Serial.println( " cm");

  Serial.print("Water Level in Percentage:");
  Serial.print( water_cent);
  Serial.println( " %");

  // Printing the raw turbidity value and voltage to the serial monitor
  Serial.print("Raw Turbidity Value: ");
  Serial.print(turbidityValue);
  Serial.println(" NTU");
  Serial.print("Voltage: ");
  Serial.print(voltage);
  Serial.println(" V");

  //printing the temperature on serial monitor
  Serial.print("Celsius temperature: ");
  Serial.print(water_temp_C); 
  Serial.print(" ^C"); 
  Serial.print(" - Fahrenheit temperature: ");
  Serial.print(water_temp_F);
  Serial.println(" ^F"); 

  //DISPLAYING INSTANCES ON LCD SCREEN

  // Displaying the distance on the LCD
  //clearing the screen first so iit doesnot print any garbage content on screen that was already fed may be during some experimentation
  //this lines will be displayed on lcd when the motor is not turned on, 
  //once the motor gets on it will show th status of ON on screen till it gets off again
  //then it will display the water level on LCD screen
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Water Level:");
  lcd.setCursor(12, 0);
  lcd.print( water_cent);
  lcd.print(" %");


  // Checking if the water is clean based on the voltage threshold
  // We have set the the threshold voltage for our container of water by measureing the turbidity and voltage of different samples of water  
  if (voltage < 1.5) 
  {
    lcd.setCursor(0, 1);
    lcd.print( "Clear!");
    Serial.println("Clear!");
  } 
  else 
  {
    lcd.setCursor(0, 1);
    lcd.print( "Dirty!");
    Serial.println("Dirty!");
  }
  
  //Displaying if the water is hot and cold with respect  Temparature sensed by sensor on Lcd screen\
  //differentiating the temperature to three different parameters
  //keeping the 20^C-25^C water as the standard temperature that states the that water is moderate

  if(water_temp_C>25)
  {
     lcd.setCursor(7,1);
     lcd.print(" HOT!");
  }
  else if(water_temp_C>20&&water_temp_C<25)
  {
    lcd.setCursor(7,1);
    lcd.print("MODERATE");
  }
  else
  {
     lcd.setCursor(7,1);
    lcd.print("COLD");
  }
 Serial.println("manualpin");
    Serial.println(digitalRead(manualPin));
  //delay is decided by develpor as after how much time he wants new calculations
  //in our case we selected the delay small so we get changes more often but it can be modeled according to the need of time while installing setup
  delay(2000);  // Delay for 2 second before the next measurement
}
