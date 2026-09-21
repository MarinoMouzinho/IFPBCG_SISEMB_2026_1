# ATV 05 (PWM)

Atividade prática com o objetivo de desenvolver um sistema embarcado utilizando o ESP32 para gerar sinais PWM (Pulse Width Modulation) e controlar a intensidade de um LED e o tom de um buzzer piezoelétrico. Atividade realizada no simulador Wokwi com ESP-IDF como framework de desenvolvimento.

---

Como pré-requisito foram elaborados:

* **Diagrama em Blocos**, representando:
  * ESP32-S3: gerador de PWM.
  * LEDs: Contador de 4 bits.
  * LED: intensidade controlada por PWM (duty cycle).
  * Buzzer: frequência sonora controlada por PWM (a mesma utilizada no LED)
  * Botão A: Incrementar contador +1 (aumenta duty cycle do LED 0,00%; 6.25%; 12,50%, … , 93.75% e 100.00%).
  * Botão B: Decrementar contador -1 (sequência inversa do botão A).

  ![Diagrama em Blocos](./Diagrama%20de%20Blocos.png)
  
* **Diagrama Esquemático**, contendo:
  * LED + resistor conectado ao GPIO configurado como saída PWM.
  * Buzzer conectado a outro GPIO configurado como PWM.
  * Botões conectados a GPIOs com resistores de pull-up ou pull-down adequados.
  * Alimentação de 3,3 V fornecida pelo ESP32.

 ![Diagrama Esquemático](./Diagrama%20Esquematico.png)
 
_Os diagramas foram desenvolvidos no __Draw.io__ e se encontram disponibilizados na raíz do projeto_

---

## Projeto Implementado no Wokwi

https://wokwi.com/projects/475732108172434433
