/**
 * @file   Config.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2018-03-08 (Last edit)
 * @brief  Implementation of a module handling storage and validation of
 *         configuration data.
 */

/*
This file is part of SillyCat firmware.

SillyCat firmware is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

SillyCat firmware is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with SillyCat firmware.  If not, see <http://www.gnu.org/licenses/>.
*/

//////////////////////////////////////////////////////////////////////////
//INCLUDES
//////////////////////////////////////////////////////////////////////////

//NOTE: Include common.h before all other headers
#include "common.h"

#include <string.h>
#include <avr/eeprom.h>

#include "CRC.h"
#include "libDebug.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

#define CONFIG_VERSION 1

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

struct config_t
{
    uint16_t version;
    uint8_t network_id[6];
    uint32_t report_interval;
    char aes_key[17];
    uint8_t node_id;
    uint16_t crc;
};

//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

static struct config_t active_config;
static struct config_t EEMEM nvm_config =
{
    .version = CONFIG_VERSION,
    .network_id = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF},
    .report_interval = 60,
    .aes_key = "1DUMMYKEYFOOBAR1",
    .node_id = 128,
    .crc = 0x45e7
};

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

static bool IsConfigValid(const struct config_t *config_p);
static void UpdateCRC(struct config_t *config_p);

//////////////////////////////////////////////////////////////////////////
//INTERUPT SERVICE ROUTINES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

void Config_Save(void)
{
    UpdateCRC(&active_config);
    eeprom_write_block(&active_config, &nvm_config, sizeof(active_config));

    INFO("Configuration saved");
}

bool Config_Load(void)
{
    bool status = false;

    /**
     * Load the configuration to a temporary variable so it can be validated
     * without changing the active configuration.
     */
    struct config_t new_config;
    eeprom_read_block(&new_config, &nvm_config, sizeof(active_config));

    if(IsConfigValid(&new_config))
    {
        active_config = new_config;
        status = true;
        INFO("Configuration loaded");
    }

    return status;
}

uint16_t Config_GetVersion(void)
{
    return active_config.version;
}

uint8_t *Config_GetNetworkId(void)
{
    return active_config.network_id;
}

char *Config_GetAESKey(void)
{
    return active_config.aes_key;
}

uint32_t Config_GetReportInterval(void)
{
    return active_config.report_interval;
}

uint8_t Config_GetNodeId(void)
{
    return active_config.node_id;
}

void Config_SetNetworkId(const uint8_t *network_id_p)
{
    sc_assert(network_id_p != NULL);

    memcpy(active_config.network_id, network_id_p, sizeof(active_config.network_id));
}

void Config_SetAESKey(const char *aes_key_p)
{
    sc_assert(aes_key_p != NULL);

    memcpy(active_config.aes_key, aes_key_p, sizeof(active_config.aes_key));
}

void Config_SetReportInterval(uint32_t report_interval)
{
    sc_assert(report_interval > 0);

    active_config.report_interval = report_interval;
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////

static bool IsConfigValid(const struct config_t *config_p)
{
    sc_assert(config_p != NULL);

    return config_p->crc == CRC_16(config_p, offsetof(struct config_t, crc));
}

static void UpdateCRC(struct config_t *config_p)
{
    sc_assert(config_p != NULL);

    config_p->crc = CRC_16(config_p, offsetof(struct config_t, crc));
}
