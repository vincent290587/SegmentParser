/*
 * SequenceUnpacker.h
 *
 *  Created on: 22 mai 2020
 *      Author: vgol
 */

#ifndef SEQUENCEUNPACKER_H_
#define SEQUENCEUNPACKER_H_

#include <stdint.h>
#include <vector>

#include "Base64.h"



class SequenceUnpacker {
public:
    SequenceUnpacker(BaseString& str, int i) {

    	ByteBuffer &wrap = myBase64.decode(str, 0);
    	this->decompressed = wrap;
    	this->decompressed.toString();

    	this->count = i;
    	this->numberOfBytes = wrap.length();

    }
    SequenceUnpacker(ByteBuffer& str, int i) {

    	this->decompressed = str;
    	this->decompressed.toString();

    	this->count = i;
    	this->numberOfBytes = str.length();

    }

    uint32_t getBits(int i) {
    	uint32_t i2 = this->bitCount;
    	uint32_t i3 = i2 & 31;
    	uint32_t i4 = i2 >> 5;
        if (i4 > (this->numberOfBytes >> 2)) {
            this->done = true;
        }
        this->bitCount += i;
        //try {
    		//printf("getInt %ld \n", this->decompressed.getInt(i4 * 4));
        	uint32_t i5 = this->decompressed.getInt(i4 * 4) >> i3;
        	int32_t i6 = 32 - i3;
        	int32_t i7 = i - i6;
            if (i7 > 0) {
//                try {
            		if (!this->decompressed.canGetInt(i4 + 1) * 4) {
            			this->done = true;
            			return 0;
            		}
                	//printf("i50 %ld \n", (int32_t)i5);
                    i5 = (i5 & ((uint32_t)-1 >> (32 - i6)));
                    //printf("i51 %ld \n", i5);
                    uint32_t tmp = this->decompressed.getInt((i4 + 1) * 4);
                    i5 |= (tmp & (((uint32_t)-1) >> (32 - i7))) << i6;
                    //printf("i52 %ld \n", ((uint32_t)-1) >> (32 - i7));
//                } catch (IndexOutOfBoundsException unused) {
//                    this->done = true;
//                    return 0;
//                }
            }
            uint32_t i8 = 32 - i;
            return (i5 << i8) >> i8;
//        } catch (IndexOutOfBoundsException unused2) {
//            this->done = true;
//            return 0;
//        }
    }

    void unpack() {

    	//r7 = this;
    	uint32_t r0 = 32;
    	uint32_t r1 = this->getBits(r0);
//    	std::vector<uint32_t> r2 = this->original;
    	uint32_t ir3 = (r1);
    	this->original.push_back(ir3);
    	//r2 = 3;
    	uint32_t r3 = this->getBits(3);
    	r3 = r3 & 7;
    	r3 = r3 + 2;
    	this->done = false;
    	while (!this->done) {
    		//L_0x0018:
    		uint32_t r4 = this->getBits(r3);
    		uint32_t r52 = -2147483648;
    		uint32_t r6 = 32 - r3;
    		uint32_t r5 = r52 >> r6;
    		if (r4 == r5) {
    			r3 = this->getBits(3);
    			r3 = r3 & 7;
    			r3 = r3 + 2;
    			r4 = this->getBits(r3);
    		}
    		//L_0x002f:
			r1 = r1 - r4;
//			std::vector<uint32_t> arr4 = this->original;
			//java.lang.Integer arr5 = java.lang.Integer.valueOf(-r1);
			this->original.push_back(-r1);
//			std::vector<uint32_t> arr41 = this->original;
			uint32_t r41 = this->original.size();
			uint32_t r51 = this->count;
			if (r41 == r51) {
				break;//goto L_0x0044;
			}                //goto L_0x0048;
			//L_0x0044:
			//boolean r4 = this->done;
    	}
    	//if (!this->done) goto L_0x0018;
    	//L_0x0048:
    	//    return;

    	//throw new UnsupportedOperationException("Method not decompiled: com.lezyne.gpsally.tools.SequenceUnpacker.unpack():void");
    }

    std::vector<uint32_t> original;

private:
    uint32_t bitCount = 0;
    uint32_t count = 0;
    ByteBuffer decompressed;
    bool done = false;
    uint32_t numberOfBytes;
    Base64 myBase64;
};



#endif /* SEQUENCEUNPACKER_H_ */
