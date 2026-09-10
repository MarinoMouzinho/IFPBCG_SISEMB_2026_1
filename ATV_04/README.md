# ATV 04 (Entradas Digitais)

Atividade prática desenvolvida com o objetivo de elaborar um controlador do estado de 4 LEDs por meio de dois botões. Atividade realizada no simulador Wokwi com ESP-IDF como framework de desenvolvimento.

---

Como pré-requisito foram elaborados:

* **Diagrama em Blocos**, representando:
  * ESP32S3;
  * 4 LED;
  * 2 Buttons;
  * Componentes para os circuitos de driver.

  ![Diagrama em Blocos](./Diagrama%20de%20Blocos.png)
  
* **Diagrama Esquemático**, contendo:
  * 4 LEDs + resistores conectados aos GPIOs configurados como saída digital.
  * 2 push-buttons + circuito pull-down, conectados aos GPIOs como entrada digital.
  * Alimentação VCC de 3,3 V fornecida pelo ESP32.

 ![Diagrama Esquemático](./Diagrama%20Esquematico.png)
 
_Os diagramas foram desenvolvidos no __Draw.io__ e se encontram disponibilizados na raíz do projeto_

---

## Projeto Implementado no Wokwi

https://wokwi.com/projects/474785932282317825

