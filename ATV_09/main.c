#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "hal/gpio_types.h"
#include "hal/uart_types.h"

#define TAM_BUF 1024
#define LED_GPIO GPIO_NUM_2
#define UART_NUM UART_NUM_2
#define TX_PIN GPIO_NUM_17
#define RX_PIN GPIO_NUM_16

static const char *TAG = "UART_LOOPBACK";

void init_uart(void) {
	const uart_config_t uart_config = {
	    .baud_rate = 115200,
	    .data_bits = UART_DATA_8_BITS,
	    .parity    = UART_PARITY_DISABLE,
	    .stop_bits = UART_STOP_BITS_1,
	    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
	    .source_clk = UART_SCLK_DEFAULT,
    };

    ESP_ERROR_CHECK(uart_param_config(UART_NUM, &uart_config));
	ESP_ERROR_CHECK(uart_set_pin(UART_NUM, TX_PIN, RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
	ESP_ERROR_CHECK(uart_driver_install(UART_NUM, TAM_BUF * 2, TAM_BUF * 2, 0, nullptr, 0));
}

void init_led(void) {
	gpio_reset_pin(LED_GPIO);
	gpio_set_direction(LED_GPIO, GPIO_MODE_OUTPUT);
	gpio_set_level(LED_GPIO, 0);
}

void app_main(void)
{
	init_uart();
	init_led();
	
	ESP_LOGI(TAG, "Inicialização concluída. Iniciando loop de transmissão...");
	
	uint8_t data[TAM_BUF];
    bool toggle_command = true;
		
	while(1){
		const char* mensagem = toggle_command ? "LIGAR" : "DESLIGAR";
		toggle_command = !toggle_command;
		
		ESP_LOGI(TAG, "[TX UART] Enviando mensagem: %s...", mensagem);
		uart_write_bytes(UART_NUM, mensagem, strlen(mensagem));
		
		int len = uart_read_bytes(UART_NUM, data, TAM_BUF, pdMS_TO_TICKS(1000));
		
		if(len>0){
			data[len]='\0';
			
			ESP_LOGI(TAG, "[RX UART] Mensagem recebida: %s", (char*)data, len);
			
			if(strcmp("LIGAR", (char*)data)==0){
				gpio_set_level(LED_GPIO, 1);
				ESP_LOGI(TAG, "LED ACESO");
			}else if(strcmp("DESLIGAR", (char*)data)==0){
				gpio_set_level(LED_GPIO, 0);
				ESP_LOGI(TAG, "LED APAGADO");
			} else {
				ESP_LOGI(TAG, "[ERRO] Comando nao reconhecido!");
			}
		}
		printf("====================================================\n");
		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}

