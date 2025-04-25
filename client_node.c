#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "open62541.h"
#include "client_node.h"
#include "scraping.h"
#include "log.h"

#define OPCUA_SERVER_URL "opc.tcp://192.168.192.26:4840"  // IP serwera OPC UA na C4
#define LOG_FILE_PATH "energy_log.csv"

// Tworzenie klienta OPC UA
static UA_Client* create_and_start_opc_ua_client(const char *server_url) {
    UA_Client *client = UA_Client_new();
    UA_ClientConfig_setDefault(UA_Client_getConfig(client));

    UA_StatusCode status = UA_Client_connect(client, server_url);
    if (status != UA_STATUSCODE_GOOD) {
        log_error("? B??d po??czenia z serwerem OPC UA: %s\n", UA_StatusCode_name(status));
        UA_Client_delete(client);
        return NULL;
    }

    log_info("? Po??czono z serwerem OPC UA: %s\n", server_url);
    return client;
}

// Od??czanie klienta
static void delete_opc_ua_client(UA_Client *client) {
    UA_Client_disconnect(client);
    UA_Client_delete(client);
}

// Funkcja pomocnicza do wysy?ki pojedynczej zmiennej
static void write_to_double_node(UA_Client *client, UA_UInt32 nodeIdNumeric, double value) {
    UA_Variant valueVariant;
    UA_Variant_setScalar(&valueVariant, &value, &UA_TYPES[UA_TYPES_DOUBLE]);

    UA_StatusCode status = UA_Client_writeValueAttribute(client, UA_NODEID_NUMERIC(1, nodeIdNumeric), &valueVariant);
    if (status != UA_STATUSCODE_GOOD) {
        log_error("? B??d zapisu warto?ci do NodeId %u: %s\n", nodeIdNumeric, UA_StatusCode_name(status));
    }
}

// G?�wna funkcja wysy?ki
void send_data(SharedData *data) {
    UA_Client *client = create_and_start_opc_ua_client(OPCUA_SERVER_URL);
    if (client == NULL) {
        return;
    }

    // Produkcja energii
    write_to_double_node(client, 4001, data->gen_data.cieplne);
    write_to_double_node(client, 4002, data->gen_data.wodne);
    write_to_double_node(client, 4003, data->gen_data.wiatrowe);
    write_to_double_node(client, 4004, data->gen_data.fotowoltaiczne);
    write_to_double_node(client, 4005, data->gen_data.inne);

    // Przesy?y
    write_to_double_node(client, 4006, data->exch_data.CZ);
    write_to_double_node(client, 4007, data->exch_data.DE);
    write_to_double_node(client, 4008, data->exch_data.SK);
    write_to_double_node(client, 4009, data->exch_data.LT);
    write_to_double_node(client, 4010, data->exch_data.UA);
    write_to_double_node(client, 4011, data->exch_data.SE);

    // Cz?stotliwo??
    write_to_double_node(client, 4012, data->freq_data.frequency);

    log_info("? Wszystkie dane zosta?y wys?ane do serwera OPC UA\n");

    delete_opc_ua_client(client);
}
