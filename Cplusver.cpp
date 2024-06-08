#define  _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 航线
typedef struct FlightInfo {
    char destination[100];    // 终点站
    char flightNumber[20];    // 航班号
    char planeNumber[20];     // 机型
    int flightDays[7];        // 星期几，0: 周日, 6: 周六
    int seatQuota;            // 最大人数
    int remainingTickets;     // 余票
    struct PassengerNode* passengerListHead; // 乘客名单
    struct WaitListNode* waitListHead;       // 等候名单头
    struct WaitListNode* waitListTail;       // 等候名单尾
} FlightInfo;

// 乘客
typedef struct PassengerNode {
    char name[100];
    int ticketCount;//一个person有多少票？比如老王要3张ticketCount==3；
    struct PassengerNode* next;
} PassengerNode;

typedef struct WaitListNode {
    char name[100];
    int requiredTickets;//一个person有多少票（候补）
    struct WaitListNode* next;
} WaitListNode;

#define MAX_FLIGHTS 999999;
FlightInfo flights[999999];//max=999999
int flightCount = 0;//先归零

void enterFlightInfo() {
    FlightInfo* f = &flights[flightCount++];
    printf("输入终点站名: ");
    scanf("%s", f->destination);
    printf("输入航班号: ");
    scanf("%s", f->flightNumber);
    printf("输入飞机号: ");
    scanf("%s", f->planeNumber);
    printf("输入飞行星期？ (用空格分隔, 例如 2 4 6): ");
    for (int i = 0; i < 7; ++i) f->flightDays[i] = 0;//初始假设7天都没有
    char days[20];
    scanf("%s", days);//字符串类型
    char* context = NULL;
    for(int i=0;i<strlen(days);i++){
        if(days[i]!=' '){
            int cou=days[i]-'0';
             if(cou>7||cou<1){
                printf("错误的日期输入！");
                exit(0);
             }
            f->flightDays[cou]=1;
        }
    }
    
    printf("输入最大人数: ");
    scanf("%d", &f->seatQuota);
    printf("输入余票量: ");
    scanf("%d", &f->remainingTickets);

    f->passengerListHead = NULL;
    f->waitListHead = NULL;
    f->waitListTail = NULL;

    printf("航班信息录入成功！\n");
}

void queryFlights() {
    char destination[100];
    printf("输入查询的终点站名: ");
    scanf("%s", destination);
    int found = 0;
    for (int i = 0; i < flightCount; i++) {
        if (strcmp(flights[i].destination, destination) == 0) { //字符串比较找到一样的！string compare
            printf("航班号: %s, 机型: %s, 飞行星期: ", flights[i].flightNumber, flights[i].planeNumber);
            for (int j = 0; j < 7; ++j) if (flights[i].flightDays[j]) printf("%d ", j + 1);
            printf(", 余票量: %d\n", flights[i].remainingTickets);
            found = 1;
        }
    }
    if (!found) {
        printf("没有找到到达 %s 的航班。\n", destination);
    }
}

void addPassenger(FlightInfo* flight, const char* name, int tickets) {
    PassengerNode* newPassenger = (PassengerNode*)malloc(sizeof(PassengerNode));
    strcpy(newPassenger->name, name);
    newPassenger->ticketCount = tickets;
    newPassenger->next = NULL;

    if (flight->passengerListHead == NULL || strcmp(flight->passengerListHead->name, name) > 0) {  //程序一号链表：客户链表操作，不带头node
        newPassenger->next = flight->passengerListHead;
        flight->passengerListHead = newPassenger;//头结点移动
    }
    else {
        PassengerNode* p = flight->passengerListHead;//此时由于要按照name排序，找链表中合适的地儿插入！
        while (p->next != NULL && strcmp(p->next->name, name) < 0) {
            p = p->next;
        }
        newPassenger->next = p->next;
        p->next = newPassenger;
    }
}

void addWaitList(FlightInfo* flight, const char* name, int tickets) {
    WaitListNode* newWait = (WaitListNode*)malloc(sizeof(WaitListNode));
    strcpy(newWait->name, name);
    newWait->requiredTickets = tickets;
    newWait->next = NULL;

    if (flight->waitListTail == NULL) {
        flight->waitListHead = newWait;
    }
    else {
        flight->waitListTail->next = newWait;
    }
    flight->waitListTail = newWait;
}

void bookTickets() {
    char flightNumber[20];
    int tickets;
    char customerName[100];

    printf("输入您的姓名: ");
    scanf("%s", customerName);
    printf("输入航班号和订票数量: ");
    scanf("%s", flightNumber);
    scanf("%d", &tickets);
    if(tickets<1||tickets>999999){
        printf("你想干什么？请输入正确的数字\n");
    }
    FlightInfo* targetFlight = NULL;
    for (int i = 0; i < flightCount; i++) {
        if (strcmp(flights[i].flightNumber, flightNumber) == 0) {
            targetFlight = &flights[i];
            break;
        }
    }

    if (targetFlight == NULL) {
        printf("未找到航班号为 %s 的航班。\n", flightNumber);
        return;
    }

    if (targetFlight->remainingTickets >= tickets) {
        targetFlight->remainingTickets -= tickets;
        addPassenger(targetFlight, customerName, tickets);//进行链表插入
        printf("订票成功！当前余票量: %d\n", targetFlight->remainingTickets);
    }
    else {
        printf("余票不足，无法完成订票。\n");

        printf("是否查看其他到达同一目的地的航班？(y/n): ");
        char choice;
        scanf(" %c", &choice);
        if (choice == 'y' || choice == 'Y') {
            int otherFlightsFound = 0;
            for (int i = 0; i < flightCount; i++) {
                if (strcmp(flights[i].destination, targetFlight->destination) == 0 && strcmp(flights[i].flightNumber, flightNumber) != 0) {
                    printf("航班号: %s, 机型: %s, 飞行日: ", flights[i].flightNumber, flights[i].planeNumber);
                    for (int j = 0; j < 7; ++j) if (flights[i].flightDays[j]) printf("%d ", j + 1);
                    printf(", 余票量: %d\n", flights[i].remainingTickets);
                    otherFlightsFound = 1;
                }
            }

            if (otherFlightsFound) {
                printf("是否订票？(y/n): ");
                scanf(" %c", &choice);
                if (choice == 'y' || choice == 'Y') {
                    printf("输入新的航班号和订票数量: ");
                    char newFlightNumber[20];
                    int newTickets;
                    scanf("%s", newFlightNumber);
                    scanf("%d", &newTickets);

                    for (int i = 0; i < flightCount; i++) {
                        if (strcmp(flights[i].flightNumber, newFlightNumber) == 0 && strcmp(flights[i].destination, targetFlight->destination) == 0) {
                            if (flights[i].remainingTickets >= newTickets) {
                                flights[i].remainingTickets -= newTickets;
                                addPassenger(&flights[i], customerName, newTickets);
                                printf("订票成功！当前余票量: %d\n", flights[i].remainingTickets);
                                return;
                            }
                            else {
                                printf("余票不足，无法完成订票。\n");
                            }
                        }
                    }
                }
            }
        }

        printf("您需要候补吗？(y/n): ");
        scanf(" %c", &choice);
        if (choice == 'y' || choice == 'Y') {
            WaitListNode* newWait = (WaitListNode*)malloc(sizeof(WaitListNode));
            strcpy(newWait->name, customerName);
            newWait->requiredTickets = tickets;
            newWait->next = NULL;
            if (targetFlight->waitListTail) {
                targetFlight->waitListTail->next = newWait;//注意：使用了无头结点的队列！（方便内部操作，不用外设enqueue，dequeue函数了）
            }
            else {
                targetFlight->waitListHead = newWait;
            }
            targetFlight->waitListTail = newWait;//成为新的tail（入队列操作）
            printf("已加入候补名单。\n");
        }
    }
}




void processWaitList(FlightInfo* flight) {
    while (flight->waitListHead) {
        WaitListNode* waitNode = flight->waitListHead;

        if (flight->remainingTickets >= waitNode->requiredTickets) {
            printf("候补乘客 %s 需要 %d 张票，当前余票 %d 张。是否为该乘客办理订票？(y/n): ", waitNode->name, waitNode->requiredTickets, flight->remainingTickets);
            char choice;
            scanf(" %c", &choice);

            if (choice == 'y' || choice == 'Y') {
                flight->waitListHead = waitNode->next;
                if (flight->waitListHead == NULL) {
                    flight->waitListTail = NULL;
                }
                flight->remainingTickets -= waitNode->requiredTickets;
                addPassenger(flight, waitNode->name, waitNode->requiredTickets);
                printf("已为候补乘客 %s 办理订票。\n", waitNode->name);
                free(waitNode);
            }
            else {
                printf("跳过候补乘客 %s。\n", waitNode->name);
                flight->waitListHead = waitNode->next;
                if (flight->waitListHead == NULL) {
                    flight->waitListTail = NULL;
                }
                free(waitNode);
            }
        }
        else {
            break;
        }
    }
}


void cancelTickets() {
    char flightNumber[20];
    char customerName[100];
    int tickets;

    printf("输入退票的航班号、乘客姓名和数量: ");
    scanf("%s", flightNumber);
    scanf("%s", customerName);
    scanf("%d", &tickets);
    if(tickets<1||tickets>999999){
        printf("你想干什么？请输入正确的数字\n");
    }
    for (int i = 0; i < flightCount; i++) {
        if (strcmp(flights[i].flightNumber, flightNumber) == 0) {
            PassengerNode** p = &flights[i].passengerListHead;
            while (*p) {
                if (strcmp((*p)->name, customerName) == 0) {
                    if ((*p)->ticketCount >= tickets) {
                        (*p)->ticketCount -= tickets;
                        flights[i].remainingTickets += tickets;
                        printf("退票成功！当前余票量: %d\n", flights[i].remainingTickets);

                        // 退票完全取消该乘客的所有票
                        if ((*p)->ticketCount == 0) {
                            PassengerNode* temp = *p;
                            *p = (*p)->next;
                            free(temp);
                        }

                        // 处理等候名单
                        processWaitList(&flights[i]);
                        return;
                    }
                    else {
                        printf("退票数量超过订票数量，无法完成退票。\n");
                        return;
                    }
                }
                p = &(*p)->next;
            }
            printf("未找到乘客 %s 的订票信息。\n", customerName);
            return;
        }
    }
    printf("未找到航班号为 %s 的航班。\n", flightNumber);
}


void saveFlightDataToFile() {
    FILE* file;
    errno_t err = fopen_s(&file, "flights.txt", "w");
    if (file == NULL) {
        printf("无法打开文件进行写入。\n");
        return;
    }
    for (int i = 0; i < flightCount; i++) {
        fprintf(file, "航线:%s\n", flights[i].destination);
        fprintf(file, "航班号:%s\n", flights[i].flightNumber);
        fprintf(file, "机型:%s\n", flights[i].planeNumber);
        fprintf(file, "星期几:");
        for (int j = 0; j < 7; ++j) if (flights[i].flightDays[j]) fprintf(file, "%d ", j + 1);
        fprintf(file, "\n");
        fprintf(file, "最大人员:%d\n", flights[i].seatQuota);
        fprintf(file, "余票量:%d\n", flights[i].remainingTickets);

        PassengerNode* p = flights[i].passengerListHead;
        int passengerCount = 0;
        while (p) {
            passengerCount++;
            p = p->next;
        }
        fprintf(file, "已订票客户:%d\n", passengerCount);

        p = flights[i].passengerListHead;
        while (p) {
            fprintf(file, "姓名:%s\n", p->name);
            fprintf(file, "订票量:%d\n", p->ticketCount);
            p = p->next;
        }

        WaitListNode* w = flights[i].waitListHead;
        int waitListCount = 0;
        while (w) {
            waitListCount++;
            w = w->next;
        }
        fprintf(file, "候补:%d\n", waitListCount);

        w = flights[i].waitListHead;
        while (w) {
            fprintf(file, "姓名:%s\n", w->name);
            fprintf(file, "所需票量:%d\n", w->requiredTickets);
            w = w->next;
        }
    }
    fclose(file);
    printf("航班数据已保存到文件。\n");
}

void loadFlightDataFromFile() {
    FILE* file;
    errno_t err = fopen_s(&file, "flights.txt", "r");
    if (file == NULL) {
        printf("无法打开文件进行读取。\n");
        return;
    }

    while (!feof(file) && flightCount < 999999) {
        FlightInfo temp;
        char line[256];
        int passengerCount, waitListCount;

        if (fscanf_s(file, "航线:%[^\n]\n", temp.destination, 100) == EOF) break;
        if (fscanf_s(file, "航班号:%[^\n]\n", temp.flightNumber, 20) == EOF) break;
        if (fscanf_s(file, "机型:%[^\n]\n", temp.planeNumber, 20) == EOF) break;

        if (fscanf_s(file, "星期几:%[^\n]\n", line, 256) == EOF) break;
        for (int i = 0; i < 7; ++i) temp.flightDays[i] = 0;

        for(int i=0;i<strlen(line);i++){
        if(line[i]!=' '){
            int cou=line[i]-'0';
             if(cou>7||cou<1){
                printf("错误的日期输入！");
                exit(0);
             }
            temp.flightDays[cou]=1;
        }
    }

        if (fscanf_s(file, "最大人员:%d\n", &temp.seatQuota) == EOF) break;
        if (fscanf_s(file, "余票量:%d\n", &temp.remainingTickets) == EOF) break;

        if (fscanf_s(file, "已订票客户:%d\n", &passengerCount) == EOF) break;
        temp.passengerListHead = NULL;
        PassengerNode** p = &temp.passengerListHead;
        for (int i = 0; i < passengerCount; ++i) {
            PassengerNode* newPassenger = (PassengerNode*)malloc(sizeof(PassengerNode));
            if (fscanf_s(file, "姓名:%[^\n]\n", newPassenger->name, 100) == EOF) break;
            if (fscanf_s(file, "订票量:%d\n", &newPassenger->ticketCount) == EOF) break;
            newPassenger->next = NULL;
            *p = newPassenger;
            p = &newPassenger->next;
        }

        if (fscanf_s(file, "候补:%d\n", &waitListCount) == EOF) break;
        temp.waitListHead = NULL;
        temp.waitListTail = NULL;
        for (int i = 0; i < waitListCount; ++i) {
            WaitListNode* newWait = (WaitListNode*)malloc(sizeof(WaitListNode));
            if (fscanf_s(file, "姓名:%[^\n]\n", newWait->name, 100) == EOF) break;
            if (fscanf_s(file, "所需票量:%d\n", &newWait->requiredTickets) == EOF) break;
            newWait->next = NULL;
            if (temp.waitListTail) {
                temp.waitListTail->next = newWait;
            }
            else {
                temp.waitListHead = newWait;
            }
            temp.waitListTail = newWait;
        }

        flights[flightCount++] = temp;
    }
    fclose(file);
    printf("航班数据已从文件加载。\n");
}

int main() {
    int choice;
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
        scanf("%d", &choice);

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
