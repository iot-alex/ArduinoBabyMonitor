/*
Based on code
created 18 Dec 2009
by David A. Mellis
modified 9 Apr 2012
by Tom Igoe

Modified by Julio Cesar Villalta III

*/

#include <Adafruit_VC0706.h>
#include <SD.h>
#include <SoftwareSerial.h> 
#include <SPI.h>
#include <Ethernet.h>        

// uncomment this line if using Arduino V23 or earlier
// #include <NewSoftSerial.h>       

#define tempAPin        0
#define yellowEthLED    2
#define yellowCamLED    3
#define sdPin           4
#define yellowThermLED  5
#define redLED          6
#define greenLED        7
#define camRXPin        8
#define camTXPin        9
#define ethPin          10



// Using SoftwareSerial (Arduino 1.0+) or NewSoftSerial (Arduino 0023 & prior):
#if ARDUINO >= 100
	// On Uno: camera TX connected to pin 2, camera RX to pin 3:
	SoftwareSerial cameraconnection = SoftwareSerial(9, 8);
	// On Mega: camera TX connected to pin 69 (A15), camera RX to pin 3:
	//SoftwareSerial cameraconnection = SoftwareSerial(69, 3);
#else
	NewSoftSerial cameraconnection = NewSoftSerial(9, 8);
#endif
Adafruit_VC0706 cam = Adafruit_VC0706(&cameraconnection);


// Using hardware serial on Mega: camera TX conn. to RX1,
// camera RX to TX1, no SoftwareSerial object is required:
//Adafruit_VC0706 cam = Adafruit_VC0706(&Serial1);
byte mac[] = { 0x90, 0xA2, 0xDA, 0x0F, 0x3A, 0xF4};
IPAddress ip(192,168,1,251);
IPAddress myDns(8,8,8,8);

// Initialize the Ethernet server library
// with the IP address and port you want to use 
// (port 80 is default for HTTP):
EthernetServer server(80);


void cameraSetup() {

	// When using hardware SPI, the SS pin MUST be set to an
	// output (even if not connected or used).  If left as a
	// floating input w/SPI on, this can cause lockuppage.
	#if !defined(SOFTWARE_SPI)
		#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
			if(sdPin != 53) pinMode(53, OUTPUT); // SS on Mega
		#else
			if(sdPin != 10) pinMode(10, OUTPUT); // SS on Uno, etc.
		#endif
	#endif


	Serial.println("VC0706 Camera test");

	// see if the card is present and can be initialized:
	if (!SD.begin(sdPin)) 
	{
		Serial.println("Card failed, or not present");
		// don't do anything more:
		return;
	}  
  
	// Try to locate the camera
	if (cam.begin()) 
	{
		Serial.println("Camera Found:");
	} 
	else 
	{
		Serial.println("No camera found?");
		return;
	}
	// Print out the camera version information (optional)
	char *reply = cam.getVersion();
	if (reply == 0) 
	{
    Serial.print("Failed to get version");
	} 
	else 
	{
		Serial.println("-----------------");
		Serial.print(reply);
		Serial.println("-----------------");
	}

	// Set the picture size - you can choose one of 640x480, 320x240 or 160x120 
	// Remember that bigger pictures take longer to transmit!
  
	//cam.setImageSize(VC0706_640x480);        // biggest
	cam.setImageSize(VC0706_320x240);        // medium
	//cam.setImageSize(VC0706_160x120);          // small

	// You can read the size back from the camera (optional, but maybe useful?)
	uint8_t imgsize = cam.getImageSize();
	Serial.print("Image size: ");
	if (imgsize == VC0706_640x480) Serial.println("640x480");
	if (imgsize == VC0706_320x240) Serial.println("320x240");
	if (imgsize == VC0706_160x120) Serial.println("160x120");


	//  Motion detection system can alert you when the camera 'sees' motion!
	cam.setMotionDetect(true);           // turn it on
	//cam.setMotionDetect(false);        // turn it off   (default)

	// You can also verify whether motion detection is active!
	Serial.print("Motion detection is ");
	if (cam.getMotionDetect()) 
		Serial.println("ON");
	else 
		Serial.println("OFF");
}
void ethSetup()
{

	// start the Ethernet connection and the server:
	Ethernet.begin(mac, ip);
	server.begin();
	Serial.print("server is at ");
	Serial.println(Ethernet.localIP());
}
void LEDSetup()
{
	// initialize digital pin 13 as an output.
	pinMode(greenLED, OUTPUT);
	pinMode(redLED, OUTPUT);
	pinMode(yellowCamLED, OUTPUT);
	pinMode(yellowEthLED, OUTPUT);
	pinMode(yellowThermLED, OUTPUT);

}



void setup() 
{
	// Open serial communications and wait for port to open:
	Serial.begin(38400);	
	cameraSetup();
	ethSetup();
	LEDSetup();
}

void blinkLED(int LED, int time, int timesRepeat)
{
	for( int i = 0; i < timesRepeat; i++)
	{
		digitalWrite(LED, HIGH);   // turn the LED on (HIGH is the voltage level)
		delay(time);              
		digitalWrite(LED, LOW);    // turn the LED off by making the voltage LOW
		delay(time*1.5);
	}  
}

void loop() 
{
	blinkLED(yellowThermLED,200,1);
	/************* TEMPERATURE ***************/
	//digitalWrite(yellowThermLED, HIGH);   // turn the LED on (HIGH is the voltage level)
	int readPin = analogRead(tempAPin);
	float voltage= readPin;// * (5000/1024);  //currently using 5V power
	float tempF = ((voltage - 500)/10) *(9.0/5.0);
	float tempC = tempF *(5.0/9.0) - 32;
	
	Serial.println(tempF);
	Serial.println(tempC);
	/*****************************************/
	blinkLED(yellowCamLED,200,2);
	char filename[13];
	/**************** CAMERA ****************/
	if (cam.motionDetected()) 
	{
		Serial.println("Motion!");   
		cam.setMotionDetect(false);

		if (! cam.takePicture()) 
			Serial.println("Failed to snap!");
		else 
			Serial.println("Picture taken!");

		//char filename[13];
		/*
		strcpy(filename, "IMAGE00.JPG");
		for (int i = 0; i < 100; i++) 
		{
			filename[5] = '0' + i/10;
			filename[6] = '0' + i%10;
			// create if does not exist, do not open existing, write, sync after write
			if (! SD.exists(filename)) 
			{
				break;
			}
		}
		*/
		Serial.println("here");
		strcpy(filename, "IMAGE000.JPG");
		Serial.println("here");
		for (int j = 0; j < 1000; j++)
		{
			filename[5] = '0' + j/100;
			filename[6] = '0' + ((j/10)%10);
			filename[7] = '0' + j%10;
			// create if does not exist, do not open existing, write, sync after write
			if (! SD.exists(filename)) 
			{
				break;
			}
		}
		File imgFile = SD.open(filename, FILE_WRITE);
		
		int32_t time = millis();
		uint16_t jpglen = cam.frameLength();
		Serial.print(jpglen, DEC);
		Serial.println(" byte image");

		Serial.print("Writing image to "); Serial.print(filename);
		
		//fileBuffer = String(filename);
		//Serial.println(fileBuffer);

		while (jpglen > 0) 
		{
			// read 32 bytes at a time;
			uint8_t *buffer;
			uint8_t bytesToRead = min(32, jpglen); // change 32 to 64 for a speedup but may not work with all setups!
			buffer = cam.readPicture(bytesToRead);
			imgFile.write(buffer, bytesToRead);

			//Serial.print("Read ");  Serial.print(bytesToRead, DEC); Serial.println(" bytes");

			jpglen -= bytesToRead;
		}
		imgFile.close();
		time = millis()-time;
		//digitalWrite(yellowCamLED,LOW);
		Serial.print("Done!\n Took ");
		Serial.print(time);
		Serial.println(" ms\n");
		cam.resumeVideo();
		cam.setMotionDetect(true);
	}
	/*****************************************/
	blinkLED(yellowEthLED,200,3);
	/**************** ETHERNET ***************/
	// listen for incoming clients
	EthernetClient client = server.available();
	if (client) 
	{
		Serial.println("new client");
		// an http request ends with a blank line
		boolean currentLineIsBlank = true;
		while (client.connected()) 
		{
		  
			if (client.available()) 
			{
				char c = client.read();
				Serial.write(c);
				// if you've gotten to the end of the line (received a newline
				// character) and the line is blank, the http request has ended,
				// so you can send a reply
				if (c == '\n' && currentLineIsBlank) 
				{
					// send a standard http response header
					client.println("HTTP/1.1 200 OK");
					client.println("Content-Type: text/html");
					client.println("Connection: close");  // the connection will be closed after completion of the response
					client.println("Refresh: 5");  // refresh the page automatically every 5 sec
					client.println();
					client.println("<!DOCTYPE HTML>");
					client.println("<html>");
					
					client.print("TempC: ");
					client.print(tempC);
					client.println("<br />");       
					
					client.print("TempF: ");
					client.print(tempF);
					client.println("<br />"); 

					client.print("Picture: ");
					client.print(filename);
					client.println("<br />");
     
								
					client.println("</html>");
					break;
				}
				if (c == '\n') 
				{
					// you're starting a new line
					currentLineIsBlank = true;
				} 
				else if (c != '\r') 
				{
					// you've gotten a character on the current line
					currentLineIsBlank = false;
				}
			}
		}
		// give the web browser time to receive the data
		delay(3);
		// close the connection:
		client.stop();
		Serial.println("client disconnected");
	}
	/************************************/
}
