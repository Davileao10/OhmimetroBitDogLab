/*
 * Ohmímetro para BitDogLab
 * Calcula resistência, valor E24, faixas de cor, exibe no SSD1306 e controla WS2812
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include <math.h>
 #include "pico/stdlib.h"
 #include "pico/bootrom.h"
 #include "hardware/adc.h"
 #include "hardware/i2c.h"
 #include "hardware/pio.h"
 #include "lib/ssd1306.h"
 #include "lib/font.h"
 #include "ws2812.pio.h"
 
 #define I2C_PORT i2c1
 #define I2C_SDA 14
 #define I2C_SCL 15
 #define endereco 0x3C
 #define ADC_PIN 28
 #define Botao_A 5
 #define Botao_B 6
 #define WS2812_PIN 7 // Pino para matriz WS2812 5x5
 
 int R_conhecido = 10000;
 float R_x = 0.0;
 float ADC_VREF = 3.31;
 int ADC_RESOLUTION = 4095;
 
 // Variáveis para histerese
 static float last_e24_value = 0.0;
 static int last_band1 = 0, last_band2 = 0, last_mult = 0;
 
 // Série E24 (5% tolerância)
 const float e24_values[] = {1.0, 1.1, 1.2, 1.3, 1.5, 1.6, 1.8, 2.0, 2.2, 2.4, 2.7, 3.0,
                             3.3, 3.6, 3.9, 4.3, 4.7, 5.1, 5.6, 6.2, 6.8, 7.5, 8.2, 9.1};
 
 // Estrutura para cores RGB
 typedef struct {
     uint8_t r;
     uint8_t g;
     uint8_t b;
 } rgb_t;
 
 // Mapeamento de cores
 const char *colors[] = {"Preto", "Marrom", "Vermelho", "Laranja", "Amarelo",
                         "Verde", "Azul", "Violeta", "Cinza", "Branco"};
 const rgb_t colors_rgb[] = {
     {0, 0, 0},      // Preto
     {7, 1, 0},     // Marrom
     {5, 0, 0},    // Vermelho
     {15, 1, 0},   // Laranja
     {7, 6, 0},  // Amarelo
     {0, 6, 0},     // Verde
     {0, 0, 6},    // Azul
     {6, 0, 4},    // Violeta
     {4, 4, 4},   // Cinza
     {50, 50, 50} // Branco
 };
 
 // Função para modo BOOTSEL
 void gpio_irq_handler(uint gpio, uint32_t events) {
     reset_usb_boot(0, 0);
 }
 
 // Função para encontrar o valor E24 mais próximo
 float find_e24_value(float resistance, int *multiplier) {
     if (resistance <= 0 || isinf(resistance) || isnan(resistance)) {
         *multiplier = 0;
         return 0.0;
     }
     float log_res = log10(resistance);
     *multiplier = floor(log_res);
     float normalized = resistance / pow(10, *multiplier);
     float closest = e24_values[0];
     float min_diff = fabs(normalized - closest);
     for (int i = 1; i < 24; i++) {
         float diff = fabs(normalized - e24_values[i]);
         if (diff < min_diff) {
             min_diff = diff;
             closest = e24_values[i];
         }
     }
     return closest * pow(10, *multiplier);
 }
 
 // Função para calcular faixas de cor
 void get_color_bands(float e24_value, int multiplier, int *band1, int *band2, int *mult) {
     if (e24_value <= 0) {
         *band1 = *band2 = *mult = 0;
         return;
     }
     int norm_mult = floor(log10(e24_value)) - 1; // Normalização
     float normalized = e24_value / pow(10, norm_mult);
     int value = (int)(normalized + 0.5);
     *band1 = value / 10;
     *band2 = value % 10;
     *mult = multiplier - 1; // Ajustar para índice de cor
     if (*mult < 0) *mult = 0; // Evitar índices negativos
     if (*band1 > 9) *band1 = 0; // Validar
     if (*band2 > 9) *band2 = 0; // Validar
 }
 
 // Função para WS2812 com valores RGB
 static inline void put_pixel_rgb(uint8_t r, uint8_t g, uint8_t b) {
     uint32_t pixel_grb = ((uint32_t)g << 16) | ((uint32_t)r << 8) | b;
     pio_sm_put_blocking(pio0, 0, pixel_grb << 8u);
 }
 
 int main() {
     stdio_init_all();
 
     // Configurar botão B para BOOTSEL
     gpio_init(Botao_B);
     gpio_set_dir(Botao_B, GPIO_IN);
     gpio_pull_up(Botao_B);
     gpio_set_irq_enabled_with_callback(Botao_B, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
 
     // Configurar botão A (não usado, mantido para expansão)
     gpio_init(Botao_A);
     gpio_set_dir(Botao_A, GPIO_IN);
     gpio_pull_up(Botao_A);
 
     // Configurar I2C
     i2c_init(I2C_PORT, 400 * 1000);
     gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
     gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
     gpio_pull_up(I2C_SDA);
     gpio_pull_up(I2C_SCL);
 
     // Configurar SSD1306
     ssd1306_t ssd;
     ssd1306_init(&ssd, 128, 64, false, endereco, I2C_PORT);
     sleep_ms(100); // Atraso para estabilizar
     ssd1306_config(&ssd);
     ssd1306_fill(&ssd, false);
     ssd1306_send_data(&ssd);
 
     // Configurar ADC
     adc_init();
     adc_gpio_init(ADC_PIN);
 
     // Configurar WS2812
     PIO pio = pio0;
     uint offset = pio_add_program(pio, &ws2812_program);
     ws2812_program_init(pio, 0, offset, WS2812_PIN, 800000, false);
 
     // Teste inicial da matriz WS2812 (acender colunas 0, 1, 2 com cores de teste por 5 segundos)
     for (int i = 0; i < 25; i++) {
         if (i == 2 || i == 7 || i == 12 || i == 17 || i == 22) {
             put_pixel_rgb(7, 1, 0); // Coluna 2: Marrom (multiplicador)
         } else if (i == 3 || i == 6 || i == 13 || i == 16 || i == 23) {
             put_pixel_rgb(6, 0, 4); // Coluna 1: Violeta (segunda faixa)
         } else if (i == 4 || i == 5 || i == 14 || i == 15 || i == 24) {
             put_pixel_rgb(8, 4, 0); // Coluna 0: Amarelo (primeira faixa)
         } else {
             put_pixel_rgb(0, 0, 0);
         }
     }
     sleep_ms(5000); // Pausar para verificar
 
     char str_res[16], str_band1[16], str_band2[16], str_mult[16];
     while (true) {
         adc_select_input(2); // ADC2 (GPIO28)
         float soma = 0.0f;
         for (int i = 0; i < 1000; i++) {
             soma += adc_read();
             sleep_us(2000); // 2 ms
         }
         float media = soma / 1000.0f;
 
         // Depurar valor do ADC
         printf("ADC media: %.1f\n", media);
 
         // Calcular resistência
         if (media > 0 && media < ADC_RESOLUTION - 10) {
             R_x = (R_conhecido * media) / (ADC_RESOLUTION - media);
         } else {
             R_x = INFINITY; // Indicar erro
         }
 
         // Encontrar valor E24 com histerese
         int multiplier;
         float e24_value;
         int band1, band2, mult;
         if (last_e24_value == 0.0 || R_x == INFINITY || 
             R_x < last_e24_value * 0.9 || R_x > last_e24_value * 1.1) {
             // Atualizar e24_value se fora do intervalo de ±10%
             e24_value = find_e24_value(R_x, &multiplier);
             get_color_bands(e24_value, multiplier, &band1, &band2, &mult);
             last_e24_value = e24_value;
             last_band1 = band1;
             last_band2 = band2;
             last_mult = mult;
             // Depurar faixas
             printf("band1: %d, band2: %d, mult: %d\n", band1, band2, mult);
             printf("colors: 1:%s 2:%s M:%s\n", colors[band1], colors[band2], colors[mult]);
         } else {
             // Manter último valor e faixas
             e24_value = last_e24_value;
             band1 = last_band1;
             band2 = last_band2;
             mult = last_mult;
         }
 
         // Formatando a resistência
         if (e24_value >= 1e6) {
             sprintf(str_res, "%.2f MOhm", e24_value / 1e6);
         } else if (e24_value >= 1e3) {
             sprintf(str_res, "%.2f kOhm", e24_value / 1e3);
         } else if (e24_value > 0) {
             sprintf(str_res, "%.2f Ohm", e24_value);
         } else {
             sprintf(str_res, "Erro");
         }
 
         // Formatando as faixas de cor (uma por linha)
         if (e24_value > 0) {
             sprintf(str_band1, "1:%s", colors[band1]);
             sprintf(str_band2, "2:%s", colors[band2]);
             sprintf(str_mult, "M:%s", colors[mult]);
         } else {
             sprintf(str_band1, "Sem resistor");
             str_band2[0] = '\0'; // String vazia
             str_mult[0] = '\0';  // String vazia
         }
 
         // Exibir no SSD1306 (uma faixa por linha)
         ssd1306_fill(&ssd, false); // Limpar o buffer
         ssd1306_draw_string(&ssd, "Ohmimetro", 0, 0);   // Título
         ssd1306_draw_string(&ssd, str_res, 0, 14);      // Resistência
         ssd1306_draw_string(&ssd, str_band1, 0, 24);    // Primeira faixa
         ssd1306_draw_string(&ssd, str_band2, 0, 38);    // Segunda faixa
         ssd1306_draw_string(&ssd, str_mult, 0, 52);     // Multiplicador
         ssd1306_send_data(&ssd);
 
         // Exibir nas WS2812 (colunas 0, 1, 2)
         for (int i = 0; i < 25; i++) {
             if (e24_value > 0 && (i == 2 || i == 7 || i == 12 || i == 17 || i == 22)) {
                 put_pixel_rgb(colors_rgb[mult].r, colors_rgb[mult].g, colors_rgb[mult].b); // Coluna 2: multiplicador
             } else if (e24_value > 0 && (i == 3 || i == 6 || i == 13 || i == 16 || i == 23)) {
                 put_pixel_rgb(colors_rgb[band2].r, colors_rgb[band2].g, colors_rgb[band2].b); // Coluna 1: segunda faixa
             } else if (e24_value > 0 && (i == 4 || i == 5 || i == 14 || i == 15 || i == 24)) {
                 put_pixel_rgb(colors_rgb[band1].r, colors_rgb[band1].g, colors_rgb[band1].b); // Coluna 0: primeira faixa
             } else {
                 put_pixel_rgb(0, 0, 0); // Desligar demais LEDs
             }
         }
 
         sleep_ms(700);
     }
 }