#pragma once

/*
████████╗██╗  ██╗███████╗ ██████╗ ███████╗      ██████╗  █████╗
╚══██╔══╝██║  ██║██╔════╝██╔═══██╗██╔════╝      ╚════██╗██╔══██╗
   ██║   ███████║█████╗  ██║   ██║███████╗█████╗ █████╔╝███████║
   ██║   ██╔══██║██╔══╝  ██║   ██║╚════██║╚════╝██╔═══╝ ██╔══██║
   ██║   ██║  ██║███████╗╚██████╔╝███████║      ███████╗██║  ██║
   ╚═╝   ╚═╝  ╚═╝╚══════╝ ╚═════╝ ╚══════╝      ╚══════╝╚═╝  ╚═╝

 File: ITTC.cpp
 Author: Aroonsak Bootchai [AB]
===============================================================
Main structure for TTC (Telemetry and Telecommand)
please note that the struct of data shall be related to the documents
https://drive.google.com/drive/u/0/folders/1u0ZkYuBGGmP1kSmudPR9p4BN9E7sliB3

 Developer: Aroonsak Bootchai
 Project: SchoolSAT
 Date: 2022-Jun-12
 Version: 010
============================================================
*/

#include <iostream>

enum class CtrlType:uint8_t {
    TC_req,
    TC_ret,
    TM_req,
    TM_ret
};

struct TTCBroadcast {
    uint8_t moduleSrcID;
    uint8_t moduleDestID;
    uint8_t commandID;
    uint32_t requestTime;
    CtrlType ctrlType;
    uint8_t dataState;
    uint64_t value0;
    uint64_t value1;
    uint64_t value2;
    uint64_t value3;
};

enum class _Datastate:uint8_t {
    ACK,
    NACK,
    TIMEOUT
};

struct TTCInternal {
    uint8_t moduleSrcID;
    uint8_t moduleDestID;
    uint8_t commandID;
    uint32_t requestTime;
    CtrlType ctrlType;
    _Datastate dataState;
    uint32_t value;
};

