/*
 * M24C64.h
 *
 *  Created on: Feb 12, 2024
 *      Author: danspechacek
 */

#ifndef INC_M24C64_H_
#define INC_M24C64_H_

#define M24C64WRITE 0xA0 //0b10100000 //10100000
#define M24C64READ 0xA1 //0b10100001 //10101001

#define FACTORYRESETBYTE 8000 //somewhat arbitrary byte  near the end of memory (8192) where we'll mark the first time programming

#endif /* INC_CS4270_H_ */
