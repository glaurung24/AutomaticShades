/* Projeto: leitor de lux para dois ambientes distintos (escritório e oficina eletrônica
* Este projeto considera os valores de lunosidade da NBR5413.
*
* Links importantes:
* - Link para biblioteca do sensor de Lux BJ1750: https://github.com/claws/BH1750
*/

// Project taken from: https://create.arduino.cc/projecthub/franciscomoreirapcb/light-meter-with-arduino-73b5b2

#include <LiquidCrystal.h>
#include <Wire.h>
#include <BH1750.h>

/* Definições gerais */
#define NUMERO_MEDICOES_SENSOR_LUX 5

/* Definições - teclas */
#define CANAL_ADC_TECLAS 0
#define TECLA_NENHUMA 0
#define TECLA_LEFT 1
#define TECLA_RIGHT 2
#define TEMPO_DEBOUNCE 200 //ms

/* Definições - display */
#define DISPLAY_NUM_COLUNAS 16
#define DISPLAY_NUM_LINHAS 2
#define TEMPO_MIN_ENTRE_MEDIDAS_SENSOR_LUX 150 //ms
#define DISPLAY_LINHA_EM_BRANCO " "

/* Definições - limites de luminosidade */
#define ESCRITORIO_LIMITE_SUPERIOR 1000 //lux
#define ESCRITORIO_LIMITE_INFERIOR 500 //lux
#define OFICINA_ELETRONICA_LIMITE_SUPERIOR 5000 //lux
#define OFICINA_ELETRONICA_LIMITE_INFERIOR 2000 //lux

/* Objetos e variáveis globais */
BH1750 medidor_lux;
uint16_t lux_medido;
char tecla_lida_primeira_vez = TECLA_NENHUMA;
char tecla_lida_segunda_vez = TECLA_NENHUMA;
char ultima_tecla_lida = TECLA_NENHUMA;
char lcd_linha2[16] = {0};

/* Inicializa objeto do display LCD com
os GPIOs utilizados pelo shield para tal fim */
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

/* Protótipos de funções */
char leitura_teclado(void);
int media_leituras_sensor_lux(void);
void formata_linha_e_classifica(char * linha_lcd, uint16_t lux_medido, uint16_t limite_superior, uint16_t limite_inferior);

/* Função: le teclas right e left
* Parâmetros: nenhum
* Retorno: tecla lida
*/
char leitura_teclado(void)
{
int leitura_adc = 0;
char tecla_lida = TECLA_NENHUMA;

/* A leitura do teclado no shield LCD + Keypad é feito com base em leitura
analógica. Ou seja, cada tecla pressionada vai provocar um nível de
tensão direfente no ADC (no canal definido por CANAL_ADC_TECLAS), permitindo
assim distinguir entre as teclas presentes.
Este projeto utiliza somente duas das 6 teclas presentes: left e right.
*/

leitura_adc = analogRead(CANAL_ADC_TECLAS);

if (leitura_adc < 50)
{
tecla_lida = TECLA_RIGHT;
}

else

{
if (leitura_adc < 650)
tecla_lida = TECLA_LEFT;
}

return tecla_lida;

}

/* Função: formata linha de LCD com medição (lux) e classificação
* Parâmetros: nenhum
* Retorno: tecla lida
*/

void formata_linha_e_classifica(char * linha_lcd, uint16_t lux_medido, uint16_t limite_superior, uint16_t limite_inferior)
{

memset(linha_lcd, ' ', DISPLAY_NUM_COLUNAS);

if ( (lux_medido >= limite_inferior) && (lux_medido <= limite_superior) )
sprintf(linha_lcd, "%d lx - OK", lux_medido);
else

{
if (lux_medido > limite_superior)
sprintf(linha_lcd, "%d lx - ALTO", lux_medido);
if (lux_medido < limite_inferior)
sprintf(linha_lcd, "%d lx - BAIXO", lux_medido);
}

}

int media_leituras_sensor_lux(void)
{

int i;
long int soma_lux = 0;
long int media = 0;
for (i=0; i<NUMERO_MEDICOES_SENSOR_LUX; i++)
{
soma_lux = soma_lux + medidor_lux.readLightLevel();
delay(TEMPO_MIN_ENTRE_MEDIDAS_SENSOR_LUX);
}

media = soma_lux / NUMERO_MEDICOES_SENSOR_LUX;
return (int)media;
}

void setup()
{

/* Configura e escreve mensagem inicial no display LCD */
lcd.begin(DISPLAY_NUM_COLUNAS, DISPLAY_NUM_LINHAS);
lcd.setCursor(0,0);
lcd.print(" Escolha o");
lcd.setCursor(0,1);
lcd.print("ambiente (L/R)");
/* Inicializa o BH1750 (medidor de lux) */

Wire.begin();

medidor_lux.begin();
}

void loop()
{

int media_lux;

/* Faz leitura do teclado com debounce */
tecla_lida_primeira_vez = leitura_teclado();

if (tecla_lida_primeira_vez != TECLA_NENHUMA)
{
/* confirma a leitura */
delay(TEMPO_DEBOUNCE);
tecla_lida_segunda_vez = leitura_teclado();

if (tecla_lida_primeira_vez == tecla_lida_segunda_vez)
{
/* Tecla confirmada */
ultima_tecla_lida = tecla_lida_primeira_vez;
lcd.clear();
}

}

/* Se houve alguma tecla lida, executa a ação correspondente */
switch (ultima_tecla_lida)
{
case TECLA_LEFT:
lcd.setCursor(0,0);
lcd.print("Escritorio");
media_lux = media_leituras_sensor_lux();
formata_linha_e_classifica(lcd_linha2,
media_lux,
ESCRITORIO_LIMITE_SUPERIOR,
ESCRITORIO_LIMITE_INFERIOR);
lcd.setCursor(0,1);
lcd.print(DISPLAY_LINHA_EM_BRANCO);
lcd.setCursor(0,1);
lcd.print(lcd_linha2);
break;
case TECLA_RIGHT:
lcd.setCursor(0,0);
lcd.print("Ofic. eletronica");
media_lux = media_leituras_sensor_lux();
formata_linha_e_classifica(lcd_linha2,
media_lux,
OFICINA_ELETRONICA_LIMITE_SUPERIOR,
OFICINA_ELETRONICA_LIMITE_INFERIOR);
lcd.setCursor(0,1);
lcd.print(DISPLAY_LINHA_EM_BRANCO);
lcd.setCursor(0,1);
lcd.print(lcd_linha2);
break;
default:
break;
}

}
