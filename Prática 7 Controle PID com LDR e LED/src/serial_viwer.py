import serial
import time

porta = '/dev/ttyACM0' 
baud_rate = 9600

print(f"Tentando conectar na porta {porta}...")

try:
    ser = serial.Serial(porta, baud_rate, timeout=1)
    print("Conectado com sucesso!")
except Exception as e:
    print(f"Erro ao conectar: {e}")
    print("Verifique se o Monitor Serial da IDE do Arduino está fechado.")
    exit()

arquivo = None
estado = "AGUARDANDO_USER"  # Estados possíveis: "AGUARDANDO_USER" ou "GRAVANDO"

print("[Ouvinte Ativo] Controle pronto. Siga as instruções abaixo:")

try:
    while True:
        # --- ESTADO 1: Esperando o comando do usuário ---
        if estado == "AGUARDANDO_USER":
            input("\nPronto para o próximo ensaio? Ajuste as variáveis e aperte ENTER para gravar...")
            
            print("Limpando buffers de comunicação...")
            ser.flushInput()
            ser.flushOutput()
            time.sleep(0.5)
            
            print("Enviando comando seguro de inicialização ('START\\n') para o Arduino...")
            ser.write(b"START\n") 
            
            # Bloqueia novas perguntas de ENTER até que o ensaio atual termine
            estado = "GRAVANDO"
        
        # --- ESTADO 2: Focado exclusivamente em escutar o Arduino ---
        if estado == "GRAVANDO":
            if ser.in_waiting > 0:
                linha = ser.readline().decode('utf-8').strip()
                
                # Detectou a resposta inicial do Arduino e cria o arquivo CSV dinâmico
                if linha.startswith("CONFIG:"):
                    parametros = linha.split("CONFIG:")[1]
                    nome_arquivo = f"{parametros}.csv"
                    
                    arquivo = open(nome_arquivo, "w")
                    print(f"\n[•] Gravação iniciada com sucesso: {nome_arquivo}")
                    
                # Detectou o fim estrito dos 30 segundos
                elif linha == "FIM":
                    if arquivo:
                        arquivo.close()
                        arquivo = None 
                    print("[✓] Janela de 30 segundos concluída. Arquivo salvo!")
                    print("O Arduino continua controlando em segundo plano.")
                    print("--------------------------------------------------------")
                    
                    # Libera o terminal para uma nova gravação/ensaio
                    estado = "AGUARDANDO_USER"
                    
                # Captura os pontos de dados (Tempo, SP, LDR, Erro, PWM)
                else:
                    if arquivo:
                        arquivo.write(linha + "\n")
                        # Opcional: print(linha) # Descomente se quiser ver os pontos no terminal

except KeyboardInterrupt:
    print("\n[!] Coletor Python finalizado pelo usuário.")
finally:
    if arquivo:
        arquivo.close()
    ser.close()