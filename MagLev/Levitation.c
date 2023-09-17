
#include "driverlib.h"
#include "device.h"
#include <Control/Application_Init.h>
#include <Control/Position_Control_Loop.h>
#include <Control/Current_Control_Loop.h>
#include <stdint.h>
#include <stdbool.h>



#define EPWM2_TIMER_TBPRD 5000U
#define EPWM2_CMPA 0U
#define EPWM2_CMPB 2500U




#define GPIO_PIN_EPWM1A 0
#define EPWM1A_PIN_CONFIG GPIO_0_EPWM1A


#define GPIO_PIN_EPWM1B 1
#define EPWM1B_PIN_CONFIG GPIO_1_EPWM1B


#define GPIO_PIN_EPWM2A 2
#define EPWM2A_PIN_CONFIG GPIO_2_EPWM2A


#define GPIO_PIN_EPWM2B 3
#define EPWM2B_PIN_CONFIG GPIO_3_EPWM2B


#define GPIO_PIN_EPWM3A 4
#define EPWM3A_PIN_CONFIG GPIO_4_EPWM3A


#define GPIO_PIN_EPWM3B 5
#define EPWM3B_PIN_CONFIG GPIO_5_EPWM3B


#define GPIO_PIN_EPWM4A 6
#define EPWM4A_PIN_CONFIG GPIO_6_EPWM4A


#define GPIO_PIN_EPWM4B 7
#define EPWM4B_PIN_CONFIG GPIO_7_EPWM4B

#define EX_ADC_RESOLUTION 12


float calibration = 3.3/4096;
float gap_setpoint = 0.006;
float Current_setpoint=0.0;
float Current_Voltage=0.0;
float Gap_Voltage=0.0;
float period = 0;
float current_error =0;
float gap_error=0;


void configureADC(uint32_t adcBase);
void initEPWM1();
void initEPWM2();
void initEPWM3();
void PinMux_init();
void initADCSOCA(void);
void initADCSOCB(void);
__interrupt void adcA1ISR(void);
__interrupt void adcB1ISR(void);



void main(void)

{

    SysCtl_selectClockOutSource(SYSCTL_CLOCKOUT_SYSCLK);

    Device_init();

    Device_initGPIO();



    Interrupt_initModule();


    Interrupt_initVectorTable();


    Interrupt_register(INT_ADCA1, &adcA1ISR);
    Interrupt_register(INT_ADCB1, &adcB1ISR);




    configureADC(ADCA_BASE);
    configureADC(ADCB_BASE);
    initEPWM1();
    initEPWM2();
    initEPWM3();
    initADCSOCA();
    initADCSOCB();
    PinMux_init();
    Application_Init();


    Interrupt_enable(INT_ADCA1);
    Interrupt_enable(INT_ADCB1);

    EINT;
    ERTM;


    EPWM_enableADCTrigger(EPWM1_BASE, EPWM_SOC_A);
    EPWM_setTimeBaseCounterMode(EPWM1_BASE, EPWM_COUNTER_MODE_UP);
    EPWM_enableADCTrigger(EPWM3_BASE, EPWM_SOC_A);
    EPWM_setTimeBaseCounterMode(EPWM3_BASE, EPWM_COUNTER_MODE_UP);


    do
    {

    }
    while (1);
}


void configureADC(uint32_t adcBase)
{

    ADC_setPrescaler(adcBase, ADC_CLK_DIV_4_0);


#if (EX_ADC_RESOLUTION == 12)
    ADC_setMode(adcBase, ADC_RESOLUTION_12BIT, ADC_MODE_SINGLE_ENDED);
#elif (EX_ADC_RESOLUTION == 16)
    ADC_setMode(adcBase, ADC_RESOLUTION_16BIT, ADC_MODE_DIFFERENTIAL);
#endif


    ADC_setInterruptPulseMode(adcBase, ADC_PULSE_END_OF_CONV);

    ADC_enableConverter(adcBase);


    DEVICE_DELAY_US(1000);
}


void initADCSOCA(void)
{
    uint16_t acqps;

    if (EX_ADC_RESOLUTION == 12)
    {
        acqps = 14;
    }
    else
    {
        acqps = 63;
    }


    ADC_setupSOC(ADCA_BASE, ADC_SOC_NUMBER1, ADC_TRIGGER_EPWM1_SOCA,
                 ADC_CH_ADCIN1, acqps);
    ADC_setupSOC(ADCA_BASE, ADC_SOC_NUMBER2, ADC_TRIGGER_EPWM1_SOCA,
                 ADC_CH_ADCIN2, acqps);

    ADC_setInterruptSource(ADCA_BASE, ADC_INT_NUMBER1, ADC_SOC_NUMBER2);
    ADC_enableInterrupt(ADCA_BASE, ADC_INT_NUMBER1);
    ADC_clearInterruptStatus(ADCA_BASE, ADC_INT_NUMBER1);

}

void initADCSOCB(void)
{
    uint16_t acqps;

    if (EX_ADC_RESOLUTION == 12)
    {
        acqps = 14;
    }
    else
    {
        acqps = 63;
    }


    ADC_setupSOC(ADCB_BASE, ADC_SOC_NUMBER1, ADC_TRIGGER_EPWM3_SOCA,ADC_CH_ADCIN1, acqps);
    ADC_setupSOC(ADCB_BASE, ADC_SOC_NUMBER2, ADC_TRIGGER_EPWM3_SOCA,ADC_CH_ADCIN2, acqps);

    ADC_setInterruptSource(ADCB_BASE, ADC_INT_NUMBER1, ADC_SOC_NUMBER2);
    ADC_enableInterrupt(ADCB_BASE, ADC_INT_NUMBER1);
    ADC_clearInterruptStatus(ADCB_BASE, ADC_INT_NUMBER1);



}

__interrupt void adcA1ISR(void)
{

    ADC_clearInterruptStatus(ADCA_BASE, ADC_INT_NUMBER1);
    if (true == ADC_getInterruptOverflowStatus(ADCA_BASE, ADC_INT_NUMBER1))
    {
        ADC_clearInterruptOverflowStatus(ADCA_BASE, ADC_INT_NUMBER1);
        ADC_clearInterruptStatus(ADCA_BASE, ADC_INT_NUMBER1);
    }
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);
    Gap_Voltage = ADC_readResult(ADCARESULT_BASE, ADC_SOC_NUMBER2)*calibration;
    Current_setpoint = Position_Control_Loop(Gap_Voltage,gap_setpoint);

}
__interrupt void adcB1ISR(void)
{

    ADC_clearInterruptStatus(ADCB_BASE, ADC_INT_NUMBER1);
    if (true == ADC_getInterruptOverflowStatus(ADCB_BASE, ADC_INT_NUMBER1))
    {
        ADC_clearInterruptOverflowStatus(ADCB_BASE, ADC_INT_NUMBER1);
        ADC_clearInterruptStatus(ADCB_BASE, ADC_INT_NUMBER1);
    }
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);
    Current_Voltage = ADC_readResult(ADCBRESULT_BASE, ADC_SOC_NUMBER2)*calibration;
    current_error= -Current_Voltage*14.705+Current_setpoint;
    period = Current_Control_Loop(Current_Voltage,Current_setpoint);

}
void initEPWM1(void)
{
    EPWM_disableADCTrigger(EPWM1_BASE, EPWM_SOC_A);
    EPWM_setADCTriggerSource(EPWM1_BASE, EPWM_SOC_A, EPWM_SOC_TBCTR_U_CMPA);
    EPWM_setADCTriggerEventPrescale(EPWM1_BASE, EPWM_SOC_A, 1);

    EPWM_setCounterCompareValue(EPWM1_BASE, EPWM_COUNTER_COMPARE_A, 10000);
    EPWM_setTimeBasePeriod(EPWM1_BASE, 20000);
    EPWM_setClockPrescaler(EPWM1_BASE, EPWM_CLOCK_DIVIDER_1,EPWM_HSCLOCK_DIVIDER_1);

    EPWM_setTimeBaseCounterMode(EPWM1_BASE, EPWM_COUNTER_MODE_STOP_FREEZE);
}
void initEPWM3(void)
{
    EPWM_disableADCTrigger(EPWM3_BASE, EPWM_SOC_A);
    EPWM_setADCTriggerSource(EPWM3_BASE, EPWM_SOC_A, EPWM_SOC_TBCTR_U_CMPA);
    EPWM_setADCTriggerEventPrescale(EPWM3_BASE, EPWM_SOC_A, 1);

    EPWM_setCounterCompareValue(EPWM3_BASE, EPWM_COUNTER_COMPARE_A, 2500);
    EPWM_setTimeBasePeriod(EPWM3_BASE, 5000);
    EPWM_setClockPrescaler(EPWM3_BASE, EPWM_CLOCK_DIVIDER_1,EPWM_HSCLOCK_DIVIDER_1);

    EPWM_setTimeBaseCounterMode(EPWM3_BASE, EPWM_COUNTER_MODE_STOP_FREEZE);
}
void initEPWM2()
{

    EPWM_setTimeBasePeriod(EPWM2_BASE, EPWM2_TIMER_TBPRD);
    EPWM_setPhaseShift(EPWM2_BASE, 0U);
    EPWM_setTimeBaseCounter(EPWM2_BASE, 0U);
    EPWM_setCounterCompareValue(EPWM2_BASE, EPWM_COUNTER_COMPARE_A,
    EPWM2_CMPA);
    EPWM_setCounterCompareValue(EPWM2_BASE, EPWM_COUNTER_COMPARE_B,
    EPWM2_CMPB);


    EPWM_setTimeBaseCounterMode(EPWM2_BASE, EPWM_COUNTER_MODE_UP);
    EPWM_disablePhaseShiftLoad(EPWM2_BASE);
    EPWM_setClockPrescaler(EPWM2_BASE, EPWM_CLOCK_DIVIDER_1,
                           EPWM_HSCLOCK_DIVIDER_1);


    EPWM_setCounterCompareShadowLoadMode(EPWM2_BASE, EPWM_COUNTER_COMPARE_A,
                                         EPWM_COMP_LOAD_ON_CNTR_ZERO);
    EPWM_setCounterCompareShadowLoadMode(EPWM2_BASE, EPWM_COUNTER_COMPARE_B,
                                         EPWM_COMP_LOAD_ON_CNTR_ZERO);

    EPWM_setActionQualifierAction(EPWM2_BASE, EPWM_AQ_OUTPUT_A,
                                  EPWM_AQ_OUTPUT_HIGH,
                                  EPWM_AQ_OUTPUT_ON_TIMEBASE_ZERO);
    EPWM_setActionQualifierAction(EPWM2_BASE, EPWM_AQ_OUTPUT_A,
                                  EPWM_AQ_OUTPUT_LOW,
                                  EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPB);
    EPWM_setActionQualifierAction(EPWM2_BASE, EPWM_AQ_OUTPUT_B,
                                  EPWM_AQ_OUTPUT_LOW,
                                  EPWM_AQ_OUTPUT_ON_TIMEBASE_ZERO);
    EPWM_setActionQualifierAction(EPWM2_BASE, EPWM_AQ_OUTPUT_B,
                                  EPWM_AQ_OUTPUT_HIGH,
                                  EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPB);
    EPWM_setDeadBandDelayMode(EPWM2_BASE, EPWM_DB_RED, true);
    EPWM_setDeadBandDelayMode(EPWM2_BASE, EPWM_DB_FED, true);
    EPWM_setDeadBandDelayPolarity(EPWM2_BASE, EPWM_DB_RED,
                                  EPWM_DB_POLARITY_ACTIVE_LOW);
    EPWM_setDeadBandDelayPolarity(EPWM2_BASE, EPWM_DB_FED,
                                  EPWM_DB_POLARITY_ACTIVE_HIGH);
    EPWM_setRisingEdgeDeadBandDelayInput(EPWM2_BASE, EPWM_DB_INPUT_EPWMA);
    EPWM_setFallingEdgeDeadBandDelayInput(EPWM2_BASE, EPWM_DB_INPUT_EPWMA);
    EPWM_setDeadBandCounterClock(EPWM2_BASE, EPWM_DB_COUNTER_CLOCK_FULL_CYCLE);
    EPWM_setRisingEdgeDelayCount(EPWM2_BASE, 100);
    EPWM_setFallingEdgeDelayCount(EPWM2_BASE, 100);
}

void PinMux_init()
{

    GPIO_setPinConfig(EPWM1A_PIN_CONFIG);
    GPIO_setPadConfig(GPIO_PIN_EPWM1A, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(GPIO_PIN_EPWM1A, GPIO_QUAL_SYNC);

    GPIO_setPinConfig(EPWM1B_PIN_CONFIG);
    GPIO_setPadConfig(GPIO_PIN_EPWM1B, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(GPIO_PIN_EPWM1B, GPIO_QUAL_SYNC);


    GPIO_setPinConfig(EPWM2A_PIN_CONFIG);
    GPIO_setPadConfig(GPIO_PIN_EPWM2A, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(GPIO_PIN_EPWM2A, GPIO_QUAL_SYNC);

    GPIO_setPinConfig(EPWM2B_PIN_CONFIG);
    GPIO_setPadConfig(GPIO_PIN_EPWM2B, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(GPIO_PIN_EPWM2B, GPIO_QUAL_SYNC);


    GPIO_setPinConfig(EPWM3A_PIN_CONFIG);
    GPIO_setPadConfig(GPIO_PIN_EPWM3A, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(GPIO_PIN_EPWM3A, GPIO_QUAL_SYNC);

    GPIO_setPinConfig(EPWM3B_PIN_CONFIG);
    GPIO_setPadConfig(GPIO_PIN_EPWM3B, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(GPIO_PIN_EPWM3B, GPIO_QUAL_SYNC);


    GPIO_setPinConfig(EPWM4A_PIN_CONFIG);
    GPIO_setPadConfig(GPIO_PIN_EPWM4A, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(GPIO_PIN_EPWM4A, GPIO_QUAL_SYNC);

    GPIO_setPinConfig(EPWM4B_PIN_CONFIG);
    GPIO_setPadConfig(GPIO_PIN_EPWM4B, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(GPIO_PIN_EPWM4B, GPIO_QUAL_SYNC);
}


