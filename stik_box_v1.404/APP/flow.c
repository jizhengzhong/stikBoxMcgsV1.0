#include "appmange.h"
#include "main.h"
#include "cmsis_os.h"
#include "flow.h"
#include "math.h"
#include "task.h"
#include "rtc.h"

manager RunFlow;
void RellayCtrl(RelaySt State)
{
    State == RelayOFF ?   CFK_POWER_OFF : CFK_POWER_ON ;
}


void flow_control(manager *pFlowPcb)
{
    char i = 0;
    char *pType[6] = {"Steri start", "Steri start", "Melt start", "Steri start", "Steri start", "Preheat start"};

    switch (pFlowPcb->flow_state) {
    case WORK_PROGRAM_FLOW_STATE_HEAT1://工作流程1--加热1
        RunFlow.f0_value = 0;
					Lock_PowerOFF;
        if (Procedure.type == Melt) {
            Temperature.Target = Procedure.steri;
            RunFlow.CurveState = HeatState1;

            if (Temperature.Measure >= Procedure.steri - 10) {
                pFlowPcb->flow_state = WORK_PROGRAM_FLOW_STATE_HEAT2;
                pFlowPcb->state = FLOW_STATE_HEAT2_6;
                RunFlow.CurveState = HeatState2;
                PrintString("Heat2 start") ;
                printf("%d:%d:%d %4.1f%4.1dkpa\r\n", sTime.Hours, sTime.Minutes, \
                       sTime.Seconds, Temperature.Measure, RunFlow.Pressure);
            }
        } else if (Temperature.Measure >= ParTab[PAR_POS_TEMP_BOIL].uVal - 3) {
            pFlowPcb->flow_state = WORK_PROGRAM_FLOW_STATE_HEAT2;
            pFlowPcb->state = FLOW_STATE_HEAT2_1;
            PrintString("Heat2 start") ;
            printf("%d:%d:%d %4.1f%4.1dkpa\r\n", sTime.Hours, sTime.Minutes, \
                   sTime.Seconds, Temperature.Measure, RunFlow.Pressure);
        }

        RellayCtrl(RelayOFF);
        RunFlow.CurveState = HeatState1;
        Temperature.Target = Procedure.steri;
        break;

    case WORK_PROGRAM_FLOW_STATE_HEAT2://工作流程2--加热2
        switch (pFlowPcb->state) {
        case FLOW_STATE_HEAT2_1:
            if (Temperature.Measure >= ParTab[PAR_POS_TEMP_BOIL].uVal + 7) {
                Temperature.Target = 0.1;
                RellayCtrl(RelayOFF);
                pFlowPcb->state = FLOW_STATE_HEAT2_2;
                printf("%d:%d:%d %4.1f%4.1dkpa\r\n", sTime.Hours, sTime.Minutes, \
                       sTime.Seconds, Temperature.Measure, RunFlow.Pressure);
            }   else {
                RellayCtrl(RelayON);
            }

            break;

        case FLOW_STATE_HEAT2_2:
            if (Temperature.Measure  <= ParTab[PAR_POS_TEMP_BOIL].uVal) {
                Temperature.Target = Procedure.steri;
                RellayCtrl(RelayON);
                pFlowPcb->state = FLOW_STATE_HEAT2_3;
                printf("%d:%d:%d %4.1f%4.1dkpa\r\n", sTime.Hours, sTime.Minutes, \
                       sTime.Seconds, Temperature.Measure, RunFlow.Pressure);
            } else {
                RellayCtrl(RelayOFF);
            }

            break;

        case FLOW_STATE_HEAT2_3:
            if (Temperature.Measure >= ParTab[PAR_POS_TEMP_BOIL].uVal + 7) {
                Temperature.Target = 0.1;
                RellayCtrl(RelayOFF);
                pFlowPcb->state = FLOW_STATE_HEAT2_4;
                taskENTER_CRITICAL();
                printf("%d:%d:%d %4.1f%4.1dkpa\r\n", sTime.Hours, sTime.Minutes, \
                       sTime.Seconds, Temperature.Measure, RunFlow.Pressure);
                taskEXIT_CRITICAL();
            } else {
                RellayCtrl(RelayON);
            }

            break;

        case FLOW_STATE_HEAT2_4:
            if (Temperature.Measure <= ParTab[PAR_POS_TEMP_BOIL].uVal + 4) {
                Temperature.Target = Procedure.steri;
                RellayCtrl(RelayON);
                pFlowPcb->state = FLOW_STATE_HEAT2_5;
                RunFlow.BalanceFlag = 0;
                RunFlow.CurveState = HeatState2;
                printf("%d:%d:%d %4.1f%4.1dkpa\r\n", sTime.Hours, sTime.Minutes, \
                       sTime.Seconds, Temperature.Measure, RunFlow.Pressure);
            }    else {
                RellayCtrl(RelayOFF);
            }

            break;

        case FLOW_STATE_HEAT2_5:
            switch (RunFlow. BalanceFlag) {
            case 0 :
                if (Temperature.Measure - Temperature.Target > -1) {
                    RunFlow.SteriMinute  = 0;
                    RunFlow.BalanceFlag = 1;
                }

                break;

            case 1:
                if (RunFlow.SteriMinute >  Procedure.balance_time) {
                    pFlowPcb->state = FLOW_STATE_HEAT2_6;
                    Temperature.Target = Procedure.steri + 0.3;
                    RunFlow.BalanceFlag = 0;
                }

                break;
            }

            break;

        case FLOW_STATE_HEAT2_6:
            if (Temperature.Measure - Procedure.SetPoint >= 0.1 && Temperature.Measure - Procedure.SetPoint <= 0.6) {
                pFlowPcb->flow_state = WORK_PROGRAM_FLOW_STATE_STERI;
                pFlowPcb->StateParameter.ReachSetPoint = 1 ;

                if (Procedure.type == Liquid) {
                    pFlowPcb->StateParameter.KeepWarmPoint == 1 ? \
                    (RunFlow.CurveState = KeepWarmState) : (RunFlow.CurveState = BalanceState);
                } else if (Procedure.type == Melt) {
                    pFlowPcb->StateParameter.KeepWarmPoint == 1 ? \
                    (RunFlow.CurveState = KeepWarmState) : (RunFlow.CurveState = MeltState);
                } else if (Procedure.type == PreHeat) {
                    RunFlow.CurveState = PreHeatBlance;
                } else {
                    RunFlow.CurveState = BalanceState	;
                }

                RunFlow.SteriMinute  = 0;
                printf("------------------------\r\n");
                RunFlow.CurveState == KeepWarmState ? PrintString("keep warm start") : PrintString(pType[Procedure.type]);
                RunFlow.TenSecond	= 0;
                RunFlow.StateParameter.TenSecondFlag = 0;
                RunFlow.LastSecond	=		sTime.Seconds;
                printf("%d:%d:%d %4.1f%4.1dkpa\r\n", sTime.Hours, sTime.Minutes, \
                       sTime.Seconds, Temperature.Measure, RunFlow.Pressure);
            }

            if (Temperature.Measure < ParTab[PAR_POS_UNLOCK_TEMP].uVal) {
                Lock_PowerON;
            } else {
                Lock_PowerOFF;
            }

            break;
        }

        break;

    case WORK_PROGRAM_FLOW_STATE_STERI://工作流程4
        pFlowPcb->RunTime = 0;

        if (RunFlow.SteriMinute >= Procedure.steri_time) {
            if ((Procedure.type == Melt || Procedure.type == Liquid) && Procedure.SteriTimes > 1) {
                //									  Procedure.SteriTimes-- ;
                Temperature.Target    =  Procedure.mode.keepwarm;
                Procedure.SetPoint       =  Procedure.mode.keepwarm;
                Procedure.steri_time  =  Procedure.dry_kwarm_time.keepwarm;
                RunFlow.SteriMinute   =  0;
                pFlowPcb->flow_state  =  WORK_PROGRAM_FLOW_STATE_EXHAUST;
                pFlowPcb->state       =  FLOW_STATE_EXHAUST_3;
                //										pFlowPcb->flow_state  =  WORK_PROGRAM_FLOW_STATE_HEAT2;
                //									  pFlowPcb->state       =  FLOW_STATE_HEAT2_6;
                RunFlow.CurveState    =  ExhaustState;
                pFlowPcb->StateParameter.ReachSetPoint = 0 ;
                pFlowPcb->StateParameter.KeepWarmPoint = 1;
//                ParUnSaved[PAR_POS_FAN_AR ].uVal = 0;
            } else {
//                ParUnSaved[PAR_POS_FAN_AR ].uVal = 100;
                pFlowPcb->flow_state 	=  WORK_PROGRAM_FLOW_STATE_EXHAUST;
                pFlowPcb->state      	=  FLOW_STATE_EXHAUST_1;
                RunFlow.SteriMinute		=  0;
                pFlowPcb->StateParameter.ReachSetPoint = 0 ;
            }

            printf("------------------------\r\n");
            PrintString("Exhaust start") ;
            RunFlow.StateParameter.TenSecondFlag = 0;
        } else {
            if (RunFlow.StateParameter.TenSecondFlag == 1 && RunFlow.LastSecond	==	sTime.Seconds) {
                printf("%d:%d:%d %4.1f%4.1dkpa\r\n", sTime.Hours, sTime.Minutes, \
                       sTime.Seconds, Temperature.Measure, RunFlow.Pressure);
                RunFlow.StateParameter.TenSecondFlag = 0;
                RunFlow.TenSecond = 0;
            }
        }

        break;

    case WORK_PROGRAM_FLOW_STATE_EXHAUST://工作流程5--排气过程
        pFlowPcb->RunTime = 0;
            RunFlow.FanOutMs    < ParUnSaved[PAR_POS_FAN_AR ].uVal ? Fan_OFF : Fan_ON ;
        switch (pFlowPcb->state) {
        case FLOW_STATE_EXHAUST_1:
            pFlowPcb->StateParameter.KeepWarmPoint = 0;
            RunFlow.CurveState = ExhaustState;
            Temperature.Target  = 0;

            if (Procedure.exhmode) {
                if (Temperature.Measure <= Procedure.exhaust) {
                    RellayCtrl(RelayON);
                    osDelay(Procedure.exhmode * 800);
                    RellayCtrl(RelayOFF);
                    osDelay(Procedure.exhmode * 800);
                }

                if (Temperature.Measure <= ParTab[PAR_POS_TEMP_BOIL].uVal) {
                    RellayCtrl(RelayOFF);
                    pFlowPcb->state = FLOW_STATE_EXHAUST_2;
                }
            } else {
                if (Temperature.Measure <=  Procedure.exhaust) {
                    RellayCtrl(RelayOFF);
                    pFlowPcb->state = FLOW_STATE_EXHAUST_2;
                }
            }

            break;

        case FLOW_STATE_EXHAUST_2:
            if (Temperature.Measure <= ParTab[PAR_POS_TEMP_BOIL].uVal) {
                RellayCtrl(RelayOFF);
                pFlowPcb->state = FLOW_STATE_EXHAUST_3;
            }

            break;

        case FLOW_STATE_EXHAUST_3:
            if (Temperature.Measure < ParTab[PAR_POS_UNLOCK_TEMP].uVal) {
                CoolFanOFF;
								Lock_PowerON;
                if ((Procedure.type == Melt || Procedure.type == Liquid) && Procedure.SteriTimes > 1) {
                    Procedure.SteriTimes-- ;
                    pFlowPcb->flow_state  =  WORK_PROGRAM_FLOW_STATE_HEAT2;
                    pFlowPcb->state       =  FLOW_STATE_HEAT2_6;
                } else {
                    pFlowPcb->flow_state = WORK_PROGRAM_FLOW_STATE_FINISH1;
                }
            } else  if (Temperature.Measure <= ParTab[PAR_POS_TEMP_BOIL].uVal) {
                RellayCtrl(RelayOFF);
            }
        }

        break;

    case WORK_PROGRAM_FLOW_STATE_FINISH1://工作流程7--结束1
        for (i = 0 ; i < 3; i++) {
            ALM_ON;
            osDelay(1000);
            ALM_OFF;
            osDelay(1000);
        }
				RunFlow.FanOutMs = 0;
				  RunFlow.FanOutMs    < ParUnSaved[PAR_POS_FAN_AR ].uVal ? Fan_OFF : Fan_ON ;
        RunFlow.CurveState = FinishState1;
        pFlowPcb ->SteriCount++;
        ParTab[PAR_POS_STERI_COUNT_CORRECT].uVal = ParReceive[PAR_POS_STERI_COUNT_CORRECT].uVal = pFlowPcb ->SteriCount;
        pFlowPcb->flow_state = WORK_PROGRAM_FLOW_STATE_FINISH2;
        pFlowPcb->leverlock = CHECK_LEVER_LOCK_FLAG;
        printf("Finish\r\n");
        PrintString("Finish Time\r\n") ;

        if (ParTab[PAR_POS_WATER_F0_SWITCH].uVal == 1) {
            printf("f0 value= %.1f\r\n", RunFlow.f0_value);    //打印F0值
        }
        printf("------------------------\r\n");
        printf("Result:   \r\n");
        printf("------------------------\r\n");
        printf("Operator:   \r\n");
        printf("------------------------\r\n");
        printf("           END          \r\n");
        printf("------------------------\r\n");
        break;

    case WORK_PROGRAM_FLOW_STATE_FINISH2://工作流程8--结束2
			  	RunFlow.FanOutMs = 0;
				  RunFlow.FanOutMs    < ParUnSaved[PAR_POS_FAN_AR ].uVal ? Fan_ON : Fan_OFF ;
        break;

    case WORK_PROGRAM_FLOW_STATE_ERROR:
        ALM_ON;
        osDelay(1000);
        ALM_OFF;
        osDelay(10000);
        break;

    default:
        break;
    }
}

ProgramModePara  Program_Mode[30] = {0};

void Fun_Program_Mode(ProgramModePara * ProgramMode, CONTROL_STRUCT *pControl, manager *pFlowPcb)
{
    pControl->ProRunState = Progarm_mode_Reach_Target;

    switch (pControl->ProRunState) {
    case  Progarm_mode_Reach_Target:
        if (Program_Mode[pControl->Ctrl_Step].SteriTime > 0) {
            Temperature.Target = Program_Mode[pControl->Ctrl_Step].SteriTemp + 0.3;

            if (fabs(Temperature.Measure - Program_Mode[pControl->Ctrl_Step].SteriTemp) < 0.3) {
                pControl->Flag.ReachSetPoint = 1;
                //								    TEMP.Flag_SteriRunTime = 1;
                pControl->ProRunTime = 0;
                pControl->ProRunState = Progarm_mode_Steri_State;
            }
        } else {
            pControl->ProRunState = Progarm_mode_End_State;
        }

        break;

    case Progarm_mode_Steri_State:
        if (pControl->ProRunTime > Program_Mode[pControl->Ctrl_Step].SteriTime) {
            pControl->Ctrl_Step++;
            pControl->ProRunTime = 0;
            pControl->ProRunState = Progarm_mode_Reach_Target;
        }

        break;

    case Progarm_mode_End_State:
        pFlowPcb->CurveState = FinishState1;
        pFlowPcb->flow_state = WORK_PROGRAM_FLOW_STATE_STANDBY;
        break;

    default :
        break;
    }
}

void APPTask(void *argument)
{
    /* USER CODE BEGIN APPTask */
    //       	uint32_t flagdd;
    /* Infinite loop */
    for (;;) {
        //    flagdd=osEventFlagsWait	(	MyStartStopEventHandle,
        //												    0x00000001,
        //												osFlagsWaitAny,
        //												osWaitForever
        //												);
        flow_control(&RunFlow);
        osDelay(10);
    }

    /* USER CODE END APPTask */
}





