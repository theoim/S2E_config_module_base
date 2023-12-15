
// include libraries
#include "lib/ssd1306.h"
#include "w7500x.h"
#include "lib/i2c.h"
#include "main.h"
#include "w7500x_gpio.h"
#include "w7500x_uart.h"

/**
 * @desc    Main function
 *
 * @param   void
 *
 * @return  int
 */
#define MAX_STRING_LENGTH 20  // 각 문자열의 최대 길이
/* Private variables ---------------------------------------------------------*/
I2C_ConfigStruct conf;

const char strings[][MAX_STRING_LENGTH] = {
    "1. Network Setting",
    "2. Serial Setting",
    "3. Options Setting"
};

static void GPIO_Config(void);
static void UART_Config(void);
int get_button1();
int get_button2();
int get_button3();
void Delay(__IO uint32_t nTime);
int line_position_select_function(int line_position);
void screen_update_function(const char (*strings)[MAX_STRING_LENGTH],  int num_strings, int line_position);
int display_cnt = 1;
int high_line_position = 4;

int main(void)
{
  	SystemInit();
    UART_Config();
    GPIO_Config();

//GPIO setting for I2C

	conf.scl_port = PORT_PA;
	conf.scl_pin = GPIO_Pin_9;
    conf.sda_port = PORT_PA;
	conf.sda_pin = GPIO_Pin_10;
    uint8_t addr = SSD1306_ADDR;

    // init ssd1306
    SSD1306_Init (&conf, addr);

     while (1){

        if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0)){
            display_cnt++;
            if(display_cnt >= 3) display_cnt = 0;
            high_line_position = line_position_select_function(display_cnt);
        }
        else if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1)){
            display_cnt--;   
            if(display_cnt < 0) display_cnt = 2;
            high_line_position = line_position_select_function(display_cnt);
        }
        // else if(get_button3()){

        // }

    screen_update_function( strings, sizeof(strings) / sizeof(strings[0]), high_line_position);
     // update
    SSD1306_UpdateScreen (&conf, addr);
    }
  // return value
  return 0;
}

void screen_update_function(const char (*strings)[MAX_STRING_LENGTH],  int num_strings, int line_position) {
        // clear screen
    SSD1306_ClearScreen ();
            // draw line
    SSD1306_DrawLine (0, MAX_X, line_position, line_position);  
    SSD1306_DrawLine (0, MAX_X, line_position + 14, line_position + 14);

    for (int i = 0; i < num_strings; i++) {
        SSD1306_SetPosition(2, (i*2)+1);
        SSD1306_DrawString(strings[i]);
    }
}

int line_position_select_function(int line_position){
    
    if(line_position == 0){
        return 4;
    } 
    else if(line_position == 1){
        return 21;
    } 
    else if(line_position == 2){
        return 38;
    } 
}

static void GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Direction = GPIO_Direction_IN;
    GPIO_InitStructure.GPIO_Pad = GPIO_InputBufferEnable | GPIO_CMOS | GPIO_PuPd_DOWN;
    GPIO_InitStructure.GPIO_AF = PAD_AF1;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

}

static void UART_Config(void)
{
    UART_InitTypeDef UART_InitStructure;

    UART_StructInit(&UART_InitStructure);
	
#if defined (USE_WIZWIKI_W7500_EVAL)
    UART_Init(UART1, &UART_InitStructure);
    UART_Cmd(UART1, ENABLE);
#else
    S_UART_Init(115200);
    S_UART_Cmd(ENABLE);
#endif
}


int get_button1()
{
    static unsigned char prev_state = 0; // 함수를 빠져 나왔다 다시 진입 해도
                                         // 전역 변수 처럼 변화된 값 그대로 유지

    unsigned char current_state; // 현재의 버튼 상태

    current_state = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0);

    // 처음 누른 상태
    if (prev_state == 0 && current_state != 0)
    {
        // Delay(60);  // noise가 지나 가기를 기다림
        prev_state = 1; // 처음 누른 상태가 아님을 가리키는 의미에서
                        // prev_state를 1로 set
        return 1;      // 아직 진짜로 눌러진 상태가 아니다.
                        // 지금은 노이즈 상태 이다.
    }
    // 버튼을 눌렀다 뗀 상태 이다.
    else if (prev_state == 1 && current_state == 0)
    {
        prev_state = 0;
        return 0; // 완전히 스위치를 눌렀다 뗀것으로 인정
    }

    return 0; // 아직 완전히 S/W를 울렀다 떼지 않은 상태 이다.
}

int get_button2()
{
    static unsigned char prev_state = 0; // 함수를 빠져 나왔다 다시 진입 해도
                                         // 전역 변수 처럼 변화된 값 그대로 유지

    unsigned char current_state; // 현재의 버튼 상태

    current_state = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1);

    // 처음 누른 상태
    if (prev_state == 0 && current_state != 0)
    {
        // Delay(60);  // noise가 지나 가기를 기다림
        prev_state = 1; // 처음 누른 상태가 아님을 가리키는 의미에서
                        // prev_state를 1로 set
        return 1;      // 아직 진짜로 눌러진 상태가 아니다.
                        // 지금은 노이즈 상태 이다.
    }
    // 버튼을 눌렀다 뗀 상태 이다.
    else if (prev_state == 1 && current_state == 0)
    {
        prev_state = 0;
        return 0; // 완전히 스위치를 눌렀다 뗀것으로 인정
    }

    return 0; // 아직 완전히 S/W를 울렀다 떼지 않은 상태 이다.
}

int get_button3()
{
    static unsigned char prev_state = 0; // 함수를 빠져 나왔다 다시 진입 해도
                                         // 전역 변수 처럼 변화된 값 그대로 유지

    unsigned char current_state; // 현재의 버튼 상태

    current_state = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_14);

    // 처음 누른 상태
    if (prev_state == 0 && current_state != 0)
    {
        Delay(60);  // noise가 지나 가기를 기다림
        prev_state = 1; // 처음 누른 상태가 아님을 가리키는 의미에서
                        // prev_state를 1로 set
        return 1;      // 아직 진짜로 눌러진 상태가 아니다.
                        // 지금은 노이즈 상태 이다.
    }
    // 버튼을 눌렀다 뗀 상태 이다.
    else if (prev_state == 1 && current_state == 0)
    {
        prev_state = 0;
        return 0; // 완전히 스위치를 눌렀다 뗀것으로 인정
    }

    return 0; // 아직 완전히 S/W를 울렀다 떼지 않은 상태 이다.
}

void Delay(__IO uint32_t nTime)
{
    while (nTime != 0) {
        nTime--;
    }
}

