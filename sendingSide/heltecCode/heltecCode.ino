/*

Date : 19/06/2023
Company : TechnVerse
Deveoper : Misbah
Code : Read Data from PDM mic and serial print for python script

*/
#include <LoRaWan_APP.h>
#include<Arduino.h> // include arduino header file
#include <driver/i2s.h> // include i2s or pdm driver


#define RF_FREQUENCY                                915000000 // Hz

#define TX_OUTPUT_POWER                             20       // dBm

#define LORA_BANDWIDTH                             0         // [0: 125 kHz,
                                                              //  1: 250 kHz,
                                                              //  2: 500 kHz,
                                                              //  3: Reserved]
#define LORA_SPREADING_FACTOR                       7         // [SF7..SF12] same as receiver side
#define LORA_CODINGRATE                             1         // [1: 4/5,
                                                              //  2: 4/6
                                                              //  3: 4/7,
                                                              //  4: 4/8]
#define LORA_PREAMBLE_LENGTH                        8         // Same for Tx and Rx
#define LORA_SYMBOL_TIMEOUT                         0         // Symbols
#define LORA_FIX_LENGTH_PAYLOAD_ON                  false
#define LORA_IQ_INVERSION_ON                        false


#define RX_TIMEOUT_VALUE                            1000

#define DDATA_SIZE                                  1000
#define BUFFER_SIZE                                 254 // Define the payload size here which maximum is 255
static RadioEvents_t RadioEvents;
void OnTxDone( void );
void OnTxTimeout( void );
bool lora_idle = true;

////////////////////////////////////////////|  Core's Parameter's  |/////////////////////////////
TaskHandle_t Task1;
TaskHandle_t Task2;
#define I2S_WS 34
#define I2S_SD 33
#define I2S_SCK -1  //#define I2S_SCK 32     for i2s interface
// Use I2S Processor 0
#define I2S_PORT I2S_NUM_0

uint32_t j = 0;
uint32_t i = 0;


uint8_t full_data_2d_array[DDATA_SIZE][BUFFER_SIZE];
size_t global_var;
bool is_done ;
bool is_send ;
void i2s_install() {
  // Set up I2S Processor configuration
  const i2s_config_t i2s_config = {
    .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_PDM),
    .sample_rate = 10000,             // .sample_rate = 44100,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,//0,
    .dma_buf_count = 8,
    .dma_buf_len = BUFFER_SIZE,
    .use_apll = false
  };
 
  i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
}
 
void i2s_setpin() {
  // Set I2S pin configuration
  const i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_SCK, // 26
    .ws_io_num = I2S_WS, //25
    .data_out_num = -1, // -1
    .data_in_num = I2S_SD // 34
  };
 
  i2s_set_pin(I2S_PORT, &pin_config);
}

void setup() {
  is_done = false;
  is_send = true;
  Serial.begin(115200); 
  Mcu.begin();
  i2s_install();
  i2s_setpin();
  i2s_start(I2S_PORT);
  vTaskDelay(1000/portTICK_PERIOD_MS);

  RadioEvents.TxDone = OnTxDone;
  RadioEvents.TxTimeout = OnTxTimeout;
  Radio.Init( &RadioEvents );
  Radio.SetChannel( RF_FREQUENCY );
  Radio.SetTxConfig( MODEM_LORA, TX_OUTPUT_POWER, 0, LORA_BANDWIDTH,
                                   LORA_SPREADING_FACTOR, LORA_CODINGRATE,
                                   LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
                                   true, 0, 0, LORA_IQ_INVERSION_ON, 3000 );

  vTaskDelay(1000/portTICK_PERIOD_MS);

  xTaskCreatePinnedToCore(
                    Task1code,   /* Task function. */
                    "Task1code",     /* name of task. */
                    10000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    0,           /* priority of the task */
                    &Task1,      /* Task handle to keep track of created task */
                    0);          /* pin task to core 0 */                  
  vTaskDelay(500/portTICK_PERIOD_MS);

  //create a task that will be executed in the Task2code() function, with priority 1 and executed on core 1
  xTaskCreatePinnedToCore(
                    Task2code,   /* Task function. */
                    "Task2code",     /* name of task. */
                    10000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    1,           /* priority of the task */
                    &Task2,      /* Task handle to keep track of created task */
                    1);          /* pin task to core 1 */
  vTaskDelay(500/portTICK_PERIOD_MS);
}

void Task1code( void * pvParameters ){
  while(1){
    if(is_send){
    if (Serial.available() > 0) {
    
    char incomingChar = Serial.read();
    if (incomingChar == 'R')
    {
      Serial.print("You Entered : ");Serial.println(incomingChar);
      while(i < DDATA_SIZE && is_done == false){
      
        //Serial.print("Data Receiving from I2S : ");
        size_t bytes_read;
        uint8_t data[BUFFER_SIZE];
        i2s_read(I2S_NUM_0, data, sizeof(data), &bytes_read, portMAX_DELAY);
        memcpy(full_data_2d_array[i], data, bytes_read);
        vTaskDelay(25/portTICK_PERIOD_MS);
        //Serial.write(full_data_2d_array[i], bytes_read); //TESTED: tested Over all works perfectly with some minor noise
        //printByteBuffer(full_data_2d_array[i], bytes_read, 1, i, bytes_read);
        global_var = bytes_read;
        i++;
        //Serial.println(i);
       
      }
        is_done = true;
        if (is_send){is_send = false;j = 0;}
        vTaskDelay(500/portTICK_PERIOD_MS);
    } //
   }
  }
 }
}
void resetArray(uint8_t array[][BUFFER_SIZE], int numRows, int numCols) {
  for (int i = 0; i < numRows; i++) {
    for (int j = 0; j < numCols; j++) {
      array[i][j] = 0; // Set each element to 0
    }
  }
}
void Task2code( void * pvParameters ){
  char txpacket[3];
  while(1){
    if(is_done){
      if(j<DDATA_SIZE){
      if(lora_idle){
        //if (j == 0){for(uint8_t mis = 0; mis <= 10 ; mis++){sprintf(txpacket,"#S"); Radio.Send( (uint8_t *)txpacket, strlen(txpacket));}}
        //else if ( j == DDATA_SIZE-1){sprintf(txpacket,"#EE"); Radio.Send( (uint8_t *)txpacket, strlen(txpacket) );}
         Radio.Send(full_data_2d_array[j], global_var ); 
         //Serial.write(full_data_2d_array[j], global_var);
        
       lora_idle = false;
       //Serial.println(j);
       j++;
      }
       
      }
      else{

        is_send = true;
        
        if (is_done){is_done = false; i=0;}
        //sprintf(txpacket,"#EE"); Radio.Send( (uint8_t *)txpacket, strlen(txpacket) );
        resetArray(full_data_2d_array, DDATA_SIZE, BUFFER_SIZE);
        vTaskDelay(500/portTICK_PERIOD_MS);
      }
      
    }
    Radio.IrqProcess( );
  }
}
void loop (){
}

void OnTxDone( void )
{
	//Serial.println("TX done......");
	lora_idle = true;
}

void OnTxTimeout( void )
{
    Radio.Sleep( );
    //Serial.println("TX Timeout......");
    lora_idle = true;
}