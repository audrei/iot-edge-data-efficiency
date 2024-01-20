/**
 * @file main.c
 * @brief Main application entry point.
 */


#include <stdio.h>
#include <stdint.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "wifi.h"



/* Defines */
#define LED_PIN 2


/**  Core Definition */
#if CONFIG_FREERTOS_UNICORE
    static const BaseType_t app_cpu = 0;
#else
    static const BaseType_t app_cpu = 1;
#endif

// Define the ADC configuration
#define DEFAULT_VREF    1100
static esp_adc_cal_characteristics_t *adc_chars;
static const adc_channel_t channel = ADC_CHANNEL_6;
static const adc_atten_t atten = ADC_ATTEN_DB_11;
static const adc_unit_t unit = ADC_UNIT_1;

// Define the GPIO pin for LM35
#define LM35_GPIO_PIN   34

/*
 * Read temperature from LM35.
 *
 * @return Temperature in celsius.
 */

float read_temperature(){

    uint32_t adc_reading = adc1_get_raw((adc_channel_t)channel);
    float voltage = esp_adc_cal_raw_to_voltage(adc_reading, adc_chars);
    float temperature = (voltage * 100.0) / 1024.0;


    return temperature;
}



/**
 * @brief Initialize board peripherals.
 */

void board_init(void){

    // Led pin config
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);
    // Configure the ADC
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(channel, atten);

    // Characterize ADC
    adc_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t));
    esp_adc_cal_characterize(unit, atten, ADC_WIDTH_BIT_12, DEFAULT_VREF, adc_chars);

}


/**
 * @brief Blink LED task.
 * @param arg Task argument (not used).
 */
void Blink_led(void *arg)
{
    while(1){
        gpio_set_level(LED_PIN, false);
        vTaskDelay(1000/portTICK_PERIOD_MS);
        gpio_set_level(LED_PIN, true);
        vTaskDelay(1000/portTICK_PERIOD_MS);       
    }
}

/**
 * @brief Print task.
 * @param arg Task argument (not used).
 */
void data_read(void *arg){   


#if 1  
    static int i = 0;

       
    struct sensor mySensors[10]; // Declarando um vetor de 10 elementos do tipo sensor
    mySensors[0].deviceId = 1;
    mySensors[0].measurementType = 1;
    mySensors[0].value = 27.0;
    

    mySensors[1].deviceId = 1;
    mySensors[1].measurementType = 1;
    mySensors[1].value = 27.0;
    

    mySensors[2].deviceId = 1;
    mySensors[2].measurementType = 1;
    mySensors[2].value = 27.0;
    

    mySensors[3].deviceId = 1;
    mySensors[3].measurementType = 1;
    mySensors[3].value = 35.0;
    
    
    mySensors[4].deviceId = 1;
    mySensors[4].measurementType = 1;
    mySensors[4].value = 27.0;
    


    mySensors[5].deviceId = 1;
    mySensors[5].measurementType = 1;
    mySensors[5].value = 27.0;
    

    mySensors[6].deviceId = 1;
    mySensors[6].measurementType = 1;
    mySensors[6].value = 28.5;
    

while(1){



        vTaskDelay(1000/portTICK_PERIOD_MS);
        mySensors[i].deviceId = i;
        //printf("Task temperature reading..\n");
        //printf("The value of i is %d\n", i);
        process_sensor_data(mySensors[i]);
        i++;
        if(i > 6) i = 0;    
    } 
#else
    struct sensor mySensors;
    mySensors.deviceId = 0;
    mySensors.measurementType = 1;

    while(1){
        mySensors.value = read_temperature();
        printf("O valor da variável float é: %f\n", mySensors.value);
        vTaskDelay(5000/portTICK_PERIOD_MS);
        process_sensor_data(mySensors);
    }

#endif
    
}



/**
 * @brief Main function for the application.
 *
 * This is the entry point of the application. It initializes hardware,
 * initializes Wi-Fi, and creates two tasks, Blink_led and Print_task.
 */
void app_main(void)
{
    // Hardware initialization
    board_init();

    // Print initialization message
    printf("App Init..\n");

    // Initialize Wi-Fi
    initialise_wifi();

    // Initialize driver
    driver_init();

    // Create a task named Blink_led (You can uncomment this if needed)
    /*
    xTaskCreatePinnedToCore(
              Blink_led,      // Function pointer to be called
              "Blink_led",    // Name of task
              1024,           // Stack size (bytes in ESP32, words in FreeRTOS)
              NULL,           // Parameter to pass to function
              1,              // Task priority (0 to configMAX_PRIORITIES - 1)
              NULL,           // Task handle
              app_cpu);
    */

    // Create a task 
    xTaskCreatePinnedToCore(
            data_read,       // Function pointer to be called
            "data_read",     // Name of task
            2048,             // Stack size (bytes in ESP32, words in FreeRTOS)
            NULL,             // Parameter to pass to function
            2,                // Task priority (0 to configMAX_PRIORITIES - 1)
            NULL,             // Task handle
            app_cpu);

    // It is not necessary to start FreeRTOS scheduler manually in ESP-IDF.
    // vTaskStartScheduler() is automatically started.
}

