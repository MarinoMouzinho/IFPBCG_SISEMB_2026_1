<div align="center">
    <h1>Atividade 08 - Comunicação Serial Assíncrona (UART) com Teste de Loopback</h1>
</div>

## Objetivo
Compreender o funcionamento básico do periférico UART (Universal Asynchronous Receiver-Transmitter) no ESP32 através da transmissão e recepção de dados em malha fechada (Loopback), validando o processamento de comandos recebidos para controle de periféricos em tempo real.

## Material utilizado

- 1 ESP32 (DevKit)
- 1 LED + 1 Resistor de 220 Ω
- 1 Jumper
- Protoboard e cabos

## Instruções
1. Configurar e inicializar o periférico UART2 do ESP32 via drivers nativos do ESP-IDF (uart_param_config e uart_driver_install) com os seguintes parâmetros:
   <br/>**Baud Rate**: 115200bps
   <br/>**Data Bits**: 8bits
   <br/>**Stop Bits**: 1bit
   <br/>**Paridade**: Nenhuma (None)
   <br/>**Controle de Fluxo**: Desativado
2. Definir os pinos de GPIO para TX e RX da UART2 (ex: GPIO 17 para TX e GPIO 16 para RX) através da função uart_set_pin.
3. O sistema deve transmitir uma mensagem em formato de string pela UART2 periodicamente.
4. As mensagens enviadas devem se alternar a cada ciclo (exemplo: Envia "LIGAR", aguarda a recepção/tempo, envia "DESLIGAR", e assim sucessivamente).
5. O firmware deve ler os dados do buffer de recepção da UART2 (retornados do próprio TX via jumper de loopback).
6. Ao identificar a string "LIGAR", o LED deve acender.
7. Ao identificar a string "DESLIGAR", o LED deve apagar.

## Resolução Desenvolvida
Disponível em [`main.c`](/main.c) e apresentada ao monitor da disciplina.
