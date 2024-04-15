/*
 * Copyright (C) 2024 jean-Ed BQ
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Remonte des infos pression temperature
 *
 * @author      jean-Ed BQ <jbournatquerat@gmail.com>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>
#include "xtimer.h"
#include "lpsxxx.h"
#include "lpsxxx_params.h"
#include "fmt.h"
#include "semtech_loramac.h"

static const uint8_t deveui[LORAMAC_DEVEUI_LEN] = { 0xca, 0xfe, 0xba, 0xbe, 0x85, 0x1a, 0x53, 0x07 };
static const uint8_t appkey[LORAMAC_APPKEY_LEN] = { 0xCD, 0xD2, 0xD3, 0x43, 0x04, 0xB6, 0x66, 0x40, 0x66, 0x9B, 0x19, 0x6B, 0x1F, 0x80, 0x24, 0xB9 };
static const uint8_t appeui[LORAMAC_APPEUI_LEN] = {0xca, 0xfe, 0xba, 0xbe, 0x00, 0x00, 0x00, 0x00};
extern semtech_loramac_t loramac;


int main(void)
{
    lpsxxx_t dev;

    printf("Test application for %s pressure sensor\n\n", LPSXXX_SAUL_NAME);
    printf("Initializing %s sensor\n", LPSXXX_SAUL_NAME);
    if (lpsxxx_init(&dev, &lpsxxx_params[0]) != LPSXXX_OK) {
        puts("Initialization failed");
        return 1;
    }

    // Initialisation de LoRaMAC
    semtech_loramac_init(&loramac);

    // Configuration du DevEUI et de l'AppKey
    semtech_loramac_set_deveui(&loramac, deveui);
    semtech_loramac_set_appkey(&loramac, appkey);
    semtech_loramac_set_appeui(&loramac, appeui);

    semtech_loramac_set_dr(&loramac, 5);
    semtech_loramac_set_adr(&loramac, true);

    
    switch(semtech_loramac_join(&loramac,LORAMAC_JOIN_OTAA)){
        case SEMTECH_LORAMAC_DUTYCYCLE_RESTRICTED:
            puts("Cannot join: dutycycle restriction");
            return 1;
        case SEMTECH_LORAMAC_BUSY:
            puts("Cannot join: mac is busy");
            return 1;
        case SEMTECH_LORAMAC_JOIN_FAILED:
            puts("Join procedure failed!");
            return 1;
        case SEMTECH_LORAMAC_ALREADY_JOINED:
            puts("Warning: already joined!");
            return 1;
        case SEMTECH_LORAMAC_JOIN_SUCCEEDED:
            puts("Join procedure succeeded!");
            break;
        default: /* should not happen */
            break;
    }

    uint16_t pres;
    int16_t temp;

    while (1) {
        lpsxxx_enable(&dev);
        xtimer_sleep(1); // attendre un peu pour que les mesures se complètent

        lpsxxx_read_temp(&dev, &temp);
        lpsxxx_read_pres(&dev, &pres);
        lpsxxx_disable(&dev);
        int temp_abs = temp / 100;
        temp -= temp_abs * 100;
        uint8_t pres_bytes[2];
        uint8_t temp_bytes[2];
        pres_bytes[0] = (uint8_t)(pres & 0xFF); // Octet de poids faible
        pres_bytes[1] = (uint8_t)((pres >> 8) & 0xFF); // Octet de poids fort

        temp_bytes[0] = (uint8_t)(temp & 0xFF); // Octet de poids faible
        temp_bytes[1] = (uint8_t)((temp >> 8) & 0xFF); // Octet de poids fort

// Créer un tableau pour le message complet
        uint8_t message[4];
        message[0] = pres_bytes[0];
        message[1] = pres_bytes[1];
        message[2] = temp_bytes[0];
        message[3] = temp_bytes[1];


        // Envoi du message via LoRaWAN
        switch (semtech_loramac_send(&loramac,message, 4)) {
            case SEMTECH_LORAMAC_NOT_JOINED:
                puts("Cannot send: not joined");
                return 1;

            case SEMTECH_LORAMAC_DUTYCYCLE_RESTRICTED:
                puts("Cannot send: dutycycle restriction");
                return 1;

            case SEMTECH_LORAMAC_BUSY:
                puts("Cannot send: MAC is busy");
                return 1;

            case SEMTECH_LORAMAC_TX_ERROR:
                puts("Cannot send: error");
                return 1;

            case SEMTECH_LORAMAC_TX_CNF_FAILED:
                puts("Fail to send: no ACK received");
                return 1;

            default:
                puts("Message sent with success");
                break;
        }

        // Attendre un certain temps avant la prochaine mesure
        xtimer_sleep(10);
    }

    return 0;
}
