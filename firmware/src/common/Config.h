/**
 * @file   Config.h
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

#ifndef CONFIG_H_
#define CONFIG_H_

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

/**
 * Save the active configuration to NVM.
 */
void Config_Save(void);

/**
 * Load configuration from NVM into the active configuration.
 *
 * @return True if the NVM configuration was loaded successfully, otherwise
 *         false.
 */
bool Config_Load(void);

/**
 * Get the configuration version of the active configuration.
 *
 * @return Configuration version number.
 */
uint16_t Config_GetVersion(void);

/**
 * Get the network ID from the active configuration.
 *
 * @return Network ID.
 */
uint8_t *Config_GetNetworkId(void);

/**
 * Get the AES-key from the active configuration.
 *
 * @return AES-key.
 */
char *Config_GetAESKey(void);

/**
 * Get the report interval from the active configuration.
 *
 * @return Report interval in seconds.
 */
uint32_t Config_GetReportInterval(void);

/**
 * Get the node ID from the active configuration.
 *
 * @return Node ID.
 */
uint8_t Config_GetNodeId(void);

/**
 * Set the network ID in the active configuration.
 *
 * @param network_id Pointer to network ID.
 */
void Config_SetNetworkId(const uint8_t *network_id_p);

/**
 *  Set the AES-key in the active configuration.
 *
 * @param aes_key_p Pointer to AES-key.
 */
void Config_SetAESKey(const char *aes_key_p);

/**
 * Set the report interval in the active configuration.
 *
 * @param report_interval Report interval in seconds.
 */
void Config_SetReportInterval(uint32_t report_interval);

#endif
