#include <hashmap.h>
#include <iostream>
#include <string>
#include <unistd.h>

#include <iostream>
#include <iterator>
#include <random>
#include <string>

namespace {
std::string const default_chars = "abcdefghijklmnaoqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";
}

std::string random_string(size_t len = 15, std::string const& allowed_chars = default_chars)
{
    std::mt19937_64 gen { std::random_device()() };
    std::uniform_int_distribution<size_t> dist { 0, allowed_chars.length() - 1 };
    std::string ret;
    std::generate_n(std::back_inserter(ret), len, [&] { return allowed_chars[dist(gen)]; });
    return ret;
}

int main()
{
    my::hashmap<std::string, std::string> h;
    for (unsigned int i = 0; i < 100000; ++i) {
        auto s = random_string();
        h.insert(s, s);
    }
    return EXIT_SUCCESS;
}
