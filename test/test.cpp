#define BOOST_UT_DISABLE_MODULE
//#include <https://raw.githubusercontent.com/boost-ext/ut/master/include/boost/ut.hpp>
#include "ut.hpp" // import boost.ut;
namespace ut = boost::ut;

#define TEST_UNITS
#include "test_facilities.hpp" // test::*

// The includes in main.cpp:
#include "sfml-addons.hpp" // sfadd::*
#include "sfml-addons-color.hpp" // sfadd::Color
#include "universe.hpp" // Universe

int main()
{
    ut::expect(true);
}
