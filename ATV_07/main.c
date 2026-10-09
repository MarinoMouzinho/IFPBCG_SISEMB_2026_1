#include <stdio.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "esp_log.h"

#define LED_PIN GPIO_NUM_2
#define BUTTON_PIN GPIO_NUM_4

#define DEBOUNCE_TIME_US (50000LL) //500ms
#define TIMER_LED_US (10 * 1000000LL) //10s
#define LONG_PRESS_TIME_US (2 * 1000000LL) //2s

static const char *TAG = "ATV_07_ISR_TIMER";

typedef struct {
    int level;
    int64_t timestamp;
} button_event_t;

static QueueHandle_t gpio_evt_queue = NULL;
static esp_timer_handle_t led_timer_handle = NULL;
static esp_timer_handle_t long_press_timer_handle = NULL;

static bool led_state = false;
static bool is_pressed = false;
static int64_t press_start_time = 0;
static int64_t last_valid_evt_time = 0;

static void led_timer_callback(void *arg)
{
    led_state = false;
    gpio_set_level(LED_PIN, 0);
    ESP_LOGI(TAG, "[TIMER] LED desligado automaticamente.");
}

static void long_press_timer_callback(void *arg)
{
    if (is_pressed) {
        led_state = false;
        gpio_set_level(LED_PIN, 0);
        esp_timer_stop(led_timer_handle);
        ESP_LOGW(TAG, "[PRESSIONAMENTO LONGO] LED desligado e timer cancelado.");
    }
}

static void IRAM_ATTR gpio_isr_handler(void *arg)
{
    button_event_t evt;
    evt.level = gpio_get_level(BUTTON_PIN);
    evt.timestamp = esp_timer_get_time();
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xQueueSendFromISR(
        gpio_evt_queue,
        &evt,
        &xHigherPriorityTaskWoken
    );

    if (xHigherPriorityTaskWoken) {
        portYIELD_FROM_ISR();
    }
}

static void button_task(void *arg)
{
    button_event_t evt;

    while (1) {
        if (xQueueReceive(gpio_evt_queue, &evt, portMAX_DELAY)) {

            // Debounce
            if ((evt.timestamp - last_valid_evt_time) < DEBOUNCE_TIME_US) {
                continue;
            }

            last_valid_evt_time = evt.timestamp;

            // Botão pressionado
            if (evt.level == 1 && !is_pressed) {
                is_pressed = true;
                press_start_time = evt.timestamp;

                // Inicia a contagem tempo de botao
                ESP_ERROR_CHECK(
                    esp_timer_start_once(
                        long_press_timer_handle,
                        LONG_PRESS_TIME_US
                    )
                );

                if (!led_state) {
                    led_state = true;
                    gpio_set_level(LED_PIN, 1);

                    ESP_ERROR_CHECK(
                        esp_timer_start_once(
                            led_timer_handle,
                            TIMER_LED_US
                        )
                    );

                    ESP_LOGI(TAG, "[ACIONAMENTO] LED ligado. Timer de 10s iniciado.");
                } else {
                    ESP_ERROR_CHECK(
                        esp_timer_restart(
                            led_timer_handle,
                            TIMER_LED_US
                        )
                    );

                    ESP_LOGI(TAG, "[RENOVAÇÃO] Timer de 10s reiniciado.");
                }
            }

            // Botão solto
            else if (evt.level == 0 && is_pressed) {
                is_pressed = false;
                int64_t press_duration = evt.timestamp - press_start_time;

                // Cancela a detecção
                ESP_LOGI(TAG, "[SOLTOU] Tempo: %lld ms", press_duration / 1000);
				esp_timer_stop(long_press_timer_handle);
            }
        }
    }
}

void app_main(void)
{
    gpio_reset_pin(LED_PIN);
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(LED_PIN, 0);

    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << BUTTON_PIN),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_ANYEDGE
    };

    ESP_ERROR_CHECK(gpio_config(&io_conf));

    gpio_evt_queue = xQueueCreate(10, sizeof(button_event_t));
    configASSERT(gpio_evt_queue != NULL);

    const esp_timer_create_args_t led_timer_args = {
        .callback = &led_timer_callback,
        .name = "led_off_timer"
    };

    ESP_ERROR_CHECK(
        esp_timer_create(&led_timer_args, &led_timer_handle)
    );

    const esp_timer_create_args_t long_timer_args = {
        .callback = &long_press_timer_callback,
        .name = "long_press_timer"
    };

    ESP_ERROR_CHECK(
        esp_timer_create(&long_timer_args, &long_press_timer_handle)
    );

    ESP_ERROR_CHECK(gpio_install_isr_service(0));

    ESP_ERROR_CHECK(
        gpio_isr_handler_add(
            BUTTON_PIN,
            gpio_isr_handler,
            (void *)BUTTON_PIN
        )
    );

    xTaskCreate(button_task, "button_task", 3072, NULL, 10, NULL);
    ESP_LOGI(TAG, "Sistema inicializado.");

    while (1) {
        vTaskDelay(portMAX_DELAY);
    }
}