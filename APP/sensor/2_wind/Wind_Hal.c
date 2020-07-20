/*
;************************************************************************************************************
;*--------------------------------------------- 闂佸搫鍊稿ú锝呪枎閵忥紕鈹嶉柍鈺佸暕缁憋拷 ----------------------------------------------------
;*
;* 闂佸搫鍊稿ú锝呪枎閵忋倕瑙︾�广儱娉﹂敓锟�? :
;* 闂佸搫鍊稿ú锝呪枎閵忋倕绀夐柣鏃囶嚙閸橈拷 :
;*
;* 闁荤偞绋忛崕閬嶅储閺嶎偅瀚氶悗娑櫳戦～锟�? :
;*-----54--------------------------------------- 闂佸搫鐗為幏鐑芥煛閸屾稑顥嬪褎顨婂鐢割敄婢跺摜绠氶梺璇�?�弿閹凤�? -------------------------------------------------
;* 婵烇絽娴傞崰妤呭极閸忓吋濯存繝濠傛４閹风兘鏁撻敓锟� :闂佺粯绮岄鍛磽閹惧瓨濯撮柨鐕傛嫹
;* 婵烇絽娴傞崰妤呭极婵傜绫嶉柕澶涢檮閸╋�? :2015-5-15
;* 闂佺粯顨呴悧濠傦耿閻�?牏鐝堕柣妤�鐗婇～锟�? :婵＄偛顑呮鎼佸箖濠婂嫮鈻旈幖鎼厛濡垿姊洪銈呮瀻闁搞劌楠搁锝夋晸閿燂�?
;*-------------------------------------------- 闂佸憡锚濡銇愰崼鏇熷亱闁割偆鍠愰幏鍗炃庨崶锝呭⒉濞寸》鎷�? -------------------------------------------------
;* 闂佸搫鍊稿ú锝呪枎閵忊剝濯存繝濠傛４閹风兘鏁撻敓锟�? : liujunjie_2008@126.com
;* 闂佸憡甯楃粙鎴犵磽閹捐绫嶉柕澶涢檮閸╋�? :
;* 闂佺粯顨呴悧濠傦耿閻�?牏鐝堕柣妤�鐗婇～锟�? :
;************************************************************************************************************
*/
#include "sensor_basic.h"

static volatile unsigned int IntCounter_1s;								// 闂佺儵鍋撻崝瀣姳椤掑偊鎷风憴鍕暡婵＄偛鍊垮畷鎶藉Ω椤垵鎮�?梺杞扮筏閹凤�?
static volatile unsigned int EdgeCounter;
static volatile unsigned int EdgeNumber1S;
volatile unsigned char Flag_FengxiangUpdate;
static volatile unsigned char Flag_FengsuUpdate;

char str_name[10];

static volatile unsigned int ADValueSum;
unsigned short T3IntCounter=0;
TIM_HandleTypeDef htim1;



/*********************************************************************************
** 闂佸憡鍨兼慨銈夊汲閻旂厧瑙︾�广儱娉﹂敓锟� 闂佹寧绋掗鐥璼igned char Sensor_Init(void)
** 闂佸憡鍨兼慨銈夊汲閻旂厧�?夐柣鏃囶嚙閸橈拷 闂佹寧鍐婚幏锟�
** 闂佺绻堥崕杈亹濞戙垹鐭楅柛灞剧♁濞堬�? 闂佹寧鍐婚幏锟�
**
** 闂佸憡鍨甸幖顐ャ亹濞戙垹鐭楅柛灞剧♁濞堬�? 闂佹寧鍐婚幏锟�
*********************************************************************************/
void Sensor_Init(void)
{
    unsigned char i = 0, data = 0;
    GPIO_InitTypeDef GPIO_InitStruct;
    EdgeCounter = 0;
    // P1.7闂佹寧绋戦懟顖炈囨繝姘劸闁愁偅�?犻梺鍛婄懕缁插ジ鎳欓幋锕�妫�?柨鐕傛嫹
    /*Configure GPIO pin : PA4 */
    GPIO_InitStruct.Pin = GPIO_PIN_4;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* EXTI interrupt init*/
    HAL_NVIC_SetPriority(EXTI4_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(EXTI4_IRQn);

    Init_Timer1(250);

    /*Four way selector*/ 
    //PC6 7 8 9
    GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
        
        
    /*interface port*/
    //PB0 1 2 5 6 7 10 11
    GPIO_InitStruct.Pin = GPIO_PIN_0  |GPIO_PIN_1 | GPIO_PIN_2 |GPIO_PIN_5|
                          GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_10 | GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /*Output of wind direction control terminal*/
    //PC10 PC11
    GPIO_InitStruct.Pin = GPIO_PIN_10 | GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    //Pin10 Pin11 setting low 
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10,GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11,GPIO_PIN_RESET);

}

void Init_Timer1(unsigned short delayMS)
{

    TIM_ClockConfigTypeDef sClockSourceConfig;
    TIM_MasterConfigTypeDef sMasterConfig;

    htim1.Instance = TIM1;//ʱ��Դ
    htim1.Init.Prescaler = 799 ;//��Ƶϵ��
    htim1.Init.CounterMode = TIM_COUNTERMODE_UP;//����ģʽ
    htim1.Init.Period = 20 * delayMS;//��װֵ
    htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim1.Init.RepetitionCounter = 0;
    htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
    {
        _Error_Handler(__FILE__, __LINE__);
    }

    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
    {
        _Error_Handler(__FILE__, __LINE__);
    }

    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
    {
        _Error_Handler(__FILE__, __LINE__);
    }
    HAL_TIM_Base_Start_IT(&htim1);

}


/*
********************************************************************************
** 闂佸憡鍨兼慨銈夊汲閻旂厧瑙︾�广儱娉﹂敓锟� 闂佹寧绋掗鐨乮d hardinit(void)
** 闂佸憡鍨兼慨銈夊汲閻旂厧�?夐柣鏃囶嚙閸橈拷 闂佹寧绋掗惌顔剧箔閸�?偞鍋ㄩ梻鍫熺☉閻忋儱鈹戦纰卞剰閻庡灚鐓″顕�骞嗛幍顔筋啀闂佹眹鍩勯悡澶屾濠靛鐭楁い蹇撴川濞堝爼鏌ｉ埀顒�濡虹紒鏃�娼欓埢搴ㄦ晸閿燂拷
** 闂佺绻堥崕杈亹濞戙垹鐭楅柛灞剧♁濞堬�? 闂佹寧鍐婚幏锟�
**
** 闂佸憡鍨甸幖顐ャ亹濞戙垹鐭楅柛灞剧♁濞堬�? 闂佹寧鍐婚幏锟�
********************************************************************************
*/ 
void hal_sensor_init()
{
    Sensor_Init();

    SampleData_Init(&sensors_data);
}

/**********************************************************************************************************
** 鍑芥暟鍚嶇�? 锛�
** 鍑芥暟鍔熻兘 锛氫覆鍙ｆ帴�?朵腑鏂湇鍔＄▼搴�
** 鍏ュ彛鍙傛暟 锛�
**
** 鍑哄彛鍙傛暟 锛�
**********************************************************************************************************/
void USART1_RX(void)
{

}

/*********************************************************************************
** 闂佸憡鍨兼慨銈夊汲閻旂厧瑙︾�广儱娉﹂敓锟� 闂佹寧鍐婚幏锟�
** 闂佸憡鍨兼慨銈夊汲閻旂厧�?夐柣鏃囶嚙閸橈拷 闂佹寧鍐婚幏锟�
** 闂佺绻堥崕杈亹濞戙垹鐭楅柛灞剧♁濞堬�? 闂佹寧鍐婚幏锟�
**
** 闂佸憡鍨甸幖顐ャ亹濞戙垹鐭楅柛灞剧♁濞堬�? 闂佹寧鍐婚幏锟�
*********************************************************************************/
void Reset_Sensor(void)
{
  Sensor_Init();
}

unsigned int GraytoDecimal(unsigned int x)
{
    unsigned int y = x;
    while(x>>=1)
    {
        y ^= x;
    }

    return y;
}

/*
********************************************************************************
** 闂佸憡鍨兼慨銈夊汲閻旂厧瑙︾�广儱娉﹂敓锟� 闂佹寧绋掗鐥璼igned char FengYu_engine(float *pFengSuYuLiang,float *pFengXiang)
** 闂佸憡鍨兼慨銈夊汲閻旂厧�?夐柣鏃囶嚙閸橈拷 闂佹寧鍐婚幏锟�
** 闂佺绻堥崕杈亹濞戙垹鐭楅柛灞剧♁濞堬�? 闂佹寧鍐婚幏锟�
**
** 闂佸憡鍨甸幖顐ャ亹濞戙垹鐭楅柛灞剧♁濞堬�? 闂佹寧鍐婚幏锟�
********************************************************************************
*/
unsigned char FengXiang_engine(float *pFengXiang)
{
    unsigned int value = 0;
    static float buffer = 0;
    unsigned char i = 0, data = 0;

    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11,GPIO_PIN_SET);

    i = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0);
    data |= (uint8_t)(i << 0);
    i = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1);
    data |= (uint8_t)(i << 1);
    i = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2);
    data |= (uint8_t)(i << 2);
    i = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5);
    data |= (uint8_t)(i << 3);

    i = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6);
    data |= (uint8_t)(i << 4);
    i = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_7);
    data |= (uint8_t)(i << 5);
    i = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_10);
    data |= (uint8_t)(i << 6);

    value = GraytoDecimal(data);
    buffer = value * 2.8125;
    *pFengXiang = round(buffer);

    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11,GPIO_PIN_RESET);
    return 1;
}

unsigned char Fengsu_engine(float *pFengSu)
{
    unsigned int value = 0;
    float value_f;

    if(Flag_FengsuUpdate == 1)
    {
        Flag_FengsuUpdate = 0;
        
        value = EdgeNumber1S;

        value_f = value;                                      // 闁哄鏅滈弻銊ッ洪弽顭戞Ч閹兼惌鐓夐幏鐑芥偄閻戞鏆犳俊顐ゅ閸ㄧ懓顔忔總绋跨９闁跨噦鎷�

        //    *pFengSu = value_f * (1000.0 / 249.856);
        *pFengSu = value_f * 4;

        return 1;
    }
    return 0;
}

/*
********************************************************************************
** 闂佸憡鍨兼慨銈夊汲閻旂厧瑙︾�广儱娉﹂敓锟� 闂佹寧鍐婚幏锟�
** 闂佸憡鍨兼慨銈夊汲閻旂厧�?夐柣鏃囶嚙閸橈拷 闂佹寧绋掗懝楣冩偩妤ｅ啫绫嶉悹铏瑰劋閻濐�?3婵炴垶鎼╅崢浠嬪蓟閸ヮ剙瀚夌�广儱鎳庨～銈囩磼鐎ｎ亶鍎忕紒顭掓嫹
** 闂佺绻堥崕杈亹濞戙垹鐭楅柛灞剧♁濞堬�? 闂佹寧鍐婚幏锟�
**
** 闂佸憡鍨甸幖顐ャ亹濞戙垹鐭楅柛灞剧♁濞堬�? 闂佹寧鍐婚幏锟�
********************************************************************************
*/ 

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    //T3IntCounter++;

    //if(T3IntCounter > 4)  //250ms
    {
        EdgeNumber1S = EdgeCounter;
        EdgeCounter = 0;
        Flag_FengsuUpdate = 1;
        //T3IntCounter = 0;
    }
}

/*
********************************************************************************
** 闂佸憡鍨兼慨銈夊汲閻旂厧瑙︾�广儱娉﹂敓锟� 闂佹寧鍐婚幏锟�
** 闂佸憡鍨兼慨銈夊汲閻旂厧�?夐柣鏃囶嚙閸橈拷 闂佹寧绋掗悺锟�1闂佸憡鐟辩徊濂告嚈閹达箑妫樻い鎾跺枑缁犳盯鏌涢弮鎾剁？闁宠鍚嬮幆鏃堟晸閿燂�?
** 闂佺绻堥崕杈亹濞戙垹鐭楅柛灞剧♁濞堬�? 闂佹寧鍐婚幏锟�
**
** 闂佸憡鍨甸幖顐ャ亹濞戙垹鐭楅柛灞剧♁濞堬�? 闂佹寧鍐婚幏锟�
********************************************************************************
*/ 
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)               // 婵犮垼鍩栭悧鐘诲磿鐎涙鈻旀い鎾跺枑閻掞�?
{
    unsigned char i;

    /* 缂備胶濯撮幏鐑芥煕濡や焦�?�妤犵偞鎹囬獮搴ㄥ冀瑜滃Σ閬嶆煟閻戝洦瀚� */
    for(i=0;i<10;i++)
    {
        if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4) == 0)                        // 濠碘槅鍋撻幏閿嬬箾閺夋埈鍎忛柛鈺佹湰缁嬪鏁撻挊澶嗘瀻闁炽儴灏欑粔鐢告煟閵忋倖娑ч梺瑙ｆ櫇娴滃憡娼忛顐㈡倠婵炴垶鎸鹃崕銈呂ｉ幖浣哥闁哄诞鍐冿拷
        {
           return;
        }
        //asm("NOP");
        //asm("NOP");
    }
    EdgeCounter++;

}

