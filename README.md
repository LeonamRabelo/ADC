# 🎮 CAPÍTULO 8 - CONVERSOR ANALÓGICO/DIGITAL

Este projeto utiliza um **joystick analógico** para controlar:  
✅ **LEDs RGB (PWM) – Ajustando brilho com o eixo do joystick**  
✅ **Movimentação de um quadrado no display OLED SSD1306**  
✅ **Alternância de bordas no display com o botão do joystick**  

📌 **O botão "A" ativa/desativa o controle PWM dos LEDs RGB.**  
📌 **O botão do joystick alterna entre dois estilos de bordas no display.**  

---

## ⚙️ Configuração dos Pinos
| **Componente**   | **Pino Raspberry Pi Pico** |
|-----------------|---------------------------|
| **Joystick X (ADC0)**  | 26 |
| **Joystick Y (ADC1)**  | 27 |
| **Botão do Joystick**  | 22 |
| **Botão A**  | 5 |
| **LED Vermelho (PWM)**  | 13 |
| **LED Verde (Digital)** | 11 |
| **LED Azul (PWM)**  | 12 |
| **I2C SDA (Display SSD1306)**  | 14 |
| **I2C SCL (Display SSD1306)**  | 15 |

---

## 🚀 Como Executar
**Requisitos**
- SDK do Raspberry Pi Pico instalado e configurado
- VS Code com as extensões: Raspberry Pi Pico Project Wokwi Simulator CMake configurado

**Passos:**
Simulação no Wokwi (VS Code):
- Importe o projeto (pasta) utilizando a extensão Raspberry Pi Pico Project.
- Compile o projeto pela extensão. No Wokwi, abra o arquivo diagram.json e execute (Play).

Execução na Placa BitDogLab:
_ Compile o projeto para gerar o arquivo .uf2.
- Envie o arquivo .uf2 para a placa Raspberry Pi Pico.
---

## 🎮 Controles e Funcionalidade
### 🟢 LEDs RGB (Vermelho e Azul)
🎮 **Eixo X (Joystick Esquerda/Direita):** Controla brilho do **LED Azul**  
🎮 **Eixo Y (Joystick Cima/Baixo):** Controla brilho do **LED Vermelho**  
🟡 **Botão A:** Liga/desliga o controle PWM dos LEDs RGB  

### 🖥️ Display SSD1306
🎮 **Movimento do Joystick:** Controla um **quadrado no display**  
🔲 **Botão do Joystick:** Alterna entre **borda simples e borda grossa**  

---

## 📌 Funcionalidades Implementadas
✔ **Controle de LEDs RGB via PWM** 🎨  
✔ **Movimentação de quadrado no SSD1306 usando joystick** 🎮  
✔ **Debounce para os botões** ⏳  
✔ **Alternância de bordas do display** 🔲  

---

## 🎥 Demonstração do Projeto
🔗 **Vídeo de demonstração:** [[https://youtu.be/H-OLcvTeReM](https://youtu.be/H-OLcvTeReM)]  

---

## Autor
Desenvolvido por Leonam S. Rabelo
