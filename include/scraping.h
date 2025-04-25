#ifndef SCRAPING_H
#define SCRAPING_H

typedef struct {
    double cieplne;
    double wodne;
    double wiatrowe;
    double fotowoltaiczne;
    double inne;
} GenerationData;

typedef struct {
    double CZ, DE, SK, LT, UA, SE;
} ExchangeData;

typedef struct {
    double frequency;
} FrequencyData;

typedef struct {
    GenerationData gen_data;
    ExchangeData exch_data;
    FrequencyData freq_data;
} SharedData;

void fetch_pse_data(const char *url, GenerationData *gen_data, ExchangeData *exch_data, FrequencyData *freq_data);

#endif
