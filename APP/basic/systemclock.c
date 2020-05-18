#include "systemclock.h"

U16 status;
void Init_Clock()
{
	 //Set P11.0 to output direction
	    //P11.0 to output ACLK
	    //Select XT1
#if 0
	    GPIO_setAsPeripheralModuleFunctionOutputPin(
	        GPIO_PORT_P11,
	        GPIO_PIN0
	        );
#endif
	    //Port select XT1
	    GPIO_setAsPeripheralModuleFunctionInputPin(
	        GPIO_PORT_P7,
	        GPIO_PIN0 + GPIO_PIN1
	        );
	    //Port select XT2
        GPIO_setAsPeripheralModuleFunctionInputPin(
            GPIO_PORT_P5,
            GPIO_PIN2 + GPIO_PIN3
            );
	    //Initializes the XT1 crystal oscillator with no timeout
	    //In case of failure, code hangs here.
	    //For time-out instead of code hang use UCS_turnOnLFXT1WithTimeout()
	    UCS_turnOnLFXT1(
	        UCS_XT1_DRIVE_0,
	        UCS_XCAP_3
	        );
	    UCS_turnOnXT2( UCS_XT2_DRIVE_8MHZ_16MHZ );
	 // UCS_initFLLSettle( 8000 , 244 );
	      UCS_initFLLSettle( 4000 , 122 );

	    UCS_setExternalClockSource(32768,16000000);
	    //ACLK 32768 ,DCO 8M,SMCLK,MCLK=DCO=8M
	    UCS_initClockSignal(UCS_ACLK,UCS_XT1CLK_SELECT,UCS_CLOCK_DIVIDER_1);
	    UCS_initClockSignal(UCS_SMCLK,UCS_XT2CLK_SELECT,UCS_CLOCK_DIVIDER_2);
	    UCS_initClockSignal(UCS_MCLK,UCS_XT2CLK_SELECT,UCS_CLOCK_DIVIDER_2);



	    // Enable global oscillator fault flag
	      SFR_clearInterrupt(SFR_OSCILLATOR_FAULT_INTERRUPT);
	      SFR_enableInterrupt(SFR_OSCILLATOR_FAULT_INTERRUPT);
	      // Enable global interrupt
	          __bis_SR_register(GIE);
    return;
}


#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=UNMI_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(UNMI_VECTOR)))
#endif
void NMI_ISR(void)
{
    do
    {
        // If it still can't clear the oscillator fault flags after the timeout,
        // trap and wait here.
        status = UCS_clearAllOscFlagsWithTimeout(1000);
    }
    while(status != 0);
}

