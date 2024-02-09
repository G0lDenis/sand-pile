#include "sand_model.h"

#include <fstream>
#include <limits>
#include <iostream>

const int MAX_SIZE = std::numeric_limits<uint16_t>::max();

SandModel::SandModel(int l, int w, std::string filename) {
	length = l;
	width = w;
	if (!filename.empty()) {
		LoadFromTsv(filename);
	}
}

void SandModel::LoadFromTsv(std::string& filename) {
	sandbox = std::deque<std::deque<int>>(width, std::deque<int>(length));
	std::ifstream stream(filename, std::ios::binary);
	if (!stream.is_open()) {
		throw std::runtime_error("failed to open " + filename);
	}
	int x = 0;
	int y = 0;
	int val = 0;
	while (true) {
		if (stream >> x && stream >> y && stream >> val) {
			sandbox[x][y] = val;
		} else break;
	}
}

bool SandModel::CollapseSand() {
	// We need only 3 rows for checking collapsing,
	// so we don't need to make a copy of full deque
	bool left_offset = false;
	bool right_offset = false;
	bool upper_offset = false;
	bool lower_offset = false;
	bool changes = false;
	// Corner grains we calculate immediately
	for (int j = 0; j < length; j++) {
		if (sandbox[upper_offset][j] > 3) {
			if (!upper_offset) {
				AddUpperRow();
				upper_offset = true;
			}
			sandbox[0][j] = 1;
		}
		if (sandbox[width - 1 - lower_offset][j] > 3) {
			if (!lower_offset) {
				AddLowerRow();
				lower_offset = true;
			}
			sandbox[width - 1][j] = 1;
		}
	}
	for (int i = upper_offset; i < width - lower_offset; i++) {
		if (sandbox[i][left_offset] > 3) {
			if (!left_offset) {
				AddLeftColumn();
				left_offset = true;
			}
			sandbox[i][0] = 1;
		}
	}
	for (int i = upper_offset; i < width - lower_offset; i++) {
		if (sandbox[i][length - 1] > 3) {
			if (!right_offset) {
				AddRightColumn();
				right_offset = true;
			}
			sandbox[i][length - 1] = 1;
		}
	}
	std::deque<std::deque<int>> aux_deq(3, std::deque<int>(length - left_offset - right_offset, 0));
	for (int i = 0; i < width - upper_offset - lower_offset; i++) {
		for (int j = 0; j < length - left_offset - right_offset; j++) {
			if (sandbox[i + upper_offset][j + left_offset] > 3) {
				sandbox[i + upper_offset][j + left_offset] -= 4;
				// upper grain
				aux_deq[(i + 2) % 3][j]++;
				// lower grain
				aux_deq[(i + 1) % 3][j]++;
				// right grain
				if (j < length - left_offset - right_offset - 1) {
					aux_deq[i % 3][j + 1]++;
				}
				// left grain
				if (j > 0) {
					aux_deq[i % 3][j - 1]++;
				}
				changes = true;
			}
		}
		// Adding accumulated grains to upper row
		for (int j = 0; j < length - left_offset - right_offset; j++) {
			if (i) sandbox[i + upper_offset - 1][j + left_offset] += aux_deq[(i + 2) % 3][j];
			aux_deq[(i + 2) % 3][j] = 0;
		}
	}
	//Adding accumulated grains to final row
	for (int j = 0; j < length - left_offset - right_offset; j++) {
		sandbox[width - 1 - lower_offset][j + left_offset] += aux_deq[(width - 1 - lower_offset) % 3][j];
	}

	return changes;
}

void SandModel::AddUpperRow() {
	if (sandbox.size() >= MAX_SIZE) {
		throw std::overflow_error("Sand pile width is overflow");
	}
	sandbox.emplace_front(std::deque<int>(length, 0));
	width++;
}

void SandModel::AddLowerRow() {
	if (sandbox.size() >= MAX_SIZE) {
		throw std::overflow_error("Sand pile width is overflow");
	}
	sandbox.emplace_back(std::deque<int>(length, 0));
	width++;
}

void SandModel::AddLeftColumn() {
	if (sandbox[0].size() >= MAX_SIZE) {
		throw std::overflow_error("Sand pile length is overflow");
	}
	for (auto& row: sandbox) {
		row.emplace_front(0);
	}
	length++;
}

void SandModel::AddRightColumn() {
	if (sandbox[0].size() >= MAX_SIZE) {
		throw std::overflow_error("Sand pile length is overflow");
	}
	for (auto& row: sandbox) {
		row.emplace_back(0);
	}
	length++;
}

std::ostream& operator<<(std::ostream& stream, const SandModel& model) {
	for (int i = 0; i < model.width; i++) {
		for (const auto j: model.sandbox[i]) {
			stream << j << '\t';
		}
		if (i < model.width - 1) stream << '\n';
	}
	return stream;
}

std::deque<std::deque<int>>& SandModel::GetDeque() {
	return sandbox;
}
