#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "hashmap.h"

#include <doctest/doctest.h>

using namespace my;

SCENARIO("Implementation shoud proceed successfully")
{
    GIVEN("An implementation")
    {
        hashmap<std::string, std::string> map;
        WHEN("After constructing")
        {
            THEN("Size is empty")
            {
                REQUIRE(map.size() == 0);
                REQUIRE(map.is_empty());
            }
        }
        WHEN("A key is inserted (even multiple times)")
        {
            map.insert("hello", "world");
            map.insert("hello", "world");
            map.insert("hello2", "world2");
            THEN("Should have one element and not be empty")
            {
                REQUIRE(map.size() == 2);
                REQUIRE(!map.is_empty());
            }
            AND_THEN("We can query value")
            {
                auto val = map.find("hello");
                REQUIRE(val != map.end());
                REQUIRE(val->second == "world");
            }
            AND_THEN("We can change the element and retrieve the last one")
            {
                auto val = map.insert("hello", "wworld");
                REQUIRE(!val.first);
                REQUIRE(val.second->second == "wworld");
                const auto it = map.cfind("hello");
                REQUIRE(it != map.cend());
                REQUIRE(it->second == "wworld");
                map.insert("hello", "world");
            }
            AND_THEN("We can remove the element")
            {
                auto val = map.pop("hello");
                REQUIRE(val);
                REQUIRE(val.value() == "world");
                val = map.pop("hello");
                REQUIRE(!val);
                REQUIRE(map.size() == 1);
                map.insert("hello", "world");
            }
            AND_THEN("can iterate")
            {
                map.insert("hello3", "world3");
                std::size_t count = 0;
                for (auto& v : map) {
                    ++count;
                    (void)v;
                }

                REQUIRE(count == 3);
            }
            AND_THEN("can iterate const")
            {
                map.insert("hello3", "world3");
                std::size_t count = 0;
                for (const auto& v : map) {
                    ++count;
                    (void)v;
                }

                const auto it = map.cbegin();
                (void)it;

                REQUIRE(count == 3);
            }
            AND_THEN("can get iterator to key")
            {
                auto it = map.find("hello");
                REQUIRE(it->second == "world");
                it = map.find("bla");
                REQUIRE(it == map.end());
            }
            AND_THEN("can get const iterator to key")
            {
                auto it = map.cfind("hello");
                REQUIRE(it->second == "world");
                it = map.cfind("bla");
                REQUIRE(it == map.cend());
            }
        }
    }
}
