#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

#define MAX_MSG_SIZE 256
#define MSG_BUFFER_SIZE (MAX_MSG_SIZE + 10)

// ����� ��������
#define QUEUE_OTK_TO_ADJ "/otk_to_adj"
#define QUEUE_ADJ_TO_OTK "/adj_to_otk"

int main() {
    mqd_t q_to_adj, q_from_adj;
    char buffer[MSG_BUFFER_SIZE];
    unsigned int priority;

    struct mq_attr attr = {
        .mq_flags = 0,
        .mq_maxmsg = 10,
        .mq_msgsize = MAX_MSG_SIZE,
        .mq_curmsgs = 0
    };

    // ��������� ������� (�������, ���� �� ����������)
    q_to_adj = mq_open(QUEUE_OTK_TO_ADJ, O_CREAT | O_WRONLY, 0666, &attr);
    q_from_adj = mq_open(QUEUE_ADJ_TO_OTK, O_CREAT | O_RDONLY, 0666, &attr);

    if (q_to_adj == (mqd_t)-1 || q_from_adj == (mqd_t)-1) {
        perror("������ �������� �������");
        exit(1);
    }

    srand(time(NULL));
    int product_id = 1;

    while (1) {
        snprintf(buffer, MAX_MSG_SIZE, "Product_%d", product_id++);

        // ������ �������� - 85% ��������
        if (rand() % 100 < 85) {
            printf("[���] %s ������������� �����������\n", buffer);
        }
        else {
            printf("[���] %s �� �������������, ���������� �� ���������\n", buffer);
            if (mq_send(q_to_adj, buffer, strlen(buffer) + 1, 0) == -1) {
                perror("������ �������� � �������");
            }
        }

        // ��������� ������������ �������� (������������� ������)
        struct timespec timeout = { 0, 0 };
        ssize_t bytes_read = mq_timedreceive(q_from_adj, buffer, MSG_BUFFER_SIZE, &priority, &timeout);

        if (bytes_read > 0) {
            buffer[bytes_read] = '\0';
            printf("[���] ������� ������������ �������: %s\n", buffer);

            // ����� ��������� ����������� ����� 2.5%
            if (rand() % 1000 < 25) {
                printf("[���] %s ����� �� �������������, ���������� �������\n", buffer);
                if (mq_send(q_to_adj, buffer, strlen(buffer) + 1, 0) == -1) {
                    perror("������ �������� � �������");
                }
            }
            else {
                printf("[���] %s ������ ������������� ����� ���������\n", buffer);
            }
        }

        sleep(1); // �������� ����� ����������
    }

    // � �������� ��������� ����� ������� �������, �� ���� �� �� ������
    return 0;
}