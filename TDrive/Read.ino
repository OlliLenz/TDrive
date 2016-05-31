void process_ibus_message() {
  msg_processed = true;
  if (iso_checksum(IbusBuffer, byte ibus_len) == IbusBuffer[ibus_len - 1]) {
    usb_send();
    if (IbusBuffer [2] = DEVS [1]) {
      cdc_commands();
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

void compare(array2c) {
  for (int i = 0; i <=
  
}

