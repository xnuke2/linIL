#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>

#define MAX_MSG_SIZE 256
#define MSG_TYPE_OTK_TO_ADJ 1
#define MSG_TYPE_ADJ_TO_OTK 2

struct msgbufnew {
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

    printf("[наладка] Отдел наладки готов к работе\n");
    struct msgbufnew message;

    while (1) {
        if (msgrcv(q_otk_adj, &message, MAX_MSG_SIZE, MSG_TYPE_OTK_TO_ADJ, 0) == -1) {
            perror("msgrcv failed");
            continue;
        }

        printf("[Наладка] Получен продукт на доработку: %s\n", message.mtext);
        sleep(2);
        printf("[Наладка] Продукт %s доработан, возвращаем в ОТК\n", message.mtext);

        message.mtype = MSG_TYPE_ADJ_TO_OTK;
        if (msgsnd(q_adj_otk, &message, strlen(message.mtext) + 1, 0) == -1) {
            perror("msgsnd failed");
        }
    }

    return 0;
}