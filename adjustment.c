#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <unistd.h>
#include <string.h>

#define MAX_MSG_SIZE 256
#define MSG_BUFFER_SIZE (MAX_MSG_SIZE + 10)

#define QUEUE_OTK_TO_ADJ "/otk_to_adj"
#define QUEUE_ADJ_TO_OTK "/adj_to_otk"

int main() {
    mqd_t q_from_otk, q_to_otk;
    char buffer[MSG_BUFFER_SIZE];
    unsigned int priority;

    struct mq_attr attr = {
        .mq_flags = 0,
        .mq_maxmsg = 10,
        .mq_msgsize = MAX_MSG_SIZE,
        .mq_curmsgs = 0
    };

    q_from_otk = mq_open(QUEUE_OTK_TO_ADJ, O_CREAT | O_RDONLY, 0666, &attr);
    q_to_otk = mq_open(QUEUE_ADJ_TO_OTK, O_CREAT | O_WRONLY, 0666, &attr);

    if (q_from_otk == (mqd_t)-1 || q_to_otk == (mqd_t)-1) {
        perror("������ �������� �������");
        exit(1);
    }

    printf("[�������] ����� ������� ����� � ������\n");

    while (1) {
        ssize_t bytes_read = mq_receive(q_from_otk, buffer, MSG_BUFFER_SIZE, &priority);
        if (bytes_read == -1) {
            perror("������ ������ �� �������");
            continue;
        }

        buffer[bytes_read] = '\0';
        printf("[�������] ������� ������� �� ���������: %s\n", buffer);

        // �������� ������� �� ���������
        sleep(2);

        printf("[�������] ������� %s ���������, ���������� � ���\n", buffer);
        if (mq_send(q_to_otk, buffer, strlen(buffer) + 1, 0) == -1) {
            perror("������ �������� � �������");
        }
    }

    // � �������� ��������� ����� ������� �������, �� ���� �� �� ������
    return 0;
}