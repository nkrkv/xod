
#include "catch.hpp"

using std::string;

extern char* formatNumber(
    double val, int width, bool pzero, unsigned int prec, char* sout);

TEST_CASE("formatNumber", "[formatNumber]") {
    char buf[16];

    SECTION("positive integer") {
        REQUIRE(formatNumber(42.0, 0, false, 0, buf) == string("42"));
        REQUIRE(formatNumber(42.0, 4, false, 0, buf) == string("  42"));
        REQUIRE(formatNumber(42.0, 4, true, 0, buf) == string("0042"));
        REQUIRE(formatNumber(42.0, -4, false, 0, buf) == string("42  "));
        REQUIRE(formatNumber(42.0, -4, true, 0, buf) == string("42  "));
    }

    SECTION("negative integer") {
        REQUIRE(formatNumber(-42.0, 0, false, 0, buf) == string("-42"));
        REQUIRE(formatNumber(-42.0, 4, false, 0, buf) == string(" -42"));
        REQUIRE(formatNumber(-42.0, 4, true, 0, buf) == string("-042"));
        REQUIRE(formatNumber(-42.0, -4, false, 0, buf) == string("-42 "));
        REQUIRE(formatNumber(-42.0, -4, true, 0, buf) == string("-42 "));
    }

    SECTION("positive real") {
        REQUIRE(formatNumber(12.34, 0, false, 0, buf) == string("12"));
        REQUIRE(formatNumber(12.34, 0, false, 2, buf) == string("12.34"));
        REQUIRE(formatNumber(12.34, 0, false, 3, buf) == string("12.340"));
        REQUIRE(formatNumber(12.34, 7, false, 2, buf) == string("  12.34"));
        REQUIRE(formatNumber(12.34, 7, true, 2, buf) == string("0012.34"));
        REQUIRE(formatNumber(12.34, -7, false, 3, buf) == string("12.340 "));
        REQUIRE(formatNumber(12.34, -7, true, 3, buf) == string("12.340 "));
    }

    SECTION("negative real") {
        REQUIRE(formatNumber(-12.34, 0, false, 0, buf) == string("-12"));
        REQUIRE(formatNumber(-12.34, 0, false, 2, buf) == string("-12.34"));
        REQUIRE(formatNumber(-12.34, 0, false, 3, buf) == string("-12.340"));
        REQUIRE(formatNumber(-12.34, 8, false, 2, buf) == string("  -12.34"));
        REQUIRE(formatNumber(-12.34, 8, true, 2, buf) == string("-0012.34"));
        REQUIRE(formatNumber(-12.34, -8, false, 3, buf) == string("-12.340 "));
        REQUIRE(formatNumber(-12.34, -8, true, 3, buf) == string("-12.340 "));
    }
}
