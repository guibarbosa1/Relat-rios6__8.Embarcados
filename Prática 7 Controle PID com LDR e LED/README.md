# Pratica 7 - Controle PID com LDR e LED

## Introducao

Este projeto implementa um controlador PID completo em um Arduino Uno para regular a luminosidade de um LED com base na leitura de um sensor LDR (Light Dependent Resistor). O objetivo e manter o valor lido pelo LDR o mais proximo possivel de um setpoint fixo (600), ajustando dinamicamente a intensidade do LED via sinal PWM.

O sistema envia dados em tempo real para um computador via comunicacao serial, onde um script Python registra os dados em arquivos CSV e gera graficos de desempenho para cada conjunto de ganhos PID testado.

## Estrutura do Diretorio

```
Pratica 7 Controle PID com LDR e LED/
├── src/
│   ├── LDR_range.c.ino          # Codigo-fonte do Arduino (controlador PID)
│   ├── serial_viwer.py          # Script Python para coleta de dados via serial
│   ├── plot_csv.py              # Script Python para gerar graficos a partir de CSVs
│   └── Relatorio7_Embarcados.pdf # Relatorio completo da pratica
├── Results/
│   ├── Kp(0.0100)Ki(0.1000)Kd(0.0010).csv   # Dados coletados
│   ├── Kp(0.0100)Ki(0.1000)Kd(0.0010).png   # Grafico correspondente
│   └── ...                                   # Demais ensaios
└── LCD_images/
    ├── 1.png ... 6.png             # Capturas do display LCD durante os ensaios
```

## Descricao dos Arquivos

### src/LDR_range.c.ino

Firmware do Arduino Uno. Implementa o controlador PID com as seguintes caractereisticas:

- **Amostragem**: 50 ms (20 Hz)
- **Setpoint**: 600 (valor analogico do LDR)
- **Saida**: PWM (0-255) no pino 3, controlando a intensidade do LED
- **Anti-Windup**: Saturacao matematica na parcela integral para evitar windup
- **Comunicacao serial**: Envia cabecalho `CONFIG:Kp(...)Ki(...)Kd(...)` seguido dos dados no formato CSV (`Tempo_ms,Setpoint,LDR,Erro,PWM`)
- **Duracao do ensaio**: 30 segundos por coleta
- **Display LCD**: Exibe setpoint, leitura do LDR, erro e saida do PID em tempo real

Os ganhos Kp, Ki e Kd sao definidos diretamente no codigo e devem ser alterados antes de cada ensaio.

### src/serial_viwer.py

Script Python responsavel por:

1. Conectar com o Arduino pela porta serial (por padrao `/dev/ttyACM0`, 9600 baud)
2. Aguardar o usuario pressionar ENTER para iniciar cada ensaio
3. Enviar o comando `START\n` para o Arduino
4. Detectar a linha `CONFIG:...` e criar automaticamente um arquivo CSV com o nome contendo os ganhos utilizados
5. Gravar todos os pontos de dados recebidos ate detectar a mensagem `FIM`
6. Permitir multiplas coletas em sequencia sem reiniciar o script

**Dependencias**: `pyserial` (`pip install pyserial`)

### src/plot_csv.py

Script Python que le um arquivo CSV gerado pelo `serial_viwer.py` e produz um grafico PNG com todas as variaveis plotadas contra o tempo.

**Dependencias**: `matplotlib` (`pip install matplotlib`)

**Uso**:
```bash
python plot_csv.py caminho/para/arquivo.csv
```

O grafico e salvo automaticamente no mesmo diretorio do CSV, com extensao `.png`.

## Estrutura dos Resultados

### Arquivos CSV

Cada ensaio gera um arquivo CSV nomeado com os ganhos utilizados, no formato:
```
Kp(X.XXXX)Ki(Y.YYYY)Kd(Z.ZZZZ).csv
```

O conteudo segue o padrao:
```
Tempo_ms,Setpoint,LDR,Erro,PWM
0,600.00,0.00,600.00,21
50,600.00,116.00,484.00,7
...
```

| Coluna    | Descricao                                            |
|-----------|------------------------------------------------------|
| Tempo_ms  | Tempo decorrido em milissegundos apos o inicio       |
| Setpoint  | Valor de referencia (constante em 600)               |
| LDR       | Leitura analogica atual do sensor LDR                |
| Erro      | Diferenca entre o setpoint e a leitura do LDR        |
| PWM       | Saida do controlador aplicada ao LED (0-255)         |

Cada CSV contem aproximadamente 600 linhas (30 segundos de dados amostrados a cada 50 ms).

### Arquivos PNG

Cada CSV possui um grafico PNG correspondente gerado pelo `plot_csv.py`. Os graficos mostram a evolucao temporal de Setpoint, LDR, Erro e PWM durante o ensaio, permitindo analisar a resposta do controlador.

### LCD_images/

Capturas de tela do display LCD do Arduino durante a execucao dos ensaios, mostrando os valores de setpoint, leitura LDR, erro e saida do PID em tempo real.

## Como Rodar

### Pre-requisitos

- Arduino Uno conectado via USB
- Arduino IDE instalado (para compilacao e upload do firmware)
- Python 3 instalado
- Dependencias Python: `pyserial`, `matplotlib`

### Passo 1 - Compilar e Carregar o Firmware

1. Abrir o arquivo `src/LDR_range.c.ino` na Arduino IDE
2. Configurar os ganhos PID desejados diretamente no codigo (linhas 14-16):
   ```c
   double Kp = 0.01;
   double Ki = 0.3;
   double Kd = 0.001;
   ```
3. Selecionar a placa Arduino Uno e a porta serial correta
4. Clicar em "Upload"
5. Aguardar a mensagem "Aguardando PC..." no display LCD

### Passo 2 - Coletar Dados

1. No terminal, navegue ate o diretorio `src/`:
   ```bash
   cd "src/"
   ```
2. Execute o script de coleta:
   ```bash
   python serial_viwer.py
   ```
3. Certifique-se de que a porta serial esta correta no script (linha 4: `/dev/ttyACM0`)
4. Pressione ENTER quando solicitado para iniciar a coleta
5. Aguarde os 30 segundos de ensaio
6. O arquivo CSV sera criado automaticamente no diretorio corrente
7. Repita os passos 4-6 para cada novo ensaio com ganhos diferentes
8. Pressione `Ctrl+C` para encerrar o script

### Passo 3 - Gerar Graficos

Para cada CSV coletado, gere o grafico correspondente:
```bash
python plot_csv.py "../Results/Kp(0.0100)Ki(0.1000)Kd(0.0010).csv"
```

Ou para gerar graficos em lote (bash):
```bash
for csv in ../Results/*.csv; do
    python plot_csv.py "$csv"
done
```

## Reproducao do Experimento

Para reproduzir este experimento, e necessario:

1. **Hardware**:
   - Arduino Uno
   - Sensor LDR conectado ao pino analogico A0
   - LED conectado ao pino digital 3 (via resistor adequado)
   - Display LCD RGB (via I2C, biblioteca `rgb_lcd.h`)
   - Cabo USB para comunicacao com o computador

2. **Montagem do circuito**:
   - LDR em divisor de tensao com resistor fixo no pino A0
   - LED com resistor em serie no pino 3 (capaz de receber PWM)
   - Display LCD conectado via I2C (SDA/SCL)

3. **Ambiente**:
   - Garantir que a luminosidade ambiente seja controlada ou constante durante os ensaios
   - Posicao do LDR e LED deve ser fixa entre todos os ensaios para manter consistencia

4. **Procedimento**:
   - Para cada conjunto de ganhos, altere as variaveis `Kp`, `Ki` e `Kd` no firmware
   - Recompile e faca upload para o Arduino
   - Aguarde a mensagem "Aguardando PC..." no LCD
   - Execute o `serial_viwer.py` e colete os 30 segundos de dados
   - Repita para todos os ganhos desejados
   - Gere os graficos com `plot_csv.py` para analisar a resposta de cada configuracao

5. **Analise**:
   - Compare os graficos para identificar qual conjunto de ganhos apresenta menor overshoot, menor tempo de estabilizacao e menor erro em regime permanente
   - O LCD permite observacao em tempo real do comportamento do controlador
