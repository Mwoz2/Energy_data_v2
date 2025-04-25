#ifndef CSV_LOGGER_H
#define CSV_LOGGER_H

#include "scraping.h"

#define CSV_GENERATION "generation_log.csv"
#define CSV_EXCHANGE   "exchange_log.csv"
#define CSV_FREQUENCY  "frequency_log.csv"

void ensure_all_csv_headers();
void log_data_to_csv(SharedData *data);

#endif // CSV_LOGGER_H
