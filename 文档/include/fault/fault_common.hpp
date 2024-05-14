/******************************************************************************
 * Copyright (c) 2022-2022 by ZEEKR. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * @Brief:External interface
 * @Author:Hen.You
 * @Date:
 *****************************************************************************/
#ifndef __FAULT_COMMON_DEFINE__
#define __FAULT_COMMON_DEFINE__

// define  topic name
#define FAULT_SOCA_REPORT "/om/soca_fault_report" //业务soca node上报告警
#define FAULT_SOCB_REPORT "/om/socb_fault_report" //业务socb node上报告警
#define FAULT_MCUA_REPORT "/om/mcua_fault_report" //业务mcua node上报告警
#define FAULT_MCUB_REPORT "/om/mcub_fault_report" //业务mcub node上报告警
#define FAULT_DEFAULT_REPORT "/om/fault_report"   // default only for test
#define FAULT_COLLECT "/om/fault_collect"         // Local 向 master
#define FAULT_REACTION "/om/faults_reaction"      // master 向 fsm

// const
constexpr unsigned INVALID_FAULT_ID = 0;
const unsigned char INVALID_FAULT_RSP_LEVEL = -1;
const unsigned char MAX_FAULT_RSP_LEVEL = 5;
const unsigned char MAX_FAULT_NA_LEVEL = 15;

const int FAULT_MSG_BUFFER_SIZE = 1000; // 故障源缓存队列长度,故障源自行确定请咨询各模块SE

// type define
typedef enum hardwareTypeST {
    SOCA = 0,
    SOCB, // currently only have socA, socB  fsm on B
    MCUA,
    MCUB,
    ERROR_HARDWARE_TYPE // use for test
} HardwareTypeST;

// function
HardwareTypeST GetHardWareType();
bool IsMasterModel();
void getTopicName(char* topicName);

#if 0
#ifndef TEST_OM_FAULT
#define TEST_OM_FAULT
#endif
#endif

#endif
