#include "argument_parser/lib/parser.h"
#include "lib/sand_model.h"
#include "bmp_saver/lib/bmp_file.h"

#include <iostream>
#include <string>
#include <tuple>


std::tuple<int, int, std::string, std::string, int, int, int> local_parse(int argc, char** argv) {
	std::vector<std::tuple<std::string, std::string, std::string>> vec1 = {{"-l", "--length",   "0"},
																		   {"-w", "--width",    "0"},
																		   {"-i", "--input",    ""},
																		   {"-o", "--output",   R"(../../images)"},
																		   {"-m", "--max-iter", "-1"},
																		   {"-f", "--freq",     "0"},
																		   {"-p", "--print",    "0"}};
	// Default values for parameters in right order
	// Added parameter print for debugging sand pile output
	std::tuple<int, int, std::string, std::string, int, int, int> res = std::make_tuple(0, 0, "", "", -1, 0, 0);
	std::vector<std::string> vec = parse(argc, argv, vec1);

	std::get<0>(res) = strtol(vec[0].c_str(), nullptr, 0);
	std::get<1>(res) = strtol(vec[1].c_str(), nullptr, 0);
	std::get<2>(res) = vec[2];
	std::get<3>(res) = vec[3];
	std::get<4>(res) = strtol(vec[4].c_str(), nullptr, 0);
	std::get<5>(res) = strtol(vec[5].c_str(), nullptr, 0);
	std::get<6>(res) = strtol(vec[6].c_str(), nullptr, 0);

	return res;
}

void WriteImage(SandModel& model, std::string& folder, int number) {
	auto bmp = BmpFile(model.GetDeque());
	std::string str = folder + "/" + std::to_string(number) + ".bmp";
	bmp.WriteToFile(str);
}

int main(int argc, char** argv) {
	int length;
	int width;
	std::string input_file;
	std::string output_dir;
	int max_iter;
	int freq;
	int print_sand;
	std::tie(length, width, input_file, output_dir, max_iter, freq, print_sand) = local_parse(argc, argv);
	if (!width || !length || input_file.empty()) {
		std::cout << "some of the parameters entered incorrectly or not entered";
		exit(1);
	}

	SandModel sand_model = SandModel(length, width);
	try {
		sand_model.LoadFromTsv(input_file);
		if (print_sand) std::cout << sand_model << "\n\n";
	}
	catch (std::runtime_error& re) {
		std::cout << re.what();
		exit(-1);
	}

	try {
		int iteration = 1;
		while ((max_iter == -1 || iteration <= max_iter) && sand_model.CollapseSand()) {
			// Uncomment for checking current iteration with another frequency
			// if (iteration % 1000 == 0) std::cout << iteration << '\n';
			if (freq && ((iteration - 1) % freq == 0)) WriteImage(sand_model, output_dir, iteration);
			if (freq && ((iteration - 1) % freq == 0) && print_sand) std::cout << sand_model << "\n\n";
			iteration++;
		}
	}
	catch (std::overflow_error& oe) {
		std::cout << oe.what();
		exit(-1);
	}

	//Final image version storages with name 0
	WriteImage(sand_model, output_dir, 0);

	return 0;
}
