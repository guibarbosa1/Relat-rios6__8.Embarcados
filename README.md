# Práticas de Sistemas Embarcados

Este repositório contém as implementações e relatórios das práticas desenvolvidas para a disciplina de Sistemas Embarcados. Os projetos envolvem o uso de microcontroladores (Arduino Uno e família 8051), desenvolvimento de firmware em C++ e Assembly, sistemas operacionais de tempo real (FreeRTOS) e malhas de controle PID.

## Estrutura do Repositório

### 📁 [Prática 6: Comunicação Serial entre Arduino Uno e 8051](./Pratica_06_Comunicacao_Serial)
O objetivo desta prática foi implementar uma arquitetura de comunicação serial assíncrona (UART) envolvendo um Arduino Uno e um microcontrolador da família 8051 (89S52). 
**O que foi feito:** O Arduino atua como transmissor de uma sequência de bytes, enquanto o 8051 atua como receptor e gerencia a exibição dos dados em um display LCD operando em modo de 4 bits.Um foco do projeto foi renderizar caracteres fora da tabela ASCII padrão acessando diretamente a ROM do display via código hexadecimal.
* **Tecnologias:** C++ (Arduino), Assembly (8051), UART, Display LCD HD44780.

### 📁 [Prática 7: Controlador PID para Iluminação](./Pratica_07_Controlador_PID) 
Projeto focado na aplicação de Conversão AD e controle em malha fechada.
* **O que será feito:** Utilização do *Kit Grove* do Arduino (composto por LDR, LED e LCD) para projetar um controlador PID que gerencie o nível de iluminação. O sistema deve manter um valor desejado de incidência de luz (*set point*) no LDR através do controle da intensidade do LED. O ajuste das constantes Proporcional (Kp), Integral (Ki) e Derivativa (Kd) será feito manualmente por tentativa e erro, sem o uso de bibliotecas externas prontas.
* **Tecnologias:** Arduino, Sensores LDR, PWM, Teoria de Controle (PID).

### 📁 [Prática 8: Cálculo de PI via Método de Monte Carlo com FreeRTOS](./Pratica_08_FreeRTOS_Monte_Carlo)
Esta prática foca no uso de Sistemas Operacionais de Tempo Real (RTOS) embarcados em microcontroladores *single-core* (ATmega328P)[. 
**O que foi feito:** Implementação do método estocástico de Monte Carlo para aproximar o valor da constante Pi (π). Foram desenvolvidas e comparadas duas arquiteturas de software: uma com 2 tarefas (cálculo e impressão) e outra com 4 tarefas (3 de cálculo concorrente e 1 de impressão). O estudo aborda o tratamento de condições de corrida com *Mutex* e os problemas físicos de *Stack Overflow* devido à limitação de memória RAM (2KB) do Arduino.
* **Tecnologias:** C/C++, FreeRTOS, Escalonamento de Tarefas, Mutex, Otimização de Memória.

---
*Desenvolvido por Gabriel Mello Silveira Targas e Guilherme Barbosa.
Universidade Federal de São Carlos*
