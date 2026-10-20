// Ejemplo: encender y apagar el relay cada 3 segundos.
// El relay lo controla el STC15F104W, que recibe comandos por UART a 9600 baud.

void setup() {
  Serial.begin(9600);
}

void encenderRelay() {
  byte cmd[] = {0xA0, 0x01, 0x01, 0xA2};
  Serial.write(cmd, sizeof(cmd));
}

void apagarRelay() {
  byte cmd[] = {0xA0, 0x01, 0x00, 0xA1};
  Serial.write(cmd, sizeof(cmd));
}

void loop() {
  encenderRelay();
  delay(3000);

  apagarRelay();
  delay(3000);
}
