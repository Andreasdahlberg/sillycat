/**
 * @file   Config.h
 * @Author Andreas Dahlberg (andreas.dahlberg90@gmail.com)
 * @date   2016-05-08 (Last edit)
 * @brief  Header of config
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

#ifndef CONFIG_H_
#define CONFIG_H_

//////////////////////////////////////////////////////////////////////////
//DEFINES
//////////////////////////////////////////////////////////////////////////

#define CONFIG_EUI_ENABLED 1
#define CONFIG_EUI_LENGTH 6

//////////////////////////////////////////////////////////////////////////
//TYPE DEFINITIONS
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////

bool Config_Save(void);
bool Config_Load(void);
uint16_t Config_GetVersion(void);
uint8_t *Config_GetNetworkId(void);
char *Config_GetAESKey(void);
uint32_t Config_GetReportInterval(void);
uint8_t Config_GetNodeId(void);
uint8_t Config_GetNodeRole(void);
void Config_SetNetworkId(const uint8_t *network_id);
void Config_SetAESKey(const char *aes_key);
void Config_SetReportInterval(uint32_t);
void Config_SetNodeRole(uint8_t);

#endif /* CONFIG_H_ */