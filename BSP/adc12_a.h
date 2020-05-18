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
//*****************************************************************************
//
// adc12_a.h - Driver for the ADC12_A Module.
//
//*****************************************************************************

#ifndef __ADC12_A_H__
#define __ADC12_A_H__

#define BOARDV_POWER_BIT           BIT0 //P2.0
#define BOARDV_POWER_FUNC_IO()     P2SEL &=~ BOARDV_POWER_BIT
#define BOARDV_POWER_OUT()         P2DIR |= BOARDV_POWER_BIT
#define BOARDV_POWER_ON()          P2OUT |= BOARDV_POWER_BIT
#define BOARDV_POWER_OFF()         P2OUT &=~ BOARDV_POWER_BIT

#define WIND_POWER_BIT           BIT2 //P2.2
#define WIND_POWER_FUNC_IO()     P2SEL &=~ WIND_POWER_BIT
#define WIND_POWER_OUT()         P2DIR |=  WIND_POWER_BIT
#define WIND_POWER_ON()          P2OUT |=  WIND_POWER_BIT
#define WIND_POWER_OFF()         P2OUT &=~ WIND_POWER_BIT

void ADC_Power_ON(void);
void ADC_Power_OFF(void);
void Init_ADC(void);
U8   Check_ADC(U16 *date);
U8 Start_ADC(U8 channel);

void ADC_Wind_Power_ON(void);
void ADC_Wind_Power_OFF(void);
void Init_Wind_ADC(void);

#endif
