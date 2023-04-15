#include<iostream>
#include<WINSOCK2.H>
#include<cstring>
#include<vector>
#include<windows.h>
#include<fstream>
#include<sstream>

using namespace std;
#pragma comment(lib, "ws2_32.lib")

//Initialize
void initialization() {
    //Initialize SOCKET
    WORD wReq = MAKEWORD(2, 2);
    WSADATA data;
    if (WSAStartup(wReq, &data) != 0) {
        cout << "Initialization failed!" << endl;
    } else {
        cout << "Initialization succeed!" << endl;
    }

    //Check SOCKET version
    if (LOBYTE(data.wVersion) != 2 || HIBYTE(data.wHighVersion) != 2) {
        cout << "SOCKET version is not correct!" << endl;
        WSACleanup();
    }
}

int main() {
    bool flag{true}; //Circulation flag
    SYSTEMTIME sys; //Timer
    char recData[100]; //Receive Cache
    stringstream strTemp; //String string of messages
    vector<string> commands{"get_coords()", "get_angles()"};

    //Initialize and instantiate.
    initialization();
    SOCKET socketServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    sockaddr_in serAddr;
    serAddr.sin_family = AF_INET;
    serAddr.sin_port = htons(5001);
    serAddr.sin_addr.S_un.S_addr = inet_addr("10.181.250.135");

    //Check if connect with server.
    if (connect(socketServer, (sockaddr *) &serAddr, sizeof(serAddr)) == SOCKET_ERROR) {  //Connect failed.
        cout << "Connect failed!" << endl;
        closesocket(socketServer);
        return 0;
    } else cout << "Connect succeed!" << endl;

    //Open the log file.
    ofstream outfile;
    outfile.open(R"(c:\users\corn\desktop\socket\Log_circle_60_v2.txt)", ios::app);

    while (flag) {
        //Record system time.
        GetLocalTime(&sys);
        strTemp << sys.wMonth << '.' << sys.wDay << " " << sys.wHour << ':'
                << sys.wMinute << ':' << sys.wSecond << ':' << sys.wMilliseconds;

        //Run all the commands.
        for (string &commandStr: commands) {
            const char *sendData{commandStr.c_str()};
            int &&sendLen = send(socketServer, sendData, strlen(sendData), 0);
            if (sendLen < 0) {
                cout << "Send failed!" << endl;
                flag = false;
            }

            //int send(Socket s, const void * msg, int len, unsigned int flags)
            //s为已建立好连接的socket，msg指向数据内容，len则为数据长度，参数flags一般设0
            //成功则返回实际传送出去的字符数，失败返回-1，错误原因存于error

            int &&retLen = recv(socketServer, recData, 100, 0);
            if (retLen < 0) {
                cout << "Receive failed!" << endl;
                flag = false;
            } else {
                recData[retLen] = '\0';
                strTemp << "  " << string(&recData[0], &recData[retLen]);
            }
        }

        //Write the log.
        if (outfile.is_open()) {
            outfile << strTemp.str() << "\n";
        } else {
            flag = false;
        }
        strTemp.str("");
    }
    
    //End the program.
    outfile.close();
    closesocket(socketServer);
    WSACleanup();
    return 0;
}