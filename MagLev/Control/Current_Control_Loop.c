
#include <Control/Current_Control_Loop.h>
#include <Control/Position_Control_Loop.h>
#include "stdint.h"
#include "PID_controller.h"
#include "Filter.h"
#include "driverlib.h"
#include "device.h"



float Current_Control_Loop(float Current_Voltage , float Current_setpoint)
{
    float Max_Voltage = 25.0f;
    uint32_t period=0;
    float Current =  Current_Voltage*14.705f;
    float Current_error = Current_setpoint - Current;
    float Current_PID_out = PID_Control(&PID_Current_Control, Current_error);
//    if(Current_PID_out<0)
//    {
//        Current_PID_out=0;
//    }
    Current_PID_out = Current_PID_out/Max_Voltage;
    period = (uint32_t)(2500+Current_PID_out*2500);
    EPWM_setCounterCompareValue(EPWM2_BASE, EPWM_COUNTER_COMPARE_B,period);
    return period;
}






