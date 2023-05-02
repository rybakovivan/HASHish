#include <array>
#include <cstdint>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

class CHash {
public:
	static constexpr std::size_t kChunkSizeBytes = 64;

	// As the name says - prepare a schedule.
	static void PrepareMessageSchedule(
		const std::uint8_t* block, std::vector<std::uint32_t>& message_schedule);
	// If that's something that interests you. x)
	static void ApplyCompressionFunction(
		std::uint32_t* hash_values,
		const std::vector<std::uint32_t>& message_schedule);

	// Computes a hash.
	std::vector<std::uint8_t> ComputeHashInternal(
		const std::vector<std::uint8_t>& message);

	// A wrapper around the ComputeHashInternal to return a string.
	std::string ComputeHash(const std::string& message);

	// Helpers.
	std::vector<std::uint8_t> HashToVector(const std::uint32_t* hash_blocks);
	static std::string VectorToString(const std::vector<std::uint8_t>& input);
	static std::vector<std::uint8_t> StringToVector(const std::string& input);

private:
	static constexpr std::size_t kNumHashValues = 8;
	static constexpr std::size_t kBlockSize = 64;
	static constexpr std::size_t kHashSize = 32;
	static constexpr std::size_t kMessageScheduleSize = 64;

	static constexpr std::array<std::uint32_t, 64> kConstants = {
		0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1,
		0x923f82a4, 0xab1c5ed5, 0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
		0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174, 0xe49b69c1, 0xefbe4786,
		0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
		0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147,
		0x06ca6351, 0x14292967, 0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
		0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85, 0xa2bfe8a1, 0xa81a664b,
		0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
		0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a,
		0x5b9cca4f, 0x682e6ff3, 0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
		0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2 };

	static std::uint32_t RotateRight(std::uint32_t value, std::uint32_t count) {
		return (value >> count) | (value << (32 - count));
	}

	std::vector<std::uint32_t> ComputeHashBlocks(
		const std::vector<std::uint8_t>& message);
	static std::vector<std::uint8_t> PadMessage(
		const std::vector<std::uint8_t>& message);
	static std::vector<std::uint32_t> ComputeInitialHashValues();
};

void CHash::PrepareMessageSchedule(
	const std::uint8_t* block, std::vector<std::uint32_t>& message_schedule) {
	message_schedule.resize(kMessageScheduleSize);

	for (std::size_t i = 0; i < kMessageScheduleSize; ++i) {
		if (i < 16) {
			message_schedule[i] = (std::uint32_t(block[4 * i]) << 24) |
				(std::uint32_t(block[4 * i + 1]) << 16) |
				(std::uint32_t(block[4 * i + 2]) << 8) |
				(std::uint32_t(block[4 * i + 3]));
		}
		else {
			const std::uint32_t s0 = RotateRight(message_schedule[i - 15], 7) ^
				RotateRight(message_schedule[i - 15], 18) ^
				(message_schedule[i - 15] >> 3);
			const std::uint32_t s1 = RotateRight(message_schedule[i - 2], 17) ^
				RotateRight(message_schedule[i - 2], 19) ^
				(message_schedule[i - 2] >> 10);
			message_schedule[i] =
				message_schedule[i - 16] + s0 + message_schedule[i - 7] + s1;
		}
	}
}

void CHash::ApplyCompressionFunction(
	std::uint32_t* hash_values,
	const std::vector<std::uint32_t>& message_schedule) {
	std::array<std::uint32_t, kNumHashValues> working_vars;
	std::memcpy(working_vars.data(), hash_values,
		kNumHashValues * sizeof(std::uint32_t));

	for (std::size_t i = 0; i < kMessageScheduleSize; ++i) {
		const std::uint32_t s1 = RotateRight(working_vars[4], 6) ^
			RotateRight(working_vars[4], 11) ^
			RotateRight(working_vars[4], 25);
		const std::uint32_t ch = (working_vars[4] & working_vars[5]) ^
			(~working_vars[4] & working_vars[6]);
		const std::uint32_t temp1 =
			working_vars[7] + s1 + ch + kConstants[i] + message_schedule[i];
		const std::uint32_t s0 = RotateRight(working_vars[0], 2) ^
			RotateRight(working_vars[0], 13) ^
			RotateRight(working_vars[0], 22);
		const std::uint32_t maj = (working_vars[0] & working_vars[1]) ^
			(working_vars[0] & working_vars[2]) ^
			(working_vars[1] & working_vars[2]);
		const std::uint32_t temp2 = s0 + maj;

		working_vars[7] = working_vars[6];
		working_vars[6] = working_vars[5];
		working_vars[5] = working_vars[4];
		working_vars[4] = working_vars[3] + temp1;
		working_vars[3] = working_vars[2];
		working_vars[2] = working_vars[1];
		working_vars[1] = working_vars[0];
		working_vars[0] = temp1 + temp2;
	}

	for (std::size_t i = 0; i < kNumHashValues; ++i) {
		hash_values[i] += working_vars[i];
	}
}

std::vector<std::uint8_t> CHash::ComputeHashInternal(
	const std::vector<std::uint8_t>& message) {
	std::vector<std::uint8_t> padded_message = PadMessage(message);
	std::vector<std::uint32_t> hash_blocks = ComputeHashBlocks(padded_message);

	return HashToVector(hash_blocks.data());
}

std::string CHash::ComputeHash(const std::string& message) {
	std::vector<std::uint8_t> message_bytes(message.begin(), message.end());
	std::vector<std::uint8_t> hash_bytes = ComputeHashInternal(message_bytes);

	return VectorToString(hash_bytes);
}

std::vector<std::uint32_t> CHash::ComputeHashBlocks(
	const std::vector<std::uint8_t>& message) {
	std::vector<std::uint32_t> hash_values = ComputeInitialHashValues();

	for (std::size_t i = 0; i < message.size(); i += kBlockSize) {
		std::vector<std::uint32_t> message_schedule;
		PrepareMessageSchedule(&message[i], message_schedule);
		ApplyCompressionFunction(hash_values.data(), message_schedule);
	}

	return hash_values;
}

std::vector<std::uint8_t> CHash::PadMessage(
	const std::vector<std::uint8_t>& message) {
	const std::size_t num_blocks = (message.size() + kBlockSize - 1) / kBlockSize;
	const std::size_t padded_length = num_blocks * kBlockSize;
	const std::size_t message_length = message.size();

	std::vector<std::uint8_t> padded_message(padded_length);

	std::memcpy(padded_message.data(), message.data(), message_length);

	padded_message[message_length] = 0x80;

	const std::size_t zero_padding_size =
		padded_length - message_length - 1 - kHashSize;
	std::memset(padded_message.data() + message_length + 1, 0, zero_padding_size);

	const std::uint64_t message_bit_length =
		static_cast<std::uint64_t>(message_length) * 8;
	const std::size_t message_bit_length_position = padded_length - kHashSize;
	std::memcpy(&padded_message[message_bit_length_position], &message_bit_length,
		sizeof(message_bit_length));

	return padded_message;
}

std::vector<std::uint32_t> CHash::ComputeInitialHashValues() {
	return { 0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
			0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19 };
}

std::vector<std::uint8_t> CHash::HashToVector(
	const std::uint32_t* hash_blocks) {
	std::vector<std::uint8_t> hash(kHashSize);
	for (std::size_t i = 0; i < kNumHashValues; ++i) {
		const std::uint8_t* hash_block_bytes =
			reinterpret_cast<const std::uint8_t*>(&hash_blocks[i]);
		hash[i * sizeof(std::uint32_t) + 0] = hash_block_bytes[3];
		hash[i * sizeof(std::uint32_t) + 1] = hash_block_bytes[2];
		hash[i * sizeof(std::uint32_t) + 2] = hash_block_bytes[1];
		hash[i * sizeof(std::uint32_t) + 3] = hash_block_bytes[0];
	}
	return hash;
}

std::string CHash::VectorToString(const std::vector<std::uint8_t>& input) {
	std::stringstream ss;
	ss << std::hex << std::setfill('0');
	for (const auto& byte : input) {
		ss << std::setw(2) << static_cast<int>(byte);
	}
	return ss.str();
}

std::vector<std::uint8_t> CHash::StringToVector(const std::string& input) {
	std::vector<std::uint8_t> output(input.size() / 2);
	for (std::size_t i = 0; i < input.size(); i += 2) {
		output[i / 2] = std::stoi(input.substr(i, 2), nullptr, 16);
	}
	return output;
}