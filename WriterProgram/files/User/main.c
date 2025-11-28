/////////////自動追加1　ここから/////////////////
/////////////宣言 マクロ/プロトタイプ宣言等//////

#include <ch32v20x.h>
#include "ch32v203_mylib.h"

// マクロ宣言


// グローバル変数宣言


// プロトタイプ宣言





/////////////自動追加1 ここまで /////////////////

#define TH 150
#define POWER 255

DotMatrix_t dm_data;

void setup() {
}

void loop() {
  // LED Matrix Pattern
  dm_data.green_data[0]= 109;
  dm_data.green_data[1]= 64;
  dm_data.green_data[2]= 50;
  dm_data.green_data[3]= 187;
  dm_data.green_data[4]= 235;
  dm_data.green_data[5]= 53;
  dm_data.green_data[6]= 49;
  dm_data.green_data[7]= 236;

}

int main (void) {
    Clock_init();

    SysTick_init();

    pinSetting();
    TIM2_InterruptSetting();

    ADC_Setting();

    UART_Setting();

    ToneSetting();

    delay(100);

    setup();

    while (1) {
        loop();
    }
}

////////////自動追加2 ここから//////////////////
////////////関数等/////////////////////////////



////////////自動追加2 ここまで/////////////////
