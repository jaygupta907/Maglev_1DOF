
#include <Control/Current_Control_Loop.h>
#include <Control/Position_Control_Loop.h>
#include "stdint.h"
#include "PID_controller.h"
#include "Filter.h"
#include "driverlib.h"
#include "device.h"





float Position_Control_Loop(float Gap_voltage,float gap_setpoint)
{
    static uint32_t position_frame_count = 0;
    float Current_setpoint=0.0f;
    float Gap = 0.015f - 0.006*Gap_voltage ;
    float position_delay = 5000.0f;
    float initial_gap = 0.010f;


    position_frame_count++;

    gap_setpoint = initial_gap + ((gap_setpoint - initial_gap) * position_frame_count / position_delay);


    if (position_frame_count >= position_delay)
    {
        position_frame_count = position_delay;
    }
    float Gap_error = -gap_setpoint + Gap;

    Current_setpoint = PID_Control(&PID_Position_Control, Gap_error);
    return Current_setpoint;


}

