#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"
#include "hardware/i2c.h"
#include "inc/ssd1306.h"
#include "inc/font.h"

// Definição de GPIOs
#define JOYSTICK_X 26  // ADC0
#define JOYSTICK_Y 27  // ADC1
#define BOTAO_JOYSTICK 22
#define BOTAO_A 5
#define LED_RED 13
#define LED_GREEN 11
#define LED_BLUE 12
#define I2C_SDA 14
#define I2C_SCL 15

// Display SSD1306
ssd1306_t ssd;

// Variáveis de estado
volatile bool estado_led_verde = false;
volatile bool estado_leds_pwm = true; // Começa ligado
volatile bool exibir_borda = false; // Estado inicial: sem borda


void inicializar_hardware(){
    stdio_init_all();

    // Inicializa botões
    gpio_init(BOTAO_JOYSTICK);
    gpio_set_dir(BOTAO_JOYSTICK, GPIO_IN);
    gpio_pull_up(BOTAO_JOYSTICK);
    
    gpio_init(BOTAO_A);
    gpio_set_dir(BOTAO_A, GPIO_IN);
    gpio_pull_up(BOTAO_A);

    // Inicializa LED Verde (controle por botão)
    gpio_init(LED_GREEN);
    gpio_set_dir(LED_GREEN, GPIO_OUT);
    gpio_put(LED_GREEN, 0);

    // Inicializa ADC para leitura do Joystick
    adc_init();
    adc_gpio_init(JOYSTICK_X);
    adc_gpio_init(JOYSTICK_Y);

    // Inicializa PWM para LEDs RGB (Vermelho e Azul)
    gpio_set_function(LED_RED, GPIO_FUNC_PWM);
    gpio_set_function(LED_BLUE, GPIO_FUNC_PWM);

    uint slice_red = pwm_gpio_to_slice_num(LED_RED);
    uint slice_blue = pwm_gpio_to_slice_num(LED_BLUE);
    
    pwm_set_wrap(slice_red, 4096);
    pwm_set_wrap(slice_blue, 4096);
    
    pwm_set_enabled(slice_red, true);
    pwm_set_enabled(slice_blue, true);

    // Inicializa I2C para o display SSD1306
    i2c_init(i2c1, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    
    // Inicializa display
    ssd1306_init(&ssd, 128, 64, false, 0x3C, i2c1);
    ssd1306_config(&ssd);
    ssd1306_send_data(&ssd);
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);
}

// Debounce do botão (evita leituras falsas)
bool debounce_botao(uint gpio) {
    static uint32_t ultimo_tempo = 0;
    uint32_t tempo_atual = to_ms_since_boot(get_absolute_time());

    if (gpio_get(gpio) == 0 && (tempo_atual - ultimo_tempo) > 200) { // 200ms de debounce
        ultimo_tempo = tempo_atual;
        return true;
    }
    return false;
}

void gpio_irq_handler(uint gpio, uint32_t events){
    if(debounce_botao(BOTAO_A)){
        estado_leds_pwm = !estado_leds_pwm; // Alterna estado dos LEDs PWM
    }
    if(debounce_botao(BOTAO_JOYSTICK)){
        gpio_put(LED_GREEN, !gpio_get(LED_GREEN)); // Atualiza LED
    }
}

// Função para ler o joystick e atualizar os LEDs RGB
void atualizar_leds_rgb(){
    int red = 0;
    int blue = 0;

    adc_select_input(0);  // Seleciona canal do eixo Y
    uint16_t valor_y = adc_read();  // Lê ADC do eixo Y
    adc_select_input(1);  // Seleciona canal do eixo X
    uint16_t valor_x = adc_read();  // Lê ADC do eixo X

    if(valor_y > 2400){
        red = valor_y - 2400;
    }else if(valor_y < 1800){
        red = 1700 - valor_y;
    }

    if(valor_x > 2400){
        blue = valor_x - 2400;
    }else if(valor_x < 1800){
        blue = 1700 - valor_x;
    }

    pwm_set_gpio_level(LED_RED, red); // Atualiza LED Vermelho
    pwm_set_gpio_level(LED_BLUE, blue); // Atualiza LED Azul
}

// Posição inicial do quadrado
int pos_y = 30;
int pos_x = 62;
const int tamanho_quadrado = 8;

// Define limites do display (evita sair da tela)
const int limite_y_min = 0;
const int limite_y_max = 64 - tamanho_quadrado;
const int limite_x_min = 0;
const int limite_x_max = 128 - tamanho_quadrado;

void mover_quadrado(){
    // Lê valores do joystick
    adc_select_input(0); // Eixo Y
    uint16_t valor_y = adc_read();
    
    adc_select_input(1); // Eixo X
    uint16_t valor_x = adc_read();

    // Define deslocamento dinâmico baseado na inclinação do joystick
    int deslocamento_y = 0;
    int deslocamento_x = 0;

    // Eixo Y (Movimento horizontal)
    if (valor_y > 3000) {
        deslocamento_y = -2;  // Movimento rápido para direita
    } else if (valor_y > 2500) {
        deslocamento_y = -1;  // Movimento lento para direita
    } else if (valor_y < 1000) {
        deslocamento_y = 2; // Movimento rápido para esquerda
    } else if (valor_y < 1500) {
        deslocamento_y = 1; // Movimento lento para esquerda
    }

    // Eixo Y (Movimento vertical)
    if (valor_x > 3000) {
        deslocamento_x = 2;  // Movimento rápido para baixo
    } else if (valor_x > 2500) {
        deslocamento_x = 1;  // Movimento lento para baixo
    } else if (valor_x < 1000) {
        deslocamento_x = -2; // Movimento rápido para cima
    } else if (valor_x < 1500) {
        deslocamento_x = -1; // Movimento lento para cima
    }

    // Atualiza a posição do quadrado dentro dos limites da tela
    pos_y += deslocamento_y;  // Muda posição horizontal (eixo X)
    pos_x += deslocamento_x;  // Muda posição vertical (eixo Y)

    if (pos_y < limite_y_min) pos_y = limite_y_min;
    if (pos_y > limite_y_max) pos_y = limite_y_max;
    if (pos_x < limite_x_min) pos_x = limite_x_min;
    if (pos_x > limite_x_max) pos_x = limite_x_max;

    // Atualiza o display
    ssd1306_fill(&ssd, false); // Limpa a tela

    //Se a borda estiver ativada (quando o led verde estiver), desenha um retângulo ao redor da tela
    if (exibir_borda) {
    // Borda externa
    ssd1306_rect(&ssd, 0, 0, 128, 64, true, false);  // Borda externa mais espessa
    // Borda interna (espessura de 1)
    ssd1306_rect(&ssd, 1, 1, 128-2, 64-2, true, false); 
    // Borda ainda mais interna
    ssd1306_rect(&ssd, 2, 2, 128-4, 64-4, true, false);
    // Borda interna mais fina
    ssd1306_rect(&ssd, 3, 3, 128-6, 64-6, true, false);
    } else {
        ssd1306_rect(&ssd, 0, 0, 128, 64, true, false);
    }

    // Desenha o quadrado que se move
    ssd1306_rect(&ssd, pos_y, pos_x, tamanho_quadrado, tamanho_quadrado, true, true); 

    ssd1306_send_data(&ssd); // Envia para o display
}


int main(){
    inicializar_hardware();
    
    gpio_set_irq_enabled_with_callback(BOTAO_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    gpio_set_irq_enabled_with_callback(BOTAO_JOYSTICK, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    
    while(true){
        if(estado_leds_pwm){
            //conforme joystick
            atualizar_leds_rgb();
        }else{
            pwm_set_gpio_level(LED_RED, 0);  //Desliga LED Vermelho
            pwm_set_gpio_level(LED_BLUE, 0); //Desliga LED Azul
        }

        mover_quadrado();
        if(gpio_get(LED_GREEN)){
            //desenha borda
            exibir_borda = true;
        }else if(!gpio_get(LED_GREEN)){
            //desenha outra borda
            exibir_borda = false;
        }
        ssd1306_send_data(&ssd);
    }
}
