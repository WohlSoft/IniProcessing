#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include <catch/catch.hpp>
#include <string>
#include <vector>
#include <ini_processing.h>


TEST_CASE( "Field reading validation", "[ini-reading]" )
{
    IniProcessing ini;
    long long integer = 0;
    double    floating = 0;
    std::string str = "";
    std::wstring wstr = L"";
    std::vector<long>   integers;
    std::vector<double> floats;
    bool    boolean = false;

    REQUIRE( ini.open(L"../validate-u16.ini") == true );

    //Count of groups
    REQUIRE( ini.childGroups().size() == 5 );

    //Group of integers
    REQUIRE( ini.beginGroup(L"integers") == true );
    SECTION( "Validating integers" )
    {
        REQUIRE( ini.hasKey(L"integer-p") == true );
        REQUIRE( ini.hasKey(L"what-the-heck") == false );

        ini.read(L"integer-p", integer, 0);
        REQUIRE( integer == 46257);

        ini.read(L"integer-n", integer, 0);
        REQUIRE( integer == -4562);

        integer = 0;
        ini.read(L"non-existing-field", integer, 243671);
        REQUIRE( integer == 243671);
    }
    ini.endGroup();

    REQUIRE( ini.beginGroup(L"fake-group") == false );
    SECTION( "Validating default values" )
    {
        integer = 0;
        ini.read(L"another-non-existing-field", integer, 54636);
        REQUIRE( integer == 54636);
    }
    ini.endGroup();

    REQUIRE( ini.beginGroup("floatings") == true );
    SECTION( "Validating floating point numbers" )
    {
        ini.read(L"floating-p1", floating, 0);
        REQUIRE( floating == 341.1235283);

        ini.read(L"floating-p2", floating, 0);
        REQUIRE( floating == +32.015);

        ini.read(L"floating-n", floating, 0);
        REQUIRE( floating == -5427.3441);

        ini.read(L"floating-s1", floating, 0);
        REQUIRE( floating ==  4.234e5);

        ini.read(L"floating-s2", floating, 0);
        REQUIRE( floating ==  4256.234e-2);
    }
    ini.endGroup();


    REQUIRE( ini.beginGroup(L"strings") == true );
    SECTION( "Validating UTF-8 strings include escape sequence probes" )
    {
        // UTF8 string
        ini.read(L"string1", str, "");
        REQUIRE( str.compare("this is a string") == 0 );

        ini.read(L"string2", str, "");
        REQUIRE( str.compare("Just nosense: 35yh;lm21lm58w4 W^HJU6v346vu634v46u") == 0 );

        ini.read(L"string3", str, "");
        REQUIRE( str.compare("Cook\"a\"doodle\"doo!\"") == 0 );

        ini.read(L"string4", str, "");
        REQUIRE( str.compare("Строка в юникоде! 😁 😼") == 0 );

        ini.read(L"string5", str, "");
        REQUIRE( str.compare("顔文字ブロックの全ての文字は絵文字である。") == 0 );

        ini.read(L"string6", str, "");
        REQUIRE( str.compare("Escaping a lot:\nMeow\t\tI\nlike-this \"bike\" which really is not a bike") == 0 );
    }

    SECTION( "Validating UTF-16 strings include escape sequence probes" )
    {
        ini.read(L"string1", wstr, L"");
        REQUIRE( wstr.compare(L"this is a string") == 0 );

        ini.read(L"string2", wstr, L"");
        REQUIRE( wstr.compare(L"Just nosense: 35yh;lm21lm58w4 W^HJU6v346vu634v46u") == 0 );

        ini.read(L"string3", wstr, L"");
        REQUIRE( wstr.compare(L"Cook\"a\"doodle\"doo!\"") == 0 );

        ini.read(L"string4", wstr, L"");
        REQUIRE( wstr.compare(L"Строка в юникоде! 😁 😼") == 0 );

        ini.read(L"string5", wstr, L"");
        REQUIRE( wstr.compare(L"顔文字ブロックの全ての文字は絵文字である。") == 0 );

        ini.read(L"string6", wstr, L"");
        REQUIRE( wstr.compare(L"Escaping a lot:\nMeow\t\tI\nlike-this \"bike\" which really is not a bike") == 0 );
    }

    ini.endGroup();

    REQUIRE( ini.beginGroup(L"arrays") == true );
    SECTION( "Validating numeric arrays" )
    {
        ini.read(L"list-int", integers, std::vector<long>());
        REQUIRE( integers.size() == 5 );
        REQUIRE( integers[0] == 5 );
        REQUIRE( integers[1] == 13 );
        REQUIRE( integers[2] == 745 );
        REQUIRE( integers[3] == -231 );
        REQUIRE( integers[4] == 3436 );

        ini.read(L"list-float", floats, std::vector<double>());
        REQUIRE( floats.size() == 5 );
        REQUIRE( floats[0] == 0.1 );
        REQUIRE( floats[1] == 84.45 );
        REQUIRE( floats[2] == -472.234 );
        REQUIRE( floats[3] == -0.3e6 );
        REQUIRE( floats[4] == +0.3e-5 );
    }
    ini.endGroup();

    REQUIRE( ini.beginGroup(L"bools") == true );
    SECTION( "Validating booleans" )
    {
        ini.read(L"bool1-t", boolean, false);
        REQUIRE( boolean );
        ini.read(L"bool1-f", boolean, true);
        REQUIRE_FALSE( boolean );
        ini.read(L"bool2-t", boolean, false);
        REQUIRE( boolean );
        ini.read(L"bool2-f", boolean, true);
        REQUIRE_FALSE( boolean );
        ini.read(L"bool3-t", boolean, false);
        REQUIRE( boolean );
        ini.read(L"bool3-f", boolean, true);
        REQUIRE_FALSE( boolean );
    }
    ini.endGroup();
}


TEST_CASE( "Field writing and then reading to validate", "[ini-writing]" )
{
    SECTION( "Write the experimental file" )
    {
        IniProcessing myOutput(L"writeme-u16.ini");

        myOutput.beginGroup(L"My-Pets");
        {
            myOutput.setValue(L"meow", 45.234);
            myOutput.setValue(L"woof", -542);
            myOutput.setValue(L"pee", 42);
            myOutput.setValue(L"koo", L"3.41 It's a string with 34 number!");
            myOutput.setValue(L"MY-DIGITS", std::vector<int>({3, -5, 134, 12, 546, 12, 0, -4}));
            myOutput.setValue(L"MY-FLOAT", std::vector<double>({0.34,-32561.23,+34e4,-452.21E34,-12,9,1,333,-135e4}));
            myOutput.setValue(L"really", true);
            myOutput.setValue(L"why", false);
            myOutput.setValue(L"jailbreak", L"Hello!\nI'm jack\t-\ttheive."
                                             "\nMy nick is \"bulldozer\""
                                             " because I have to escape "
                                             "from jail by destroying a wall.");
        }
        myOutput.endGroup();

        myOutput.beginGroup(L"My-Chickens");
        {
            myOutput.setValue(L"hens", 34);
            myOutput.setValue(L"cocks", 1);
            myOutput.setValue(L"eggs", 45266);
            myOutput.setValue(L"chicks", 46715171257);
            myOutput.setValue(L"cost", -15.245);
            myOutput.setValue(L"cost2", -15.245346521);
            myOutput.setValue(L"result", -11.245e-7);
        }
        myOutput.endGroup();

        REQUIRE( myOutput.writeIniFile() );
    }

    SECTION( "Modify one of exist fields" )
    {
        IniProcessing ini;
        REQUIRE( ini.open(L"writeme-u16.ini") );

        REQUIRE( ini.beginGroup(L"My-Pets") );
        SECTION( "My-Pets" )
        {
            double floating = 0;
            ini.read(L"meow", floating, 0);
            REQUIRE( floating == 45.234);

            ini.setValue(L"meow", +87.3466e3);
        }
        ini.endGroup();

        REQUIRE( ini.writeIniFile() );
    }

    SECTION( "Reading to validate result" )
    {
        long long integer = 0;
        double    floating = 0;
        std::wstring str = L"";
        std::vector<long>   integers;
        std::vector<double> floats;
        bool    boolean = false;

        IniProcessing ini(L"writeme-u16.ini");

        REQUIRE( ini.beginGroup(L"My-Pets") );
        SECTION( "My-Pets" )
        {
            ini.read(L"meow", floating, 0);
            REQUIRE( floating == +87.3466e3);

            ini.read(L"woof", integer, 0);
            REQUIRE( integer == -542);

            ini.read(L"pee", integer, 0);
            REQUIRE( integer == 42);

            ini.read(L"koo", str, L"");
            REQUIRE( str.compare(L"3.41 It's a string with 34 number!") == 0 );

            ini.read(L"MY-DIGITS", integers, std::vector<long>());
            REQUIRE( integers.size() == 8 );

            ini.read(L"MY-FLOAT", floats, std::vector<double>());
            REQUIRE( floats.size() == 9 );

            ini.read(L"really", boolean, false);
            REQUIRE( boolean == true);

            ini.read(L"why", boolean, true);
            REQUIRE( boolean == false);

            ini.read(L"jailbreak", str, L"");
            REQUIRE( str.compare(L"Hello!\nI'm jack\t-\ttheive."
                                  "\nMy nick is \"bulldozer\""
                                  " because I have to escape "
                                  "from jail by destroying a wall.") == 0);
        }
        ini.endGroup();

        REQUIRE( ini.beginGroup(L"My-Chickens") );
        SECTION( "My-Chickens" )
        {
            ini.read(L"hens", floating, 0);
            REQUIRE( floating == 34);

            ini.read(L"cocks", floating, 0);
            REQUIRE( floating == 1);

            ini.read(L"eggs", floating, 0);
            REQUIRE( floating == 45266);

            ini.read(L"chicks", floating, 0);
            REQUIRE( floating == 46715171257);

            ini.read(L"cost", floating, 0);
            REQUIRE( floating == -15.245);

            ini.read(L"cost2", floating, 0);
            REQUIRE( floating == -15.245346521);

            ini.read(L"result", floating, 0);
            REQUIRE( floating == -11.245e-7);
        }
        ini.endGroup();
    }
}
