#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#include "scraping.h"
#include "client_node.h"
#include "csv_logger.h"

SharedData shared_data;
sem_t data_ready;

void *data_collector(void *arg) {
    while (1) {
        fetch_pse_data("https://www.pse.pl/transmissionMapService",
                       &shared_data.gen_data,
                       &shared_data.exch_data,
                       &shared_data.freq_data);

        log_data_to_csv(&shared_data);  // Zapis do pliku CSV

        sem_post(&data_ready);  // Sygna? do wysy?ki danych
        printf("? Dane pobrane i zapisane do CSV\n");

        sleep(15);  // Czekaj 15 sekund
    }
    return NULL;
}

void *data_sender(void *arg) {
    while (1) {
        sem_wait(&data_ready);      // Czekaj na nowe dane
        send_data(&shared_data);    // Wy?lij dane do serwera OPC UA
    }
    return NULL;
}

int main() {
    ensure_all_csv_headers(); // nowa wersja

    pthread_t t1, t2;
    sem_init(&data_ready, 0, 0);

    pthread_create(&t1, NULL, data_collector, NULL);
    pthread_create(&t2, NULL, data_sender, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    sem_destroy(&data_ready);
    return 0;
}
