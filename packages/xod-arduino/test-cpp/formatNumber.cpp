
#include "catch.hpp"

extern char* formatNumber(double val, int width, unsigned int prec, char* sout);

TEST_CASE("formatNumber", "[formatNumber]") {
    char buf[16];

    SECTION("positive integer") {
        REQUIRE(formatNumber(42.0, 0, 0, buf) == std::string("42"));
        REQUIRE(formatNumber(42.0, 4, 0, buf) == std::string("  42"));
        REQUIRE(formatNumber(42.0, -4, 0, buf) == std::string("42  "));
    }

    SECTION("negative integer") {
        REQUIRE(formatNumber(-42.0, 0, 0, buf) == std::string("-42"));
        REQUIRE(formatNumber(-42.0, 4, 0, buf) == std::string(" -42"));
        REQUIRE(formatNumber(-42.0, -4, 0, buf) == std::string("-42 "));
    }

    SECTION("positive real") {
        REQUIRE(formatNumber(12.34, 0, 0, buf) == std::string("12"));
        REQUIRE(formatNumber(12.34, 0, 2, buf) == std::string("12.34"));
        REQUIRE(formatNumber(12.34, 0, 3, buf) == std::string("12.340"));
        REQUIRE(formatNumber(12.34, 7, 2, buf) == std::string("  12.34"));
        REQUIRE(formatNumber(12.34, -7, 3, buf) == std::string("12.340 "));
    }

    SECTION("negative real") {
        REQUIRE(formatNumber(-12.34, 0, 0, buf) == std::string("-12"));
        REQUIRE(formatNumber(-12.34, 0, 2, buf) == std::string("-12.34"));
        REQUIRE(formatNumber(-12.34, 0, 3, buf) == std::string("-12.340"));
        REQUIRE(formatNumber(-12.34, 8, 2, buf) == std::string("  -12.34"));
        REQUIRE(formatNumber(-12.34, -8, 3, buf) == std::string("-12.340 "));
    }
}
