/*
  Modbus RTU Server Kitchen Sink
  This sketch creates a Modbus RTU Server and demostrates
  how to use various Modbus Server APIs.
  created 18 July 2018
  by Sandeep Mistry

  Modified by: R.Roz

#ifdef MODBUS_SIMPLE_ECHO_DEVICE
    Simple Echo device (maps coils state to inputs and Holding regs to input regs):
        3 COILS Pins    D3-5
        3 PWMs  Pins    D9-11
        
#else
        
    Normal Modbus slave device
        3 COILS Pins    D3-5
        3 DInputs Pins  D6,7,8
        3 PWMs  Pins    D9-11
        6 ADCs  Pins    A0-5
        
#endif       

Connection to RS845-TTL converter :
  ○ 4pin header
    § DI - Driver in (the transmitter pin TX of Arduino)
    § DE - Driver enable (enabled when this pin is HIGH) - to D2 Arduino (both together DE and RE)
    § RE - Receiver enable (enabled when this pin is LOW) - to D2 Arduino
    § RO - Receiver out (the receiver pin RX of Arduino)
  ○ 4pin header
    § VCC (+5VDC)  -  to  Arduino 5V
    § GND (-)      -  to  Arduino GND
  ○ Screw terminal block
    § B - Connect to pin B of the other 485 IC
    § A - Connect to pin A of the other 485 IC


*/

#include <ArduinoRS485.h> // ArduinoModbus depends on the ArduinoRS485 library */
#include <ArduinoModbus.h>

#define MODBUS_SIMPLE_ECHO_DEVICE  1   // just echo coils to inputs and holding regs to input regs
//#define MODBUS_SIMPLE_ECHO_DEVICE    // else map to real pins (digital, analog, inputs, outputs

#define MODBUS_SLAVE_ID 42


#ifdef MODBUS_SIMPLE_ECHO_DEVICE
  const int numCoils = 3;             // Pins 3,4,5 Digital outputs 
  const int numDiscreteInputs = 3;
  const int numHoldingRegisters = 3;  // Pins 9,10,11 PWM (Analog) outputs 
  const int numInputRegisters = 3;
#else
  const int numCoils = 3;             // Pins D3,4,5,  Digital outputs 
  const int numDiscreteInputs = 3;    // Pins D6,7,8   Digital inputs 
  const int numHoldingRegisters = 3;  // Pins D9,10,11 PWM (Analog) outputs 
  const int numInputRegisters = 6;    // Pins A0-A5 Analog inputs
 
#endif


static const unsigned long TIMER_INTERVAL = 10000; // ms
static unsigned long lastRefreshTime = 0;

void setup() {
/*  Serial.begin(9600);
   while (!Serial);
  Serial.println("Modbus RTU Server Kitchen Sink");
*/

  // start the Modbus RTU server, with (slave) id 42
  if (!ModbusRTUServer.begin(MODBUS_SLAVE_ID, 9600)) {
    Serial.println("Failed to start Modbus RTU Server!");
    while (1);
  }
  delay(2000);

 
  pinMode( 3, OUTPUT);    // sets the digital pin as output
  pinMode( 4, OUTPUT);    // sets the digital pin as output
  pinMode( 5, OUTPUT);    // sets the digital pin as output

  pinMode( 6, INPUT);    // sets the digital pin as input
  pinMode( 7, INPUT);    // sets the digital pin as input
  pinMode( 8, INPUT);    // sets the digital pin as input

  pinMode( 9, OUTPUT);    // sets the digital pin as output
  pinMode(10, OUTPUT);    // sets the digital pin as output
  pinMode(11, OUTPUT);    // sets the digital pin as output

  pinMode(A0, INPUT);    // sets the analog pin as input
  pinMode(A1, INPUT);    // sets the analog pin as input
  pinMode(A2, INPUT);    // sets the analog pin as input
  pinMode(A3, INPUT);    // sets the analog pin as input
  pinMode(A4, INPUT);    // sets the analog pin as input
  pinMode(A5, INPUT);    // sets the analog pin as input
  
  // configure coils at address 0x00
  ModbusRTUServer.configureCoils(0x00, numCoils);

  // configure discrete inputs at address 0x00
  ModbusRTUServer.configureDiscreteInputs(0x00, numDiscreteInputs);

  // configure holding registers at address 0x00
  ModbusRTUServer.configureHoldingRegisters(0x00, numHoldingRegisters);

  // configure input registers at address 0x00
  ModbusRTUServer.configureInputRegisters(0x00, numInputRegisters);



/*
  */
}

void loop() {
  if(millis() - lastRefreshTime >= TIMER_INTERVAL)
  {
        lastRefreshTime = millis();
    //  do something every x secs
  }

  // poll for Modbus RTU requests
  ModbusRTUServer.poll();

  #ifdef MODBUS_SIMPLE_ECHO_DEVICE
      // map the coil values to the discrete input values
      for (int i = 0; i < numCoils; i++) {
        int coilValue = ModbusRTUServer.coilRead(i);
        digitalWrite(3+i, coilValue);
        ModbusRTUServer.discreteInputWrite(i, coilValue);
      }
    
      // map the holding register values to the input register values
      for (int i = 0; i < numHoldingRegisters; i++) {
        long holdingRegisterValue = ModbusRTUServer.holdingRegisterRead(i);
        analogWrite(9+i, holdingRegisterValue);
        ModbusRTUServer.inputRegisterWrite(i, holdingRegisterValue);
      }
  #else
      // map the coil values to pins
      for (int i = 0; i < numCoils; i++) {
        int coilValue = ModbusRTUServer.coilRead(i);
        digitalWrite(3+i, coilValue);
      }
    
      // map input values from pins
      for (int i = 0; i < numDiscreteInputs; i++) {
        int inputValue = digitalRead(6+i);
        ModbusRTUServer.discreteInputWrite(i, inputValue);
      }
  
      // map the holding register values to PWM pins
      for (int i = 0; i < numHoldingRegisters; i++) {
        long holdingRegisterValue = ModbusRTUServer.holdingRegisterRead(i);
        analogWrite(9+i, holdingRegisterValue);
      }
  
      // map the input register values from analog pins - delay and two measurements are recomended in forums
      for (int i = 0; i < numInputRegisters; i++) {
        
        long inputRegisterValue = analogRead(i);
        delay(10);
        inputRegisterValue = analogRead(i);
        delay(10);
        ModbusRTUServer.inputRegisterWrite(i, inputRegisterValue);
      }
  
  #endif
}
