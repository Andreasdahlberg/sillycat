/**
 * @file   Config.c
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2016-01-31 (Last edit)
 * @brief  Implementation of Config
 *
 * Detailed description of file.
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

#include "libDebug.h"

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

#define CONFIG_VERSION 1

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

typedef struct
{
    uint16_t version;
    uint8_t network_id[6];
    char aes_key[17]; //TODO: Store in flash+WP? This is not secure. Store null?
    uint32_t report_interval_s;
    uint8_t node_id;
    uint8_t node_role;
} config_type;

//////////////////////////////////////////////////////////////////////////
//VARIABLES
//////////////////////////////////////////////////////////////////////////

static config_type active_config;
static config_type EEMEM nvm_config =
{
    CONFIG_VERSION,
    {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF},
    "1DUMMYKEYFOOBAR1",
    60,
    128,
    2,
};

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

static bool ValidateConfig(config_type *config);

//////////////////////////////////////////////////////////////////////////
//INTERUPT SERVICE ROUTINES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//FUNCTIONS
//////////////////////////////////////////////////////////////////////////

///
/// @brief Save the active configuration to NVM
///
/// @param  None
/// @return bool true if active configuration was valid, otherwise false
///
bool Config_Save(void)
{
    bool status = false;

    if(ValidateConfig(&active_config) == true)
    {
        eeprom_write_block(&active_config, &nvm_config, sizeof(active_config));

        status = true;
        INFO("Configuration saved");
    }
    return status;
}

///
/// @brief Load an configuration from NVM into the active configuration
///
/// @param  None
/// @return bool true if the NVM configuration was valid, otherwise false
///
bool Config_Load(void)
{
    bool status = false;
    config_type new_config;

    sc_assert(CONFIG_VERSION == eeprom_read_word(&nvm_config.version));

    eeprom_read_block(&new_config, &nvm_config, sizeof(active_config));

    if(ValidateConfig(&new_config) == true)
    {
        active_config = new_config;
        status = true;
        INFO("Configuration loaded");
    }
    return status;
}

///
/// @brief Get the configuration version of the active configuration
///
/// @param  None
/// @return uint16_t Configuration version
///
uint16_t Config_GetVersion(void)
{
    return active_config.version;
}

///
/// @brief Get a pointer to the network id in the active configuration
///
/// @param  None
/// @return uint8_t* Pointer to network id
///
uint8_t *Config_GetNetworkId(void)
{
    return active_config.network_id;
}

///
/// @brief Get a pointer to the AES-key in the active configuration
///
/// @param  None
/// @return char* Pointer to AES-key
///
char *Config_GetAESKey(void)
{
    return active_config.aes_key;
}

///
/// @brief Get the report interval from the active configuration
///
/// @param  None
/// @return uint32_t Report interval in seconds
///
uint32_t Config_GetReportInterval(void)
{
    return 10;//active_config.report_interval_s;
}

///
/// @brief Get the node id from the active configuration
///
/// @param  None
/// @return uint8_t Node id
///
uint8_t Config_GetNodeId(void)
{
    return active_config.node_id;
}

///
/// @brief Get the node role from the active configuration
///
/// @param  None
/// @return uint8_t Node role, (master=1, node=2, listener=3)
///
uint8_t Config_GetNodeRole(void)
{
    return active_config.node_role;
}

///
/// @brief Set the network id in the active configuration
///
/// @param  network_id Pointer to network id
/// @return None
///
void Config_SetNetworkId(const uint8_t *network_id)
{
    sc_assert(network_id != NULL);

    memcpy(active_config.network_id, network_id, sizeof(active_config.network_id));
    return;
}

///
/// @brief Set the AES-key in the active configuration
///
/// @param  aes_key Pointer to AES-key
/// @return None
///
void Config_SetAESKey(const char *aes_key)
{
    sc_assert(aes_key != NULL);

    memcpy(active_config.aes_key, aes_key, sizeof(active_config.aes_key));
    return;
}

///
/// @brief Set the report interval in the active configuration
///
/// @param  report_interval Report interval in seconds
/// @return None
///
void Config_SetReportInterval(uint32_t report_interval)
{
    sc_assert(report_interval > 0);

    active_config.report_interval_s = report_interval;
    return;
}

///
/// @brief Set the node role in the active configuration
///
/// @param  node_role Node role (master=1, node=2, listener=3)
/// @return None
///
void Config_SetNodeRole(uint8_t node_role)
{
    sc_assert(node_role > 0);

    active_config.node_role = node_role;
    return;
}

//////////////////////////////////////////////////////////////////////////
//LOCAL FUNCTIONS
//////////////////////////////////////////////////////////////////////////

static bool ValidateConfig(config_type *config)
{
    sc_assert(config != NULL);

    return (config->node_id != 0 &&
            (config->node_role > 0 && config->node_role < 4));
}