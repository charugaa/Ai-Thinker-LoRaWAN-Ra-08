/*!
 * \file      classA.c
 *
 * \brief     LoRaMac classA device implementation
 *
 * \author    Miguel Luis ( Semtech )
 *
 * \author    Gregory Cristian ( Semtech )
 */

#include <stdio.h>
#include "utilities.h"
#include "LoRaMac.h"
#include "Commissioning.h"

#ifndef ACTIVE_REGION
#warning "No active region defined, LORAMAC_REGION_CN470 will be used as default."
#define ACTIVE_REGION LORAMAC_REGION_EU433
#endif

#define APP_TX_DUTYCYCLE 5000
#define APP_TX_DUTYCYCLE_RND 1000
#define LORAWAN_DEFAULT_DATARATE DR_0
#define LORAWAN_CONFIRMED_MSG_ON true
#define LORAWAN_ADR_ON 1
#define LORAWAN_APP_PORT 2

static uint8_t DevEui[] = LORAWAN_DEVICE_EUI;
static uint8_t AppEui[] = LORAWAN_APPLICATION_EUI;
static uint8_t AppKey[] = LORAWAN_APPLICATION_KEY;

#if (OVER_THE_AIR_ACTIVATION == 0)
static uint8_t NwkSKey[] = LORAWAN_NWKSKEY;
static uint8_t AppSKey[] = LORAWAN_APPSKEY;
static uint32_t DevAddr = LORAWAN_DEVICE_ADDRESS;
#endif

static enum eDeviceState
{
    DEVICE_STATE_INIT,
    DEVICE_STATE_JOIN,
    DEVICE_STATE_SEND,
    DEVICE_STATE_CYCLE,
    DEVICE_STATE_SLEEP
} DeviceState;

static bool NextTx = true;

/*!
 * \brief Timer to handle the application data transmission duty cycle
 */
static TimerEvent_t TxNextPacketTimer;

/*!
 * \brief Function executed on TxNextPacket Timeout event
 */
static void OnTxNextPacketTimerEvent(void)
{
    TimerStop(&TxNextPacketTimer);
    MibRequestConfirm_t mibReq;
    mibReq.Type = MIB_NETWORK_JOINED;

    if (LoRaMacMibGetRequestConfirm(&mibReq) == LORAMAC_STATUS_OK)
    {
        if (mibReq.Param.IsNetworkJoined == true)
        {
            printf("Network joined. Ready to send data.\r\n");
            DeviceState = DEVICE_STATE_SEND;
            NextTx = true;
        }
        else
        {
            printf("Not joined. Retrying...\r\n");
            MlmeReq_t mlmeReq;
            mlmeReq.Type = MLME_JOIN;
            mlmeReq.Req.Join.DevEui = DevEui;
            mlmeReq.Req.Join.AppEui = AppEui;
            mlmeReq.Req.Join.AppKey = AppKey;

            if (LoRaMacMlmeRequest(&mlmeReq) == LORAMAC_STATUS_OK)
            {
                DeviceState = DEVICE_STATE_SLEEP;
            }
            else
            {
                DeviceState = DEVICE_STATE_CYCLE;
            }
        }
    }
}

static void MlmeConfirm(MlmeConfirm_t *mlmeConfirm)
{
    switch (mlmeConfirm->MlmeRequest)
    {
    case MLME_JOIN:
        if (mlmeConfirm->Status == LORAMAC_EVENT_INFO_STATUS_OK)
        {
            printf("Join successful\r\n");
            DeviceState = DEVICE_STATE_SEND;
        }
        else
        {
            printf("Join failed. Retrying...\r\n");
            MlmeReq_t mlmeReq;
            mlmeReq.Type = MLME_JOIN;
            mlmeReq.Req.Join.DevEui = DevEui;
            mlmeReq.Req.Join.AppEui = AppEui;
            mlmeReq.Req.Join.AppKey = AppKey;
            mlmeReq.Req.Join.NbTrials = 8;

            if (LoRaMacMlmeRequest(&mlmeReq) == LORAMAC_STATUS_OK)
            {
                DeviceState = DEVICE_STATE_SLEEP;
            }
            else
            {
                DeviceState = DEVICE_STATE_CYCLE;
            }
        }
        break;

    default:
        printf("Unhandled MLME request: %d\r\n", mlmeConfirm->MlmeRequest);
        break;
    }

    NextTx = true;
}

/**
 * Main entry point for the application.
 */
int app_start(void)
{
    printf("ClassA app start\r\n");

    LoRaMacPrimitives_t LoRaMacPrimitives;
    LoRaMacCallback_t LoRaMacCallbacks;
    MibRequestConfirm_t mibReq;

    DeviceState = DEVICE_STATE_INIT;

    while (1)
    {
        switch (DeviceState)
        {
        case DEVICE_STATE_INIT:
        {
            LoRaMacPrimitives.MacMlmeConfirm = MlmeConfirm;
            LoRaMacInitialization(&LoRaMacPrimitives, &LoRaMacCallbacks, ACTIVE_REGION);

            TimerInit(&TxNextPacketTimer, OnTxNextPacketTimerEvent);

            mibReq.Type = MIB_ADR;
            mibReq.Param.AdrEnable = LORAWAN_ADR_ON;
            LoRaMacMibSetRequestConfirm(&mibReq);

            mibReq.Type = MIB_PUBLIC_NETWORK;
            mibReq.Param.EnablePublicNetwork = LORAWAN_PUBLIC_NETWORK;
            LoRaMacMibSetRequestConfirm(&mibReq);

            DeviceState = DEVICE_STATE_JOIN;
            break;
        }
        case DEVICE_STATE_JOIN:
        {
#if (OVER_THE_AIR_ACTIVATION != 0)
            MlmeReq_t mlmeReq;

            mlmeReq.Type = MLME_JOIN;
            mlmeReq.Req.Join.DevEui = DevEui;
            mlmeReq.Req.Join.AppEui = AppEui;
            mlmeReq.Req.Join.AppKey = AppKey;
            mlmeReq.Req.Join.NbTrials = 8;

            if (LoRaMacMlmeRequest(&mlmeReq) == LORAMAC_STATUS_OK)
            {
                DeviceState = DEVICE_STATE_SLEEP;
            }
            else
            {
                DeviceState = DEVICE_STATE_CYCLE;
            }
#else
            mibReq.Type = MIB_NET_ID;
            mibReq.Param.NetID = LORAWAN_NETWORK_ID;
            LoRaMacMibSetRequestConfirm(&mibReq);

            mibReq.Type = MIB_DEV_ADDR;
            mibReq.Param.DevAddr = DevAddr;
            LoRaMacMibSetRequestConfirm(&mibReq);

            mibReq.Type = MIB_NWK_SKEY;
            mibReq.Param.NwkSKey = NwkSKey;
            LoRaMacMibSetRequestConfirm(&mibReq);

            mibReq.Type = MIB_APP_SKEY;
            mibReq.Param.AppSKey = AppSKey;
            LoRaMacMibSetRequestConfirm(&mibReq);

            mibReq.Type = MIB_NETWORK_JOINED;
            mibReq.Param.IsNetworkJoined = true;
            LoRaMacMibSetRequestConfirm(&mibReq);

            DeviceState = DEVICE_STATE_SEND;
#endif
            break;
        }
        case DEVICE_STATE_SEND:
        {
            if (NextTx == true)
            {
                printf("Sending data...\r\n");
                NextTx = false;
            }

            DeviceState = DEVICE_STATE_CYCLE;
            break;
        }
        case DEVICE_STATE_CYCLE:
        {
            DeviceState = DEVICE_STATE_SLEEP;

            TimerSetValue(&TxNextPacketTimer, APP_TX_DUTYCYCLE + randr(0, APP_TX_DUTYCYCLE_RND));
            TimerStart(&TxNextPacketTimer);
            break;
        }
        case DEVICE_STATE_SLEEP:
        {
            TimerLowPowerHandler();
            break;
        }
        default:
            DeviceState = DEVICE_STATE_INIT;
            break;
        }
    }

    return 0;
}