#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "math.h"


typedef struct FlightInfo {
    char destination[100];    // �յ�վ
    char flightNumber[20];    // �����
    char planeNumber[20];     // ����
    int flightDay;            // ���ڼ��أ�
    int seatQuota;            // ����Ա��
    int remainingTickets;     // ��Ʊ��
    struct PassengerNode* passengerListHead; // �˿�����
    struct WaitListNode* waitListHead;       // �Ⱥ�����ͷ
    struct WaitListNode* waitListTail;       // �Ⱥ�����β
} FlightInfo;

// �˿���Ϣ�ṹ��
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

// 1. ¼�뺽����Ϣ
//
//������Ҫһ���������ֶ�¼�뺽����Ϣ��

// ����ȫ�ֺ�������ͺ�������
#define MAX_FLIGHTS 100
FlightInfo flights[MAX_FLIGHTS];
int flightCount = 0;

void enterFlightInfo() {
    if (flightCount >= MAX_FLIGHTS) {
        printf("�����¼������\n");
        return;
    }
    FlightInfo* f = &flights[flightCount++];
    printf("�����յ�վ��: ");
    scanf_s("%s", f->destination);
    printf("���뺽���: ");
    scanf_s("%s", f->flightNumber);
    printf("����ɻ���: ");
    scanf_s("%s", f->planeNumber);
    printf("����������� (1-7, ������Ϊ1): ");
    scanf_s("%d", &f->flightDay);
    printf("�����Ա����: ");
    scanf_s("%d", &f->seatQuota);
    printf("������Ʊ��: ");
    scanf_s("%d", &f->remainingTickets);

    f->passengerListHead = NULL;
    f->waitListHead = NULL;
    f->waitListTail = NULL;

    printf("������Ϣ¼��ɹ���\n");
}
//2. ��ѯ����
//
//�������������û��ṩ���յ�վ�������Ӧ�ĺ�����Ϣ��

void queryFlights() {
    char destination[100];
    printf("�����ѯ���յ�վ��: ");
    scanf_s("%s", destination);
    int found = 0;
    for (int i = 0; i < flightCount; i++) {
        if (strcmp(flights[i].destination, destination) == 0) {
            printf("�����: %s, �ɻ���: %s, ���ڼ�����: %d, ��Ʊ��: %d\n",
                flights[i].flightNumber, flights[i].planeNumber,
                flights[i].flightDay, flights[i].remainingTickets);
            found = 1;
        }
    }
    if (!found) {
        printf("û���ҵ����� %s �ĺ��ࡣ\n", destination);
    }
}
// 3. ����Ʊ
//
//������ܽ����ݺ���źͶ�Ʊ����������Ʊ����


void bookTickets() {
    char flightNumber[20];
    int tickets;
    printf("���뺽��źͶ�Ʊ����: ");
    scanf_s("%s %d", flightNumber, &tickets);
    for (int i = 0; i < flightCount; i++) {
        if (strcmp(flights[i].flightNumber, flightNumber) == 0) {
            if (flights[i].remainingTickets >= tickets) {
                flights[i].remainingTickets -= tickets;
                printf("��Ʊ�ɹ�����ǰ��Ʊ��: %d\n", flights[i].remainingTickets);
                return;
            }
            else {
                printf("��Ʊ���㣬�޷���ɶ�Ʊ��\n");
                return;
            }
        }
    }
    printf("δ�ҵ������Ϊ %s �ĺ��ࡣ\n", flightNumber);
}
 /*4. ������Ʊ

������Ʊҵ�񣬲���ѯ�Ƿ��еȴ������еĿͻ����Խ�����ЩƱ��*/
void cancelTickets() {
    char flightNumber[20];
    int tickets;
    printf("������Ʊ�ĺ���ź�����: ");
    scanf_s("%s %d", flightNumber, &tickets);
    for (int i = 0; i < flightCount; i++) {
        if (strcmp(flights[i].flightNumber, flightNumber) == 0) {
            flights[i].remainingTickets += tickets;
            printf("��Ʊ�ɹ�����ǰ��Ʊ��: %d\n", flights[i].remainingTickets);
            // ���������Ӵ���������Ⱥ�����
            return;
        }
    }
    printf("δ�ҵ������Ϊ %s �ĺ��ࡣ\n", flightNumber);
}
//1. �������ݵ��ļ�
//
//��������Ὣ���к�����Ϣд�뵽һ���ı��ļ��С����Ǽ����ļ��ĸ�ʽ��ÿ��һ�����࣬�ֶ��ɶ��ŷָ���


void saveFlightDataToFile() {
    FILE* file;
   errno_t err = fopen_s(&file, "flights.txt", "w");
    if (file == NULL) {
        printf("�޷����ļ�����д�롣\n");
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
    printf("���������ѱ��浽�ļ���\n");
}

// 2. ���ļ���������
//
//�����������ļ��ж�ȡ������Ϣ�����Ǽ����ļ��ĸ�ʽ�뱣��ʱ��ͬ��


void loadFlightDataFromFile() {
    FILE* file;
        errno_t err= fopen_s(&file, "flights.txt", "r");
    if (file == NULL) {
        printf("�޷����ļ����ж�ȡ��\n");
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
    printf("���������Ѵ��ļ����ء�\n");
}


 /*3. ���ϵ�������

���������������ϵ��������У��Ա��ڳ�������ʱ�������ݣ����ڳ����˳�ǰ�������ݡ�*/


int main() {
    int choice;
    loadFlightDataFromFile();  // ��������ʱ��������

    do {
        printf("\n���տ��˶�Ʊϵͳ\n");
        printf("1. ¼�뺽����Ϣ\n");
        printf("2. ��ѯ����\n");
        printf("3. ��Ʊ\n");
        printf("4. ��Ʊ\n");
        printf("5. ���溽�����ݵ��ļ�\n");
        printf("6. ���ļ����غ�������\n");
        printf("0. �˳�\n");
        printf("ѡ�����: ");
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
            saveFlightDataToFile();  // ���˳�ǰ��������
            printf("�˳�ϵͳ��\n");
            break;
        default:
            printf("��Ч���룬������ѡ��\n");
            break;
        }
    } while (choice != 0);

    return 0;
}