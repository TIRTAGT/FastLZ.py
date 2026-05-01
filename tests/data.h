#pragma once
#include <cstdint>
#include <cstddef>
#include <vector>
#include <string>

using std::vector;
using std::string;
using std::byte;

vector<byte> string_to_bytes(const string& str);
vector<byte> uint8_to_bytes(const vector<uint8_t>& value);

extern vector<vector<byte>> compressed_data;
extern vector<vector<byte>> decompressed_data;
