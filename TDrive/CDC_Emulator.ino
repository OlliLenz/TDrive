void cdc_emulator() {
  if (ibus_len = sizeof(HU_pulls_CDC)){
    cdc_announce_send = false;
  } else if (ibus_len = sizeof(HU_ReqPar_CDC)) {
    CDC_par_sent = true;
  } else {USB.println("Message not known to CDC!");}
  
  if (ready2send && !CDC_announce_sent){
    IBUSSend(CDCD_Respond);
    CDC_announce_sent = true;
  }
  if (!CDC_par_sent) {
    IBUSSend(CDC_Status);
    CDC_par_sent = true;
    // Clear buffer
  }
}

void cdc_commands() {
  
}
