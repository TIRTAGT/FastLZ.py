#include "data.h"

vector<byte> string_to_bytes(const string& str) {
	vector<byte> bytes;
	bytes.reserve(str.size());

	for (char ch : str) {
		bytes.push_back(static_cast<byte>(ch));
	}

	return bytes;
}

vector<byte> uint8_to_bytes(const vector<uint8_t>& value) {
	vector<byte> bytes;
	bytes.reserve(value.size());

	for (uint8_t byte_value : value) {
		bytes.push_back(static_cast<byte>(byte_value));
	}

	return bytes;
}

vector<vector<byte>> compressed_data = {
	uint8_to_bytes({0x0B, 0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x20, 0x77, 0x6F, 0x72, 0x6C, 0x64})
};

vector<vector<byte>> decompressed_data = {
	string_to_bytes("Hello world!")
};
