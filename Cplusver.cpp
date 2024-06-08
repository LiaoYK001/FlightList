#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "math.h"


typedef struct FlightInfo {
    char destination[100];    // 终点站
    char flightNumber[20];    // 航班号
    char planeNumber[20];     // 机型
    int flightDay;            // 星期几呢？
    int seatQuota;            // 最大成员组
    int remainingTickets;     // 余票量
    struct PassengerNode* passengerListHead; // 乘客名单
    struct WaitListNode* waitListHead;       // 等候名单头
    struct WaitListNode* waitListTail;       // 等候名单尾
} FlightInfo;

// 乘客信息结构体
typedef struct PassengerNode {
    char name[100];
    int ticketCount;
    int cabinClass;
    struct PassengerNode* next;
} PassengerNode;
typedef struct WaitListNode {
    char name[100];
    int requiredTickets;
    struct WaitListNode* next;
} WaitListNode;

// 1. 录入航班信息
//
//我们需要一个函数来手动录入航班信息，

// 定义全局航班数组和航班数量
#define MAX_FLIGHTS 100
FlightInfo flights[MAX_FLIGHTS];
int flightCount = 0;

void enterFlightInfo() {
    if (flightCount >= MAX_FLIGHTS) {
        printf("航班记录已满。\n");
        return;
    }
    FlightInfo* f = &flights[flightCount++];
    printf("输入终点站名: ");
    scanf_s("%s", f->destination);
    printf("输入航班号: ");
    scanf_s("%s", f->flightNumber);
    printf("输入飞机号: ");
    scanf_s("%s", f->planeNumber);
    printf("输入飞行周日 (1-7, 星期天为1): ");
    scanf_s("%d", &f->flightDay);
    printf("输入乘员定额: ");
    scanf_s("%d", &f->seatQuota);
    printf("输入余票量: ");
    scanf_s("%d", &f->remainingTickets);

    f->passengerListHead = NULL;
    f->waitListHead = NULL;
    f->waitListTail = NULL;

    printf("航班信息录入成功！\n");
}
//2. 查询航线
//
//这个函数会根据用户提供的终点站名输出相应的航班信息。

void queryFlights() {
    char destination[100];
    printf("输入查询的终点站名: ");
    scanf_s("%s", destination);
    int found = 0;
    for (int i = 0; i < flightCount; i++) {
        if (strcmp(flights[i].destination, destination) == 0) {
            printf("航班号: %s, 飞机号: %s, 星期几飞行: %d, 余票量: %d\n",
                flights[i].flightNumber, flights[i].planeNumber,
                flights[i].flightDay, flights[i].remainingTickets);
            found = 1;
        }
    }
    if (!found) {
        printf("没有找到到达 %s 的航班。\n", destination);
    }
}
// 3. 处理订票
//
//这个功能将根据航班号和订票数量来处理订票请求。


void bookTickets() {
    char flightNumber[20];
    int tickets;
    printf("输入航班号和订票数量: ");
    scanf_s("%s %d", flightNumber, &tickets);
    for (int i = 0; i < flightCount; i++) {
        if (strcmp(flights[i].flightNumber, flightNumber) == 0) {
            if (flights[i].remainingTickets >= tickets) {
                flights[i].remainingTickets -= tickets;
                printf("订票成功！当前余票量: %d\n", flights[i].remainingTickets);
                return;
            }
            else {
                printf("余票不足，无法完成订票。\n");
                return;
            }
        }
    }
    printf("未找到航班号为 %s 的航班。\n", flightNumber);
}
 /*4. 处理退票

处理退票业务，并查询是否有等待名单中的客户可以接收这些票。*/
void cancelTickets() {
    char flightNumber[20];
    int tickets;
    printf("输入退票的航班号和数量: ");
    scanf_s("%s %d", flightNumber, &tickets);
    for (int i = 0; i < flightCount; i++) {
        if (strcmp(flights[i].flightNumber, flightNumber) == 0) {
            flights[i].remainingTickets += tickets;
            printf("退票成功！当前余票量: %d\n", flights[i].remainingTickets);
            // 这里可以添加代码来处理等候名单
            return;
        }
    }
    printf("未找到航班号为 %s 的航班。\n", flightNumber);
}
//1. 保存数据到文件
//
//这个函数会将所有航班信息写入到一个文本文件中。我们假设文件的格式是每行一个航班，字段由逗号分隔。


void saveFlightDataToFile() {
    FILE* file;
   errno_t err = fopen_s(&file, "flights.txt", "w");
    if (file == NULL) {
        printf("无法打开文件进行写入。\n");
        return;
    }
    for (int i = 0; i < flightCount; i++) {
        fprintf(file, "%s,%s,%s,%d,%d,%d\n",
            flights[i].destination,
            flights[i].flightNumber,
            flights[i].planeNumber,
            flights[i].flightDay,
            flights[i].seatQuota,
            flights[i].remainingTickets);
    }
    fclose(file);
    printf("航班数据已保存到文件。\n");
}

// 2. 从文件加载数据
//
//这个函数会从文件中读取航班信息，我们假设文件的格式与保存时相同。


void loadFlightDataFromFile() {
    FILE* file;
        errno_t err= fopen_s(&file, "flights.txt", "r");
    if (file == NULL) {
        printf("无法打开文件进行读取。\n");
        return;
    }
    while (fscanf_s(file, "%99[^,],%19[^,],%19[^,],%d,%d,%d\n",
        flights[flightCount].destination,
        flights[flightCount].flightNumber,
        flights[flightCount].planeNumber,
        &flights[flightCount].flightDay,
        &flights[flightCount].seatQuota,
        &flights[flightCount].remainingTickets) == 6) {
        flights[flightCount].passengerListHead = NULL;
        flights[flightCount].waitListHead = NULL;
        flights[flightCount].waitListTail = NULL;
        flightCount++;
        if (flightCount >= MAX_FLIGHTS) break;
    }
    fclose(file);
    printf("航班数据已从文件加载。\n");
}


 /*3. 整合到主函数

将这两个函数整合到主函数中，以便在程序启动时加载数据，并在程序退出前保存数据。*/


int main() {
    int choice;
    loadFlightDataFromFile();  // 程序启动时加载数据

    do {
        printf("\n航空客运订票系统\n");
        printf("1. 录入航班信息\n");
        printf("2. 查询航线\n");
        printf("3. 订票\n");
        printf("4. 退票\n");
        printf("5. 保存航班数据到文件\n");
        printf("6. 从文件加载航班数据\n");
        printf("0. 退出\n");
        printf("选择操作: ");
        scanf_s("%d", &choice);

        switch (choice) {
        case 1:
            enterFlightInfo();
            break;
        case 2:
            queryFlights();
            break;
        case 3:
            bookTickets();
            break;
        case 4:
            cancelTickets();
            break;
        case 5:
            saveFlightDataToFile();
            break;
        case 6:
            loadFlightDataFromFile();
            break;
        case 0:
            saveFlightDataToFile();  // 在退出前保存数据
            printf("退出系统。\n");
            break;
        default:
            printf("无效输入，请重新选择。\n");
            break;
        }
    } while (choice != 0);

    return 0;
}