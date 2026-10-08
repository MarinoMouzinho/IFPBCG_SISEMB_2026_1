#include <stdio.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "esp_log.h"

#define LED_PIN GPIO_NUM_2
#define BUTTON_PIN GPIO_NUM_4

#define DEBOUNCE_TIME_US 50000 // 50 ms
#define TIMER_LED_US (30 * 1000000LL) // 30 segundos
#define LONG_PRESS_TIME_US (2 * 1000000LL) // 2 segundos

static const char *TAG = "ATV_07_ISR_TIMER";

// Estrutura para os eventos de interrupção
typedef struct {
    int level;
    int64_t timestamp;
} button_event_t;

static QueueHandle_t gpio_evt_queue = NULL;
static esp_timer_handle_t led_timer_handle = NULL;
static bool led_state = false;

static void led_timer_callback(void *arg)
{
    led_state = false;
    gpio_set_level(LED_PIN, 0);
    ESP_LOGI(TAG, "[TIMER EXPIROU] LED DESLIGADO automaticamente");
}

static void IRAM_ATTR gpio_isr_handler(void *arg)
{
    button_event_t evt;
    evt.level = gpio_get_level(BUTTON_PIN);
    evt.timestamp = esp_timer_get_time();

    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xQueueSendFromISR(gpio_evt_queue, &evt, &xHigherPriorityTaskWoken);
    
    if (xHigherPriorityTaskWoken) {
        portYIELD_FROM_ISR();
    }
}

static void button_task(void *arg)
{
    button_event_t evt;
    int64_t last_valid_evt_time = 0;
    int64_t press_start_time = 0;
    bool is_pressed = false;

    while (1) {
        if (xQueueReceive(gpio_evt_queue, &evt, portMAX_DELAY)) {
            if ((evt.timestamp - last_valid_evt_time) < DEBOUNCE_TIME_US) continue;

            last_valid_evt_time = evt.timestamp;

            if (evt.level == 1 && !is_pressed) {
                is_pressed = true;
                press_start_time = evt.timestamp;

                if (!led_state) {
                    led_state = true;
                    gpio_set_level(LED_PIN, 1);
                    esp_timer_start_once(led_timer_handle, TIMER_LED_US);
                    ESP_LOGI(TAG, "[BOTÃO PRESSIONADO] LED LIGADO. Timer de 10s iniciado.");
                } else {
                    esp_timer_restart(led_timer_handle, TIMER_LED_US);
                    ESP_LOGI(TAG, "[RENOVACÃO] LED já estava aceso. Timer de 10s REINICIADO.");
                }
            } 
            else if (evt.level == 0 && is_pressed) {
                is_pressed = false;
                int64_t press_duration = evt.timestamp - press_start_time;

                if (press_duration >= LONG_PRESS_TIME_US) {
                    esp_timer_stop(led_timer_handle);
                    led_state = false;
                    gpio_set_level(LED_PIN, 0);
                    ESP_LOGW(TAG, "[PRESSIONAMENTO LONGO] Mantido por >=2s! LED desligado e timer cancelado.");
                }
            }
        }
    }
}

int app_main(void){
  // Configuração do GPIO do LED
    gpio_reset_pin(LED_PIN);
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(LED_PIN, 0);

    // Configuração do GPIO do Botão com Interrupção em ambas as bordas (SUBIDA e DESCIDA)
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << BUTTON_PIN),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE, // Usando resistor de 10k externo
        .intr_type = GPIO_INTR_ANYEDGE
    };
    gpio_config(&io_conf);
    gpio_evt_queue = xQueueCreate(10, sizeof(button_event_t));
    gpio_install_isr_service(0);
    gpio_isr_handler_add(BUTTON_PIN, gpio_isr_handler, (void *)BUTTON_PIN);

    const esp_timer_create_args_t timer_args = {
        .callback = &led_timer_callback,
        .name = "led_off_timer"
    };

    ESP_ERROR_CHECK(esp_timer_create(&timer_args, &led_timer_handle));
    xTaskCreate(button_task, "button_task", 3072, NULL, 10, NULL);
    ESP_LOGI(TAG, "Sistema inicializado via ISR + esp_timer.");

    while (1) {
        vTaskDelay(portMAX_DELAY);
    }
}