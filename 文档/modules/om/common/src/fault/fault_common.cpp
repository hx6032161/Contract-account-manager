/******************************************************************************
 * Copyright (c) 2022-2022 by ZEEKR. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * @Brief:External interface
 * @Author:Hen.You
 * @Date:
 *****************************************************************************/
#include "fault_common.hpp"

#include <sys/ioctl.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include "fault_configure.hpp"
std::string soca_ip = fault::common::FaultConfig::Instance().GetParam("soca_ip");
std::string socb_ip = fault::common::FaultConfig::Instance().GetParam("socb_ip");
// const

const char *gTopicNameArray[] = {FAULT_SOCA_REPORT, FAULT_SOCB_REPORT, FAULT_MCUA_REPORT, FAULT_MCUB_REPORT,
                                 FAULT_DEFAULT_REPORT};

bool CompareSocBIpAddr(char *ip)
{
    std::string ip_addr = ip;
    int length = ip_addr.length();
    std::size_t found = ip_addr.rfind(socb_ip, length - 3);
    return found != std::string::npos;
}

bool CompareSocAIpAddr(char *ip)
{
    std::string ip_addr = ip;
    int length = ip_addr.length();
    std::size_t found = ip_addr.rfind(soca_ip, length - 3);
    if (found != std::string::npos) {
        return true;
    }
    return false;
}

HardwareTypeST GetMasterType()
{
    return SOCB;
}
HardwareTypeST GetHardWareType()
{
    // base ip and  hardwareinfo
    HardwareTypeST type = ERROR_HARDWARE_TYPE;

    int i = 0;
    int sockfd = 0;
    struct ifconf ifc;
    char buf[1024] = {0};
    char ipbuf[20] = {0};
    struct ifreq *ifr;
    ifc.ifc_len = 1024;
    ifc.ifc_buf = buf;

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        return ERROR_HARDWARE_TYPE;
    }
    ioctl(sockfd, SIOCGIFCONF, &ifc);
    ifr = (struct ifreq *)buf;

    for (i = (ifc.ifc_len / sizeof(struct ifreq)); i > 0; i--) {
        inet_ntop(AF_INET, &((struct sockaddr_in *)&ifr->ifr_addr)->sin_addr, ipbuf, 20);
        // printf("ip: %s \n",ipbuf);
        if (CompareSocBIpAddr(ipbuf)) {
            type = SOCB;
            break;
        } else if (CompareSocAIpAddr(ipbuf)) {
            type = SOCA;
            break;
        } else {
            type = ERROR_HARDWARE_TYPE;
        }
        ifr = ifr + 1;
    }
    close(sockfd);

    return type;
}

// master node deploy with fsm ,fsm on soc_B
bool IsMasterModel()
{
#ifdef TEST_OM_FAULT
    return true; // for test
#else

    bool is_master = false;
    HardwareTypeST type = ERROR_HARDWARE_TYPE;
    type = GetHardWareType();

    if (type == SOCB) {
        is_master = true;
    }
    return is_master;
#endif
}

// only use to get  fault_report  topic name
void getTopicName(char *topicName)
{
    HardwareTypeST type = ERROR_HARDWARE_TYPE;
    type = GetHardWareType();
    memcpy(topicName, gTopicNameArray[type], strlen(gTopicNameArray[type]));
}
