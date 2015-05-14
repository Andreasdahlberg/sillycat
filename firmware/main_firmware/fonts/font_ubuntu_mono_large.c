/*
**  Font data for Ubuntu Mono 36pt
*/

#include <stdint.h>
#include <avr/pgmspace.h>
#include "font_ubuntu_mono.h"

/* Character bitmaps for Ubuntu Mono 36pt */
const uint8_t ubuntuMono_36ptBitmaps[] PROGMEM =
{
	/* @0 ' ' (2 pixels wide) */
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //
	0x00, //

	/* @30 '0' (20 pixels wide) */
	0x01, 0xF8, 0x00, //        ######
	0x07, 0xFE, 0x00, //      ##########
	0x0F, 0xFF, 0x00, //     ############
	0x1F, 0x0F, 0x80, //    #####    #####
	0x3E, 0x07, 0xC0, //   #####      #####
	0x3C, 0x03, 0xC0, //   ####        ####
	0x78, 0x01, 0xE0, //  ####          ####
	0x78, 0x01, 0xE0, //  ####          ####
	0x78, 0x01, 0xE0, //  ####          ####
	0xF0, 0x00, 0xF0, // ####            ####
	0xF0, 0x00, 0xF0, // ####            ####
	0xF0, 0x00, 0xF0, // ####            ####
	0xF0, 0xE0, 0xF0, // ####    ###     ####
	0xF1, 0xF0, 0xF0, // ####   #####    ####
	0xF1, 0xF0, 0xF0, // ####   #####    ####
	0xF1, 0xF0, 0xF0, // ####   #####    ####
	0xF1, 0xF0, 0xF0, // ####   #####    ####
	0xF0, 0xE0, 0xF0, // ####    ###     ####
	0xF0, 0x00, 0xF0, // ####            ####
	0xF0, 0x00, 0xF0, // ####            ####
	0xF0, 0x00, 0xF0, // ####            ####
	0x78, 0x01, 0xE0, //  ####          ####
	0x78, 0x01, 0xE0, //  ####          ####
	0x78, 0x01, 0xE0, //  ####          ####
	0x3C, 0x03, 0xC0, //   ####        ####
	0x3E, 0x07, 0xC0, //   #####      #####
	0x1F, 0x0F, 0x80, //    #####    #####
	0x0F, 0xFF, 0x00, //     ############
	0x07, 0xFE, 0x00, //      ##########
	0x01, 0xF8, 0x00, //        ######

	/* @120 '1' (17 pixels wide) */
	0x00, 0xE0, 0x00, //         ###
	0x01, 0xE0, 0x00, //        ####
	0x03, 0xE0, 0x00, //       #####
	0x0F, 0xE0, 0x00, //     #######
	0x1F, 0xE0, 0x00, //    ########
	0x7F, 0xE0, 0x00, //  ##########
	0xFD, 0xE0, 0x00, // ###### ####
	0x71, 0xE0, 0x00, //  ###   ####
	0x41, 0xE0, 0x00, //  #     ####
	0x01, 0xE0, 0x00, //        ####
	0x01, 0xE0, 0x00, //        ####
	0x01, 0xE0, 0x00, //        ####
	0x01, 0xE0, 0x00, //        ####
	0x01, 0xE0, 0x00, //        ####
	0x01, 0xE0, 0x00, //        ####
	0x01, 0xE0, 0x00, //        ####
	0x01, 0xE0, 0x00, //        ####
	0x01, 0xE0, 0x00, //        ####
	0x01, 0xE0, 0x00, //        ####
	0x01, 0xE0, 0x00, //        ####
	0x01, 0xE0, 0x00, //        ####
	0x01, 0xE0, 0x00, //        ####
	0x01, 0xE0, 0x00, //        ####
	0x01, 0xE0, 0x00, //        ####
	0x01, 0xE0, 0x00, //        ####
	0x01, 0xE0, 0x00, //        ####
	0x01, 0xE0, 0x00, //        ####
	0x7F, 0xFF, 0x80, //  ################
	0x7F, 0xFF, 0x80, //  ################
	0x7F, 0xFF, 0x80, //  ################

	/* @210 '2' (19 pixels wide) */
	0x07, 0xF8, 0x00, //      ########
	0x1F, 0xFE, 0x00, //    ############
	0x7F, 0xFF, 0x00, //  ###############
	0xFC, 0x1F, 0x80, // ######     ######
	0x70, 0x07, 0x80, //  ###         ####
	0x20, 0x07, 0xC0, //   #          #####
	0x00, 0x03, 0xC0, //               ####
	0x00, 0x03, 0xC0, //               ####
	0x00, 0x03, 0xC0, //               ####
	0x00, 0x03, 0xC0, //               ####
	0x00, 0x07, 0xC0, //              #####
	0x00, 0x07, 0x80, //              ####
	0x00, 0x0F, 0x80, //             #####
	0x00, 0x1F, 0x00, //            #####
	0x00, 0x3E, 0x00, //           #####
	0x00, 0x7E, 0x00, //          ######
	0x00, 0xFC, 0x00, //         ######
	0x01, 0xF8, 0x00, //        ######
	0x03, 0xF0, 0x00, //       ######
	0x07, 0xE0, 0x00, //      ######
	0x07, 0xC0, 0x00, //      #####
	0x0F, 0x80, 0x00, //     #####
	0x1F, 0x00, 0x00, //    #####
	0x3E, 0x00, 0x00, //   #####
	0x3C, 0x00, 0x00, //   ####
	0x38, 0x00, 0x00, //   ###
	0x78, 0x00, 0x00, //  ####
	0x7F, 0xFF, 0xE0, //  ##################
	0x7F, 0xFF, 0xE0, //  ##################
	0x7F, 0xFF, 0xE0, //  ##################

	/* @300 '3' (18 pixels wide) */
	0x07, 0xF0, 0x00, //      #######
	0x3F, 0xFC, 0x00, //   ############
	0x7F, 0xFE, 0x00, //  ##############
	0x38, 0x1F, 0x00, //   ###      #####
	0x20, 0x0F, 0x00, //   #         ####
	0x00, 0x07, 0x80, //              ####
	0x00, 0x07, 0x80, //              ####
	0x00, 0x07, 0x80, //              ####
	0x00, 0x07, 0x80, //              ####
	0x00, 0x07, 0x80, //              ####
	0x00, 0x0F, 0x00, //             ####
	0x00, 0x1F, 0x00, //            #####
	0x00, 0x3E, 0x00, //           #####
	0x07, 0xFC, 0x00, //      #########
	0x07, 0xF8, 0x00, //      ########
	0x07, 0xFE, 0x00, //      ##########
	0x00, 0x3F, 0x00, //           ######
	0x00, 0x0F, 0x80, //             #####
	0x00, 0x07, 0xC0, //              #####
	0x00, 0x03, 0xC0, //               ####
	0x00, 0x03, 0xC0, //               ####
	0x00, 0x03, 0xC0, //               ####
	0x00, 0x03, 0xC0, //               ####
	0x00, 0x03, 0xC0, //               ####
	0x00, 0x07, 0xC0, //              #####
	0x00, 0x0F, 0x80, //             #####
	0x60, 0x1F, 0x00, //  ##        #####
	0xFF, 0xFF, 0x00, // ################
	0xFF, 0xFC, 0x00, // ##############
	0x1F, 0xF0, 0x00, //    #########

	/* @390 '4' (20 pixels wide) */
	0x00, 0x1F, 0x00, //            #####
	0x00, 0x3F, 0x00, //           ######
	0x00, 0x7F, 0x00, //          #######
	0x00, 0x7F, 0x00, //          #######
	0x00, 0xFF, 0x00, //         ########
	0x01, 0xEF, 0x00, //        #### ####
	0x01, 0xEF, 0x00, //        #### ####
	0x03, 0xCF, 0x00, //       ####  ####
	0x07, 0x8F, 0x00, //      ####   ####
	0x07, 0x8F, 0x00, //      ####   ####
	0x0F, 0x0F, 0x00, //     ####    ####
	0x0E, 0x0F, 0x00, //     ###     ####
	0x1E, 0x0F, 0x00, //    ####     ####
	0x1C, 0x0F, 0x00, //    ###      ####
	0x3C, 0x0F, 0x00, //   ####      ####
	0x38, 0x0F, 0x00, //   ###       ####
	0x78, 0x0F, 0x00, //  ####       ####
	0x70, 0x0F, 0x00, //  ###        ####
	0xF0, 0x0F, 0x00, // ####        ####
	0xFF, 0xFF, 0xF0, // ####################
	0xFF, 0xFF, 0xF0, // ####################
	0xFF, 0xFF, 0xF0, // ####################
	0x00, 0x0F, 0x00, //             ####
	0x00, 0x0F, 0x00, //             ####
	0x00, 0x0F, 0x00, //             ####
	0x00, 0x0F, 0x00, //             ####
	0x00, 0x0F, 0x00, //             ####
	0x00, 0x0F, 0x00, //             ####
	0x00, 0x0F, 0x00, //             ####
	0x00, 0x0F, 0x00, //             ####

	/* @480 '5' (18 pixels wide) */
	0x1F, 0xFF, 0x80, //    ##############
	0x1F, 0xFF, 0x80, //    ##############
	0x1F, 0xFF, 0x80, //    ##############
	0x1E, 0x00, 0x00, //    ####
	0x1E, 0x00, 0x00, //    ####
	0x1C, 0x00, 0x00, //    ###
	0x1C, 0x00, 0x00, //    ###
	0x1C, 0x00, 0x00, //    ###
	0x1C, 0x00, 0x00, //    ###
	0x1C, 0x00, 0x00, //    ###
	0x1C, 0x00, 0x00, //    ###
	0x3C, 0x00, 0x00, //   ####
	0x3F, 0xC0, 0x00, //   ########
	0x3F, 0xF8, 0x00, //   ###########
	0x3F, 0xFE, 0x00, //   #############
	0x00, 0xFF, 0x00, //         ########
	0x00, 0x1F, 0x80, //            ######
	0x00, 0x07, 0x80, //              ####
	0x00, 0x07, 0xC0, //              #####
	0x00, 0x03, 0xC0, //               ####
	0x00, 0x03, 0xC0, //               ####
	0x00, 0x03, 0xC0, //               ####
	0x00, 0x03, 0xC0, //               ####
	0x00, 0x03, 0xC0, //               ####
	0x00, 0x07, 0x80, //              ####
	0x00, 0x0F, 0x80, //             #####
	0x60, 0x1F, 0x00, //  ##        #####
	0xFF, 0xFF, 0x00, // ################
	0xFF, 0xFC, 0x00, // ##############
	0x1F, 0xF0, 0x00, //    #########

	/* @570 '6' (19 pixels wide) */
	0x00, 0x0F, 0x80, //             #####
	0x00, 0x7F, 0x80, //          ########
	0x01, 0xFF, 0x80, //        ##########
	0x03, 0xF8, 0x00, //       #######
	0x07, 0xC0, 0x00, //      #####
	0x0F, 0x80, 0x00, //     #####
	0x1F, 0x00, 0x00, //    #####
	0x3E, 0x00, 0x00, //   #####
	0x3C, 0x00, 0x00, //   ####
	0x7C, 0x00, 0x00, //  #####
	0x78, 0x00, 0x00, //  ####
	0x78, 0x00, 0x00, //  ####
	0x7B, 0xFC, 0x00, //  #### ########
	0xFF, 0xFF, 0x00, // ################
	0xFF, 0xFF, 0x80, // #################
	0xFC, 0x0F, 0xC0, // ######      ######
	0xF0, 0x03, 0xC0, // ####          ####
	0xF0, 0x03, 0xE0, // ####          #####
	0xF0, 0x01, 0xE0, // ####           ####
	0xF0, 0x01, 0xE0, // ####           ####
	0xF0, 0x01, 0xE0, // ####           ####
	0xF0, 0x01, 0xE0, // ####           ####
	0x70, 0x01, 0xE0, //  ###           ####
	0x78, 0x01, 0xE0, //  ####          ####
	0x78, 0x03, 0xC0, //  ####         ####
	0x3C, 0x07, 0xC0, //   ####       #####
	0x3E, 0x0F, 0x80, //   #####     #####
	0x1F, 0xFF, 0x00, //    #############
	0x0F, 0xFE, 0x00, //     ###########
	0x03, 0xF8, 0x00, //       #######

	/* @660 '7' (19 pixels wide) */
	0xFF, 0xFF, 0xE0, // ###################
	0xFF, 0xFF, 0xE0, // ###################
	0xFF, 0xFF, 0xE0, // ###################
	0x00, 0x01, 0xC0, //                ###
	0x00, 0x03, 0xC0, //               ####
	0x00, 0x07, 0x80, //              ####
	0x00, 0x0F, 0x00, //             ####
	0x00, 0x0F, 0x00, //             ####
	0x00, 0x1E, 0x00, //            ####
	0x00, 0x1E, 0x00, //            ####
	0x00, 0x3C, 0x00, //           ####
	0x00, 0x38, 0x00, //           ###
	0x00, 0x78, 0x00, //          ####
	0x00, 0x78, 0x00, //          ####
	0x00, 0xF0, 0x00, //         ####
	0x00, 0xF0, 0x00, //         ####
	0x01, 0xE0, 0x00, //        ####
	0x01, 0xE0, 0x00, //        ####
	0x01, 0xE0, 0x00, //        ####
	0x03, 0xC0, 0x00, //       ####
	0x03, 0xC0, 0x00, //       ####
	0x03, 0xC0, 0x00, //       ####
	0x07, 0x80, 0x00, //      ####
	0x07, 0x80, 0x00, //      ####
	0x07, 0x80, 0x00, //      ####
	0x07, 0x80, 0x00, //      ####
	0x0F, 0x00, 0x00, //     ####
	0x0F, 0x00, 0x00, //     ####
	0x0F, 0x00, 0x00, //     ####
	0x0F, 0x00, 0x00, //     ####

	/* @750 '8' (20 pixels wide) */
	0x01, 0xFC, 0x00, //        #######
	0x07, 0xFF, 0x00, //      ###########
	0x1F, 0xFF, 0x80, //    ##############
	0x3F, 0x0F, 0xC0, //   ######    ######
	0x3C, 0x03, 0xC0, //   ####        ####
	0x78, 0x01, 0xE0, //  ####          ####
	0x78, 0x01, 0xE0, //  ####          ####
	0x78, 0x01, 0xE0, //  ####          ####
	0x78, 0x01, 0xE0, //  ####          ####
	0x78, 0x01, 0xE0, //  ####          ####
	0x7C, 0x03, 0xC0, //  #####        ####
	0x3E, 0x03, 0xC0, //   #####       ####
	0x1F, 0x07, 0x80, //    #####     ####
	0x0F, 0xFF, 0x00, //     ############
	0x07, 0xFE, 0x00, //      ##########
	0x0F, 0xFF, 0x00, //     ############
	0x1E, 0x1F, 0xC0, //    ####    #######
	0x3C, 0x07, 0xE0, //   ####       ######
	0x78, 0x03, 0xE0, //  ####         #####
	0xF8, 0x01, 0xF0, // #####          #####
	0xF0, 0x00, 0xF0, // ####            ####
	0xF0, 0x00, 0xF0, // ####            ####
	0xF0, 0x00, 0xF0, // ####            ####
	0xF0, 0x00, 0xF0, // ####            ####
	0xF8, 0x01, 0xF0, // #####          #####
	0x78, 0x01, 0xE0, //  ####          ####
	0x7E, 0x07, 0xE0, //  ######      ######
	0x3F, 0xFF, 0xC0, //   ################
	0x0F, 0xFF, 0x80, //     #############
	0x03, 0xFC, 0x00, //       ########

	/* @840 '9' (19 pixels wide) */
	0x03, 0xF8, 0x00, //       #######
	0x0F, 0xFE, 0x00, //     ###########
	0x1F, 0xFF, 0x00, //    #############
	0x3E, 0x0F, 0x80, //   #####     #####
	0x7C, 0x07, 0x80, //  #####       ####
	0x78, 0x03, 0xC0, //  ####         ####
	0xF0, 0x03, 0xC0, // ####          ####
	0xF0, 0x01, 0xC0, // ####           ###
	0xF0, 0x01, 0xE0, // ####           ####
	0xF0, 0x01, 0xE0, // ####           ####
	0xF0, 0x01, 0xE0, // ####           ####
	0xF0, 0x01, 0xE0, // ####           ####
	0xF8, 0x01, 0xE0, // #####          ####
	0x78, 0x01, 0xE0, //  ####          ####
	0x7E, 0x07, 0xE0, //  ######      ######
	0x3F, 0xFF, 0xE0, //   #################
	0x1F, 0xFF, 0xE0, //    ################
	0x07, 0xFB, 0xE0, //      ######## #####
	0x00, 0x03, 0xC0, //               ####
	0x00, 0x03, 0xC0, //               ####
	0x00, 0x03, 0xC0, //               ####
	0x00, 0x07, 0x80, //              ####
	0x00, 0x0F, 0x80, //             #####
	0x00, 0x0F, 0x00, //             ####
	0x00, 0x1F, 0x00, //            #####
	0x00, 0x7E, 0x00, //          ######
	0x03, 0xFC, 0x00, //       ########
	0x3F, 0xF0, 0x00, //   ##########
	0x3F, 0xC0, 0x00, //   ########
	0x3E, 0x00, 0x00, //   #####

	/* @930 'C' (20 pixels wide) */
	0x00, 0x7F, 0x80, //          ########
	0x01, 0xFF, 0xE0, //        ############
	0x07, 0xFF, 0xF0, //      ###############
	0x0F, 0xC0, 0xE0, //     ######      ###
	0x1F, 0x00, 0x20, //    #####          #
	0x3E, 0x00, 0x00, //   #####
	0x3C, 0x00, 0x00, //   ####
	0x7C, 0x00, 0x00, //  #####
	0x78, 0x00, 0x00, //  ####
	0x78, 0x00, 0x00, //  ####
	0xF8, 0x00, 0x00, // #####
	0xF0, 0x00, 0x00, // ####
	0xF0, 0x00, 0x00, // ####
	0xF0, 0x00, 0x00, // ####
	0xF0, 0x00, 0x00, // ####
	0xF0, 0x00, 0x00, // ####
	0xF0, 0x00, 0x00, // ####
	0xF0, 0x00, 0x00, // ####
	0xF0, 0x00, 0x00, // ####
	0xF0, 0x00, 0x00, // ####
	0x78, 0x00, 0x00, //  ####
	0x78, 0x00, 0x00, //  ####
	0x78, 0x00, 0x00, //  ####
	0x3C, 0x00, 0x00, //   ####
	0x3E, 0x00, 0x00, //   #####
	0x1F, 0x00, 0x20, //    #####          #
	0x0F, 0xC0, 0xE0, //     ######      ###
	0x07, 0xFF, 0xF0, //      ###############
	0x03, 0xFF, 0xE0, //       #############
	0x00, 0x7F, 0x80, //          ########
};

/* Character descriptors for Ubuntu Mono 36pt */
/* { [Char width in bits], [Offset into ubuntuMono_36ptCharBitmaps in bytes] } */
FONT_CHAR_INFO ubuntuMono_36ptDescriptors[] =
{
	{2, 0}, 		/*   */
	{0, 0}, 		/* ! */
	{0, 0}, 		/* " */
	{0, 0}, 		/* # */
	{0, 0}, 		/* $ */
	{0, 0}, 		/* % */
	{0, 0}, 		/* & */
	{0, 0}, 		/* ' */
	{0, 0}, 		/* ( */
	{0, 0}, 		/* ) */
	{0, 0}, 		/* * */
	{0, 0}, 		/* + */
	{0, 0}, 		/* , */
	{0, 0}, 		/* - */
	{0, 0}, 		/* . */
	{0, 0}, 		/* / */
	{20, 30}, 		/* 0 */
	{17, 120}, 		/* 1 */
	{19, 210}, 		/* 2 */
	{18, 300}, 		/* 3 */
	{20, 390}, 		/* 4 */
	{18, 480}, 		/* 5 */
	{19, 570}, 		/* 6 */
	{19, 660}, 		/* 7 */
	{20, 750}, 		/* 8 */
	{19, 840}, 		/* 9 */
	{0, 0}, 		/* : */
	{0, 0}, 		/* ; */
	{0, 0}, 		/* < */
	{0, 0}, 		/* = */
	{0, 0}, 		/* > */
	{0, 0}, 		/* ? */
	{0, 0}, 		/* @ */
	{0, 0}, 		/* A */
	{0, 0}, 		/* B */
	{20, 930}, 		/* C */
};

/* Font information for Ubuntu Mono 36pt */
FONT_INFO ubuntuMono_36ptFontInfo =
{
	3, /*  Character width */	
	30, /*  Character height */
	' ', /*  Start character */
	'C', /*  End character */
	2, /*  Width, in pixels, of space character */
	ubuntuMono_36ptDescriptors, /*  Character descriptor array */
	ubuntuMono_36ptBitmaps, /*  Character bitmap array */
};

