#include <iostream>
#include <fstream>
#include <filesystem>
#include <span>
#include <vector>
#include <iomanip>
#include <sstream>
#include <limits>
#include <cstdint>
#include <string_view>

#include "test.hpp"

/* test unpack binary data
int main(int argc, const char* argv[])
try {
    std::string_view data(reinterpret_cast<const char*>(binary::buffer), sizeof(binary::buffer));

    std::ofstream file("test_unpack.txt", std::ios::binary);
    if (!file) {
        throw std::runtime_error("Failed to open file for writing");
    }

    file.write(data.data(), data.size());
    file.close();
    std::cout << "Press any key to exit...";
    std::cin.get();
    return 0;
} catch (const std::exception& e) {
    std::cerr << "error: " << e.what() << std::endl;
    std::cout << "Press any key to exit...";
    std::cin.get();
    return 1;
}
*/

int main(int argc, const char* argv[])
try {
    std::span<const char*> args{ argv, static_cast<std::size_t>(argc) };

    std::cout << "================================================" << std::endl;
    std::cout << "bin2header - convert binary file to header file" << std::endl;
    std::cout << "================================================" << std::endl;

    if (args.size() <= 1) {
        std::cout << "usage: bin2header <input_file> or drag and drop a file onto the executable\n"
            << "  input_file: path to the binary file to convert\n"
            << "  output: header file with the same name as input file, but with .hpp extension\n";
        std::cout << "Press any key to exit...";
        std::cin.get();
        return 0;
    }

    std::cout << "[+] check if source file exists" << std::endl;
    std::filesystem::path input_path(args[1]);
    if (!std::filesystem::exists(input_path)) {
        throw std::runtime_error("error: source file does not exist");
    }

    std::cout << "[+] generate output filename" << std::endl;
    std::filesystem::path output_path = input_path;
    output_path.replace_extension(".hpp");

    std::cout << "[+] read file into process" << std::endl;
    std::ifstream read_stream(input_path, std::ios::binary | std::ios::ate);
    if (!read_stream.is_open()) {
        throw std::runtime_error("error: failed to open source file");
    }
    auto file_size = read_stream.tellg();
    if (file_size < 0) {
        throw std::runtime_error("error: failed to get file size");
    }
    std::vector<unsigned char> buffer(static_cast<size_t>(file_size));
    read_stream.seekg(0, std::ios::beg);
    read_stream.read(reinterpret_cast<char*>(buffer.data()), buffer.size());
    read_stream.close();

    std::cout << "[+] convert file to hex" << std::endl;
    std::ostringstream output;
    output << "#pragma once\n\nnamespace binary {\n\tunsigned char buffer[] = {";
    constexpr size_t line_max = 24;
    for (size_t i = 0; i < buffer.size(); ++i) {
        if (i % line_max == 0)
            output << "\n\t\t";
        output << "0x" << std::hex << std::setw(2) << std::setfill('0')
            << static_cast<uint32_t>(buffer[i]) << std::dec << ", ";
    }
    output << "\n\t};\n}";

    std::cout << "[+] write to output file" << std::endl;
    std::ofstream write_stream(output_path, std::ios::out | std::ios::binary | std::ios::trunc);
    if (!write_stream.is_open()) {
        throw std::runtime_error("error: failed to open output file");
    }
    write_stream << output.str();
    write_stream.close();

    std::cout << "[+] success! file dumped to header file" << std::endl;
    std::cout << "[+] output: " << output_path << std::endl;
    std::cout << "Press any key to exit...";
    std::cin.get();
    return 0;
} catch (const std::exception& e) {
    std::cerr << "error: " << e.what() << std::endl;
    std::cout << "Press any key to exit...";
    std::cin.get();
    return 1;
}