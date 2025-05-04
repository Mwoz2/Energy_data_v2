#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "open62541.h"
#include "client_node.h"
#include "scraping.h"
#include "log.h"

#define OPCUA_SERVER_URL "opc.tcp://192.168.192.26:4840" 
#define LOG_FILE_PATH "energy_log.csv"


static UA_Client* create_and_start_opc_ua_client(const char *server_url) {
    UA_Client *client = UA_Client_new();
    UA_ClientConfig_setDefault(UA_Client_getConfig(client));

    UA_StatusCode status = UA_Client_connect(client, server_url);
    if (status != UA_STATUSCODE_GOOD) {
        log_error("Blad polaczenia z serwerem OPC UA: %s\n", UA_StatusCode_name(status));
        UA_Client_delete(client);
        return NULL;
    }

    log_info("Polaczono z serwerem OPC UA: %s\n", server_url);
    return client;
}


static void delete_opc_ua_client(UA_Client *client) {
    UA_Client_disconnect(client);
    UA_Client_delete(client);
}

static void write_to_double_node(UA_Client *client, UA_UInt32 nodeIdNumeric, double value) {
    UA_Variant valueVariant;
    UA_Variant_setScalar(&valueVariant, &value, &UA_TYPES[UA_TYPES_DOUBLE]);

    UA_StatusCode status = UA_Client_writeValueAttribute(client, UA_NODEID_NUMERIC(1, nodeIdNumeric), &valueVariant);
    if (status != UA_STATUSCODE_GOOD) {
        log_error("Blad zapisu do NodeId %u: %s\n", nodeIdNumeric, UA_StatusCode_name(status));
    }
}

static void write_to_string_node(UA_Client *client, UA_UInt32 nodeIdNumeric, const char *text) {
    UA_Variant value;
    UA_Variant_init(&value);

    UA_String uaString = UA_STRING_ALLOC(text);
    UA_Variant_setScalar(&value, &uaString, &UA_TYPES[UA_TYPES_STRING]);

    UA_StatusCode status = UA_Client_writeValueAttribute(client, UA_NODEID_NUMERIC(1, nodeIdNumeric), &value);

if (status != UA_STATUSCODE_GOOD){
    log_error("Blad wyslania czasu na serwer %u: %s\n", nodeIdNumeric, UA_StatusCode_name(status));
    log_info("Restart programu za 5s\n");
    exit(1);
}
UA_String_clear(&uaString);
}

void send_data(SharedData *data) {
    UA_Client *client = create_and_start_opc_ua_client(OPCUA_SERVER_URL);
    if (client == NULL) {
        return;
    }

    write_to_double_node(client, 4002, data->gen_data.cieplne);
    write_to_double_node(client, 4003, data->gen_data.wodne);
    write_to_double_node(client, 4004, data->gen_data.wiatrowe);
    write_to_double_node(client, 4005, data->gen_data.fotowoltaiczne);
    write_to_double_node(client, 4006, data->gen_data.inne);
    write_to_double_node(client, 4007, data->exch_data.CZ);
    write_to_double_node(client, 4008, data->exch_data.DE);
    write_to_double_node(client, 4009, data->exch_data.SK);
    write_to_double_node(client, 4010, data->exch_data.LT);
    write_to_double_node(client, 4011, data->exch_data.UA);
    write_to_double_node(client, 4012, data->exch_data.SE);
    write_to_double_node(client, 4013, data->freq_data.frequency);

    char datetime_str[20];
    time_t now = time(NULL);
    strftime(datetime_str, sizeof(datetime_str), "%d/%m/%Y %H:%M", localtime(&now));
    write_to_string_node(client, 4014, datetime_str);

    log_info("Dane wyslane do serwera OPC UA\n");

    delete_opc_ua_client(client);
}
