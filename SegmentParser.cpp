
#include <stdio.h>
#include <stdint.h>
#include <string.h>

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

static void _dump_as_hex(uint8_t const *p_data, uint16_t length) {

	if (!p_data) return;

	for (uint16_t i=0; i< length && i < 32; i++) {
		printf("%02X ", p_data[i]);
	}
	printf("\n");
}

static void _dump_as_char(uint8_t const *p_data, uint16_t length) {

	if (!p_data) return;

	for (uint16_t i=0; i< length && i < 32; i++) {
		printf("%c ", p_data[i]);
	}
	printf("\n");
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

	uint8_t index = 4;

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

	index+=3; // padding 0

	int16_t grade = (int16_t)_decode_uint16_little(m_cur_u_seg.buffer+index);
	index+=2;
	NRF_LOG_INFO("grade: %d", grade);

	index+=3; // padding 0

	// dump name
	uint16_t name_length = m_cur_u_seg.buffer[index];
	index+=1;

	NRF_LOG_INFO("name_length: %u", name_length);

	_dump_as_char(m_cur_u_seg.buffer+index, name_length); // name
	index+=name_length;

	uint16_t poly_length = _decode_uint16_little(m_cur_u_seg.buffer+index);
	index+=2;

	NRF_LOG_HEXDUMP_INFO(m_cur_u_seg.buffer+index, poly_length);
	index+=poly_length;

	NRF_LOG_INFO("Polyline length: %u", poly_length);

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

	uint16_t comp_dist_length = _decode_uint16_little(m_cur_u_seg.buffer+index);
	index+=2;

	NRF_LOG_INFO("Dist stream length: %u", comp_dist_length);

	index+=comp_dist_length;

	uint16_t comp_stream_length = _decode_uint16_little(m_cur_u_seg.buffer+index);
	index+=2;

	NRF_LOG_INFO("Effort stream length: %u", comp_dist_length);

	index+=comp_stream_length;
}

///////////////////////////////////////////////////////////////////


int main(int argc, char **argv) {

	memcpy(m_cur_u_seg.buffer, m_seg_file, sizeof(m_seg_file));

	_handle_segment_parse();

	return 0;
}
