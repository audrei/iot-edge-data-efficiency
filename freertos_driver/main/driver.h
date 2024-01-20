/*
 * @brief Driver to control the Transmission Data
 *
 * Creator: Audrei Silva
 * Date: 2022
 */


#ifndef _TRANSMISSION_DRIVER_H_
#define _TRANSMISSION_DRIVER_H_


/*-----------------------------------------------------------
 * MACROS AND DEFINITIONS
 *----------------------------------------------------------*/
// Use this macro to enable or disable debug mode
// Set to 1 to enable, 0 to disable
// When enabled, debug messages will be printed to the console
#define DEBUG_MODE 1
/*
*
* This macro enables the timestamp feature to be added to log messages
*/
//#define ENABLE_TIMESTAMP 

/* Queue definitions */  
#define MAX_LENGHT 5 //queue max leght

/* Timmer definitions */
#define TIMER_TICK 1    //tick of the timer
#define MAX_TIME  30000 // timer timeout in miliseconds


/*
* Colocar comprimento da fila de acordo com payload size e packet size.
*/
#define TRANSMISSION_BUFFER_SIZE 1500  // Define transmission buffer size here


/*
* Just add a element in a qeue if it is out of a measure threshould
*/
#define CRITICAL_MEASURE_THRESHOLD 
#define MEASURE_THRESHOLD 
#define MEASURE_TOLERANCE_PERCENTAGE (5) //percent of variation to acept a data like a new measurement
#define MEASURE_TOLERANCE_PERCENTAGE_CRITICAL (15) //percent of variation to acept a data like a new measurement
#define CRITICAL_THRESHOLD_RESULT 2
/*
 * Macro description.
 *
 *  This macro compares a value with a reference and determines the tolerance level based on defined percentage thresholds.
 * @param measurement The actual value measured.
 * @param reference  The previously measured value.
 * 
 * @return:
 *  
 *   If the value is within the reference +/- MEASURE_TOLERANCE_PERCENTAGE, it returns 0.
 *   If the value is within the reference +/- MEASURE_TOLERANCE_PERCENTAGE_CRITICAL, it returns 1.
 *   If the value is outside the reference +/- MEASURE_TOLERANCE_PERCENTAGE_CRITICAL, it returns 2.
*/

#define OUTSIDE_TOLERANCE(val, ref) \
    (((val) >= (ref - (MEASURE_TOLERANCE_PERCENTAGE  * ref / 100)) && (val) <= (ref + (MEASURE_TOLERANCE_PERCENTAGE  * ref / 100))) ? 0 : \
     ((val) >= (ref - (MEASURE_TOLERANCE_PERCENTAGE_CRITICAL * ref / 100)) && (val) <= (ref + (MEASURE_TOLERANCE_PERCENTAGE_CRITICAL * ref / 100))) ? 1 : 2)


//#define desligaradio()  {}//função para desligar o rádio
//#define Sendmessage(msg)  {bluettothsendmsg(msg);}

/**
 * Check all the required application for task transmission_handler.
 * These macros are application specific and can be changed and customized.
*/

#define transmission_Process_stack_size (4096u)             //bytes in esp 32 and words in freeRTOS
#define transmission_process_priority   PROCESS_PRIORITY   //(0 to configMAX_PRIORITIES - 1)
#define transmission_process_CPU        (1)               //In the case of more than one cpu
//process driver definitions
/**Ideal para maior economia de energia é usar a prioridade do processo como máxima */
#define PROCESS_PRIORITY (5)

/**
 * @brief Represents a sensor measurement.
 */

//struct sensor {
//  int deviceId;         /**< The ID of the device that generated the measurement. */ 
//  int measurementType;  /**< The type of measurement that was performed. */
//  float value;          /**< The value of the measurement. */
//  time_t timestamp;     /**< The timestamp when the measurement was taken. */
//  int errorCode;        /**< The sinalization of error in the system. */
//}Sensor_t;

struct sensor {
  int deviceId;         /**< The ID of the device that generated the measurement. */ 
  int measurementType;  /**< The type of measurement that was performed. */
  float value;          /**< The value of the measurement. */
}Sensor_t;

/*
 * Initializes the device driver and sets up any required resources.
 *
 * This function should be called once before using the driver. It initializes
 * the device driver and performs any necessary setup, such as configuring
 * hardware or allocating memory. If the initialization fails, an error code
 * is returned.
 */
void driver_init(void);

/*
 * Processes the sensor data and adds it to the queue for transmission.
 *
 * This function takes a sensor reading and processes the data as required for
 * the specific sensor type. The processed data is then added to a queue for
 * transmission. This function decides when is the best time, and then enables transmission 
 * over a network connection using a specific protocol.
 
 * @param my_sensor The sensor data to be processed.
 */
void process_sensor_data(struct sensor my_sensor);


#endif