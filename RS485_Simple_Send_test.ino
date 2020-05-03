int EN = 2; //RS485 has a enable/disable pin to transmit or receive data. Arduino Digital Pin 2 = Rx/Tx 'Enable'; High to Transmit, Low to Receive

void setup() 
{
  pinMode(EN, OUTPUT);
  Serial.begin(9600);
} 

void loop()
{
// send data 
  digitalWrite(EN, HIGH);//Enable data transmit
  Serial.print('A');
  delay(1000);
}
