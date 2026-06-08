#include <stdio.h>
#include "driver/i2c.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_timer.h"
#include "sdkconfig.h"

#include "dht11.h"
#include "max30100.h"

static SemaphoreHandle_t xSemaphore = NULL;

#define I2C_SDA 8
#define I2C_SCL 9
#define I2C_FRQ 100000
#define I2C_PORT 0

max30100_config_t max30100 = {};

int64_t get_bpm_time = 0;
int64_t dht11_time = 0;

/* ---------------- I2C INIT ---------------- */
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
    return i2c_driver_install(i2c_port, I2C_MODE_MASTER, 0, 0, 0);
}

/* ---------------- MAX30100 TASK ---------------- */
void get_bpm(void *param)
{
    max30100_data_t result = {};

    while (1)
    {
        int64_t start = esp_timer_get_time();

        printf("MAX30100 Task | Core: %d\n", xPortGetCoreID());

        if (xSemaphoreTake(xSemaphore, portMAX_DELAY) == pdTRUE)
        {
            esp_err_t ret = max30100_update(&max30100, &result);

            if (ret == ESP_OK && result.pulse_detected)
            {
                printf("BPM: %.2f | SpO2: %.2f\n",
                       result.heart_bpm,
                       result.spO2);
            }
        }

        int64_t end = esp_timer_get_time();
        get_bpm_time = (end - start) / 1000;

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

/* ---------------- DHT11 TASK ---------------- */
void dht11(void *data)
{
    while (1)
    {
        int64_t start = esp_timer_get_time();

        printf("DHT11 Task | Core: %d\n", xPortGetCoreID());

        printf("Temp: %d\n", DHT11_read().temperature);
        printf("Humidity: %d\n", DHT11_read().humidity);

        xSemaphoreGive(xSemaphore);

        int64_t end = esp_timer_get_time();
        dht11_time = (end - start) / 1000;

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

/* ---------------- APP MAIN ---------------- */
void app_main()
{
    DHT11_init(GPIO_NUM_4);

    ESP_ERROR_CHECK(i2c_master_init(I2C_PORT));

    ESP_ERROR_CHECK(max30100_init(&max30100,
                                  I2C_PORT,
                                  MAX30100_DEFAULT_OPERATING_MODE,
                                  MAX30100_DEFAULT_SAMPLING_RATE,
                                  MAX30100_DEFAULT_LED_PULSE_WIDTH,
                                  MAX30100_DEFAULT_IR_LED_CURRENT,
                                  MAX30100_DEFAULT_START_RED_LED_CURRENT,
                                  MAX30100_DEFAULT_MEAN_FILTER_SIZE,
                                  MAX30100_DEFAULT_PULSE_BPM_SAMPLE_SIZE,
                                  true,
                                  false));

    /* Create semaphore */
    xSemaphore = xSemaphoreCreateBinary();
    xSemaphoreGive(xSemaphore);   

    /* Tasks */
    xTaskCreate(get_bpm, "Get BPM", 4096, NULL, 1, NULL);
    xTaskCreate(dht11, "DHT11", 4096, NULL, 2, NULL);

    while (1)
    {
        printf("\n--- MAIN ---\n");
        printf("MAX30100 Time: %lld ms\n", get_bpm_time);
        printf("DHT11 Time: %lld ms\n", dht11_time);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
