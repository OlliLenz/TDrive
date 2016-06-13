#define IBUS Serial1
#define USB Serial
#define get_ibus_message serialEvent1
#define CDC_Emulator // uncomment to disable CDC Emulator

byte HU_pulls_CDC [6] = { 0x68, 0x03, 0x18, 0x01, 0x72 };
byte CDC_Register [6] = { 0x18, 0x04, 0xFF, 0x02, 0xE1 };  //CD changer register or Response

byte HU_ReqPar_CDC [8] = { 0x68, 0x05, 0x18, 0x38, 0x00, 0x00, 0x4D };
byte CDC_Status [13] = {0x18, 0x0A, 0x68, 0x39, 0x00, 0x02, 0x00, 0x3F, 0x00, 0x06, 0x05, 0x7D };  //CDC status not playing CD 06 05


byte IbusBuffer [37]; //IBUS Rx Buffer, known max is 37 bytes.
byte SendBuffer [37]; //IBUS Tx Buffer, known max is 37 bytes.
// boolean SendTask false; //True if message in the SendBuffer has to be sent.

const byte RxPin = 2;
unsigned long ibus_time = micros();
unsigned long CDC_time = millis(); // To not to lose CDC emulation, time based check will be implemented.

boolean end_of_message = false;
boolean ready2send = false;
boolean CDC_Registered = false;
boolean send_sequence = false;
boolean CDC_par_sent = true;
boolean msg_processed = true;

int ibus_len = 0;

byte DEVS [3] = { 0X68,  // Head Unit
                  0x18,  // CDC
                  0xC8}; // PhoneMsg


void setup() {
    IBUS.begin(9600, SERIAL_8E1);
    USB.begin(9600, SERIAL_8E1);
}

void loop() {
  if (end_of_message && !msg_processed){
    process_ibus_message();
  }

  if (ibus_time - micros() > 1200 && !end_of_message){
    Serial.flush();
    end_of_message = true;
    USB.write(IbusBuffer, IbusBuffer[1]);
    USB.println("end of message");
  }
  else if (ibus_time - micros() > 3000 && !ready2send) {
    ready2send = true;
    USB.println("Bus ready to send!");
  }
  #if defined(CDC_Emulator)
    cdc_announcer();
  #endif
}

void get_ibus_message() {
  while (Serial.available()) {
    ibus_time = micros();
    if (end_of_message) {
      ibus_len = 0;
      end_of_message = false;
    }
    IbusBuffer[ibus_len] = Serial.read();
    ibus_len++;
    ready2send = false;
    if (ibus_len == 2) { if (IbusBuffer[1] == 0 || IbusBuffer[1] > 0x35) { // dump the buffer
      IBUS.flush();
      ibus_len = 0;
      msg_processed = true;
      USB.print("Not a valid message: Ibus junk length! Dumping serial buffer. Length: ");
      USB.println(IbusBuffer[1], DEC);
    }}
    else if (ibus_len > 4 && IbusBuffer[1] == ibus_len - 2){
      IBUS.flush();
      ibus_len = 0;
      ibus_time = micros();
      end_of_message = true;
      msg_processed = false;
      USB.println("Lenght matches, program will make iso_checksum in next sequence.");
    }
    else if (ibus_len == IbusBuffer[1]) {
      USB.write(IbusBuffer, ibus_len, HEX);
    }
  }
}

//checksum calculator for IBUS
byte iso_checksum(byte *data, byte len) //len is the number of bytes (not the # of last byte)
{
  byte crc=0;
  for(byte i=0; i<len; i++)
  {
    crc=crc^data[i];
  }
  return crc;
}

void IBUSSend(byte *data) {
  send_sequence = true;
  IBUS.write(data, data[1]+2);
  send_sequence = false;
  ready2send = false;
  ibus_time = micros();
}

void process_ibus_message() {
  msg_processed = true;
  if (iso_checksum(IbusBuffer, ibus_len) == IbusBuffer[ibus_len - 1]) {
    USBSend(IbusBuffer, ibus_len);
    if (IbusBuffer [2] = DEVS [1]) {
      cdc_set_parameters();
    }
  }
}

void cdc_set_parameters() {
  if (ibus_len = sizeof(HU_pulls_CDC)){
    CDC_Registered = false;

  } else if (ibus_len = sizeof(HU_ReqPar_CDC)) {
    CDC_par_sent = false;
  } else {USB.println("Message not known to CDC!");}
}

void cdc_announcer() {
  if (ready2send && !CDC_Registered) {
    IBUS.write(CDC_Register, sizeof(CDC_Register));
    CDC_time = millis(); // To not to lose CDC emulation, time based check will be implemented.
    CDC_Registered = true;
  } else if (ready2send && !CDC_par_sent) {
      IBUS.write(CDC_Status, sizeof(CDC_Status));
      CDC_par_sent = true;
  }
}

void USBSend(byte data[], int len) { // Sends ibus buffer to the PC.
  for (int i=0; i <= len; i++) {
    USB.print(data[i], HEX);
    USB.print(" ");
  }
  USB.println("");
}

void usb_debug () {
  for (int i=0; i <= IbusBuffer[1] + 1; i++ ){
    USB.print(IbusBuffer[i], HEX);
    USB.print(" ");
  }
  USB.println("");
}

unsigned long lcd_time = millis();
int txt_pos = 0;
byte txt [11];
string artist;
string album;
string title;
byte lcd_msg[18] = { //first 6 bytes are fixed (18 bytes max)
  0xC8,0x10,0x80,0x23,0x42,0x30,0x44,0x61,0x75,0x67,0x68,0x74,0x72,0x79,0x19,0x19,0x19,0x30};//message default
byte clear_screen [7] = {0xC8, 0x05, 0x80, 0x23, 0x41, 0x20, 0x0F};  //Clear radio display

// it is 9 digit lcd
void update_lcd {
  if (sizeof(title)>11){
    IBUS.write(txt, 11);
  }
  txt = byte(artist + title); // replace the combination according to your wish
  if (busready) {
    IBUS.write(txt);
  }
}
