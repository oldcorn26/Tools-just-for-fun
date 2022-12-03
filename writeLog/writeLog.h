//
// Created by Corn on 2022/12/3.
//

#ifndef WRITELOG_H
#define WRITELOG_H
#include <iostream>
#include <windows.h>
#include <fstream>
#include <sstream>

using namespace std;

/**
 * Write a log file.
 * @param msg the message added in log.
 * @param fileAddress is the address we save log file.
 * @return 0 if success and 1 if fail.
 */
int WriteLog(string &msg, string &fileAddress);

#endif //WRITELOG_H
