#include <QSettings>
#include <QCoreApplication>

#include <ctime>
#include <iostream>

#include "iniproc/ini_processing.h"

std::ostream& operator<<(std::ostream& out, QString& x )
{
    out << x.toStdString();
    return out;
}

struct TePair
{
    std::string text;
    double time;
};

std::vector<TePair> tests1;
std::vector<TePair> tests2;

std::vector<TePair> tests;

void dumpTests()
{
    printf("==========================================================================\n");
    printf("    My INI-parser             |          QSettings           |    Result? \n");
    printf("==========================================================================\n");

    for(unsigned int i=0; i<tests1.size(); i++)
    {
        TePair&v1 = tests1[i];
        TePair&v2 = tests2[i];

        printf("%-21s:",  v1.text.c_str());
        printf("%5f|",  v1.time);
        printf("%-21s:",  v2.text.c_str());
        printf("%5f|",  v2.time);
        if(v1.time < v2.time)
            printf("%8s", "My!\n");
        else
            printf("%8s", "Q!\n");
    }
    printf("==========================================================================\n");
}

template<class IniParser, class String>
void testIniParser(std::string iniFile)
{

    std::clock_t begin = 0;
    std::clock_t end = 0;
    double       elapsed_secs = 0.0;

#define BEGIN() begin   = std::clock();
#define END(tmsg)   end     = std::clock();\
                    elapsed_secs = (double(end - begin) / CLOCKS_PER_SEC) * 1000.0;\
                    tests.push_back({tmsg, elapsed_secs});



    BEGIN()
    /*IniProcessing*/
    IniParser ini(iniFile.c_str(), QSettings::IniFormat);
    END("Time to init");

    BEGIN()
    ini.beginGroup("character-3");
    END("Time to begin");

    BEGIN()
        String      whoIAm              = ini.value("name", "Dr. Zhopa").toString();
    END("Time to take Str");
    BEGIN()
        double      failEffectGravity   = ini.value("fail-effect-gravity", 1.3).toDouble();
    END("Time to take double");
    BEGIN()
        String      myFolderIs          = ini.value("sprite-folder", "wat???").toString();
    END("Time to take Str");
    BEGIN()
    ini.endGroup();
    END("Time to end");

    BEGIN()
    ini.beginGroup("character-3-env-common-air");
    END("Time to begin 2");
    BEGIN()
        double      ican                = ini.value("walk_force", 0.0).toDouble();
    END("Time to take double");
    BEGIN()
        String      gravityAccelX       = ini.value("gravity_accel", 0.0).toString();
    END("Time to take Str");
    BEGIN()
        double      gravityAccel        = ini.value("gravity_accel", 0.0).toDouble();
    END("Time to take Double");
    BEGIN()
    ini.endGroup();
    END("Time to end 2");

    BEGIN()
    std::cout  << "I'm a " << whoIAm << " ^^\n"<<
                 "my fail effect gravity is " << failEffectGravity << " pixels per 1/65 second, whee!\n"
               <<"My folder also named as " << myFolderIs << " :D\nAlso my force is " << ican <<
                 " and gravity acceleration is "<<gravityAccel<<", or just "<<gravityAccelX<<" :)\n\n";
    END("Time to spit stuff");

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

    //Test big INI
    tests.clear();
    begin   = std::clock();
    testIniParser<IniProcessing, std::string>("../example-big.ini");
    end     = std::clock();
    elapsed_secs1 = (double(end - begin) / CLOCKS_PER_SEC) * 1000.0;
    tests1 = tests;

    tests.clear();
    begin  = std::clock();
    testIniParser<QSettings, QString>("../example-big.ini");
    end    = std::clock();
    elapsed_secs2 = (double(end - begin) / CLOCKS_PER_SEC) * 1000.0;
    tests2 = tests;
    dumpTests();
    printf("\nTotal time (big ini): My (debug/release): %f, QSettings (release): %f\n\n", elapsed_secs1, elapsed_secs2);

    //Test middle INI
    tests.clear();
    begin   = std::clock();
    testIniParser<IniProcessing, std::string>("../example-mid.ini");
    end     = std::clock();
    elapsed_secs1 = (double(end - begin) / CLOCKS_PER_SEC) * 1000.0;
    tests1 = tests;

    tests.clear();
    begin  = std::clock();
    testIniParser<QSettings, QString>("../example-mid.ini");
    end    = std::clock();
    elapsed_secs2 = (double(end - begin) / CLOCKS_PER_SEC) * 1000.0;
    tests2 = tests;
    dumpTests();
    printf("\nTotal time (middle ini): My (debug/release): %f, QSettings (release): %f\n\n", elapsed_secs1, elapsed_secs2);

    //Test small INI
    tests.clear();
    begin   = std::clock();
    testIniParser<IniProcessing, std::string>("../example-tiny.ini");
    end     = std::clock();
    elapsed_secs1 = (double(end - begin) / CLOCKS_PER_SEC) * 1000.0;
    tests1 = tests;

    tests.clear();
    begin  = std::clock();
    testIniParser<QSettings, QString>("../example-tiny.ini");
    end    = std::clock();
    elapsed_secs2 = (double(end - begin) / CLOCKS_PER_SEC) * 1000.0;
    tests2 = tests;
    dumpTests();
    printf("\nTotal time (tiny ini): My (debug/release): %f, QSettings (release): %f\n\n", elapsed_secs1, elapsed_secs2);


    x.quit();

    return 0;
}
