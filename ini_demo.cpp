#include <QSettings>
#include <QCoreApplication>

#include <ctime>
#include <iostream>

#include "iniproc/ini_processing.h"

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

void dumpTests()
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

    if(sumTime1 < sumTime2)
        printf(" MY WIN\n");
    else if(sumTime1 > sumTime2)
        printf(" QSettings WIN\n");
    else
        printf(" DRAW! (both My and QSettings has same time!)\n");

    printf("My (%f) vs QSettings (%f)\n", sumTime1, sumTime2);
    printf("==========================================================================\n");
}

template<class IniParser, class String>
void testIniParser(std::string iniFile, TestValues1<String> &g_testData, bool deepTestOfBig = false)
{
    std::clock_t begin = 0;
    std::clock_t end = 0;
    double       elapsed_secs = 0.0;
#define BEGIN()     begin   = std::clock();
#define END(tmsg)   end     = std::clock();\
    elapsed_secs = (double(end - begin) / CLOCKS_PER_SEC) * 1000.0;\
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


int main(int argc, char **argv)
{
    std::cout.precision(10);
    std::cout.setf(std::ios::fixed);
    QCoreApplication x(argc, argv);
    std::clock_t begin = 0;
    std::clock_t end = 0;
    double elapsed_secs1 = 0.0;
    double elapsed_secs2 = 0.0;
    //
    //Test big INI (some)
    {
        tests.clear();
        begin   = std::clock();
        testIniParser<IniProcessing, std::string>("../example-big-1.ini", g_testDataM, false);
        end     = std::clock();
        elapsed_secs1 = (double(end - begin) / CLOCKS_PER_SEC) * 1000.0;
        tests1 = tests;
        tests.clear();
        begin  = std::clock();
        testIniParser<QSettings, QString>("../example-big-1.ini", g_testDataQ, false);
        end    = std::clock();
        elapsed_secs2 = (double(end - begin) / CLOCKS_PER_SEC) * 1000.0;
        tests2 = tests;
        dumpTests();
        printf("\nTotal time (big ini, some): My (debug/release): %f, QSettings (release): %f\n\n",
               elapsed_secs1,
               elapsed_secs2);
        g_testDataM.spit();
        g_testDataQ.spit();
    }
    //
    //Test big INI (full)
    {
        tests.clear();
        begin   = std::clock();
        testIniParser<IniProcessing, std::string>("../example-big-2.ini", g_testDataM, true);
        end     = std::clock();
        elapsed_secs1 = (double(end - begin) / CLOCKS_PER_SEC) * 1000.0;
        tests1 = tests;
        tests.clear();
        begin  = std::clock();
        testIniParser<QSettings, QString>("../example-big-2.ini", g_testDataQ, true);
        end    = std::clock();
        elapsed_secs2 = (double(end - begin) / CLOCKS_PER_SEC) * 1000.0;
        tests2 = tests;
        dumpTests();
        printf("\nTotal time (big ini, full): My (debug/release): %f, QSettings (release): %f\n\n",
               elapsed_secs1,
               elapsed_secs2);
        g_testDataM.spit();
        g_testDataQ.spit();
    }
    //
    //Test middle INI
    {
        tests.clear();
        begin   = std::clock();
        testIniParser<IniProcessing, std::string>("../example-mid.ini", g_testDataM);
        end     = std::clock();
        elapsed_secs1 = (double(end - begin) / CLOCKS_PER_SEC) * 1000.0;
        tests1 = tests;
        tests.clear();
        begin  = std::clock();
        testIniParser<QSettings, QString>("../example-mid.ini", g_testDataQ);
        end    = std::clock();
        elapsed_secs2 = (double(end - begin) / CLOCKS_PER_SEC) * 1000.0;
        tests2 = tests;
        dumpTests();
        printf("\nTotal time (middle ini): My (debug/release): %f, QSettings (release): %f\n\n", elapsed_secs1, elapsed_secs2);
        g_testDataM.spit();
        g_testDataQ.spit();
    }
    //
    //Test small INI
    {
        tests.clear();
        begin   = std::clock();
        testIniParser<IniProcessing, std::string>("../example-tiny.ini", g_testDataM);
        end     = std::clock();
        elapsed_secs1 = (double(end - begin) / CLOCKS_PER_SEC) * 1000.0;
        tests1 = tests;
        tests.clear();
        begin  = std::clock();
        testIniParser<QSettings, QString>("../example-tiny.ini", g_testDataQ);
        end    = std::clock();
        elapsed_secs2 = (double(end - begin) / CLOCKS_PER_SEC) * 1000.0;
        tests2 = tests;
        dumpTests();
        printf("\nTotal time (tiny ini): My (debug/release): %f, QSettings (release): %f\n\n", elapsed_secs1, elapsed_secs2);
        g_testDataM.spit();
        g_testDataQ.spit();
    }
    x.quit();
    return 0;
}
