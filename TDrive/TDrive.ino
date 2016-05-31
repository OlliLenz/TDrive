#define IBUS = Serial1
#define USB = Serial
#define get_ibus_message = serialEvent1

const byte HU_pulls_CDC [5] = { 0x68, 0x03, 0x18, 0x01, 0x72 };
const byte CDCD_Respond [6]=  { 0x76, 0x04, 0xFF, 0x02, 0x00, 0xE1 };  //CD changer register or Response

const byte HU_ReqPar_CDC [7] = { 0x68, 0x05, 0x18, 0x38, 0x00, 0x00, 0x4D };
byte CDC_Status [12] = {0x18, 0x0A, 0x68, 0x39, 0x00, 0x02, 0x00, 0x3F, 0x00, 0x06, 0x05, 0x7D };  //CDC status not playing CD 06 05


byte IbusBuffer [37]; //IBUS Rx Buffer, known max is 37 bytes.
const byte RxPin = 2;
ibus_time = micros();

boolean end_of_message = false;
boolean ready2send = false;
boolean cdc_announce_send = false;
boolean send_sequence = false;
boolean CDC_par_sent = true;
boolean msg_processed = true;

int ibus_len = 0;

byte DEVS [3] = { 0X68,  // Head Unit
                  0x18,  // CDC
                  0xC8}; // PhoneMsg

void setup() {
    IBUS.begin(9600, SERIAL_8E1);
}

void loop() {
  if (end_of_message && !msg_processed){
    process_ibus_message();
  }
  
  if (ibus_time() - micros() > 1000 && !end_of_message){
    Serial.flush();
    end_of_message = true;
    USB.println("end of message");
  }
  else if (ibus_time() - micros() > 1500 && !ready2send) {
    ready2send = true;
    USB.println("Bus ready to send!");
  }
}

void get_ibus_message() {
  while (Serial.available() && !send_sequence) {
    IbusBuffer += Serial.read();
    ibus_time = micros();
    if (end_of_message) {
      ibus_len = 0;
      end_of_message = false;
    }
    ibus_len++;
    end_of_message = false;
    ready2send = false;
    if (ibus_len == 1 && (IbusBuffer[1]) == 0 || IbusBuffer[1] > 0x35)) { // dump the buffer
      IBUS.flush();
      ibus_len = 0;
      ibus_time = micros();
      msg_processed = true;
      USB.print("Not a valid message: Ibus junk length! Dumping serial buffer. Length: ");
      USB.println(IbusBuffer[1],DEC);
    } 
    else if (ibus_len > 4 && (IbusBuffer[1], DEC) == (ibus_len - 2)){
      IBUS.flush();
      ibus_time = micros();
      end_of_message = true;
      msg_processed = false;
      USB.print("Lenght matches, program will make iso_checksum in next sequence.");
    }

    else {
        ibus_len = 0;
        USB.println("Not a valid message: Message length matches but iso_checksum failed! Dumping serial buffer.");
      }
    }
}
