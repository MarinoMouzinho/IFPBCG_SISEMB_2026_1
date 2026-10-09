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

static const char *TAG = "ATV06_GPIO";

void init_led(void){
  gpio_reset_pin(LED_PIN);
  gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);
  gpio_set_level(LED_PIN, 0);
}

void init_button(void){
  gpio_reset_pin(BUTTON_PIN);
  gpio_set_direction(BUTTON_PIN, GPIO_MODE_INPUT);
  gpio_set_pull_mode(BUTTON_PIN, GPIO_FLOATING);
}

int app_main(void){
  init_led();
  init_button();

  bool led_state = false;
  bool last_button_raw = false;
  bool stable_button_state = false;
  
  int64_t last_debounce_time = 0;
  int64_t press_start_time = 0;
  int64_t led_start_time = 0;
  bool long_press_action_executed = false;

  ESP_LOGI(TAG, "Sistema inicializado por Polling não bloqueante.");
  
  while(1){
    int64_t now = esp_timer_get_time();
    bool current_button_raw = gpio_get_level(BUTTON_PIN);

    if (current_button_raw != last_button_raw) {
        last_debounce_time = now; // Reinicia o timer
    }

    if ((now - last_debounce_time) > DEBOUNCE_TIME_US) {
        if (current_button_raw != stable_button_state) {
            stable_button_state = current_button_raw;

            if (stable_button_state == true) {
                press_start_time = now;
                long_press_action_executed = false;

                if (!led_state) {
                    led_state = true;
                    gpio_set_level(LED_PIN, 1);
                    led_start_time = now;
                    ESP_LOGI(TAG, "Botão Pressionado: LED LIGADO por 30s");
                } else {
                    led_start_time = now;
                    ESP_LOGI(TAG, "Botão Pressionado novamente: Temporizador de 30s REINICIADO");
                }
            }
        }
    }

    if (stable_button_state == true && !long_press_action_executed) {
        if ((now - press_start_time) >= LONG_PRESS_TIME_US) {
            led_state = false;
            gpio_set_level(LED_PIN, 0);
            long_press_action_executed = true;
            ESP_LOGW(TAG, "Pressionamento Longo (>2s): Desligamento Manual Imediato!");
        }
    }

    if (led_state) {
        if ((now - led_start_time) >= TIMER_LED_US) {
            led_state = false;
            gpio_set_level(LED_PIN, 0);
            ESP_LOGI(TAG, "Temporizador de 30s esgotado: LED DESLIGADO automaticamente");
        }
    }

    last_button_raw = current_button_raw;

  }
}