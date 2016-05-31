void IBUSSend(send_buffer) {
  if (ready2send()) {
    send_sequence = true;
    for (int i=0; i <= sizeof(send_buffer; i++){
      IBUS.write(send_buffer[i]);
    }
    send_buffer = {};
    send_sequence = false;
    ready2send = true;
  }
}

void usb_send() { // Sends ibus buffer to the PC.
  for (int i=0; i <= ibus_len; i++) {
    USB.print(IbusBuffer[i], HEX);
    USB.print(" ");
  }
  USB.println("");
}

