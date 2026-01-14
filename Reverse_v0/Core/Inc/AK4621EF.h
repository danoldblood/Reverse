/*
 * AK4621.h
 *
 *  Created on: Nov 29, 2022
 *      Author: danspechacek
 */

/*DATASHEET P 29
 * The internal registers may be written by the 3-wire P interface pins: CSN, CCLK, CDTI.
 * The data on this interface consists of
 * Chip address (2bits, C0/1)
 * Read/Write (1 bit)
 * Register address (MSB first, 5 bits) and
 * Control data (MSB first, 8 bits).
 * Address and data are clocked in on the rising edge of CCLK and
 * data is latched after the 16th rising edge of CCLK,
 * following a high-to-low transition of CSN.
 * Operation of the control serial port may be completely asynchronous with the
audio sample rate.
 * The maximum clock speed of the CCLK is 5MHz.
 * The chip address is fixed to “10”.
 * The access to the chip address except for “10” is invalid.
 * PDN pin = “L” resets the registers to their default values.
 */

//So, 101 followed by 5 bit register, followed by 8 bit data

#ifndef INC_AK4621EF_H_
#define INC_AK4621EF_H_

#define WRITE_POWERDOWNCONTROL 0xA0 //101 00000
#define WRITE_RESETCONTROL 0xA1 //101 00001
#define WRITE_CLOCKFORMATCONTROL 0xA2//101 00010
#define WRITE_DEEMVOLUMECONTROL 0xA3 //101 00011
//4 and 5 are reserved
#define WRITE_LCHDATTCONTROL 0xA6//101 00110
#define WRITE_RCHDATTCONTROL 0xA7//101 00111
#define WRITE_LCHEXTDATTCONTROL 0xA8//101 01000
#define WRITE_RCHEXTDATTCONTROL 0xA9//101 01001

//mode 3 is i2s - 011 for DIF2, DIF1, DIF0
//CMODE, CKS1, and CKS0 all 0 for Normal Speed MCLK 256fs
//0      0         1 for Double speed MCLK 256fs (if we're bumping beyond 54kHz)
//DFS1, DFS0 both 0 for Normal speed 32kHz-54kHz
//0 and 1 for double speed if we want to bump to 54-108kHz range

#define I2SNORMALSPEED 0x60  //01100000

//bit 0 is RSTDA, DAC reset; bit 1 is RSTAD, ADC reset; bit 4 is SDAD, ADC short delay sharp roll off filter

#define POWERUPADCDAC 0x03 //00000011
#define POWERDOWNADCDAC 0x00 //00000000

/*FROM DATASHEET P30, STARTUP SEQUENCE
 * When the PDN pin goes “L” to “H” upon power-up etc.,
 * the AK4621 will be ready for normal operation by the sequence
 * below. In this case, all control registers are set to default values
 * and the AK4621 is in the reset state.
 * (1) Set the clock mode and the audio data interface mode.
 * (2) Cancel the reset state by setting RSTAD bit or RSTDA bit to “1”.
 * 	   Refer to Reset Contorl Register (01H).
 * (3) ADC output and DAC output must be muted externally until canceling each reset state.
 * The clock mode must be changed after setting RSTAD bit and RSTDA bit to “0”.
 * At that time, ADC outputs and DAC outputs must be muted externally.*/


#endif /* INC_AK4621_H_ */
