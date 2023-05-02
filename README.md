# HASHish
A some-what of a reliable SHA256 replica I decided to write (as a utility) in-preparation for the development of a cheat loader.
Should be reliable for computing message digests.

## Usage
To use **HASHish** function, create an instance of the `CHash` class and call the `ComputeHash` function:

```cpp
#include "CHash.hxx"

std::unique_ptr<CHash> pHasher = std::make_unique<CHash>();

std::string message = "This is a message to be hashed.";
std::string hash_value = pHasher->ComputeHash(message);

std::cout << "Message: " << message << std::endl;
std::cout << "Hash value: " << hash_value << std::endl;
