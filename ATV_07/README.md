<div align="center">
    <h1>Atividade 07 - Controle de Eventos Críticos com Interrupções e Temporizadores</h1>
</div>

## Objetivo
Refatorar o sistema de iluminação temporizada da atividade anterior substituindo a técnica de polling por Interrupções de Hardware (ISR) e temporizadores do ESP32, liberando a CPU para execução de outras tarefas ou modos de baixo consumo.

## Material utilizado

- 1 ESP32 (DevKit)
- 1 LED + 1 Resistor de 220 Ω
- 1 Botão + 1 Resistor de 10 kΩ
- Protoboard e cabos

## Instruções
1. Toda a detecção de ação no botão deve ser realizada via Interrupção de Hardware (GPIO ISR).
2. A rotina de interrupção deve ser enxuta e alocada preferencialmente na IRAM (IRAM_ATTR).
3. O sistema deve tratar o efeito bounce mecânico do botão por software sem travar a execução do processador.
4. Primeiro Acionamento: Liga o LED e inicia a contagem de 10 segundos.
5. Renovação de Tempo: Se o LED já estiver aceso e ocorrer um novo acionamento rápido, o tempo de 10 segundos deve ser renovado/reiniciado sem apagar o LED.
6. Desligamento Forçado (Pressionamento Longo): Se o botão for mantido pressionado por 2 segundos ou mais, o LED deve apagar imediatamente e qualquer temporizador ativo deve ser cancelado.
7. O laço principal (app_main) deve permanecer livre/bloqueado e não pode realizar leituras contínuas do pino do botão.
8. Fica estritamente proibido o uso de vTaskDelay(), rom_delay_us() ou laços de espera dentro da ISR ou do fluxo de temporização.

## Resolução Desenvolvida
Disponível em [`main.c`](./main.c) e apresentada ao monitor da disciplina.
