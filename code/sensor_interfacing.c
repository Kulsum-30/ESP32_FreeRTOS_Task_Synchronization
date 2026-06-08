#include <stdio.h>

#include "driver/i2c.h"
#include "driver/gpio.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "sdkconfig.h"

#include "dht11.h"
#include "max30100.h"

#define I2C_SDA   8
#define I2C_SCL   9
#define I2C_FRQ   100000
#define I2C_PORT  0

max30100_config_t max30100 = {0};

/* Initialize I2C */
esp_err_t i2c_master_init(i2c_port_t i2c_port)
{
    i2c_config_t conf = {0};

    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = I2C_SDA;
    conf.scl_io_num = I2C_SCL;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = I2C_FRQ;

    i2c_param_config(i2c_port, &conf);

    return i2c_driver_install(
        i2c_port,
        I2C_MODE_MASTER,
        0,
        0,
        0
    );
}

/* MAX30100 Task */
void get_bpm(void *param)
{
    printf("MAX30100 Test\n");

    max30100_data_t result = {0};

    while (1)
    {
        printf(
            "MAX30100 Task Priority : %d\n",
            uxTaskPriorityGet(NULL)
        );

        esp_err_t ret =
            max30100_update(&max30100, &result);

        if (ret != ESP_OK)
        {
            printf(
                "Error updating MAX30100: %s\n",
                esp_err_to_name(ret)
            );
        }
        else
        {
            printf(
                "Sensor data updated successfully\n"
            );

            printf(
                "Pulse Detected: %s\n",
                result.pulse_detected ?
                "Yes" : "No"
            );

            if (result.pulse_detected)
            {
                printf(
                    "BPM: %f | SpO2: %f%%\n",
                    result.heart_bpm,
                    result.spO2
                );
            }
        }

        vTaskDelay(
            1000 / portTICK_PERIOD_MS
        );
    }
}

/* DHT11 Task */
void dht11_task(void *data)
{
    while (1)
    {
        printf(
            "DHT11 Task Priority : %d\n",
            uxTaskPriorityGet(NULL)
        );

        printf(
            "Temperature : %d\n",
            DHT11_read().temperature
        );

        printf(
            "Humidity : %d\n",
            DHT11_read().humidity
        );

        printf(
            "Status Code : %d\n",
            DHT11_read().status
        );

        vTaskDelay(
            1000 / portTICK_PERIOD_MS
        );
    }
}

/* Main Function */
void app_main(void)
{
    DHT11_init(GPIO_NUM_4);

    ESP_ERROR_CHECK(
        i2c_master_init(I2C_PORT)
    );

    ESP_ERROR_CHECK(
        max30100_init(
            &max30100,
            I2C_PORT,
            MAX30100_DEFAULT_OPERATING_MODE,
            MAX30100_DEFAULT_SAMPLING_RATE,
            MAX30100_DEFAULT_LED_PULSE_WIDTH,
            MAX30100_DEFAULT_IR_LED_CURRENT,
            MAX30100_DEFAULT_START_RED_LED_CURRENT,
            MAX30100_DEFAULT_MEAN_FILTER_SIZE,
            MAX30100_DEFAULT_PULSE_BPM_SAMPLE_SIZE,
            true,
            false
        )
    );

    xTaskCreate(
        get_bpm,
        "Get BPM",
        2048,
        NULL,
        1,
        NULL
    );

    xTaskCreate(
        dht11_task,
        "DHT11",
        2048,
        NULL,
        2,
        NULL
    );

    while (1)
    {
        printf(
            "Main Task Priority : %d\n",
            uxTaskPriorityGet(NULL)
        );

        vTaskDelay(
            1000 / portTICK_PERIOD_MS
        );
    }
}
