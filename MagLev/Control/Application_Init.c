
#include <Control/Filter.h>
#include <Control/PID_controller.h>

float NOTCH_NUM_COEFF_0 = 0.9901f;
float NOTCH_NUM_COEFF_1 = -1.9801f;
float NOTCH_NUM_COEFF_2 = 0.9901f;
float NOTCH_DEN_COEFF_1 = -1.9801f;
float NOTCH_DEN_COEFF_2 = 0.9802f;

float BP_NUM_COEFF_0 = 1.009f;
float BP_NUM_COEFF_1 = -1.998f;
float BP_NUM_COEFF_2 = 0.989f;
float BP_DEN_COEFF_1 = -1.9980f;
float BP_DEN_COEFF_2 = 0.9980f;



float PID_POS_CONTROL_KP =2000.0f;
float PID_POS_CONTROL_KI = 1000.0f;
float PID_POS_CONTROL_KD = 50.0f;
float PID_POS_CONTROL_TS = 0.0002f;
float PID_POS_CONTROL_N = 1000.0f;
float PID_POS_CONTROL_ACC_LIMIT_P = 25.0f;
float PID_POS_CONTROL_ACC_LIMIT_N = -25.0f;




float PID_CUR_CONTROL_KP = 100.0f;
float PID_CUR_CONTROL_KI = 10.0f;
float PID_CUR_CONTROL_KD = 0.0f;
float PID_CUR_CONTROL_TS = 0.00005f;
float PID_CUR_CONTROL_N= 1000.0f;
float PID_CUR_CONTROL_ACC_LIMIT_P = 25.0f;
float PID_CUR_CONTROL_ACC_LIMIT_N = -25.0f;



void Application_Init(void)
{
    ASW_IIR_second_order_init(&Filter_Notch, NOTCH_NUM_COEFF_0,
                              NOTCH_NUM_COEFF_1, NOTCH_NUM_COEFF_2,
                              NOTCH_DEN_COEFF_1, NOTCH_DEN_COEFF_2);
    ASW_IIR_second_order_init(&Filter_BP, BP_NUM_COEFF_0, BP_NUM_COEFF_1,
                              BP_NUM_COEFF_2, BP_DEN_COEFF_1,
                              BP_DEN_COEFF_2);
    Initialise_PID(&PID_Position_Control, PID_POS_CONTROL_KP,
                   PID_POS_CONTROL_KI, PID_POS_CONTROL_KD,
                   PID_POS_CONTROL_N,
                   PID_POS_CONTROL_ACC_LIMIT_P, PID_POS_CONTROL_ACC_LIMIT_N,
                   PID_POS_CONTROL_TS);

    Initialise_PID(&PID_Current_Control, PID_CUR_CONTROL_KP,
                   PID_CUR_CONTROL_KI, PID_CUR_CONTROL_KD,
                   PID_CUR_CONTROL_N,
                   PID_CUR_CONTROL_ACC_LIMIT_P, PID_CUR_CONTROL_ACC_LIMIT_N,
                   PID_CUR_CONTROL_TS);

}

