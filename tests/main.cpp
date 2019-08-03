#include <iostream>

#include <functional>

#include <vector>

#include "Argon2/Argon2.h"
#include "Argon2/Constants.h"

#include "Blake2/Blake2b.h"

#include "Utilities/Utilities.h"

bool testHashFunction(std::string expectedOutput, std::string testName, std::function<std::vector<uint8_t>(void)> hashFunction)
{
    const std::string result = byteArrayToHexString(hashFunction());

    if (result != expectedOutput)
    {
        std::cout << "❌ Failed test for " << testName
                  << "\nExpected: " << expectedOutput
                  << "\nActual: " << result << std::endl;

        return false;
    }
    else
    {
        std::cout << "✔️  Passed test for " << testName << std::endl;

        return true;
    }
}

int main()
{
    std::vector<bool> results;

    const auto blakeExpected1 = "786a02f742015903c6c6fd852552d272912f4740e15847618a86e217f71f5419d25e1031afee585313896444934eb04b903a685b1448b755d56f701afe9be2ce";
    const auto blakeExpected2 = "a8add4bdddfd93e4877d2746e62817b116364a1fa7bc148d95090bc7333b3673f82401cf7aa2e4cb1ecd90296e3f14cb5413f8ed77be73045b13914cdcd6a918";

    results.push_back(testHashFunction(blakeExpected1, "Blake2b 1/2", [](){
        return Blake2b::Hash("");
    }));

    results.push_back(testHashFunction(blakeExpected2, "Blake2b 2/2", [](){
        return Blake2b::Hash("The quick brown fox jumps over the lazy dog");
    }));

    const std::vector<uint8_t> password = {
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1
    };

    const std::vector<uint8_t> salt = {
        2, 2, 2, 2, 2, 2, 2, 2,
        2, 2, 2, 2, 2, 2, 2, 2
    };

    const std::vector<uint8_t> key = {
        3, 3, 3, 3, 3, 3, 3, 3
    };

    const std::vector<uint8_t> associatedData = {
        4, 4, 4, 4, 4, 4, 4, 4,
        4, 4, 4, 4
    };

    const auto argonHash = [&password, &salt, &key, &associatedData](const Constants::ArgonVariant variant)
    {
        return Argon2::DeriveKey(
            variant,
            password,
            salt,
            key,
            associatedData,
            3,
            32,
            4,
            32
        );
    };

    const auto argon2DExpected = "512b391b6f1162975371d30919734294f868e3be3984f3c1a13a4db9fabe4acb";
    const auto argon2IExpected = "c814d9d1dc7f37aa13f0d77f2494bda1c8de6b016dd388d29952a4c4672b6ce8";
    const auto argon2IDExpected = "0d640df58d78766c08c037a34a8b53c9d01ef0452d75b65eb52520e96b01e659";

    results.push_back(testHashFunction(argon2DExpected, "Argon2D", [argonHash](){
        return argonHash(Constants::ARGON2D);
    }));

    results.push_back(testHashFunction(argon2IExpected, "Argon2I", [argonHash](){
        return argonHash(Constants::ARGON2I);
    }));

    results.push_back(testHashFunction(argon2IDExpected, "Argon2ID", [argonHash](){
        return argonHash(Constants::ARGON2ID);
    }));

    Argon2 argon2(Constants::ARGON2D, key, associatedData, 3, 32, 4, 32);

    results.push_back(testHashFunction(argon2DExpected, "Argon General 1/2", [&argon2, &password, &salt](){
        return argon2.Hash(password, salt);
    }));

    results.push_back(testHashFunction(argon2DExpected, "Argon General 2/2", [&argon2, &password, &salt](){
        return argon2.Hash(password, salt);
    }));

    const bool success = std::all_of(results.begin(), results.end(), [](const bool x) { return x; });

    if (success)
    {
        std::cout << "\nAll tests passed" << std::endl;
        return 0;
    }
    else
    {
        std::cout << "\nSome tests did not pass!" << std::endl;
        return 1;
    }
}
