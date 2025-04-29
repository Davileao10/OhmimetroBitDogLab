# 🎨 Ohmímetro Colorido

Um **ohmímetro** baseado na placa **BitDogLab (RP2040)** que mede resistências de **510 Ω a 100 kΩ**, identifica o valor padrão da série **E24** (tolerância de 5%), calcula as faixas de cor correspondentes e exibe os resultados no display **OLED SSD1306** e na matriz de LEDs **WS2812**.

---

## 📋 Descrição

Este projeto foi desenvolvido como parte de uma atividade prática para consolidar conceitos de **medição eletrônica** e **programação embarcada**.

O ohmímetro utiliza o microcontrolador **RP2040** da BitDogLab para:

- Medir resistências usando o **ADC (pino 28)**.
- Ajustar o valor medido para a série **E24**.
- Determinar as **faixas de cor** (1ª, 2ª e multiplicador).
- Exibir os resultados no **display SSD1306**.
- Mostrar as cores das faixas na **matriz WS2812 5x5**.

O projeto é funcional, modular e inclui **histerese** para estabilidade nas leituras.

> **Autor:** Davi Nascimento Leão  
> **Polo:** Feira de Santana  
> **Data:** Abril 2025

---

## ✨ Funcionalidades

- **Medição de Resistência:** Utiliza um divisor de tensão com resistor conhecido de **10 kΩ**.
- **Série E24:** Ajusta automaticamente para o valor mais próximo da série E24.
- **Faixas de Cor:** Calcula e exibe a primeira, segunda faixas e o multiplicador.
- **Exibição no Display:** Valor e faixas mostrados no **SSD1306**.
- **Matriz WS2812:** Exibe as cores das faixas nas colunas **0**, **1** e **2**.
- **Histerese:** Atualiza leituras apenas com variação acima de **±10%**.
- **Depuração:** Envia valores para o terminal via **stdio**.
- **Botão BOOTSEL:** (GPIO6) Reinicia para o modo de programação.

---

## 📁 Estrutura do Repositório

```
├── Ohmimetro01.c       # Código principal
├── lib/
│   ├── ssd1306.h       # Biblioteca SSD1306
│   ├── font.h          # Fonte para o display
├── ws2812.pio          # Programa PIO para matriz WS2812
├── README.md           # Este documento
```

---

## ⚙️ Requisitos

### 🔧 Hardware

- Placa **BitDogLab** com **RP2040**.
- Display **OLED SSD1306** (I2C, 128x64).
- Matriz de LEDs **WS2812** (5x5).
- Resistor de **10 kΩ** para o divisor de tensão.
- Resistores de teste (ex.: **5,10 kΩ**, **10 kΩ**).
- Botões **A (GPIO5)** e **B (GPIO6)**.

### 🖥️ Software

- **Pico SDK**
- **CMake**
- **GCC ARM**
- **Terminal Serial** (ex.: PuTTY)

> **Bibliotecas incluídas**: `ssd1306.h`, `font.h`, `ws2812.pio`

---

## 🚀 Como Usar

### 1. Configurar o Ambiente

Instale o Pico SDK e configure:

```bash
git clone <URL_DO_REPOSITORIO>
cd ohmimetro-colorido
```

### 2. Conectar o Hardware

- Display SSD1306 nos pinos **GPIO14 (SDA)** e **GPIO15 (SCL)**.
- Matriz WS2812 no pino **GPIO7**.
- Circuito divisor de tensão no **GPIO28**.

### 3. Compilar e Carregar

```bash
mkdir build
cd build
cmake ..
make
```

- Carregue o `.uf2` para a BitDogLab utilizando o botão **BOOTSEL**.

### 4. Executar

- Conecte um resistor de teste (ex.: **10 kΩ**).
- Observe as informações no display e as cores na matriz.
- Para depuração, abra um terminal serial a **115200 baud**.

---

## 🖥️ Demonstração

> **Veja o vídeo de demonstração:** [(https://drive.google.com/file/d/1sjbX0mB1EUuy63MLrf7fsuFSszJl3LHb/view?usp=sharing)]

Exemplos:

- **10 kΩ**:  
  - Display: `"10.00 kOhm"`, `"1:Marrom"`, `"2:Preto"`, `"M:Laranja"`.
  - WS2812: Colunas com cores marrom, preto e laranja.

- **5,10 kΩ**:
  - Display: `"5.10 kOhm"`, `"1:Verde"`, `"2:Marrom"`, `"M:Marrom"`.

- **2 kΩ**:
  - Display: `"2.00 kOhm"`, `"1:Vermelho"`, `"2:Preto"`, `"M:Vermelho"`.

---

## 🛠️ Estrutura do Código

### 📂 `Ohmimetro01.c`

- Configura ADC, I2C, PIO e botões.
- Lê resistência, ajusta valor E24, calcula faixas de cor.
- Exibe no **display** e **matriz WS2812**.
- Implementa **histerese** e **depuração serial**.

### 🔑 Funções Principais

- `find_e24_value`: Ajusta resistência para a série E24.
- `get_color_bands`: Calcula faixas de cor.
- `put_pixel_rgb`: Controla a matriz WS2812.

---

## 📝 Notas

- **Cores WS2812**: RGBs adaptados para visibilidade em matriz 5x5 (pode ser ajustado).
- **Histerese**: Garante estabilidade nas leituras.
- **Limitação**: Suporta resistores de **510 Ω a 100 kΩ**. Para outras faixas, ajustar **R_conhecido**.

---

## 🙌 Créditos

- **Autor:** Davi Nascimento Leão
- **Bibliotecas:** Pico SDK e implementações abertas de SSD1306 e WS2812.
- **Inspiração:** Atividade prática da disciplina de Eletrônica, Polo Feira de Santana.

