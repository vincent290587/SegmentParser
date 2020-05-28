
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "PolyLine.h"

#include "SequenceUnpacker.h"

#define NRF_LOG_HEXDUMP_INFO(...)          _dump_as_hex(__VA_ARGS__)
#define NRF_LOG_HEXDUMP_DEBUG(...)         NRF_LOG_HEXDUMP_INFO(__VA_ARGS__)

#define NRF_LOG_INFO(...)                  printf(__VA_ARGS__);printf("\n")

class UploadSegment {
public:
	UploadSegment(uint8_t const id[], const float lat, const float lon) : startPnt(lat, lon) {

		memcpy(seg_id, id, 8);
		cur_size   = 0;
		total_size = 0;
		buffer     = nullptr;
	}
	~UploadSegment() {
		deallocate();
	}

	int isID(uint8_t const id[]) {
		return memcmp(seg_id, id, 8) == 0;
	}

	void copyID(uint8_t id[]) {
		memcpy(id, seg_id, 8);
	}

	void allocate(const uint32_t size) {
		if (!total_size) {
			total_size = size+20;
			buffer = new uint8_t[total_size];
		}

	}

	void deallocate() {
		if (buffer) {
			delete[] buffer;
			total_size = 0;
		}
	}

	int append(uint8_t const p_data[], uint32_t length) {

		if (buffer && length + cur_size <= total_size) {
			memcpy(buffer+cur_size, p_data, length);
			cur_size += length;
			return 0;
		}
		return -1;
	}

	uint8_t  seg_id[8];
	uint32_t cur_size;
	uint32_t total_size;
	uint8_t  *buffer;
	LatLng   startPnt;
	uint16_t poly_index;
	uint16_t effort_index;
};

//static const uint8_t m_seg_file[] = {
//
//		//0x8A, 0x86, 0x4D, 0x00, 0x00, 0x00, 0x00, 0x00,
//		//0x14,
//		//0x8A, 0x86, 0x4D, 0x00, 0x00, 0x00, 0x00,
//		//0x00, 0xA2, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//		//0x00, 0x00, 0x00, 0x00,
//		//0x15,
//		0x9D, 0x03, 0x00, 0x00, 0x47, 0x51, 0xEB,
//		0x24, 0xE0, 0xF3, 0x28, 0x03, 0x13, 0xF6, 0xE9,
//		0x24, 0x52, 0x00, 0x2A,
//		//0x15,
//		0x03, 0x99, 0x00, 0x00, 0x00, 0x00, 0x00,
//		0x00, 0x00, 0x13, 0x4D, 0x61, 0x74, 0x68, 0x65,
//		0x72, 0x6E, 0x65, 0x73,
//		//0x15,
//		0x73, 0x65, 0x72, 0x62, 0x72, 0x75, 0x67,
//		0x20, 0x2D, 0x20, 0x38, 0x00, 0x54, 0x38, 0x4F,
//		0x00, 0xD1, 0xC7, 0x06,
//		//0x15,
//		0x00, 0x59, 0x1E, 0x33, 0x32, 0x4F, 0x40,
//		0x3F, 0x44, 0x37, 0x2A, 0x3F, 0x2E, 0x39, 0x2A,
//		0x3F, 0x38, 0x4D, 0x26,
//		//0x15,
//		0x55, 0x32, 0x3D, 0x2A, 0x4F, 0x28, 0x45,
//		0x34, 0x43, 0x22, 0x23, 0x30, 0x1F, 0x20, 0x45,
//		0x2E, 0x33, 0x30, 0x43,
//		//0x15,
//		0x3E, 0x35, 0x22, 0x23, 0x30, 0x3F, 0x42,
//		0x39, 0x3E, 0x2F, 0x38, 0x58, 0x00, 0x3F, 0x00,
//		0x00, 0x00, 0x0B, 0x00,
//		//0x15,
//		0x18, 0x00, 0x34, 0x00, 0x00, 0x00, 0x74,
//		0x1D, 0xDE, 0x26, 0x7A, 0x22, 0x09, 0x68, 0x26,
//		0xE7, 0xEB, 0xFC, 0x02,
//		//0x15,
//		0x67, 0x97, 0xF4, 0xCC, 0x77, 0x00, 0x00,
//		0x10, 0x00, 0x04, 0x00, 0x00, 0x00, 0x79, 0xF0,
//		0x1C, 0x82, 0x13, 0x1F,
//		//0x16,
//		0x02, 0x0E, 0x59, 0x00, 0x00, 0x00, 0x9E,
//		0x00, 0x57, 0xB3, 0x00, 0x00, 0x00, 0x00, 0x00,
//		0x00, 0x00, 0x00, 0x00
//};

unsigned char m_seg_file[] = {
  0xf9, 0x08, 0x00, 0x00, 0x0f, 0xab, 0x5e, 0x23, 0x45, 0xbe, 0x74, 0x04,
  0xc5, 0x5b, 0x5b, 0x23, 0x17, 0xa5, 0x74, 0x04, 0xb0, 0x02, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x13, 0x42, 0x6c, 0x75, 0x6d, 0x65, 0x6e, 0x74,
  0x68, 0x61, 0x6c, 0x20, 0x2d, 0x20, 0x4a, 0x75, 0x6e, 0x67, 0x6c, 0x69,
  0x7b, 0x00, 0x36, 0xe5, 0x4b, 0x00, 0xc5, 0x8f, 0x09, 0x00, 0xc3, 0x01,
  0x3b, 0x97, 0x02, 0xc5, 0x01, 0x95, 0x04, 0x93, 0x04, 0xdb, 0x01, 0x95,
  0x02, 0xf7, 0x01, 0xbd, 0x01, 0x93, 0x01, 0x27, 0xbb, 0x03, 0x7a, 0xaf,
  0x04, 0xc2, 0x01, 0xfd, 0x02, 0x8e, 0x01, 0x87, 0x02, 0xd0, 0x01, 0x05,
  0x06, 0x09, 0x0e, 0x09, 0x0c, 0x09, 0x10, 0x09, 0x0e, 0x0b, 0x0c, 0x09,
  0x0a, 0x09, 0x0e, 0x05, 0x12, 0x09, 0x0e, 0x0b, 0x0e, 0x09, 0x08, 0x09,
  0x0e, 0x09, 0x12, 0x0b, 0x0e, 0x05, 0x0e, 0x09, 0x0a, 0x09, 0x0c, 0x09,
  0x0e, 0x09, 0x12, 0x05, 0x0e, 0x09, 0x0a, 0x0b, 0x0e, 0x03, 0x0e, 0x0b,
  0x12, 0x09, 0x0c, 0x05, 0x0a, 0x09, 0x0e, 0x09, 0x0e, 0x09, 0x12, 0x05,
  0x0e, 0x09, 0x0a, 0x09, 0x0e, 0x05, 0x0e, 0x09, 0x12, 0x0b, 0x0c, 0x03,
  0x0c, 0x0b, 0x0a, 0x05, 0x0e, 0x61, 0x01, 0xfe, 0x00, 0x00, 0x00, 0x20,
  0x00, 0x4c, 0x00, 0x23, 0x00, 0x00, 0x00, 0x9b, 0x84, 0x10, 0x5f, 0xbb,
  0xe7, 0x90, 0x12, 0xe1, 0x37, 0xb9, 0x21, 0x53, 0x71, 0x2c, 0x14, 0x7b,
  0xbf, 0x83, 0x1d, 0x01, 0x69, 0x83, 0xbd, 0xd7, 0x11, 0x01, 0xff, 0x1f,
  0x38, 0xb2, 0xcd, 0x15, 0x7d, 0x8c, 0x6f, 0x32, 0x08, 0x3a, 0xe5, 0xf2,
  0x09, 0xcb, 0x65, 0x12, 0x48, 0x49, 0x06, 0xf7, 0x21, 0x1f, 0xd7, 0xf7,
  0x2d, 0xb0, 0xed, 0x54, 0x16, 0x48, 0x02, 0xec, 0xfe, 0xf0, 0xc7, 0x03,
  0x09, 0x20, 0x3e, 0x0b, 0x00, 0x00, 0x00, 0x30, 0x00, 0x03, 0x00, 0x00,
  0x00, 0x41, 0xa0, 0x1b, 0xa1, 0x9c, 0x30, 0xa0, 0x40, 0x1e, 0x9c, 0xbc,
  0x7f, 0x3a, 0x1c, 0x87, 0x38, 0xd4, 0xe3, 0x3a, 0xf1, 0x00, 0x40, 0x00,
  0x07, 0x10, 0xc3, 0x98, 0xe4, 0x01, 0x3e, 0x8e, 0x0b, 0x82, 0x07, 0x20,
  0x19, 0xea, 0xdc, 0xe3, 0xe3, 0x01, 0x41, 0x01, 0x00, 0x35, 0x01, 0x0b,
  0x39, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

#define MAX_ARRAY_PRINTF        64

static void _dump_as_hex(uint8_t const *p_data, uint16_t length) {

	if (!p_data) return;

	printf("{0x");
	for (uint16_t i=0; i< length && i < MAX_ARRAY_PRINTF; i++) {
		printf("%02X", p_data[i]);
		if (i< length-1 && i < MAX_ARRAY_PRINTF-1) {
			printf(",0x");
		}
	}
	printf("} \n");
}

static void _dump_as_char(uint8_t const *p_data, uint16_t length) {

	if (!p_data) return;

	printf("{");
	for (uint16_t i=0; i< length && i < 32; i++) {
		printf("%c", p_data[i]);
		if (i< length-1 && i < MAX_ARRAY_PRINTF-1) {
			printf(",");
		}
	}
	printf("} \n");
}

static void _encode_uint32_little(uint32_t value, uint8_t p_data[]) {

	uint8_t index = 0;

	p_data[index++] = value & 0xFF;
	p_data[index++] = (value >> 8) & 0xFF;
	p_data[index++] = (value >> 16) & 0xFF;
	p_data[index++] = (value >> 24) & 0xFF;
}

static uint32_t _decode_uint32_little(uint8_t const *p_data) {

	uint32_t res = 0;

	res = p_data[0];
	res |= p_data[1] << 8;
	res |= p_data[2] << 16;
	res |= p_data[3] << 24;

	return res;
}

static uint16_t _decode_uint16_little(uint8_t const *p_data) {

	uint16_t res = 0;

	res = p_data[0];
	res |= p_data[1] << 8;

	return res;
}

uint16_t FitCRC_Get16(uint16_t crc, uint8_t byte)
{
   static const uint16_t crc_table[16] =
   {
      0x0000, 0xCC01, 0xD801, 0x1400, 0xF001, 0x3C00, 0x2800, 0xE401,
      0xA001, 0x6C00, 0x7800, 0xB401, 0x5000, 0x9C01, 0x8801, 0x4400
   };
   uint16_t tmp;

   // compute checksum of lower four bits of byte
   tmp = crc_table[crc & 0xF];
   crc  = (crc >> 4) & 0x0FFF;
   crc  = crc ^ tmp ^ crc_table[byte & 0xF];

   // now compute checksum of upper four bits of byte
   tmp = crc_table[crc & 0xF];
   crc  = (crc >> 4) & 0x0FFF;
   crc  = crc ^ tmp ^ crc_table[(byte >> 4) & 0xF];

   return crc;
}

#define INCREMENT_INDEX(INCR)              do { index += (INCR); if (index > m_cur_u_seg.cur_size) return; } while(0)

static void _handle_segment_parse(UploadSegment& m_cur_u_seg) {

	uint32_t index = 0;
	uint16_t nb_points_seg = 0;

	// distance32
	// start_lat32
	// start_lon32
	// end_lat32
	// end_lon32
	// grade16
	// 00 00 00 00 00 00
	// name_length8
	// name[1..19]
	// poly_length16
	// poly[]
	// pr_time16
	// kom_time16
	// isHazardous16
	// tot_compressed_nb_words16
	// cStrDist_length16
	// compressedStreamDistance[]
	// cStrEffort_length16
	// compressedStreamEffort[]
	// padding_4_align
	// allocateSize16
	// crc16

	uint32_t dist = _decode_uint32_little(m_cur_u_seg.buffer+index);
	INCREMENT_INDEX(4);
	NRF_LOG_INFO("dist : %lu", dist);

	int32_t i_lat = (int32_t)_decode_uint32_little(m_cur_u_seg.buffer+index) / 119;
	INCREMENT_INDEX(4);
	NRF_LOG_INFO("i_lat: %ld", i_lat);

	int32_t i_lon = (int32_t)_decode_uint32_little(m_cur_u_seg.buffer+index) / 119;
	INCREMENT_INDEX(4);
	NRF_LOG_INFO("i_lon: %ld", i_lon);

	i_lat = (int32_t)_decode_uint32_little(m_cur_u_seg.buffer+index) / 119;
	INCREMENT_INDEX(4);
	NRF_LOG_INFO("i_lat: %ld", i_lat);

	i_lon = (int32_t)_decode_uint32_little(m_cur_u_seg.buffer+index) / 119;
	INCREMENT_INDEX(4);
	NRF_LOG_INFO("i_lon: %ld", i_lon);

	int16_t grade = (int16_t)_decode_uint16_little(m_cur_u_seg.buffer+index);
	INCREMENT_INDEX(2);
	NRF_LOG_INFO("grade * 10: %d", grade * 10 / 255);

	INCREMENT_INDEX(6); // padding 0

	// dump name
	uint16_t name_length = m_cur_u_seg.buffer[index];
	INCREMENT_INDEX(1);

	NRF_LOG_INFO("name_length: %u", name_length);

	_dump_as_char(m_cur_u_seg.buffer+index, name_length); // name
	INCREMENT_INDEX(name_length);

	uint16_t poly_length = _decode_uint16_little(m_cur_u_seg.buffer+index);
	INCREMENT_INDEX(2);

	NRF_LOG_INFO("Polyline length: %u", poly_length);

	m_cur_u_seg.poly_index = index;

	INCREMENT_INDEX(poly_length);

	// pr time
	uint16_t pr_time =  _decode_uint16_little(m_cur_u_seg.buffer+index);
	INCREMENT_INDEX(2);

	// kom time
	uint16_t kom_time =  _decode_uint16_little(m_cur_u_seg.buffer+index);
	INCREMENT_INDEX(2);

	NRF_LOG_INFO("PR/KOM times: %u %u", pr_time, kom_time);

	INCREMENT_INDEX(2); // hazardous

	uint16_t words_comp =  _decode_uint16_little(m_cur_u_seg.buffer+index);
	INCREMENT_INDEX(2);

	NRF_LOG_INFO("words_sum: %u", words_comp);

	uint16_t comp_dist_length = _decode_uint16_little(m_cur_u_seg.buffer+index);
	INCREMENT_INDEX(2);

	NRF_LOG_INFO("Dist stream length: %u", comp_dist_length);

	if (0) {
		NRF_LOG_HEXDUMP_DEBUG(m_cur_u_seg.buffer+index, comp_dist_length);
		ByteBuffer bBuffer;
		bBuffer.addU(m_cur_u_seg.buffer+index, comp_dist_length);
		SequenceUnpacker unpacker(bBuffer, nb_points_seg-1);
		unpacker.unpack();
		for (int i=0; i < unpacker.original.size(); i++) {
			printf("Ddist:  %ld \n", (int32_t)unpacker.original[i]);
		}
	}
	INCREMENT_INDEX(comp_dist_length);

	uint16_t comp_stream_length = _decode_uint16_little(m_cur_u_seg.buffer+index);
	INCREMENT_INDEX(2);

	NRF_LOG_INFO("Effort stream length: %u", comp_stream_length);

	m_cur_u_seg.effort_index = index;

	INCREMENT_INDEX(comp_stream_length);

	uint16_t nb_padding = ((words_comp-1) << 2) - (comp_dist_length + comp_stream_length);
	NRF_LOG_INFO("Nb padding: %u", nb_padding);
	INCREMENT_INDEX(nb_padding);

	uint16_t allocate_length = _decode_uint16_little(m_cur_u_seg.buffer+index);
	INCREMENT_INDEX(2);
	NRF_LOG_INFO("allocate_length: %u", allocate_length);

	uint16_t crc16 = 0;
    for (int i5 = 0; i5 < index; i5++) {
    	crc16 = FitCRC_Get16(crc16, m_cur_u_seg.buffer[i5]);
    }
	NRF_LOG_INFO("CRC16 : %lu vs %lu", crc16, _decode_uint16_little(m_cur_u_seg.buffer + index));

	if (crc16 == _decode_uint16_little(m_cur_u_seg.buffer + index)) {

		index = m_cur_u_seg.poly_index;

		NRF_LOG_HEXDUMP_DEBUG(m_cur_u_seg.buffer+index, poly_length);
		{
			ByteBuffer bBuffer;
			bBuffer.addU(m_cur_u_seg.buffer+index, poly_length);

			PolyLine myPoly;
			myPoly.decodeBinaryPolyline(bBuffer);
			nb_points_seg = myPoly._line.size();
			myPoly.toString();
		}

		index = m_cur_u_seg.effort_index;

		NRF_LOG_HEXDUMP_DEBUG(m_cur_u_seg.buffer+index, comp_stream_length);
		//_dump_as_char(m_cur_u_seg.buffer+index, comp_stream_length);
		{
			ByteBuffer bBuffer;
			bBuffer.addU(m_cur_u_seg.buffer+index, comp_stream_length);
			SequenceUnpacker unpacker(bBuffer, nb_points_seg-1);
			unpacker.unpack();
			for (int i=0; i < unpacker.original.size(); i++) {
				printf("Dt:  %ld \n", (int32_t)unpacker.original[i]);
			}
		}
	}
}

///////////////////////////////////////////////////////////////////

int main(int argc, char **argv) {

	uint8_t seg_id[8];
	UploadSegment cur_seg(seg_id, 0, 0);

	cur_seg.allocate(sizeof(m_seg_file));
	cur_seg.append(m_seg_file, sizeof(m_seg_file));

	_handle_segment_parse(cur_seg);

	char input[16];
	for (int i=0; i < sizeof(input); i++) {
		input[i] = (char)i+'[';
	}

	printf("////////////////////////////////////////////////////////////////\n");

#if TEST0
//	std::string to_decode = "AQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQE";
//	std::string decoded;
//
//	Base64 myBase;
//	myBase.Decode(to_decode, decoded);
//
//	NRF_LOG_INFO("Input: %s", to_decode.c_str());
//	NRF_LOG_INFO("Decoded: %s", decoded.c_str());
//	NRF_LOG_HEXDUMP_INFO((uint8_t*const) decoded.c_str(), decoded.length());
#endif
#if TEST1
//	const char       *enc = "AQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQEBAQE";
	char       *out;
	size_t      out_len;

	const char data[] = "ABC123Test Lets Try this' input and see What happens";
	char *p_enc = b64_encode((const unsigned char*)data, sizeof(data));

	out_len = b64_decoded_size(p_enc);
	printf("decoded size:     '%lu'\n", out_len);

	out = (char*)malloc(out_len+1);

	if (!b64_decode(p_enc, (unsigned char *)out, out_len)) {
		printf("Decode Failure\n");
		return 1;
	}
	out[out_len] = '\0';

	NRF_LOG_HEXDUMP_INFO((uint8_t*const)out, out_len);
	printf("dec:     '%s'\n", out);
	free(out);
#endif
#if TEST2
	const char enc[] = "AAECAwQFBgcICQoLDA0ODxAREhMUFRYXGBkaGxwdHh8";
	BaseString input_str;
	input_str.append((const char*)enc, sizeof(enc));

	printf("input:     '%s'\n", input_str.toString());

	SequenceUnpacker unpacker(input_str, input_str.length());

	unpacker.unpack();

	for (int i=0; i < unpacker.original.size(); i++) {
		printf("decoded:  '%ld \n", (int32_t)unpacker.original[i]);
	}
#endif
#if TEST3
//	const uint8_t enc[] = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B};
//	const uint8_t enc[] = {0x04,0x00,0x00,0x00,0x79,0xF0,0x1C,0x82,0x13,0x1F,0x02,0x0E,0x59,0x00,0x00,0x00};
	const uint8_t enc[] = {0x34,0x00,0x00,0x00,0x74,0x1D,0xDE,0x26,0x7A,0x22,0x09,0x68,0x26,0xE7,0xEB,0xFC,0x02,0x67,0x97,0xF4,0xCC,0x77,0x00,0x00};

	ByteBuffer bBuffer;
	bBuffer.addU(enc, sizeof(enc));

	SequenceUnpacker unpacker(bBuffer, 24);

	unpacker.unpack();

	for (int i=0; i < unpacker.original.size(); i++) {
		printf("decoded:  '%ld \n", (int32_t)unpacker.original[i]);
	}
#endif
#if TEST4
	const uint8_t enc[] = {0x54,0x38,0x4F,0x00,0xD1,0xC7,0x06,0x00,0x59,0x1E,0x33,0x32,0x4F,0x40,
			0x3F,0x44,0x37,0x2A,0x3F,0x2E,0x39,0x2A,0x3F,0x38,0x4D,0x26,0x55,0x32,
			0x3D,0x2A,0x4F,0x28,0x45,0x34,0x43,0x22,0x23,0x30,0x1F,0x20,0x45,0x2E,
			0x33,0x30,0x43,0x3E,0x35,0x22,0x23,0x30,0x3F,0x42,0x39,0x3E,0x2F,0x38};
	ByteBuffer bBuffer;
	bBuffer.addU(enc, sizeof(enc));

	PolyLine myPoly;
	myPoly.decodeBinaryPolyline(bBuffer);
	myPoly.toString();
#endif

	return 0;
}
