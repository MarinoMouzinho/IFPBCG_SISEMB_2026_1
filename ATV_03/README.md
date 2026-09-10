# ATV 03 (Saídas Digitais)

Atividade prática desenvolvida para controlar 4 LEDs utilizando as GPIOs do ESP32 configuradas como saídas digitais.
Esta atividade foi realizada no simulador Wokwi e utilizou a ESP-IDF como framework de desenvolvimento.

Como pré-requisito da simulação foram elaborados:

* Diagrama em Blocos, representando:
  * ESP32-S3: microcontrolador responsável pelo acionamento das saídas digitais.
  * LED1, LED2, LED3 e LED4: atuadores controlados pelo ESP32.

  ![Diagrama em Blocos](./Diagrama%20de%20Blocos.png)
  
* Esquemático, incluindo:
  * 4 LEDs conectados a GPIOs configuradas como saída digital, cada um em série com seu resistor.
  * Alimentação de 3,3 V fornecida pelo ESP32.

 ![Diagrama Esquemático](./Diagrama%20Esquematico.png)
 
_Os diagramas foram elaborados no __Draw.io__ e se encontram disponibilizados na raíz do projeto_

---

## Projeto Wokwi

```
https://wokwi.com/projects/473238350316473345
```

---

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "esp_log.h"

// Mapeamento dos LEDs conforme o diagram.json
#define LED_BIT0_GPIO GPIO_NUM_4  // LSB (Verde)
#define LED_BIT1_GPIO GPIO_NUM_5  // Amarelo
#define LED_BIT2_GPIO GPIO_NUM_6  // Vermelho
#define LED_BIT3_GPIO GPIO_NUM_7  // MSB (Azul)

// Mapeamento dos Botões conforme o diagram.json
#define BUTTON_A_GPIO GPIO_NUM_1  // Botao verde - Incrementar
#define BUTTON_B_GPIO GPIO_NUM_2  // Botao azul - Muda passo (+1 / +2)

#define DEBOUNCE_TIME_US 50000     // 50ms (sem vTaskDelay bloqueante)

static const gpio_num_t leds[] = {
  LED_BIT0_GPIO,
  LED_BIT1_GPIO,
  LED_BIT2_GPIO,
  LED_BIT3_GPIO
};

typedef struct {
  gpio_num_t pin;
  int last_state;
  int stable_state;
  int64_t last_debounce_time;
} button_t;

static const char *TAG = "CONTADOR_4BITS";
static uint8_t contador = 0;
static uint8_t incremento = 1;

// Atualiza as saídas dos 4 LEDs conforme o valor do contador (0x0 a 0xF)
static void update_leds(uint8_t valor) {
  for (int i = 0; i < 4; i++) {
    int bit_val = (valor >> i) & 0x01;
    gpio_set_level(leds[i], bit_val);
  }
}

void incrementar(void) {
  contador = (contador + incremento) % 16;
}

void switch_incremento(void) {
  incremento = (incremento == 1) ? 2 : 1; 
}

// Tratamento de debounce por software baseado no esp_timer_get_time()
bool verificar_botao(button_t *btn) {
  int current_read = gpio_get_level(btn->pin);
  int64_t now = esp_timer_get_time();

  if (current_read != btn->last_state) {
      btn->last_debounce_time = now;
      btn->last_state = current_read;
  }

  if ((now - btn->last_debounce_time) > DEBOUNCE_TIME_US) {
      printf("tempo debounce passou...");
      if (current_read != btn->stable_state) {
          btn->stable_state = current_read;
          // Detecta a alteração/pressionamento do botão
          if (btn->stable_state == 0) { 
            printf("estado mudou...");
            return true;
          }
      }
  }
  return false;
}

void configure_leds(void) {
  for (int i = 0; i < 4; i++) {
      gpio_reset_pin(leds[i]);
      gpio_set_direction(leds[i], GPIO_MODE_OUTPUT);
  }
}

void configure_buttons(void) {
  gpio_config_t btn_config = {
        .pin_bit_mask = (1ULL << BUTTON_A_GPIO) | (1ULL << BUTTON_B_GPIO),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE, 
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&btn_config);
}

void app_main(void) {
  configure_leds();
  configure_buttons();

  // Leitura inicial do estado dos botões
  button_t btnA = { .pin = BUTTON_A_GPIO, .last_state = 1, .stable_state = 1, .last_debounce_time = 0 };
  button_t btnB = { .pin = BUTTON_B_GPIO, .last_state = 1, .stable_state = 1, .last_debounce_time = 0 };

  // Inicializa com o valor zero
  update_leds(contador);

  while (1) {
      // Botão A: Incrementar
      if (verificar_botao(&btnA)) {
          incrementar();
          update_leds(contador);
          printf("Teste");
          ESP_LOGI(TAG, "Botao A Pressionado! Novo valor: 0x%X (Decimal: %d)", contador, contador);
      }

      // Botão B: Alternar Passo (+1 ou +2)
      if (verificar_botao(&btnB)) {
          switch_incremento();
          printf("Teste");
          ESP_LOGI(TAG, "Botao B Pressionado! Novo passo de incremento: +%d", incremento);
      }

      
      // Pequeno atraso para liberar processamento no FreeRTOS / Watchdog
      vTaskDelay(pdMS_TO_TICKS(10));
  }
}
