/*!
 * \file      custom_payload.c
 *
 * \brief     Custom payload sending module for LoRaWAN
 *
 * \author    charugaa
 *
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "LoRaMac.h"
#include "utilities.h"
#include "timer.h"
#include "rtc-board.h"
#include "custom_payload.h" // Ukljucujemo samo header fajl

/*!
 * Defines the application port for custom payload
 */
#define CUSTOM_APP_PORT 5

/*!
 * User application data buffer size
 */
#define CUSTOM_APP_DATA_MAX_SIZE 16

/*!
 * User application data
 */
static uint8_t CustomAppData[CUSTOM_APP_DATA_MAX_SIZE];

/*!
 * User application data size
 */
static uint8_t CustomAppDataSize = 0;

/*!
 * Timer for sending payload every 5 minutes
 */
static TimerEvent_t CustomPayloadTimer;

/*!
 * Function to prepare the custom payload
 */
static void PrepareCustomPayload(void)
{
    // Clear the buffer
    memset(CustomAppData, 0, CUSTOM_APP_DATA_MAX_SIZE);

    // Get UNIX timestamp (seconds since epoch)
    extern uint32_t RtcGetTime(void); // Deklaracija funkcije RtcGetTime
    uint32_t timestamp = RtcGetTime();

    // Add timestamp to the payload (4 bytes)
    CustomAppData[0] = (timestamp >> 24) & 0xFF;
    CustomAppData[1] = (timestamp >> 16) & 0xFF;
    CustomAppData[2] = (timestamp >> 8) & 0xFF;
    CustomAppData[3] = timestamp & 0xFF;

    // Add serial number (4 bytes)
    uint32_t serialNumber = 12345678;
    CustomAppData[4] = (serialNumber >> 24) & 0xFF;
    CustomAppData[5] = (serialNumber >> 16) & 0xFF;
    CustomAppData[6] = (serialNumber >> 8) & 0xFF;
    CustomAppData[7] = serialNumber & 0xFF;

    // Set data size
    CustomAppDataSize = 8;
}

/*!
 * Function to send the custom payload
 */
static void CustomSendPayload(void)
{
    McpsReq_t mcpsReq;
    LoRaMacTxInfo_t txInfo;

    // Prepare the payload
    PrepareCustomPayload();

    // Check if the payload can be sent
    if (LoRaMacQueryTxPossible(CustomAppDataSize, &txInfo) != LORAMAC_STATUS_OK)
    {
        printf("Not enough space in the transmission buffer.\n");
        return;
    }

    // Configure the MCPS request
    mcpsReq.Type = MCPS_UNCONFIRMED; // Use unconfirmed messages
    mcpsReq.Req.Unconfirmed.fPort = CUSTOM_APP_PORT;
    mcpsReq.Req.Unconfirmed.fBuffer = CustomAppData;
    mcpsReq.Req.Unconfirmed.fBufferSize = CustomAppDataSize;
    mcpsReq.Req.Unconfirmed.Datarate = DR_0; // Use default data rate

    // Send the request
    if (LoRaMacMcpsRequest(&mcpsReq) == LORAMAC_STATUS_OK)
    {
        printf("Payload sent successfully.\n");
    }
    else
    {
        printf("Failed to send payload.\n");
    }
}

/*!
 * Timer callback for sending payload every 5 minutes
 */
static void OnCustomPayloadTimerEvent(void)
{
    // Stop the timer to avoid overlapping
    TimerStop(&CustomPayloadTimer);

    // Send the custom payload
    CustomSendPayload();

    // Restart the timer for another 5 minutes
    TimerSetValue(&CustomPayloadTimer, 5 * 60 * 1000); // 5 minutes in milliseconds
    TimerStart(&CustomPayloadTimer);
}

/*!
 * Function to initialize the custom payload timer
 */
void InitCustomPayloadTimer(void)
{
    // Initialize the timer
    TimerInit(&CustomPayloadTimer, OnCustomPayloadTimerEvent);

    // Set the timer for 5 minutes
    TimerSetValue(&CustomPayloadTimer, 5 * 60 * 1000); // 5 minutes in milliseconds

    // Start the timer
    TimerStart(&CustomPayloadTimer);

    printf("Custom payload timer started. Payload will be sent every 5 minutes.\n");
}