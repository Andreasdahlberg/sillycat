# Hardware description

## Main Unit

### BOM
| Qty | Line Item              | Manufacturer              | MPN                    | Octopart URL                                                      | Schematic Reference                  | 
|-----|------------------------|---------------------------|------------------------|-------------------------------------------------------------------|--------------------------------------|  
| 1   | RFM69HW-868S2          | HOPERF                    | RFM69HW-868S2          | https://octopart.com/rfm69hw-868s2-hoperf-95997182                | U3                                   | 
| 1   | 105133-0001            | Molex                     | 105133-0001            | https://octopart.com/105133-0001-molex-22605641                   | U7                                   | 
| 1   | MC-306 32.768K-E3:ROHS | Epson                     | MC-306 32.768K-E3:ROHS | https://octopart.com/mc-306+32.768k-e3%3Arohs-epson-39545791      | U6                                   | 
| 1   | NHD-2.23-12832UCY3     | Newhaven Display          | NHD-2.23-12832UCY3     | https://octopart.com/nhd-2.23-12832ucy3-newhaven+display-20012478 | N/A                                  | 
| 1   | MCP79510-I/MS          | Microchip                 | MCP79510-I/MS          | https://octopart.com/mcp79510-i%2Fms-microchip-20093826           | U4                                   | 
| 1   | ATMEGA328P-AUR         | Microchip                 | ATMEGA328P-AUR         | https://octopart.com/atmega328p-aur-microchip-77760216            | U1                                   | 
| 1   | CL21A106KPFNNNG        | Samsung                   | CL21A106KPFNNNG        | https://octopart.com/cl21a106kpfnnng-samsung-21703273             | C15                                  | 
| 1   | CL21B103KAANNNC        | Samsung                   | CL21B103KAANNNC        | https://octopart.com/cl21b103kaannnc-samsung-19832247             | C4                                   | 
| 2   | CL21C180JBANNNC        | Samsung                   | CL21C180JBANNNC        | https://octopart.com/cl21c180jbannnc-samsung-9287384              | C3,C5                                | 
| 5   | CL21B104KACNNNC        | Samsung                   | CL21B104KACNNNC        | https://octopart.com/cl21b104kacnnnc-samsung-19832262             | C1,C2,C7,C10,C12                     | 
| 5   | CL21B105KAFNNNE        | Samsung                   | CL21B105KAFNNNE        | https://octopart.com/cl21b105kafnnne-samsung-9301147              | C6,C8,C11,C13,C14                    | 
| 1   | LD1117V33              | STMicroelectronics        | LD1117V33              | https://octopart.com/ld1117v33-stmicroelectronics-526800          | U8                                   | 
| 3   | RC2012J101CS           | Samsung                   | RC2012J101CS           | https://octopart.com/rc2012j101cs-samsung-63172                   | R2,R3,R5                             | 
| 1   | RC2012F472CS           | Samsung                   | RC2012F472CS           | https://octopart.com/rc2012f472cs-samsung-26451238                | R1                                   | 
| 1   | 22284200               | Molex                     | 22284200               | https://octopart.com/0022284200-molex-17769515                    | U2                                   | 
| 1   | 61200621621            | Würth Elektronik          | 61200621621            | https://octopart.com/61200621621-w%C3%BCrth+elektronik-32855456   | U11                                  | 
| 1   | BH32T-C                | Memory Protection Devices | BH32T-C                | https://octopart.com/bh32t-c-memory+protection+devices-30310562   | U5                                   | 
| 10  | RC2012F103CS           | Samsung                   | RC2012F103CS           | https://octopart.com/rc2012f103cs-samsung-22085131                | R4,R6,R7,R8,R9,R10,R11,R12,R13,R14   | 
| 1   | 22284040               | Molex                     | 22-28-4040             | https://octopart.com/22-28-4040-molex-278172                      | J1,J4                                | 

## Links
[Octopart BOM](https://octopart.com/bom-tool/QjR0VuVt)

[OSHPark PCB](https://oshpark.com/shared_projects/RTkTkiwv)

## Node Unit

## Gerbers
[NI Ultiboard](https://en.wikipedia.org/wiki/NI_Ultiboard) is used to generate the gerber files.

1. [Remove alignment points](https://docs.oshpark.com/design-tools/ni-ultiboard/removing-alignment-points/)
1. Click on File->Export and select the *Exports* tab
1. Check the Gerber RS-274X option and select the following layers:
	* Copper(all)
	* Board Outline
	* Silkscreen Top
	* Silkscreen Bottom
	* Solder Mask Top
	* Solder Mask Bottom
	* Drill(all)
1. Check the NC drill and select the following options:
	* Imperial
	* Coordinate format 2.4
