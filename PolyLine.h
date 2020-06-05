/*
 * PolyLine.h
 *
 *  Created on: 22 mai 2020
 *      Author: vgol
 */

#ifndef POLYLINE_H_
#define POLYLINE_H_

#include <stdint.h>
#include <vector>

#include "Base64.h"


class LatLng {
public:
	LatLng() {
		lat=0;
		lon=0;
	}
	LatLng(float _lat, float _lon) {
		lat=_lat;
		lon=_lon;
	}

	float lat;
	float lon;
};

class PolyLine {
public:
	PolyLine() {

	}

	int decodeBinaryPolyline(ByteBuffer& bArr2) {

		_line.clear();

		int i;
		int i2;
		size_t length = bArr2.length();
		size_t i3 = 8;
		int32_t b = (bArr2[0] & 255) | ((bArr2[1] & 255) << 8) | ((bArr2[2] & 255) << 16) | ((bArr2[3] & 255) << 24);
		uint8_t b2 = 7;
		int32_t i4 = (bArr2[4] & 255) | ((bArr2[5] & 255) << 8) | ((bArr2[6] & 255) << 16) | ((bArr2[7] & 255) << 24);
		_line.push_back(LatLng(((float) b) / 100000.0f, ((float) i4) / 100000.0f));
		while (i3 < length) {
			int i5 = 0;
			int i6 = 0;
			while (true) {
				i = i3 + 1;
				uint8_t b3 = bArr2[i3];
				i5 |= (b3 & 0xFF) << i6;
				i6 += b2;
				if (((b3 & 128) >> b2) == 0) {
					break;
				}
				i3 = i;
			}
			int32_t i7 = ((i5 & 1) != 0 ? ~(i5 >> 1) : i5 >> 1) + b;
			int i8 = 0;
			int i9 = 0;
			while (true) {
				i2 = i + 1;
				uint8_t b4 = bArr2[i];
				i8 |= (b4 & 0xFF) << i9;
				i9 += b2;
				if (((b4 & 128) >> b2) == 0) {
					break;
				}
				i = i2;
			}
			int i10 = i8 & 1;
			int i11 = i8 >> 1;
			if (i10 != 0) {
				i11 = ~i11;
			}
			i4 += i11;
			_line.push_back(LatLng(((float) i7) / 100000.0f, ((float) i4) / 100000.0f));
			b = i7;
			i3 = i2;
			b2 = 7;
		}

		return 0;
	}

	void toString(void) {

		printf("PolyLine[%lu]: { \n", _line.size());
		for (size_t i=0; i< _line.size(); i++) {
			printf("%f %f", _line[i].lat, _line[i].lon);
			if (i< _line.size()-1) {
				printf(",\n");
			}
		}
		printf("\n} \n");

	}

    std::vector<LatLng> _line;
};

//
//    public static class BinaryPolylineEncoder {
//        private int curBytes = 0;
//        private int lastLatitude;
//        private int lastLongitude;
//        private byte[] output;
//
//        public BinaryPolylineEncoder(int i) {
//            this.output = new byte[(i * 4)];
//        }
//
//        public byte[] bytes() {
//            byte[] bArr = new byte[this.curBytes];
//            for (int i = 0; i < this.curBytes; i++) {
//                bArr[i] = this.output[i];
//            }
//            return bArr;
//        }
//
//        public void addFirstCoordinate(LatLng latLng) {
//            this.lastLatitude = (int) (latLng.latitude * 100000.0d);
//            int i = (int) (latLng.longitude * 100000.0d);
//            this.lastLongitude = i;
//            byte[] bArr = this.output;
//            int i2 = this.curBytes;
//            int i3 = i2 + 1;
//            this.curBytes = i3;
//            int i4 = this.lastLatitude;
//            bArr[i2] = (byte) (i4 & 255);
//            int i5 = i3 + 1;
//            this.curBytes = i5;
//            bArr[i3] = (byte) ((i4 >>> 8) & 255);
//            int i6 = i5 + 1;
//            this.curBytes = i6;
//            bArr[i5] = (byte) ((i4 >>> 16) & 255);
//            int i7 = i6 + 1;
//            this.curBytes = i7;
//            bArr[i6] = (byte) ((i4 >>> 24) & 255);
//            int i8 = i7 + 1;
//            this.curBytes = i8;
//            bArr[i7] = (byte) (i & 255);
//            int i9 = i8 + 1;
//            this.curBytes = i9;
//            bArr[i8] = (byte) ((i >>> 8) & 255);
//            int i10 = i9 + 1;
//            this.curBytes = i10;
//            bArr[i9] = (byte) ((i >>> 16) & 255);
//            this.curBytes = i10 + 1;
//            bArr[i10] = (byte) ((i >>> 24) & 255);
//        }
//
//        private void addCoordinatePart(int i) {
//            int i2;
//            int i3 = i << 1;
//            if (i < 0) {
//                i3 = ~i3;
//            }
//            int[] iArr = new int[10];
//            int i4 = 0;
//            while (i3 > 0) {
//                iArr[i4] = i3 & WorkQueueKt.MASK;
//                i3 >>>= 7;
//                i4++;
//            }
//            int i5 = 0;
//            while (true) {
//                i2 = i4 - 1;
//                if (i5 >= i2) {
//                    break;
//                }
//                byte[] bArr = this.output;
//                int i6 = this.curBytes;
//                this.curBytes = i6 + 1;
//                bArr[i6] = (byte) (iArr[i5] | 128);
//                i5++;
//            }
//            if (i4 > 0) {
//                byte[] bArr2 = this.output;
//                int i7 = this.curBytes;
//                this.curBytes = i7 + 1;
//                bArr2[i7] = (byte) iArr[i2];
//                return;
//            }
//            byte[] bArr3 = this.output;
//            int i8 = this.curBytes;
//            this.curBytes = i8 + 1;
//            bArr3[i8] = 0;
//        }
//
//        public void addCoordinate(LatLng latLng) {
//            int i = (int) (latLng.longitude * 100000.0d);
//            int i2 = (int) (latLng.latitude * 100000.0d);
//            if (i != 0 && Math.abs(i) < Integer.MAX_VALUE && i2 != 0 && Math.abs(i2) < Integer.MAX_VALUE) {
//                if (this.lastLongitude == 0 && this.lastLatitude == 0) {
//                    addCoordinatePart(i2);
//                    addCoordinatePart(i);
//                    this.lastLatitude = i2;
//                    this.lastLongitude = i;
//                    return;
//                }
//                addCoordinatePart(i2 - this.lastLatitude);
//                addCoordinatePart(i - this.lastLongitude);
//                this.lastLatitude = i2;
//                this.lastLongitude = i;
//            }
//        }
//    }



#endif /* POLYLINE_H_ */
