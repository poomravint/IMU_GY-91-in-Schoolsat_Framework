#pragma once
/*
████████╗██╗  ██╗███████╗ ██████╗ ███████╗      ██████╗  █████╗
╚══██╔══╝██║  ██║██╔════╝██╔═══██╗██╔════╝      ╚════██╗██╔══██╗
   ██║   ███████║█████╗  ██║   ██║███████╗█████╗ █████╔╝███████║
   ██║   ██╔══██║██╔══╝  ██║   ██║╚════██║╚════╝██╔═══╝ ██╔══██║
   ██║   ██║  ██║███████╗╚██████╔╝███████║      ███████╗██║  ██║
   ╚═╝   ╚═╝  ╚═╝╚══════╝ ╚═════╝ ╚══════╝      ╚══════╝╚═╝  ╚═╝

 File: CMqueue.cpp
 Author: Aroonsak Bootchai [AB]
===============================================================
 [description here]

 Developer: Aroonsak Bootchai
 Project: SchoolSAT
 Date: 2022-Jun-12
 Version: 010
===============================================================
*/

#include "ITTC.hpp"
#include <fmt/format.h>
#include <fcntl.h>
#include <iostream>
#include <mqueue.h>
#include <unistd.h>

mq_attr MQ_ATTR = {
      .mq_flags = O_RDONLY | O_CREAT, .mq_maxmsg = 10, .mq_msgsize = 1024};

class CMqueue {
 private:
  /* data */
  std::string name = "/mqueue";
 public:
   mq_attr attr = MQ_ATTR;
  int mode = -1;  
   mqd_t mq_dest = -1;
  CMqueue(std::string _name) { name = _name; };
  ~CMqueue() {
    // Close the message queue
    if (mq_close(mq_dest) == -1) {
      std::cout << "failed to close mqueue" << std::endl << std::flush;
    }

    // Unlink the message queue
    if (mq_unlink(name.c_str()) == -1) {
      std::cout << "failed to unlink mqueue" << std::endl << std::flush;
    }
  };
  int OpenRead() {
    mode = 0;
    mq_dest = mq_open(name.c_str(), O_RDONLY, 0664, &attr);
    if (mq_dest == (mqd_t)-1) {
      return -1;
    }
    return 0;
  };

  int OpenWrite() {
    mode = 1;
    mq_dest = mq_open(name.c_str(), O_WRONLY, 0664, nullptr);
    if (mq_dest == (mqd_t)-1) {
      return -1;
    }
    return 0;
  };

  int OpenRDWR(){
    mode = 2;
    mq_dest = mq_open(name.c_str(), O_RDWR | O_CREAT,0644,&attr);
     if (mq_dest == (mqd_t)-1) {
      return -1;
    }
    return 0;
  };

  int Send(TTCInternal msg) {
    if(mode == 0){
      return -1;
    }
    if (mq_send(mq_dest, (char*)&msg, sizeof(msg), 0) == -1) {
      return -1;
    }
    return 0;
  };

  int Close(){
     if (mq_close(mq_dest) == -1) {
      std::cout << "failed to close mqueue" << std::endl << std::flush;
    }
    return 0;
  };
};
