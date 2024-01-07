/*
Copyright (c) 2016-2024 Vitaliy Novichkov <admin@wohlnet.ru>

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
*/

#include <QSettings>
#include <QCoreApplication>

#include <iostream>
#include <chrono>

#include "../src/ini_processing.h"

class ElapsedTimer
{
public:
    typedef std::chrono::nanoseconds TimeT;
    ElapsedTimer() {}
    void start()
    {
        recent = std::chrono::high_resolution_clock::now();
    }
    void restart()
    {
        recent = std::chrono::high_resolution_clock::now();
    }
    int64_t elapsed()
    {
        using std::chrono::nanoseconds;
        using std::chrono::duration_cast;
        return duration_cast<nanoseconds>(std::chrono::high_resolution_clock::now() - recent).count();
    }
    std::chrono::high_resolution_clock::time_point recent;
};

#define MYBENCHMARK(resultTime, expression)\
{\
    ElapsedTimer clock;\
    clock.start();\
    {expression}\
    resultTime = clock.elapsed();\
}

std::ostream &operator<<(std::ostream &out, QString &x)
{
    out << x.toStdString();
    return out;
}

struct TePair
{
    std::string text;
    double time;
};

template<class String>
struct TestValues1
{
    String      whoIAm;
    double      failEffectGravity;
    String      myFolderIs;
    double      ican;
    String      gravityAccelX;
    double      gravityAccel;
    std::vector<int> intarray;
    std::vector<double> doublearray;
    void spit()
    {
        std::cout  << "I'm a " << whoIAm << " ^^\n" <<
                   "my fail effect gravity is " << failEffectGravity << " pixels per 1/65 second, whee!\n"
                   << "My folder also named as " << myFolderIs << " :D\nAlso my force is " << ican <<
                   " and gravity acceleration is " << gravityAccel << ", or just " << gravityAccelX << " :)\n\n";
    }
};

static TestValues1<QString>        g_testDataQ;
static TestValues1<std::string>    g_testDataM;

static std::vector<TePair> tests1;
static std::vector<TePair> tests2;

static std::vector<TePair> tests;

void dumpTests(bool noComplare = false)
{
    printf("==========================================================================\n");
    printf("    My INI-parser             |          QSettings           |    Result? \n");
    printf("==========================================================================\n");
    double sumTime1 = 0.0;
    double sumTime2 = 0.0;

    for(unsigned int i = 0; i < tests1.size(); i++)
    {
        TePair &v1 = tests1[i];
        TePair &v2 = tests2[i];
        printf("%-21s:",  v1.text.c_str());
        printf("%5f|",  v1.time);
        printf("%-21s:",  v2.text.c_str());
        printf("%5f|",  v2.time);
        sumTime1 += v1.time;
        sumTime2 += v2.time;

        if(v1.time < v2.time)
            printf("%8s", "My!\n");
        else if(v1.time > v2.time)
            printf("%8s", "Q!\n");
        else
            printf("%8s", "DRAW!\n");
    }

    printf("==========================================================================\n");

    if(noComplare)
    {
        printf("Test result was %f\n", sumTime1);
    }
    else
    {
        if(sumTime1 < sumTime2)
            printf(" MY WIN\n");
        else if(sumTime1 > sumTime2)
            printf(" QSettings WIN\n");
        else
            printf(" DRAW! (both My and QSettings has same time!)\n");

        printf("My (%f) vs QSettings (%f)\n", sumTime1, sumTime2);
    }
    printf("==========================================================================\n");
    fflush(stdout);
}

template<class IniParser, class String>
void testIniParser(std::string iniFile, TestValues1<String> &g_testData, bool deepTestOfBig = false)
{
    ElapsedTimer timer;
    double       elapsed_secs = 0.0;
#define BEGIN()     timer.restart();
#define END(tmsg)   \
    elapsed_secs = double(timer.elapsed()) / 100000.0;\
    tests.push_back({tmsg, elapsed_secs});
    /************************************************************/
    BEGIN()
    /*IniProcessing*/
    IniParser ini(iniFile.c_str(), QSettings::IniFormat);
    END("Time to init");

    if(deepTestOfBig)
    {
        static String chrs[] =
        {
            "character-1",
            "character-2",
            "character-3",
            "character-4",
            "character-5"
        };
        static String chrs_world[] =
        {
            "character-1-world",
            "character-2-world",
            "character-3-world",
            "character-4-world",
            "character-5-world"
        };
        static const char *chrs_phys[3][5] =
        {
            {
                "character-1-env-common-air",
                "character-2-env-common-air",
                "character-3-env-common-air",
                "character-4-env-common-air",
                "character-5-env-common-air"
            },
            {
                "character-1-env-common-water",
                "character-2-env-common-water",
                "character-3-env-common-water",
                "character-4-env-common-water",
                "character-5-env-common-water"
            },
            {
                "character-1-env-common-quicksand",
                "character-2-env-common-quicksand",
                "character-3-env-common-quicksand",
                "character-4-env-common-quicksand",
                "character-5-env-common-quicksand"
            }
        };
        String dummyString;
        double dummyDouble;
        unsigned int dummyUint;
        int     dummySint;
        bool    dummyBool;
        Q_UNUSED(dummyDouble);
        Q_UNUSED(dummyUint);
        Q_UNUSED(dummySint);
        Q_UNUSED(dummyBool);

        for(int i = 0; i < 5; i++)
        {
            BEGIN()
            ini.beginGroup(chrs[i]);
            END("Time to begin");
            BEGIN()
            g_testData.whoIAm              = ini.value("name", "Dr. Zhopa").toString();
            END("Time to take Str");
            BEGIN()
            g_testData.failEffectGravity   = ini.value("fail-effect-gravity", 1.3).toDouble();
            END("Time to take double");
            BEGIN()
            g_testData.myFolderIs          = ini.value("sprite-folder", "wat???").toString();
            END("Time to take Str");
            BEGIN()
            ini.endGroup();
            END("Time to end");
        }

        for(int i = 0; i < 5; i++)
        {
            BEGIN()
            ini.beginGroup(chrs_world[i]);
            END("Time to begin");
            BEGIN()
            dummyString = ini.value("sprite-name", "Dr. Zhopa").toString();
            END("Time to take Str");
            BEGIN()
            dummyUint = ini.value("frames-total", 0).toUInt();
            END("Time to take UInt");
            BEGIN()
            dummyUint = ini.value("frames-speed", 0).toUInt();
            END("Time to take UInt");
            BEGIN()
            dummySint = ini.value("offset-y", 0).toInt();
            END("Time to take UInt");
            BEGIN()
            dummyString = ini.value("frames-down", "Dr. Zhopa").toString();
            END("Time to take Str");
            BEGIN()
            dummyString = ini.value("frames-right", "Dr. Zhopa").toString();
            END("Time to take Str");
            BEGIN()
            dummyString = ini.value("frames-left", "Dr. Zhopa").toString();
            END("Time to take Str");
            BEGIN()
            dummyString = ini.value("frames-up", "Dr. Zhopa").toString();
            END("Time to take Str");
            BEGIN()
            ini.endGroup();
            END("Time to end");
        }

        for(int i = 0; i < 3; i++)
            for(int j = 0; j < 5; j++)
            {
                BEGIN()
                ini.beginGroup(chrs_phys[i][j]);
                END("Time to begin");
                BEGIN()
                dummyDouble   = ini.value("walk_force", 1.3).toDouble();
                END("Time to take double");
                BEGIN()
                dummyDouble   = ini.value("run_force", 1.3).toDouble();
                END("Time to take double");
                BEGIN()
                dummyDouble   = ini.value("decelerate_stop", 1.3).toDouble();
                END("Time to take double");
                BEGIN()
                dummyDouble   = ini.value("decelerate_turn", 1.3).toDouble();
                END("Time to take double");
                BEGIN()
                dummyDouble   = ini.value("decelerate_air", 1.3).toDouble();
                END("Time to take double");
                BEGIN()
                dummyDouble   = ini.value("slippery_c", 1.3).toDouble();
                END("Time to take double");
                BEGIN()
                dummyDouble   = ini.value("gravity_accel", 1.3).toDouble();
                END("Time to take double");
                BEGIN()
                dummyDouble   = ini.value("gravity_scale", 1.3).toDouble();
                END("Time to take double");
                BEGIN()
                dummyDouble   = ini.value("velocity_jump", 1.3).toDouble();
                END("Time to take double");
                BEGIN()
                dummyDouble   = ini.value("velocity_jump_bounce", 1.3).toDouble();
                END("Time to take double");
                BEGIN()
                dummyDouble   = ini.value("velocity_jump_spring", 1.3).toDouble();
                END("Time to take double");
                BEGIN()
                dummyDouble   = ini.value("velocity_jump_c", 1.3).toDouble();
                END("Time to take double");
                BEGIN()
                dummyUint   = ini.value("jump_time", 12).toUInt();
                END("Time to take UInt");
                BEGIN()
                dummyUint   = ini.value("jump_time_spring", 12).toUInt();
                END("Time to take UInt");
                BEGIN()
                dummyDouble   = ini.value("velocity_climb_x", 1.3).toDouble();
                END("Time to take double");
                BEGIN()
                dummyDouble   = ini.value("velocity_climb_y_up", 1.3).toDouble();
                END("Time to take double");
                BEGIN()
                dummyDouble   = ini.value("velocity_climb_y_down", 1.3).toDouble();
                END("Time to take double");
                BEGIN()
                dummyDouble   = ini.value("MaxSpeed_walk", 1.3).toDouble();
                END("Time to take double");
                BEGIN()
                dummyDouble   = ini.value("MaxSpeed_run", 1.3).toDouble();
                END("Time to take double");
                BEGIN()
                dummyDouble   = ini.value("MaxSpeed_up", 1.3).toDouble();
                END("Time to take double");
                BEGIN()
                dummyDouble   = ini.value("MaxSpeed_down", 1.3).toDouble();
                END("Time to take double");
                BEGIN()
                dummyBool   = ini.value("zero_speed_y_on_enter", false).toBool();
                END("Time to take Bool");
                BEGIN()
                dummyDouble   = ini.value("slow_up_speed_y_coeff", 1.3).toDouble();
                END("Time to take double");
                BEGIN()
                dummyBool   = ini.value("slow_speed_x_on_enter", false).toBool();
                END("Time to take Bool");
                BEGIN()
                dummyDouble   = ini.value("slow_speed_x_coeff", 1.3).toDouble();
                END("Time to take double");
                BEGIN()
                ini.endGroup();
                END("Time to end");
            }
    }
    else
    {
        BEGIN()
        ini.beginGroup("character-3");
        END("Time to begin");
        BEGIN()
        g_testData.whoIAm              = ini.value("name", "Dr. Zhopa").toString();
        END("Time to take Str");
        BEGIN()
        g_testData.failEffectGravity   = ini.value("fail-effect-gravity", 1.3).toDouble();
        END("Time to take double");
        BEGIN()
        g_testData.myFolderIs          = ini.value("sprite-folder", "wat???").toString();
        END("Time to take Str");
        BEGIN()
        ini.endGroup();
        END("Time to end");
        BEGIN()
        ini.beginGroup("character-3-env-common-air");
        END("Time to begin 2");
        BEGIN()
        g_testData.ican                = ini.value("walk_force", 0.0).toDouble();
        END("Time to take double");
        BEGIN()
        g_testData.gravityAccelX       = ini.value("gravity_accel", 0.0).toString();
        END("Time to take Str");
        BEGIN()
        g_testData.gravityAccel        = ini.value("gravity_accel", 0.0).toDouble();
        END("Time to take Double");
        BEGIN()
        ini.endGroup();
        END("Time to end 2");
    }

    /************************************************************/
#undef BEGIN
#undef END
}

void testIniParserNEW(std::string iniFile, TestValues1<std::string> &g_testData, bool deepTestOfBig = false)
{
    ElapsedTimer timer;
    double       elapsed_secs = 0.0;
#define BEGIN()     timer.restart();
#define END(tmsg)   \
    elapsed_secs = double(timer.elapsed()) / 100000.0;\
    tests.push_back({tmsg, elapsed_secs});
    /************************************************************/
    BEGIN()
    /*IniProcessing*/
    IniProcessing ini;

    if(!ini.open(iniFile.c_str()))
        printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\nWHAAAT???? %d, %d\n%%%%%%%%%%%%%%%%%%%%%%%%%%\n",
               ini.lastError(), ini.lineWithError());

    END("Time to init");

    if(deepTestOfBig)
    {
        static const char *chrs[] =
        {
            "character-1",
            "character-2",
            "character-3",
            "character-4",
            "character-5"
        };
        static const char *chrs_world[] =
        {
            "character-1-world",
            "character-2-world",
            "character-3-world",
            "character-4-world",
            "character-5-world"
        };
        static const char *chrs_phys[3][5] =
        {
            {
                "character-1-env-common-air",
                "character-2-env-common-air",
                "character-3-env-common-air",
                "character-4-env-common-air",
                "character-5-env-common-air"
            },
            {
                "character-1-env-common-water",
                "character-2-env-common-water",
                "character-3-env-common-water",
                "character-4-env-common-water",
                "character-5-env-common-water"
            },
            {
                "character-1-env-common-quicksand",
                "character-2-env-common-quicksand",
                "character-3-env-common-quicksand",
                "character-4-env-common-quicksand",
                "character-5-env-common-quicksand"
            }
        };
        std::string dummyString;
        double dummyDouble;
        unsigned int dummyUint;
        int     dummySint;
        bool    dummyBool;

        for(int i = 0; i < 5; i++)
        {
            BEGIN()
            ini.beginGroup(chrs[i]);
            END("Time to begin");
            BEGIN()
            ini.read("name", g_testData.whoIAm, "Dr. Zhopa");
            END("Time to take Str");
            BEGIN()
            ini.read("fail-effect-gravity", g_testData.failEffectGravity, 1.3);
            END("Time to take double");
            BEGIN()
            ini.read("sprite-folder", g_testData.myFolderIs, "wat???");
            END("Time to take Str");
            BEGIN()
            ini.endGroup();
            END("Time to end");
        }

        for(int i = 0; i < 5; i++)
        {
            BEGIN()
            ini.beginGroup(chrs_world[i]);
            END("Time to begin");
            BEGIN()
            ini.read("sprite-name", dummyString, "Dr. Zhopa");
            END("Time to take Str");
            BEGIN()
            ini.read("frames-total", dummyUint, 0);
            END("Time to take UInt");
            BEGIN()
            ini.read("frames-speed", dummyUint, 0);
            END("Time to take UInt");
            BEGIN()
            ini.read("offset-y", dummySint, 0);
            END("Time to take UInt");
            BEGIN()
            ini.read("frames-down", dummyString, "Dr. Zhopa");
            END("Time to take Str");
            BEGIN()
            ini.read("frames-right", dummyString, "Dr. Zhopa");
            END("Time to take Str");
            BEGIN()
            ini.read("frames-left", dummyString, "Dr. Zhopa");
            END("Time to take Str");
            BEGIN()
            ini.read("frames-up", dummyString, "Dr. Zhopa");
            END("Time to take Str");
            BEGIN()
            ini.endGroup();
            END("Time to end");
        }

        for(int i = 0; i < 3; i++)
            for(int j = 0; j < 5; j++)
            {
                BEGIN()
                ini.beginGroup(chrs_phys[i][j]);
                END("Time to begin");
                BEGIN()
                ini.read("walk_force", dummyDouble, 1.3);
                END("Time to take double");
                BEGIN()
                ini.read("run_force", dummyDouble, 1.3);
                END("Time to take double");
                BEGIN()
                ini.read("decelerate_stop", dummyDouble, 1.3);
                END("Time to take double");
                BEGIN()
                ini.read("decelerate_turn", dummyDouble, 1.3);
                END("Time to take double");
                BEGIN()
                ini.read("decelerate_air", dummyDouble, 1.3);
                END("Time to take double");
                BEGIN()
                ini.read("slippery_c", dummyDouble, 1.3);
                END("Time to take double");
                BEGIN()
                ini.read("gravity_accel", dummyDouble, 1.3);
                END("Time to take double");
                BEGIN()
                ini.read("gravity_scale", dummyDouble, 1.3);
                END("Time to take double");
                BEGIN()
                ini.read("velocity_jump", dummyDouble, 1.3);
                END("Time to take double");
                BEGIN()
                ini.read("velocity_jump_bounce", dummyDouble, 1.3);
                END("Time to take double");
                BEGIN()
                ini.read("velocity_jump_spring", dummyDouble, 1.3);
                END("Time to take double");
                BEGIN()
                ini.read("velocity_jump_c", dummyDouble, 1.3);
                END("Time to take double");
                BEGIN()
                ini.read("jump_time", dummyUint, 12u);
                END("Time to take UInt");
                BEGIN()
                ini.read("jump_time_spring", dummyUint, 12);
                END("Time to take UInt");
                BEGIN()
                ini.read("velocity_climb_x", dummyDouble, 1.3);
                END("Time to take double");
                BEGIN()
                ini.read("velocity_climb_y_up", dummyDouble, 1.3);
                END("Time to take double");
                BEGIN()
                ini.read("velocity_climb_y_down", dummyDouble, 1.3);
                END("Time to take double");
                BEGIN()
                ini.read("MaxSpeed_walk", dummyDouble, 1.3);
                END("Time to take double");
                BEGIN()
                ini.read("MaxSpeed_run", dummyDouble, 1.3);
                END("Time to take double");
                BEGIN()
                ini.read("MaxSpeed_up", dummyDouble, 1.3);
                END("Time to take double");
                BEGIN()
                ini.read("MaxSpeed_down", dummyDouble, 1.3);
                END("Time to take double");
                BEGIN()
                ini.read("zero_speed_y_on_enter", dummyBool, false);
                END("Time to take Bool");
                BEGIN()
                ini.read("slow_up_speed_y_coeff", dummyDouble, 1.3);
                END("Time to take double");
                BEGIN()
                ini.read("slow_speed_x_on_enter", dummyBool, false);
                END("Time to take Bool");
                BEGIN()
                ini.read("slow_speed_x_coeff", dummyDouble, 1.3);
                END("Time to take double");
                BEGIN()
                ini.endGroup();
                END("Time to end");
            }
    }
    else
    {
        BEGIN()
        ini.beginGroup("levels-main");
        END("Time to begin");
        BEGIN()
        ini.read("intarray", g_testData.intarray, {0});
        END("Time to take int[]");
        BEGIN()
        ini.read("doublearray", g_testData.doublearray, {0});
        END("Time to take double[]");
        BEGIN()
        ini.endGroup();
        END("Time to end");
    }

    /************************************************************/
#undef BEGIN
#undef END
}

int main(int argc, char **argv)
{
    std::cout.precision(10);
    std::cout.setf(std::ios::fixed);
    QCoreApplication x(argc, argv);
    double elapsed_secs1 = 0.0;
    double elapsed_secs2 = 0.0;

    ElapsedTimer timer;
    //
    //Test big INI (some)
    {
        tests.clear();
        timer.restart();
        testIniParser<IniProcessing, std::string>("../example-big-1.ini", g_testDataM, false);
        elapsed_secs1 = double(timer.elapsed()) / 1000000.0;
        tests1 = tests;
        tests.clear();
        timer.restart();
        testIniParser<QSettings, QString>("../example-big-1.ini", g_testDataQ, false);
        elapsed_secs2 = double(timer.elapsed()) / 1000000.0;
        tests2 = tests;
        dumpTests();
        printf("\nTotal time (big ini, partial reading): My (debug/release): %f, QSettings (release): %f\n\n",
               elapsed_secs1,
               elapsed_secs2);
        fflush(stdout);
        g_testDataM.spit();
        g_testDataQ.spit();
    }
    //
    //Test big INI (full)
    {
        tests.clear();
        timer.restart();
        testIniParser<IniProcessing, std::string>("../example-big-2.ini", g_testDataM, true);
        elapsed_secs1 = double(timer.elapsed()) / 1000000.0;
        tests1 = tests;
        tests.clear();
        timer.restart();
        testIniParser<QSettings, QString>("../example-big-2.ini", g_testDataQ, true);
        elapsed_secs2 = double(timer.elapsed()) / 1000000.0;
        tests2 = tests;
        dumpTests();
        printf("\nTotal time (big ini, reading of everything): My (debug/release): %f, QSettings (release): %f\n\n",
               elapsed_secs1,
               elapsed_secs2);
        fflush(stdout);
        g_testDataM.spit();
        g_testDataQ.spit();
    }
    //
    //Test big INI (full, new functions)
    {
        tests2 = tests1;//compare with old functions
        tests.clear();
        timer.restart();
        testIniParserNEW("../example-big-2.ini", g_testDataM, true);
        elapsed_secs1 = double(timer.elapsed()) / 1000000.0;
        tests1 = tests;
        dumpTests();
        printf("\nTotal time (big ini, full with new functions): My (new): %f, My (old): %f\n\n",
               elapsed_secs1,
               elapsed_secs2);
        fflush(stdout);
        g_testDataM.spit();
    }
    //
    //Test middle INI
    {
        tests.clear();
        timer.restart();
        testIniParser<IniProcessing, std::string>("../example-mid.ini", g_testDataM);
        elapsed_secs1 = double(timer.elapsed()) / 1000000.0;
        tests1 = tests;
        tests.clear();
        timer.restart();
        testIniParser<QSettings, QString>("../example-mid.ini", g_testDataQ);
        elapsed_secs2 = double(timer.elapsed()) / 1000000.0;
        tests2 = tests;
        dumpTests();
        printf("\nTotal time (middle ini): My (debug/release): %f, QSettings (release): %f\n\n", elapsed_secs1, elapsed_secs2);
        fflush(stdout);
        g_testDataM.spit();
        g_testDataQ.spit();
    }
    //
    //Test small INI
    {
        tests.clear();
        timer.restart();
        testIniParser<IniProcessing, std::string>("../example-tiny.ini", g_testDataM);
        elapsed_secs1 = double(timer.elapsed()) / 1000000.0;
        tests1 = tests;
        tests.clear();
        timer.restart();
        testIniParser<QSettings, QString>("../example-tiny.ini", g_testDataQ);
        elapsed_secs2 = double(timer.elapsed()) / 1000000.0;
        tests2 = tests;
        dumpTests();
        printf("\nTotal time (tiny ini): My (debug/release): %f, QSettings (release): %f\n\n", elapsed_secs1, elapsed_secs2);
        fflush(stdout);
        g_testDataM.spit();
        g_testDataQ.spit();
    }

    //
    //Test small INI with "read"
    {
        tests.clear();
        timer.start();
        printf("||||||||||||Testing of .read() output|||||||||\n");
        testIniParserNEW("../example-tiny.ini", g_testDataM, false);
        elapsed_secs1 = double(timer.elapsed()) / 1000000.0;
        tests1 = tests;
        //tests2 = tests;
        dumpTests();
        tests.clear();
    }

    x.quit();
    return 0;
}
