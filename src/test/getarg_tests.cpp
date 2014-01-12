#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "util.h"

BOOST_AUTO_TEST_SUITE(getarg_tests)

static void
ResetArgs(const std::string& strArg)
{
    std::vector<std::string> vecArg;
    boost::split(vecArg, strArg, boost::is_space(), boost::token_compress_on);

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-GUS");
    BOOST_CHECK(GetBoolArg("-GUS"));
    BOOST_CHECK(GetBoolArg("-GUS", false));
    BOOST_CHECK(GetBoolArg("-GUS", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-GUSo"));
    BOOST_CHECK(!GetBoolArg("-GUSo", false));
    BOOST_CHECK(GetBoolArg("-GUSo", true));

    ResetArgs("-GUS=0");
    BOOST_CHECK(!GetBoolArg("-GUS"));
    BOOST_CHECK(!GetBoolArg("-GUS", false));
    BOOST_CHECK(!GetBoolArg("-GUS", true));

    ResetArgs("-GUS=1");
    BOOST_CHECK(GetBoolArg("-GUS"));
    BOOST_CHECK(GetBoolArg("-GUS", false));
    BOOST_CHECK(GetBoolArg("-GUS", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noGUS");
    BOOST_CHECK(!GetBoolArg("-GUS"));
    BOOST_CHECK(!GetBoolArg("-GUS", false));
    BOOST_CHECK(!GetBoolArg("-GUS", true));

    ResetArgs("-noGUS=1");
    BOOST_CHECK(!GetBoolArg("-GUS"));
    BOOST_CHECK(!GetBoolArg("-GUS", false));
    BOOST_CHECK(!GetBoolArg("-GUS", true));

    ResetArgs("-GUS -noGUS");  // -GUS should win
    BOOST_CHECK(GetBoolArg("-GUS"));
    BOOST_CHECK(GetBoolArg("-GUS", false));
    BOOST_CHECK(GetBoolArg("-GUS", true));

    ResetArgs("-GUS=1 -noGUS=1");  // -GUS should win
    BOOST_CHECK(GetBoolArg("-GUS"));
    BOOST_CHECK(GetBoolArg("-GUS", false));
    BOOST_CHECK(GetBoolArg("-GUS", true));

    ResetArgs("-GUS=0 -noGUS=0");  // -GUS should win
    BOOST_CHECK(!GetBoolArg("-GUS"));
    BOOST_CHECK(!GetBoolArg("-GUS", false));
    BOOST_CHECK(!GetBoolArg("-GUS", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--GUS=1");
    BOOST_CHECK(GetBoolArg("-GUS"));
    BOOST_CHECK(GetBoolArg("-GUS", false));
    BOOST_CHECK(GetBoolArg("-GUS", true));

    ResetArgs("--noGUS=1");
    BOOST_CHECK(!GetBoolArg("-GUS"));
    BOOST_CHECK(!GetBoolArg("-GUS", false));
    BOOST_CHECK(!GetBoolArg("-GUS", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-GUS", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-GUS", "eleven"), "eleven");

    ResetArgs("-GUS -bar");
    BOOST_CHECK_EQUAL(GetArg("-GUS", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-GUS", "eleven"), "");

    ResetArgs("-GUS=");
    BOOST_CHECK_EQUAL(GetArg("-GUS", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-GUS", "eleven"), "");

    ResetArgs("-GUS=11");
    BOOST_CHECK_EQUAL(GetArg("-GUS", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-GUS", "eleven"), "11");

    ResetArgs("-GUS=eleven");
    BOOST_CHECK_EQUAL(GetArg("-GUS", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-GUS", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-GUS", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-GUS", 0), 0);

    ResetArgs("-GUS -bar");
    BOOST_CHECK_EQUAL(GetArg("-GUS", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-GUS=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-GUS", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-GUS=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-GUS", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--GUS");
    BOOST_CHECK_EQUAL(GetBoolArg("-GUS"), true);

    ResetArgs("--GUS=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-GUS", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noGUS");
    BOOST_CHECK(!GetBoolArg("-GUS"));
    BOOST_CHECK(!GetBoolArg("-GUS", true));
    BOOST_CHECK(!GetBoolArg("-GUS", false));

    ResetArgs("-noGUS=1");
    BOOST_CHECK(!GetBoolArg("-GUS"));
    BOOST_CHECK(!GetBoolArg("-GUS", true));
    BOOST_CHECK(!GetBoolArg("-GUS", false));

    ResetArgs("-noGUS=0");
    BOOST_CHECK(GetBoolArg("-GUS"));
    BOOST_CHECK(GetBoolArg("-GUS", true));
    BOOST_CHECK(GetBoolArg("-GUS", false));

    ResetArgs("-GUS --noGUS");
    BOOST_CHECK(GetBoolArg("-GUS"));

    ResetArgs("-noGUS -GUS"); // GUS always wins:
    BOOST_CHECK(GetBoolArg("-GUS"));
}

BOOST_AUTO_TEST_SUITE_END()
