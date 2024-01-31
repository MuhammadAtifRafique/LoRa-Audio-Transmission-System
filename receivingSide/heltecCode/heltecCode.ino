#include "LoRaWan_APP.h"
#include "Arduino.h"


#define RF_FREQUENCY                                915000000 // Hz

#define TX_OUTPUT_POWER                             12       // dBm

#define LORA_BANDWIDTH                              0        // [0: 125 kHz,
                                                              //  1: 250 kHz,
                                                              //  2: 500 kHz,
                                                              //  3: Reserved]
#define LORA_SPREADING_FACTOR                       7         // [SF7..SF12]
#define LORA_CODINGRATE                             1         // [1: 4/5,
                                                              //  2: 4/6,
                                                              //  3: 4/7,
                                                              //  4: 4/8]
#define LORA_PREAMBLE_LENGTH                        8         // Same for Tx and Rx
#define LORA_SYMBOL_TIMEOUT                         0         // Symbols
#define LORA_FIX_LENGTH_PAYLOAD_ON                  false
#define LORA_IQ_INVERSION_ON                        false


#define RX_TIMEOUT_VALUE                            1000
#define BUFFER_SIZE                                 254 // Define the payload size here
#define DDATA_SIZE                                  1000
char txpacket[BUFFER_SIZE];
char rxpacket[BUFFER_SIZE];
uint8_t full_data_2d_array[DDATA_SIZE][BUFFER_SIZE];
static RadioEvents_t RadioEvents;
bool is_done = false;

int16_t rxSize;
uint16_t val, temp_val;

bool lora_idle = true;

void setup() {
  val = 0;
    Serial.begin(115200);
    Mcu.begin();
    RadioEvents.RxDone = OnRxDone;
    Radio.Init( &RadioEvents );
    Radio.SetChannel( RF_FREQUENCY );
    Radio.SetRxConfig( MODEM_LORA, LORA_BANDWIDTH, LORA_SPREADING_FACTOR,
                               LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH,
                               LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON,
                               0, true, 0, 0, LORA_IQ_INVERSION_ON, true );
}
void resetArray(uint8_t array[][BUFFER_SIZE], int numRows, int numCols) {
  for (int i = 0; i < numRows; i++) {
    for (int j = 0; j < numCols; j++) {
      array[i][j] = 0; // Set each element to 0
    }
  }
}


void loop()
{
  if(lora_idle)
  {
    lora_idle = false;
    Radio.Rx(0);
  }
  Radio.IrqProcess( );
}

void OnRxDone( uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr )
{
  
  rxSize=size;
  memcpy(rxpacket, payload, size );
  Serial.write(rxpacket, BUFFER_SIZE);
   lora_idle = true;
}