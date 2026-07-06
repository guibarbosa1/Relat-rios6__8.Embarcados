#include <Wire.h>
#include "rgb_lcd.h"

rgb_lcd lcd;

const int pinoLED = 3;
const int pinoLDR = A0;

// Parâmetros do Sistema
double setPoint = 600.0; 
double erro, output;

// Ganhos do Controlador
double Kp = 0.01; 
double Ki = 0.3;
double Kd = 0.001;

// --- VARIÁVEIS MATEMÁTICAS PARA O PID ---
double integral = 0.0;
double erroAnterior = 0.0;
const double dt = 0.05; // Intervalo de tempo estrito de 50ms expresso em segundos (50 / 1000)

// Variáveis para Controle de Tempo Não-Bloqueante
unsigned long tempoInicio = 0;
unsigned long tempoAnteriorControle = 0;
unsigned long tempoAnteriorLCD = 0;

const unsigned long intervaloControle = 50;  // Amostragem/Log a cada 50ms
const unsigned long intervaloLCD = 500;      // Atualização do LCD a cada 500ms

// Controle de Estados do Sistema
bool primeiroStartRecebido = false; 
bool gravandoLog = false;
double valorLDR = 0.0; 

String comandoBuffer = "";

void setup() {
    Serial.begin(9600); 
    pinMode(pinoLED, OUTPUT);
    lcd.begin(16, 2);
    
    // Estado Inicial: congelado aguardando o PC
    lcd.setRGB(100, 100, 255); // Fundo azul (Espera)
    lcd.setCursor(0, 0);
    lcd.print("Aguardando PC...");
    lcd.setCursor(0, 1);
    lcd.print("Rode o Python");
    
    analogWrite(pinoLED, 0); 
    
    // Limpa resíduos elétricos da inicialização
    delay(1000);
    while(Serial.available() > 0) {
        Serial.read(); 
    }
}

void loop() {
    unsigned long tempoAtual = millis();

    // --- 1. ESCUTA DA PORTA SERIAL ---
    while (Serial.available() > 0) {
        char caractereRecebido = (char)Serial.read();
        
        if (caractereRecebido == '\n') {
            comandoBuffer.trim();
            
            if (comandoBuffer == "START" && !gravandoLog && !primeiroStartRecebido) {
                tempoInicio = millis();
                gravandoLog = true;
                primeiroStartRecebido = true; 
                
                // Zera as memórias do PID ao iniciar um novo ensaio
                integral = 0.0;
                erroAnterior = 0.0;
                
                // Envia as configurações para o CSV
                Serial.print("CONFIG:");
                Serial.print("Kp("); Serial.print(Kp, 4); Serial.print(")");
                Serial.print("Ki("); Serial.print(Ki, 4); Serial.print(")");
                Serial.print("Kd("); Serial.print(Kd, 4); Serial.println(")");
                
                Serial.println("Tempo_ms,Setpoint,LDR,Erro,PWM");
                
                lcd.setRGB(255, 255, 255); 
                lcd.clear();
            }
            comandoBuffer = ""; 
        } 
        else {
            comandoBuffer += caractereRecebido;
        }
    }

    if (!primeiroStartRecebido) {
        return; 
    }

    // --- 2. CRONÔMETRO 1: CÁLCULO DO PID COMPLETO (A cada 50ms) ---
    if (tempoAtual - tempoAnteriorControle >= intervaloControle) {
        tempoAnteriorControle = tempoAtual;

        valorLDR = analogRead(pinoLDR);
        erro = setPoint - valorLDR;

        // A) PARCELA PROPORCIONAL
        double P = Kp * erro;

        // B) PARCELA INTEGRAL (Acumulador no tempo)
        integral += erro * dt;
        
        // Anti-Windup Matemático: Evita que a integral sature sozinha se o LED demorar a responder
        double I = Ki * integral;
        if (I > 255.0) { I = 255.0; integral = 255.0 / Ki; }
        if (I < 0.0)   { I = 0.0;   integral = 0.0; }

        // C) PARCELA DERIVATIVA (Taxa de variação do erro)
        double derivada = (erro - erroAnterior) / dt;
        double D = Kd * derivada;
        
        erroAnterior = erro; // Guarda o erro atual para o próximo cálculo

        // EQUAÇÃO COMPLETA DO PID
        output = P + I + D;

        // Saturação final do Atuador (PWM do pino do Arduino)
        if (output > 255) output = 255;
        if (output < 0) output = 0;

        analogWrite(pinoLED, (int)output);

        // Envio do Log para o Python
        if (gravandoLog) {
            unsigned long tempoDecorridoLog = tempoAtual - tempoInicio;

            if (tempoDecorridoLog >= 30000) {
                Serial.println("FIM"); 
                gravandoLog = false;   
                lcd.setRGB(0, 255, 0); // Fundo verde: fim do ensaio!
            } else {
                Serial.print(tempoDecorridoLog); Serial.print(",");
                Serial.print(setPoint); Serial.print(",");
                Serial.print(valorLDR); Serial.print(",");
                Serial.print(erro); Serial.print(",");
                Serial.println((int)output);
            }
        }
    }

    // --- 3. CRONÔMETRO 3: ATUALIZAÇÃO DO DISPLAY LCD (A cada 500ms) ---
    if (tempoAtual - tempoAnteriorLCD >= intervaloLCD) {
        tempoAnteriorLCD = tempoAtual;

        lcd.setCursor(0, 0);
        lcd.print("SP:"); lcd.print((int)setPoint);
        lcd.print(" LDR:"); lcd.print((int)valorLDR);
        lcd.print("   "); 

        lcd.setCursor(0, 1);
        lcd.print("Err:"); lcd.print((int)erro);
        lcd.print(" Out:"); lcd.print((int)output);
        
        if (gravandoLog) {
            lcd.print(" REC"); 
        } else {
            lcd.print(" OFF"); 
        }
        lcd.print("   "); 
    }
}