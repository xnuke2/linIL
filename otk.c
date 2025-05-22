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

// Имена очередей
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

    // Открываем очереди (создаем, если не существуют)
    q_to_adj = mq_open(QUEUE_OTK_TO_ADJ, O_CREAT | O_WRONLY, 0666, &attr);
    q_from_adj = mq_open(QUEUE_ADJ_TO_OTK, O_CREAT | O_RDONLY, 0666, &attr);

    if (q_to_adj == (mqd_t)-1 || q_from_adj == (mqd_t)-1) {
        perror("Ошибка открытия очереди");
        exit(1);
    }

    srand(time(NULL));
    int product_id = 1;

    while (1) {
        snprintf(buffer, MAX_MSG_SIZE, "Product_%d", product_id++);

        // Первая проверка - 85% проходят
        if (rand() % 100 < 85) {
            printf("[ОТК] %s соответствует требованиям\n", buffer);
        }
        else {
            printf("[ОТК] %s не соответствует, отправляем на доработку\n", buffer);
            if (mq_send(q_to_adj, buffer, strlen(buffer) + 1, 0) == -1) {
                perror("Ошибка отправки в очередь");
            }
        }

        // Проверяем возвращенные продукты (неблокирующее чтение)
        struct timespec timeout = { 0, 0 };
        ssize_t bytes_read = mq_timedreceive(q_from_adj, buffer, MSG_BUFFER_SIZE, &priority, &timeout);

        if (bytes_read > 0) {
            buffer[bytes_read] = '\0';
            printf("[ОТК] Получен доработанный продукт: %s\n", buffer);

            // После доработки вероятность брака 2.5%
            if (rand() % 1000 < 25) {
                printf("[ОТК] %s снова не соответствует, отправляем обратно\n", buffer);
                if (mq_send(q_to_adj, buffer, strlen(buffer) + 1, 0) == -1) {
                    perror("Ошибка отправки в очередь");
                }
            }
            else {
                printf("[ОТК] %s теперь соответствует после доработки\n", buffer);
            }
        }

        sleep(1); // Задержка между продуктами
    }

    // В реальной программе нужно закрыть очереди, но сюда мы не дойдем
    return 0;
}