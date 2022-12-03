//
// Created by Corn on 2022/12/3.
//

#include "writeLog.h"

/**
 * Write a log file.
 * @param msg the message added in log.
 * @param fileAddress is the address we save log file.
 * @return 0 if success and 1 if fail.
 */
int WriteLog(string &msg, string &fileAddress) {
    SYSTEMTIME sys;
    GetLocalTime(&sys); //取系统时间
    stringstream strTemp;
    strTemp << sys.wMonth << '.' << sys.wDay << " " << sys.wHour << ':'
            << sys.wMinute << ':' << sys.wSecond << ':' << sys.wMilliseconds << "  ";

    ofstream outfile;
//    outfile.open(R"(c:\users\corn\desktop\test\LOG.txt)", ios::app); //文件的物理地址，文件的打开方式
    outfile.open(fileAddress, ios::app);
    if (outfile.is_open()) {
        outfile << strTemp.str() << msg << "\n";
        outfile.close();
        return 0;
    } else {
        cout << "Write failed!" << endl;
        return 1;
    }
}