#include <cstdint>
#include <deque>
#include <string>

class BmpFile {
private:
	std::deque<std::deque<int>>& con_deq;
	size_t bytes_per_line{0};
	size_t real_bytes{0};
	uint32_t width{0};
	uint32_t length{0};
public:
	explicit BmpFile(std::deque<std::deque<int>>& deq);

	void WriteToFile(std::string& filename);

	void WriteHeaders(std::ofstream& stream);
};
