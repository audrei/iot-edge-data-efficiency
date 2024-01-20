/*
 * Driver for controlling the transmission of data over a network connection.
 *
 * This driver provides a set of functions for controlling the transmission of
 * data over a network connection using a specific protocol. The driver is
 * responsible for initializing the network connection, configuring the
 * transmission settings, and handling errors that may occur during the
 * transmission process. It can be used to transmit data from a variety of
 * sources, including sensors, user input, and file transfers.
 *
 * @author Audrei Silva
 * 
 * @date 2022
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver.h"
#include "freertos/timers.h"
#include "esp_log.h"
#include <stdio.h>
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "math.h"
#include "wifi.h"
#include "driver/gpio.h"

/*-----------------------------------------------------------
 *DECLARATIONS PRIVATE
 *----------------------------------------------------------*/
/**
 * @brief Handler for the timer used to control data transmission.
 *
 * This variable stores a timer handle used to control the timing of data
 * transmission. It is a private variable and should not be accessed or modified
 * outside of this file.
 */
TimerHandle_t xTimer; 

/**
 * @brief Queue for storing data to be transmitted.
 *
 * This variable stores a queue handle used to store data that needs to be
 * transmitted. It is a private variable and should not be accessed or modified
 * outside of this file.
 */
static QueueHandle_t xQueue;

/**
 * @brief Mutex for controlling access to shared resources.
 *
 * This variable stores a semaphore handle used to control access to shared
 * resources, such as the transmission queue. It is a private variable and
 * should not be accessed or modified outside of this file.
 */
static SemaphoreHandle_t xMutex;

/**
 * @brief Handle for the task responsible for data transmission.
 *
 * This variable stores a task handle used to identify the task responsible for
 * transmitting data. It is a private variable and should not be accessed or
 * modified outside of this file.
 */
static TaskHandle_t task_handle;

/*-----------------------------------------------------------
 * FUNCTION PROTOTYPE
 *----------------------------------------------------------*/
/**
 * @brief Callback function for timer events.
 *
 * This function is called when the timer specified by the TimerHandle_t 
 * parameter expires. The function performs the transmission with the data in qeue 
 * in response to the timer event.
 *
 * @param pxTimer The timer handle associated with the timer event.
 */
void callBackTimer(TimerHandle_t pxTimer);

/**
 * @brief Handler function to suspend data transmission.
 *
 * This function is called to suspend data transmission, typically in 
 * response to some event or condition. 
 */
static void suspend_transmission_handler(void);

/**
 * @brief Handler function to resume data transmission.
 *
 * This function is called to resume data transmission after it has been 
 * suspended, typically in response to some event or condition. The function 
 * performs any necessary actions to restart data transmission.
 */
static void resume_transmission_handler(void);


/*-----------------------------------------------------------
 * GLOBAL VARIABLES
 *----------------------------------------------------------*/
/**
 * Stores the previous measurement for comparison.
 *
 * This static variable is used to track the previous measurement and compare it
 * to the current measurement in order to control the process. It is initialized
 * to inf by default in drive init.
 */
static float reference;

/*
 * This function is called when the data is ready to be transmitted.
 * It retrieves the data from a buffer and sends it over a network
 * connection using a specific protocol.
 *
 * @param pvParameter A pointer to the parameter passed to the function.
 * This is not used in this function.
 */

void transmission_handler(void *pvParameter)
{
    static struct sensor sensor_data_transmission;
    static uint8_t transmission_buffer[TRANSMISSION_BUFFER_SIZE];
    int buffer_index = 0;
        
    while(true){
    
            printf("Processo em execução\n");
            //initiate the transmission Loop

            //check if the qeue is empty in the case of error
            if(uxQueueMessagesWaiting(xQueue) == 0){
                //transmite a error message 
                /*
                    O que fazer aqui?????
                */
                
#ifdef DEBUG_MODE
                ESP_LOGI("Tx","Error message");
#else
                while(true){};
#endif
            }
            //initialize tcp client
            tcp_client();
            // Loop to transmitting the data
            while(uxQueueMessagesWaiting(xQueue) != 0){
                //Take a semaphoro
                xSemaphoreTake(xMutex, portMAX_DELAY);
                if (xQueueReceive(xQueue, &sensor_data_transmission, portMAX_DELAY) == pdPASS) {
                    //After removing an element from the queue, you can now release it to add one more.
                    xSemaphoreGive(xMutex);
                    // Copy the element to the transmission buffer
                    // Fazer a copia do qeue receive direto para o bufer.
                    memcpy(&transmission_buffer[buffer_index], &sensor_data_transmission, sizeof(struct sensor));
                    buffer_index += sizeof(struct sensor);

#ifdef DEBUG_MODE
                    //ESP_LOGI("Tx", "The Temperature is: %0.2f", sensor_data_transmission.value);
#endif
                } 
            }
            //Transmission
            send_data_buffer(transmission_buffer,buffer_index);

            //reset buffer index
            buffer_index = 0;
            // Reset timmer"
            xTimerReset(xTimer, 0);

            // Reset the queue
            xQueueReset(xQueue);

            //suspend transmission handler until a new event are trigger
            suspend_transmission_handler();
            
                 
    }
}


static void suspend_transmission_handler(void){
    vTaskSuspend(task_handle);
}

static void resume_transmission_handler(void){
    vTaskResume(task_handle);
}


void driver_init(){

#ifdef DEBUG_MODE
    printf("Driver init..\n");
#endif
    
    //Creating a periodic timer and auto-reaload
    xTimer = xTimerCreate(  "Timer",                    //Name of the timer
                            pdMS_TO_TICKS(MAX_TIME),    //period of the time
                            false,                      //one - shoot
                            0,                          // Timer ID
                            callBackTimer);             // Callback function
    //check the sucessfull creation of the timer
    if(xTimer == NULL)
    {
#ifdef DEBUG_MODE
        ESP_LOGI("xTimer","error to create a timer");
#endif
        while(1);
    }else{
        xTimerStart(xTimer,0);
    }

    //creating a queue
    xQueue = xQueueCreate( MAX_LENGHT, sizeof( struct sensor ));
    //check the sucessfull creation of the queue
   if(xQueue == NULL){
#ifdef DEBUG_MODE
        ESP_LOGI("xTimer","erro to creat a queue");
#endif
        while(1);
    } 

    //Transmitting process initialization
    xTaskCreatePinnedToCore(                        // Use xTaskCreate() in vanilla FreeRTOS
              transmission_handler,                 // Function pointer to be called
              "Task transmission_handler",          // Name of task
              transmission_Process_stack_size,      // Stack size (bytes in ESP32, words in FreeRTOS)
              NULL,                                 // Parameter to pass to function
              transmission_process_priority,        // Task priority (0 to configMAX_PRIORITIES - 1)
              &task_handle,                         // Task handle
              transmission_process_CPU); 
    /*
        The transmission process are suspend until 
        until the data is ready to be transmitted
    */   
    vTaskSuspend(task_handle);

    //creating a mutex
    xMutex = xSemaphoreCreateMutex(); 

    reference = INFINITY;

}


void process_sensor_data(struct sensor  my_sensor){
  

#ifdef MEASURE_THRESHOLD
        static uint8_t threshold_result;

        threshold_result = OUTSIDE_TOLERANCE(my_sensor.value, reference);

       //check if the threshold tolerance was hit
       if(threshold_result){
           reference = my_sensor.value;
#endif
           
#ifdef ENABLE_TIMESTAMP
           my_sensor.timestamp = esp_timer_get_time();
#endif
#ifdef DEBUG_MODE
       ESP_LOGI("QEUE","Put in qeue");
#endif
           //Take mutex
           xSemaphoreTake(xMutex, portMAX_DELAY);
           //Add data in the QEUE
           xQueueSend(xQueue, &my_sensor, 0);
           //Give back Mutex
           xSemaphoreGive(xMutex);

#ifdef MEASURE_THRESHOLD
       }
#endif

   //check if the qeue is full
  if (uxQueueSpacesAvailable(xQueue) == 0 
#ifdef CRITICAL_MEASURE_THRESHOLD
    || threshold_result == CRITICAL_THRESHOLD_RESULT
#endif
   ){
        
#ifdef DEBUG_MODE
       if(threshold_result == CRITICAL_THRESHOLD_RESULT) 
            ESP_LOGI("QEUE","critical");
       else
            ESP_LOGI("QEUE","The QEUE is full");
#endif
       //transmite dados
       resume_transmission_handler();
   }
   
    
}



void callBackTimer(TimerHandle_t pxTimer){

#ifdef DEBUG_MODE
    ESP_LOGI("xTimer","timer timeout -->enble transmission is true");
#endif  

    resume_transmission_handler();
        
}


