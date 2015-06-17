// For running on Arduino, commented the below line.
//#include <SPI85.h>
#include <SPI.h>

#include <Mirf.h>
#include <nRF24L01.h>

// For running on Arduino, commented the below line.
//#include <MirfHardwareSpi85Driver.h>
#include <MirfHardwareSpiDriver.h>

struct packet_struct {	
	byte system_id;		// 1 byte
	byte component_id;	// 1 byte
	int data;		// 2 bytes
};

void setup () {

		// Only for Arduino		
		Serial.begin(9600);

                // For running on Arduino, commented the below line.
                //Mirf.cePin = PB4;
                //Mirf.csnPin = PB3;

                // For running on Arduino, commented the below line.
                //Mirf.spi = &MirfHardwareSpi85;
                Mirf.spi = &MirfHardwareSpi;

                Mirf.init();
  
                Mirf.setRADDR((byte *)"disp");   
                Mirf.payload = sizeof(packet_struct);              
                
		Mirf.channel = 50;
   
                Mirf.config();        
}

void loop () {
        
        //-----------------------------------------------------------------------------------------------------------------
        //#Asanka: experimenting with the received signal strength
        byte carrier_detect_reg_value = (byte) 0;
        byte dummy_byte_data = B10101010;
        //-----------------------------------------------------------------------------------------------------------------
        
        static packet_struct packet;
        //int num_sensors = 2;
        int num_sensors = 1;

        for (int sensor_number = 0; sensor_number<num_sensors; sensor_number++) {
                                               
                // generating request packet
                packet.system_id = 0;
                packet.component_id = sensor_number;
                packet.data = 0;
                
                // sending request packet
                Mirf.setTADDR((byte *)"disp"); 
                Mirf.send((byte *) &packet);
                // take note of the time we send this packet
                unsigned long time = millis();
                
                // wait till the sending is done  
                while (Mirf.isSending()) { }
                Serial.print("Controller: Sent a request packet to the sensor:");
                Serial.println(sensor_number);
                            
                //-----------------------------------------------------------------------------------------------------------------
               //#Asanka: experimenting with the received signal strength
               Mirf.readRegister(CD, &carrier_detect_reg_value, sizeof(carrier_detect_reg_value));
               Serial.print("carrier_detect_reg_value = ");
               Serial.println(carrier_detect_reg_value, BIN);
               Serial.println(dummy_byte_data, BIN);
               //-----------------------------------------------------------------------------------------------------------------             
                            
                // wait for the response data packet
                while (!Mirf.dataReady()) {
                        
                        //-----------------------------------------------------------------------------------------------------------------
                       //#Asanka: experimenting with the received signal strength
                       //RX mode
                       Mirf.ceHi();
                       Mirf.readRegister(CD, &carrier_detect_reg_value, sizeof(carrier_detect_reg_value));
                       Serial.print("carrier_detect_reg_value = ");
                       Serial.println(carrier_detect_reg_value, BIN);
                       Serial.println(carrier_detect_reg_value, HEX);
                       Serial.println(dummy_byte_data, BIN);
                       //-----------------------------------------------------------------------------------------------------------------  
                        
                        // it no response for a 1 second, we start sensing cycle again.
                        // ToDo: actually what should happpen here is moving to the next iteration of the for loop. But putting 'continue' will continue the above while
                        // loop instead of continuing outer for loop. So I put 'return' to goto the next iteration of the loop() cycle for the moment. Do something about it later!!
                        //if ((millis() - time) > 1000) {
                        if ((millis() - time) > 500) {
                                return;
                        }
                }
               Serial.print("Controller: Received a data packet from the sensor:");
               Serial.println(sensor_number);
  
               //-----------------------------------------------------------------------------------------------------------------
               //#Asanka: experimenting with the received signal strength
               //RX mode
               Mirf.ceHi();
               Mirf.readRegister(CD, &carrier_detect_reg_value, sizeof(carrier_detect_reg_value));
               Serial.print("carrier_detect_reg_value = ");
               Serial.println(carrier_detect_reg_value, BIN);
               Serial.println(dummy_byte_data, BIN);
               //-----------------------------------------------------------------------------------------------------------------  
               
                
                // process the received sensor data
                Mirf.getData((byte *) &packet);
                Serial.print("component ID = ");
                Serial.println(packet.component_id);
                Serial.print("data = ");
                Serial.println(packet.data);
                
                // wait for a while before sampling the sensors in the next iteration
                //delay(1000);
                delay(500);
        }
} 
  
  
  
