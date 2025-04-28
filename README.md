Ohmímetro Colorido
Um ohmímetro baseado na placa BitDogLab (RP2040) que mede resistências de 510 Ω a 100 kΩ, identifica o valor padrão da série E24 (tolerância de 5%), calcula as faixas de cor correspondentes e exibe os resultados no display OLED SSD1306 e na matriz de LEDs WS2812.

📋 Descrição
Este projeto foi desenvolvido como parte de uma atividade prática para consolidar conceitos de medição eletrônica e programação embarcada. O ohmímetro utiliza o microcontrolador RP2040 da BitDogLab para:

Medir resistências usando o ADC (pino 28).
Ajustar o valor medido para a série E24.
Determinar as faixas de cor (1ª, 2ª e multiplicador).
Exibir o valor da resistência e as faixas no display SSD1306 (I2C).
Mostrar as cores das faixas na matriz WS2812 5x5 (PIO).

O projeto é funcional, modular e inclui histerese para estabilidade nas leituras. Testado com resistores de 5 kΩ e 10 kΩ, atende a todos os requisitos da atividade.
Autor: Davi Nascimento LeãoPolo: Feira de SantanaData: Abril 2025
✨ Funcionalidades

Medição de Resistência: Lê a tensão no resistor via ADC (GPIO28) em um divisor de tensão com resistor conhecido de 10 kΩ, calculando a resistência com média de 1000 amostras.
Série E24: Ajusta a resistência medida para o valor mais próximo da série E24 (tolerância de 5%), como 10 kΩ ou 5.1 kΩ.
Faixas de Cor: Calcula as três faixas de cor (ex.: Marrom, Preto, Laranja para 10 kΩ).
Exibição no Display: Mostra no SSD1306 (128x64, I2C) o valor (ex.: "10.00 kOhm") e as faixas (ex.: "1:Marrom", "2:Preto", "M:Laranja").
Exibição na Matriz WS2812: Exibe as cores das faixas nas colunas 0 (1ª faixa), 1 (2ª faixa) e 2 (multiplicador) da matriz 5x5.
Histerese: Estabiliza leituras, atualizando o valor E24 apenas se a resistência variar mais de ±10%.
Depuração: Exibe valores do ADC e faixas no terminal via stdio.
Botão BOOTSEL: Botão B (GPIO6) reinicia a placa para o modo BOOTSEL.

📁 Estrutura do Repositório
├── Ohmimetro01.c       # Código principal do ohmímetro
├── lib/
│   ├── ssd1306.h       # Biblioteca para o display SSD1306
│   ├── font.h          # Fonte para renderização de texto no display
├── ws2812.pio          # Programa PIO para controle da matriz WS2812
├── README.md           # Este arquivo

⚙️ Requisitos
Hardware

Placa BitDogLab com RP2040.
Display OLED SSD1306 (128x64, I2C, conectado aos pinos GPIO14 SDA e GPIO15 SCL).
Matriz de LEDs WS2812 5x5 (conectada ao pino GPIO7).
Resistor conhecido de 10 kΩ para o divisor de tensão.
Resistores para teste (ex.: 5 kΩ, 10 kΩ) conectados ao pino GPIO28 (ADC).
Botões A (GPIO5, não usado) e B (GPIO6, BOOTSEL).

Software

Pico SDK: Para compilar e carregar o código no RP2040.
Ferramentas: CMake, GCC ARM, e um terminal para depuração (ex.: PuTTY).
Bibliotecas: Incluídas no diretório lib/ (ssd1306.h, font.h) e ws2812.pio.

🚀 Como Usar

Configurar o Ambiente:

Instale o Pico SDK e configure o ambiente de desenvolvimento (veja Pico SDK Getting Started).
Clone este repositório:git clone <URL_DO_REPOSITORIO>
cd ohmimetro-colorido




Conectar o Hardware:

Conecte o display SSD1306 aos pinos GPIO14 (SDA) e GPIO15 (SCL).
Conecte a matriz WS2812 ao pino GPIO7.
Monte o circuito divisor de tensão com um resistor de 10 kΩ e o resistor desconhecido no pino GPIO28 (ADC2).
Verifique as conexões dos botões (GPIO5 e GPIO6).


Compilar e Carregar:

Crie um diretório build e compile:mkdir build
cd build
cmake ..
make


Carregue o firmware na BitDogLab (use o modo BOOTSEL, pressionando o botão B ou via USB).


Executar:

Conecte um resistor (ex.: 10 kΩ) ao circuito.
O display SSD1306 mostrará o valor (ex.: "10.00 kOhm") e as faixas (ex.: "1:Marrom", "2:Preto", "M:Laranja").
A matriz WS2812 exibirá as cores nas colunas 0, 1 e 2.
Para depuração, abra um terminal serial (ex.: PuTTY, 115200 baud) para ver os valores do ADC e faixas.


Testar:

Teste com resistores de 5 kΩ, 10 kΩ e sem resistor (circuito aberto).
Pressione o botão B (GPIO6) para entrar no modo BOOTSEL, se necessário.



🖥️ Demonstração
Veja o vídeo de demonstração: [INSIRA O LINK DO VÍDEO AQUI]  

10 kΩ: Display mostra "10.00 kOhm", "1:Marrom", "2:Preto", "M:Laranja"; WS2812 exibe as cores correspondentes.
5 kΩ: Display mostra "5.10 kOhm", "1:Verde", "2:Marrom", "M:Marrom"; WS2812 atualiza as cores.
2kΩ: Display mostra "5.10 kOhm", "1:Vermelho", "2:Preto", "M:Vermelho"; WS2812 exibe as cores correspondentes.

🛠️ Estrutura do Código

Ohmimetro01.c:
Configura ADC (GPIO28), I2C (SSD1306), PIO (WS2812) e botões.
Lê a resistência via ADC, calcula o valor E24 e as faixas de cor.
Exibe no display SSD1306 e na matriz WS2812.
Inclui histerese (±10%) e depuração via terminal.


Funções Principais:
find_e24_value: Ajusta a resistência para a série E24.
get_color_bands: Calcula as faixas de cor.
put_pixel_rgb: Controla os LEDs WS2812.


Bibliotecas:
ssd1306.h: Renderiza texto no display.
ws2812.pio: Comunicação com a matriz WS2812.



📝 Notas

Cores WS2812: Os valores RGB (ex.: Marrom {7, 1, 0}, Laranja {15, 1, 0}) foram ajustados para a matriz. Se necessário, modifique colors_rgb para maior visibilidade.
Histerese: Garante estabilidade, evitando flutuações nas leituras.
Limitações: Suporta resistores de 510 Ω a 100 kΩ (série E24). Para outras faixas, ajuste R_conhecido ou o circuito.

🙌 Créditos

Autor: Davi Nascimento Leão.
Bibliotecas: Baseadas em exemplos do Pico SDK e bibliotecas abertas para SSD1306 e WS2812.
Inspiração: Atividade prática da disciplina de Eletrônica, polo Feira de Santana.

