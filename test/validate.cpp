#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include <catch/catch.hpp>
#include <string>
#include <vector>
#include <IniProcessor/ini_processing.h>


TEST_CASE( "Field reading validation", "[ini-reading]" )
{
    IniProcessing ini;
    long long integer = 0;
    double    floating = 0;
    std::string str = "";
    std::vector<long>   integers;
    std::vector<double> floats;
    bool    boolean = false;

    REQUIRE( ini.open("../validate.ini") == true );

    //Count of groups
    REQUIRE( ini.childGroups().size() == 5 );

    //Group of integers
    REQUIRE( ini.beginGroup("integers") == true );
    SECTION( "Validating integers" )
    {
        REQUIRE( ini.hasKey("integer-p") == true );
        REQUIRE( ini.hasKey("what-the-heck") == false );

        ini.read("integer-p", integer, 0);
        REQUIRE( integer == 46257);

        ini.read("integer-n", integer, 0);
        REQUIRE( integer == -4562);

        integer = 0;
        ini.read("non-existing-field", integer, 243671);
        REQUIRE( integer == 243671);
    }
    ini.endGroup();

    REQUIRE( ini.beginGroup("fake-group") == false );
    SECTION( "Validating default values" )
    {
        integer = 0;
        ini.read("another-non-existing-field", integer, 54636);
        REQUIRE( integer == 54636);
    }
    ini.endGroup();

    REQUIRE( ini.beginGroup("floatings") == true );
    SECTION( "Validating floating point numbers" )
    {
        ini.read("floating-p1", floating, 0);
        REQUIRE( floating == 341.1235283);

        ini.read("floating-p2", floating, 0);
        REQUIRE( floating == +32.015);

        ini.read("floating-n", floating, 0);
        REQUIRE( floating == -5427.3441);

        ini.read("floating-s1", floating, 0);
        REQUIRE( floating ==  4.234e5);

        ini.read("floating-s2", floating, 0);
        REQUIRE( floating ==  4256.234e-2);
    }
    ini.endGroup();


    REQUIRE( ini.beginGroup("strings") == true );
    SECTION( "Validating strings include escape sequence probes" )
    {
        ini.read("string1", str, "");
        REQUIRE( str.compare("this is a string") == 0 );

        ini.read("string2", str, "");
        REQUIRE( str.compare("Just nosense: 35yh;lm21lm58w4 W^HJU6v346vu634v46u") == 0 );

        ini.read("string3", str, "");
        REQUIRE( str.compare("Cook\"a\"doodle\"doo!\"") == 0 );

        ini.read("string4", str, "");
        REQUIRE( str.compare("Строка в юникоде! 😁 😼") == 0 );

        ini.read("string5", str, "");
        REQUIRE( str.compare("顔文字ブロックの全ての文字は絵文字である。") == 0 );

        ini.read("string6", str, "");
        REQUIRE( str.compare("Escaping a lot:\nMeow\t\tI\nlike-this \"bike\" which really is not a bike") == 0 );
    }
    ini.endGroup();

    REQUIRE( ini.beginGroup("arrays") == true );
    SECTION( "Validating numeric arrays" )
    {
        ini.read("list-int", integers, std::vector<long>());
        REQUIRE( integers.size() == 5 );
        REQUIRE( integers[0] == 5 );
        REQUIRE( integers[1] == 13 );
        REQUIRE( integers[2] == 745 );
        REQUIRE( integers[3] == -231 );
        REQUIRE( integers[4] == 3436 );

        ini.read("list-float", floats, std::vector<double>());
        REQUIRE( floats.size() == 5 );
        REQUIRE( floats[0] == 0.1 );
        REQUIRE( floats[1] == 84.45 );
        REQUIRE( floats[2] == -472.234 );
        REQUIRE( floats[3] == -0.3e6 );
        REQUIRE( floats[4] == +0.3e-5 );
    }
    ini.endGroup();

    REQUIRE( ini.beginGroup("bools") == true );
    SECTION( "Validating booleans" )
    {
        ini.read("bool1-t", boolean, false);
        REQUIRE( boolean );
        ini.read("bool1-f", boolean, true);
        REQUIRE_FALSE( boolean );
        ini.read("bool2-t", boolean, false);
        REQUIRE( boolean );
        ini.read("bool2-f", boolean, true);
        REQUIRE_FALSE( boolean );
        ini.read("bool3-t", boolean, false);
        REQUIRE( boolean );
        ini.read("bool3-f", boolean, true);
        REQUIRE_FALSE( boolean );
    }
    ini.endGroup();
}


TEST_CASE( "Field writing and then reading to validate", "[ini-writing]" )
{
    SECTION( "Write the experimental file" )
    {
        IniProcessing myOutput("writeme.ini");

        myOutput.beginGroup("My-Pets");
        {
            myOutput.setValue("meow", 45.234);
            myOutput.setValue("woof", -542);
            myOutput.setValue("pee", 42);
            myOutput.setValue("koo", "3.41 It's a string with 34 number!");
            myOutput.setValue("MY-DIGITS", std::vector<int>({3, -5, 134, 12, 546, 12, 0, -4}));
            myOutput.setValue("MY-FLOAT", std::vector<double>({0.34,-32561.23,+34e4,-452.21E34,-12,9,1,333,-135e4}));
            myOutput.setValue("really", true);
            myOutput.setValue("why", false);
            myOutput.setValue("jailbreak", "Hello!\nI'm jack\t-\ttheive."
                                           "\nMy nick is \"bulldozer\""
                                           " because I have to escape "
                                           "from jail by destroying a wall.");
        }
        myOutput.endGroup();

        myOutput.beginGroup("My-Chickens");
        {
            myOutput.setValue("hens", 34);
            myOutput.setValue("cocks", 1);
            myOutput.setValue("eggs", 45266);
            myOutput.setValue("chicks", 46715171257);
            myOutput.setValue("cost", -15.245);
            myOutput.setValue("cost2", -15.245346521);
            myOutput.setValue("result", -11.245e-7);
        }
        myOutput.endGroup();

        REQUIRE( myOutput.writeIniFile() );
    }

    SECTION( "Modify one of exist fields" )
    {
        IniProcessing ini;
        REQUIRE( ini.open("writeme.ini") );

        REQUIRE( ini.beginGroup("My-Pets") );
        SECTION( "My-Pets" )
        {
            double floating = 0;
            ini.read("meow", floating, 0);
            REQUIRE( floating == 45.234);

            ini.setValue("meow", +87.3466e3);
        }
        ini.endGroup();

        REQUIRE( ini.writeIniFile() );
    }

    SECTION( "Reading to validate result" )
    {
        long long integer = 0;
        double    floating = 0;
        std::string str = "";
        std::vector<long>   integers;
        std::vector<double> floats;
        bool    boolean = false;

        IniProcessing ini("writeme.ini");

        REQUIRE( ini.beginGroup("My-Pets") );
        SECTION( "My-Pets" )
        {
            ini.read("meow", floating, 0);
            REQUIRE( floating == +87.3466e3);

            ini.read("woof", integer, 0);
            REQUIRE( integer == -542);

            ini.read("pee", integer, 0);
            REQUIRE( integer == 42);

            ini.read("koo", str, "");
            REQUIRE( str.compare("3.41 It's a string with 34 number!") == 0 );

            ini.read("MY-DIGITS", integers, std::vector<long>());
            REQUIRE( integers.size() == 8 );

            ini.read("MY-FLOAT", floats, std::vector<double>());
            REQUIRE( floats.size() == 9 );

            ini.read("really", boolean, false);
            REQUIRE( boolean == true);

            ini.read("why", boolean, true);
            REQUIRE( boolean == false);

            ini.read("jailbreak", str, "");
            REQUIRE( str.compare("Hello!\nI'm jack\t-\ttheive."
                                 "\nMy nick is \"bulldozer\""
                                 " because I have to escape "
                                 "from jail by destroying a wall.") == 0);
        }
        ini.endGroup();

        REQUIRE( ini.beginGroup("My-Chickens") );
        SECTION( "My-Chickens" )
        {
            ini.read("hens", floating, 0);
            REQUIRE( floating == 34);

            ini.read("cocks", floating, 0);
            REQUIRE( floating == 1);

            ini.read("eggs", floating, 0);
            REQUIRE( floating == 45266);

            ini.read("chicks", floating, 0);
            REQUIRE( floating == 46715171257);

            ini.read("cost", floating, 0);
            REQUIRE( floating == -15.245);

            ini.read("cost2", floating, 0);
            REQUIRE( floating == -15.245346521);

            ini.read("result", floating, 0);
            REQUIRE( floating == -11.245e-7);
        }
        ini.endGroup();
    }
}
