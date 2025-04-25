#include <stdio.h>
#include <sys/stat.h>
#include <time.h>
#include "csv_logger.h"

static void ensure_header(const char *filename, const char *header) {
    struct stat st;
    if (stat(filename, &st) != 0 || st.st_size == 0) {
        FILE *fp = fopen(filename, "w");
        if (fp) {
            fprintf(fp, "Timestamp,%s\n", header);
            fclose(fp);
        } else {
            printf("[ERROR] Nie mo?na utworzy? pliku %s!\n", filename);
        }
    }
}

void ensure_all_csv_headers() {
    ensure_header(CSV_GENERATION, "Cieplne,Wodne,Wiatrowe,PV,Inne");
    ensure_header(CSV_EXCHANGE,   "CZ,DE,SK,LT,UA,SE");
    ensure_header(CSV_FREQUENCY,  "Frequency");
}

static void get_timestamp(char *buffer, size_t size) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(buffer, size, "%d/%m/%Y %H:%M", t);
}

void log_data_to_csv(SharedData *data) {
    char timestamp[20];
    get_timestamp(timestamp, sizeof(timestamp));

    FILE *fp_gen = fopen(CSV_GENERATION, "a");
    if (fp_gen) {
        fprintf(fp_gen, "%s,%.2f,%.2f,%.2f,%.2f,%.2f\n", timestamp,
                data->gen_data.cieplne,
                data->gen_data.wodne,
                data->gen_data.wiatrowe,
                data->gen_data.fotowoltaiczne,
                data->gen_data.inne);
        fclose(fp_gen);
    }

    FILE *fp_ex = fopen(CSV_EXCHANGE, "a");
    if (fp_ex) {
        fprintf(fp_ex, "%s,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f\n", timestamp,
                data->exch_data.CZ,
                data->exch_data.DE,
                data->exch_data.SK,
                data->exch_data.LT,
                data->exch_data.UA,
                data->exch_data.SE);
        fclose(fp_ex);
    }

    FILE *fp_freq = fopen(CSV_FREQUENCY, "a");
    if (fp_freq) {
        fprintf(fp_freq, "%s,%.2f\n", timestamp, data->freq_data.frequency);
        fclose(fp_freq);
    }
}
