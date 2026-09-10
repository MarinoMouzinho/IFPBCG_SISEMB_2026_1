# ATV 04 (Entradas Digitais)

Atividade prática desenvolvida para controlar 4 LEDs utilizando as GPIOs do ESP32 configuradas como saídas digitais e 2 Buttons como entradas digitais. Esta atividade foi realizada no simulador Wokwi e utilizou a ESP-IDF como framework de desenvolvimento.

Como pré-requisito da simulação foram elaborados:

* Diagrama em Blocos, representando:
  * ESP32-S3: microcontrolador responsável pelo acionamento das saídas digitais e leitura das entradas.
  * LED1, LED2, LED3 e LED4: atuadores controlados pelo ESP32.
  * BUTTON_A, BUTTON_B: entradas digitais.

  ![Diagrama em Blocos](./Diagrama%20de%20Blocos.png)
  
* Esquemático, incluindo:
  * 4 LEDs conectados a GPIOs configuradas como saída digital, cada um em série com seu resistor.
  * 2 push-buttons
  * Alimentação de 3,3 V fornecida pelo ESP32.

 ![Diagrama Esquemático](./Diagrama%20Esquematico.png)
 
_Os diagramas foram elaborados no __Draw.io__ e se encontram disponibilizados na raíz do projeto_

---

## Projeto Implementado no Wokwi

```text
https://wokwi.com/projects/474785932282317825
```