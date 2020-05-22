
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "SequenceUnpacker.h"

#define NRF_LOG_HEXDUMP_INFO(...)          _dump_as_hex(__VA_ARGS__)
#define NRF_LOG_HEXDUMP_DEBUG(...)         do {} while(0)

#define NRF_LOG_INFO(...)                  printf(__VA_ARGS__);printf("\n")

typedef struct {
	uint32_t cur_size;
	uint32_t total_size;
	uint8_t  seg_id[8];
	uint8_t  buffer[10000];
} sUploadSegment;

static sUploadSegment m_cur_u_seg;

static const uint8_t m_seg_file[] = {

		//0x8A, 0x86, 0x4D, 0x00, 0x00, 0x00, 0x00, 0x00,
		//0x14,
		//0x8A, 0x86, 0x4D, 0x00, 0x00, 0x00, 0x00,
		//0x00, 0xA2, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		//0x00, 0x00, 0x00, 0x00,
		//0x15,
		0x9D, 0x03, 0x00, 0x00, 0x47, 0x51, 0xEB,
		0x24, 0xE0, 0xF3, 0x28, 0x03, 0x13, 0xF6, 0xE9,
		0x24, 0x52, 0x00, 0x2A,
		//0x15,
		0x03, 0x99, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x13, 0x4D, 0x61, 0x74, 0x68, 0x65,
		0x72, 0x6E, 0x65, 0x73,
		//0x15,
		0x73, 0x65, 0x72, 0x62, 0x72, 0x75, 0x67,
		0x20, 0x2D, 0x20, 0x38, 0x00, 0x54, 0x38, 0x4F,
		0x00, 0xD1, 0xC7, 0x06,
		//0x15,
		0x00, 0x59, 0x1E, 0x33, 0x32, 0x4F, 0x40,
		0x3F, 0x44, 0x37, 0x2A, 0x3F, 0x2E, 0x39, 0x2A,
		0x3F, 0x38, 0x4D, 0x26,
		//0x15,
		0x55, 0x32, 0x3D, 0x2A, 0x4F, 0x28, 0x45,
		0x34, 0x43, 0x22, 0x23, 0x30, 0x1F, 0x20, 0x45,
		0x2E, 0x33, 0x30, 0x43,
		//0x15,
		0x3E, 0x35, 0x22, 0x23, 0x30, 0x3F, 0x42,
		0x39, 0x3E, 0x2F, 0x38, 0x58, 0x00, 0x3F, 0x00,
		0x00, 0x00, 0x0B, 0x00,
		//0x15,
		0x18, 0x00, 0x34, 0x00, 0x00, 0x00, 0x74,
		0x1D, 0xDE, 0x26, 0x7A, 0x22, 0x09, 0x68, 0x26,
		0xE7, 0xEB, 0xFC, 0x02,
		//0x15,
		0x67, 0x97, 0xF4, 0xCC, 0x77, 0x00, 0x00,
		0x10, 0x00, 0x04, 0x00, 0x00, 0x00, 0x79, 0xF0,
		0x1C, 0x82, 0x13, 0x1F,
		//0x16,
		0x02, 0x0E, 0x59, 0x00, 0x00, 0x00, 0x9E,
		0x00, 0x57, 0xB3, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00
};

#define MAX_ARRAY_PRINTF        32

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

static void _handle_segment_parse(void) {

	uint8_t index = 0;

	// start_lat32
	// start_lon32
	// end_lat32
	// end_lon32
	// grade16
	// 00 00 00
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

	uint32_t dist = _decode_uint32_little(m_cur_u_seg.buffer+index);
	index+=4;
	NRF_LOG_INFO("dist: %lu", dist);

	int32_t i_lat = (int32_t)_decode_uint32_little(m_cur_u_seg.buffer+index) / 119;
	index+=4;
	NRF_LOG_INFO("i_lat: %ld", i_lat);

	int32_t i_lon = (int32_t)_decode_uint32_little(m_cur_u_seg.buffer+index) / 119;
	index+=4;
	NRF_LOG_INFO("i_lon: %ld", i_lon);

	i_lat = (int32_t)_decode_uint32_little(m_cur_u_seg.buffer+index) / 119;
	index+=4;
	NRF_LOG_INFO("i_lat: %ld", i_lat);

	i_lon = (int32_t)_decode_uint32_little(m_cur_u_seg.buffer+index) / 119;
	index+=4;
	NRF_LOG_INFO("i_lon: %ld", i_lon);

	int16_t grade = (int16_t)_decode_uint16_little(m_cur_u_seg.buffer+index);
	index+=2;
	NRF_LOG_INFO("grade: %d * 10", grade * 10 / 255);

	index+=6; // padding 0

	// dump name
	uint16_t name_length = m_cur_u_seg.buffer[index];
	index+=1;

	NRF_LOG_INFO("name_length: %u", name_length);

	_dump_as_char(m_cur_u_seg.buffer+index, name_length); // name
	index+=name_length;

	uint16_t poly_length = _decode_uint16_little(m_cur_u_seg.buffer+index);
	index+=2;

	NRF_LOG_INFO("Polyline length: %u", poly_length);

	NRF_LOG_HEXDUMP_INFO(m_cur_u_seg.buffer+index, poly_length);
	index+=poly_length;

	// pr time
	uint16_t pr_time =  _decode_uint16_little(m_cur_u_seg.buffer+index);
	index+=2;

	// kom time
	uint16_t kom_time =  _decode_uint16_little(m_cur_u_seg.buffer+index);
	index+=2;

	NRF_LOG_INFO("PR/KOM times: %u %u", pr_time, kom_time);

	index+=2; // hazardous

	uint16_t words_comp =  _decode_uint16_little(m_cur_u_seg.buffer+index);
	index+=2;

	NRF_LOG_INFO("words_comp: %u", words_comp);

	uint16_t comp_dist_length = _decode_uint16_little(m_cur_u_seg.buffer+index);
	index+=2;

	NRF_LOG_INFO("Dist stream length: %u", comp_dist_length);

	NRF_LOG_HEXDUMP_INFO(m_cur_u_seg.buffer+index, comp_dist_length);

	index+=comp_dist_length;

	uint16_t comp_stream_length = _decode_uint16_little(m_cur_u_seg.buffer+index);
	index+=2;

	NRF_LOG_INFO("Effort stream length: %u", comp_stream_length);

	NRF_LOG_HEXDUMP_INFO(m_cur_u_seg.buffer+index, comp_stream_length);
	_dump_as_char(m_cur_u_seg.buffer+index, comp_stream_length);

	index+=comp_stream_length;
}

///////////////////////////////////////////////////////////////////


int main(int argc, char **argv) {

	memcpy(m_cur_u_seg.buffer, m_seg_file, sizeof(m_seg_file));

	_handle_segment_parse();

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
#if 1//TEST3
//	const uint8_t enc[] = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B};
	const uint8_t enc[] = {0x04,0x00,0x00,0x00,0x79,0xF0,0x1C,0x82,0x13,0x1F,0x02,0x0E,0x59,0x00,0x00,0x00};
//	const uint8_t enc[] = {0x34,0x00,0x00,0x00,0x74,0x1D,0xDE,0x26,0x7A,0x22,0x09,0x68,0x26,0xE7,0xEB,0xFC,0x02,0x67,0x97,0xF4,0xCC,0x77,0x00,0x00};
//	const uint8_t enc[] = {0x54,0x38,0x4F,0x00,0xD1,0xC7,0x06,0x00,0x59,0x1E,0x33,0x32,0x4F,0x40,0x3F,0x44,0x37,0x2A,0x3F,0x2E,0x39,0x2A,0x3F,0x38,0x4D,0x26,0x55,0x32,0x3D,0x2A,0x4F,0x28};

	Base64 myBase64;
	ByteBuffer bBuffer;
	bBuffer.addU(enc, sizeof(enc));

	SequenceUnpacker unpacker(bBuffer, b64_encoded_size(sizeof(enc)));

	unpacker.unpack();

	for (int i=0; i < unpacker.original.size(); i++) {
		printf("decoded:  '%ld \n", (int32_t)unpacker.original[i]);
	}
#endif
	return 0;
}
