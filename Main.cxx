#include "CHash.hxx"
#include <random>
#include <format>

constexpr std::string_view kPrecompiledHash = "cfbf361d4e99c319e8acf1c3b780dfc2897afb084889aaba89ad729fd903fcde";

// cc: Stackoverflow
std::string random_string(std::size_t length)
{
	static const std::string alphabet = "abcdefghijklmnopqrstuvwxyz";
	static std::default_random_engine rng(std::chrono::system_clock::now().time_since_epoch().count());
	static std::uniform_int_distribution<std::size_t> distribution(0, alphabet.size() - 1);
	std::string str;
	while (str.size() < length) str += alphabet[distribution(rng)];
	return str;
}

int main() try
{
	std::unique_ptr<CHash> pHasher = std::make_unique<CHash>();

	const std::string szHello = "i am a test";
	const std::string szHash = pHasher->ComputeHash(szHello);

	for (std::size_t i = 1; i < 40; i++)
	{
		/* Currently only supporting 32 characters, if you think this is an issue: i encourage you to finish the implementation. */
		if (i >= 32) {
			std::cout << "\n\n:frowning: seems like i can't do this - \nseems like you'll have to finish the implementation. \nhint: std::vector<std::uint8_t> CHash::PadMessage(const std::vector<std::uint8_t>& message)" << std::endl;
			break;
		}

		std::string random = random_string(i);
		std::cout << "HASH: " << pHasher->ComputeHash(random) << '\n';
	}

	return EXIT_SUCCESS;
}
catch (std::runtime_error err)
{
	std::cerr << std::format("An error occured: {}", err.what()) << std::endl;
	return EXIT_FAILURE;
}