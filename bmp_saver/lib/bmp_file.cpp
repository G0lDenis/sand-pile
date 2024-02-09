#include "bmp_file.h"

#include <stdexcept>
#include <fstream>
#include <iostream>
#include <cmath>

BmpFile::BmpFile(std::deque<std::deque<int>>& deq) : con_deq(deq) {};

void BmpFile::WriteHeaders(std::ofstream& stream) {
	// Bytes are inverted in BMP
	// Set alignment to 1 byte and return it back

#pragma pack(push, 1)
	struct FileHeader {
		uint16_t type = 0x4D42;
		uint32_t size{0};
		uint16_t reserved_1{0};
		uint16_t reserved_2{0};
		uint32_t offset{0};
	} file_h;
#pragma pack(pop)

	struct InfoHeader {
		uint32_t head_size{40};
		uint32_t bmp_width{0};
		uint32_t bmp_length{0};
		uint16_t plane{1};
		uint16_t bit_per_pix{4};
		uint32_t compression{0};
		uint32_t pict_size{0}; // picture is not compressed
		uint32_t pix_per_x_meter{0};
		uint32_t pix_per_y_meter{0};
		uint32_t palette{5};
		uint32_t used{5};
	} info_h;

	info_h.bmp_width = width;
	info_h.bmp_length = length;

	struct WhiteRGBQuad {
		uint8_t blue{255};
		uint8_t green{255};
		uint8_t red{255};
		uint8_t reserved{0};
	} white;

	struct GreenRGBQuad {
		uint8_t blue{0};
		uint8_t green{255};
		uint8_t red{0};
		uint8_t reserved{0};
	} green;

	struct PurpleRGBQuad {
		uint8_t blue{255};
		uint8_t green{0};
		uint8_t red{139};
		uint8_t reserved{0};
	} purple;

	struct YellowRGBQuad {
		uint8_t blue{0};
		uint8_t green{255};
		uint8_t red{255};
		uint8_t reserved{0};
	} yellow;

	struct BlackRGBQuad {
		uint8_t blue{0};
		uint8_t green{0};
		uint8_t red{0};
		uint8_t reserved{0};
	} black;

	bytes_per_line = 4 * static_cast<size_t>(ceil(double(info_h.bit_per_pix * width) / 32));
	real_bytes = static_cast<size_t>(ceil(double(info_h.bit_per_pix * width) / 8));

	file_h.size = sizeof(file_h) + sizeof(InfoHeader) + 5 * sizeof(WhiteRGBQuad) + bytes_per_line * width;
	file_h.offset = sizeof(file_h) + sizeof(InfoHeader) + 5 * sizeof(WhiteRGBQuad);

	stream.write(reinterpret_cast<const char*>(&file_h), sizeof(file_h));
	stream.write(reinterpret_cast<const char*>(&info_h), sizeof(info_h));
	stream.write(reinterpret_cast<const char*>(&white), sizeof(white));
	stream.write(reinterpret_cast<const char*>(&green), sizeof(green));
	stream.write(reinterpret_cast<const char*>(&purple), sizeof(purple));
	stream.write(reinterpret_cast<const char*>(&yellow), sizeof(yellow));
	stream.write(reinterpret_cast<const char*>(&black), sizeof(black));
}

void BmpFile::WriteToFile(std::string& filename) {
	std::ofstream stream(filename, std::ios::binary);
	if (!stream.is_open()) {
		throw std::runtime_error("failed to open " + filename);
	}

	width = con_deq.size();
	if (!con_deq.empty()) {
		length = con_deq[0].size();
	}
	WriteHeaders(stream);

	if (!width) {
		stream.close();
		return;
	}

	for (auto i = width; i > 0;) {
		i--;
		uint8_t bytes[bytes_per_line];
		size_t k = 0;
		int j = 0;
		for (; j < real_bytes * 2; j++) {
			k = j / 2;
			uint8_t num;

			if (j >= length || con_deq[i][j] == 0) num = 0;
			else if (con_deq[i][j] == 1) num = 1;
			else if (con_deq[i][j] == 2) num = 2;
			else if (con_deq[i][j] == 3) num = 3;
			else num = 4;
			bytes[k] = (bytes[k] << 4) + num;
		}
		k++;

		for (; k < bytes_per_line; k++) {
			bytes[k] = 0;
		}
		for (auto m: bytes) {
			stream << m;
		}
	}
	stream.close();
}
