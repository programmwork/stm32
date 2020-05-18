/* --COPYRIGHT--,BSD
 * Copyright (c) 2016, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/
//******************************************************************************
//!
//!   ADC12_A_A - Sample A0 Input, AVcc Ref, LED ON if A0 > 0.5*AVcc
//!   MSP430F552x Demo
//!
//!   Description: A single sample is made on A0 with reference to AVcc.
//!   Software sets ADC12SC to start sample and conversion - ADC12SC
//!   automatically cleared at EOC. ADC12_A_A internal oscillator times sample (16x)
//!   and conversion. In Mainloop MSP430 waits in LPM0 to save power until ADC12_A_A
//!   conversion complete, ADC12_A_ISR will force exit from LPM0 in Mainloop on
//!   reti. If A0 > 0.5*AVcc, P1.0 set, else reset.
//!
//!                 MSP430F552x
//!             -----------------
//!         /|\|                 |
//!          | |                 |
//!          --|RST       P6.0/A0|<- Vin
//!            |             P1.0|--> LED
//!            |                 |
//!
//!
//! This example uses the following peripherals and I/O signals.  You must
//! review these and change as needed for your own board:
//! - ADC12_A_A peripheral
//! - GPIO Port peripheral
//! - A0
//!
//! This example uses the following interrupt handlers.  To use this example
//! in your own application you must add these interrupt handlers to your
//! vector table.
//! - ADC12_A_VECTOR
//!
//******************************************************************************

#include "sysinit.h"
#include "main.h"
//#include "driverlib.h"

uint16_t adc_result = 0, adc_getResult_flag = 0;

void Init_ADC(void)
{
    
}
void ADC_Power_ON(void)
{
/*   BOARDV_POWER_FUNC_IO();
    BOARDV_POWER_OUT();
    BOARDV_POWER_ON();
    delay_ms(100);*/
}
void ADC_Power_OFF(void)
{
//    BOARDV_POWER_OFF();
}
uint8_t Check_ADC(uint16_t *date)
{
    uint16_t i = 0;

    i = 1000;
    while(i--)
    {
        if(adc_getResult_flag == 1)
        {
            *date = adc_result;
            break;
        }
    }

    adc_getResult_flag = 0;

    if(i > 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void Start_ADC(uint8_t channel)
{

	
	
}
//----------------------------------------------------------
//
//
//-----------------------------------------------------------
void Init_Wind_ADC(void)
{
    
}
void ADC_Wind_Power_ON(void)
{
    
}
void ADC_Wind_Power_OFF(void)
{
    
}



