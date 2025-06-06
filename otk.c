#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <time.h>

#define MAX_MSG_SIZE 256
#define MSG_TYPE_OTK_TO_ADJ 1
#define MSG_TYPE_ADJ_TO_OTK 2

struct msgbuf {
    long mtype;
    char mtext[MAX_MSG_SIZE];
};

int main() {
    key_t key_otk_adj = ftok(".", 'a');
    key_t key_adj_otk = ftok(".", 'b');

    int q_otk_adj = msgget(key_otk_adj, 0666 | IPC_CREAT);
    int q_adj_otk = msgget(key_adj_otk, 0666 | IPC_CREAT);

    if (q_otk_adj == -1 || q_adj_otk == -1) {
        perror("msgget failed");
        exit(1);
    }

    srand(time(NULL));
    int product_id = 1;
    struct msgbuf message;

    while (1) {
        snprintf(message.mtext, MAX_MSG_SIZE, "Product_%d", product_id++);
        message.mtype = MSG_TYPE_OTK_TO_ADJ;

        if (rand() % 100 < 85) {
            printf("[���] %s ������������� �����������\n", message.mtext);
        }
        else {
            printf("[���] %s �� �������������, ��������� �� ���������\n", message.mtext);
            if (msgsnd(q_otk_adj, &message, strlen(message.mtext) + 1, 0) == -1) {
                perror("msgsnd failed");
            }
        }

        // �������� ������������ ���������
        if (msgrcv(q_adj_otk, &message, MAX_MSG_SIZE, MSG_TYPE_ADJ_TO_OTK, IPC_NOWAIT) > 0) {
            printf("[���] �������� ������������ �������: %s\n", message.mtext);

            if (rand() % 1000 < 25) {
                printf("[���] %s ����� �� �������������, ���������� � �����\n", message.mtext);
                message.mtype = MSG_TYPE_OTK_TO_ADJ;
                if (msgsnd(q_otk_adj, &message, strlen(message.mtext) + 1, 0) == -1) {
                    perror("msgsnd failed");
                }
            }
            else {
                printf("[���] %s ������������� ����� ���������\n", message.mtext);
            }
        }

        sleep(1);
    }

    return 0;
}