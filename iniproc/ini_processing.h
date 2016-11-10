#ifndef INIPROCESSING_H
#define INIPROCESSING_H

#include <string>
#include <cstring>
#include <cstdlib>
#include <unordered_map>

#include "ini_processing_variant.h"

class IniProcessing
{
    public:
        enum ErrCode
        {
            ERR_OK = 0,
            ERR_NOFILE,
            ERR_SECTION_SYNTAX,
            ERR_KEY_SYNTAX,
            ERR_NO_MEMORY
        };

    private:
        struct      params
        {
            std::string filePath;
            bool        opened;
            int         lineWithError;
            ErrCode     errorCode;
            bool        modified;
            typedef     std::unordered_map<std::string, std::string> IniKeys;
            typedef     std::unordered_map<std::string, IniKeys> IniSections;
            IniSections iniData;
            IniKeys    *currentGroup;
        } m_params;
        bool parse();

        /* Parse given INI-style file. May have [section]s, name=value pairs
           (whitespace stripped), and comments starting with ';' (semicolon). Section
           is "" if name=value pair parsed before any section heading. name:value
           pairs are also supported as a concession to Python's ConfigParser.
           For each name=value pair parsed, call handler function with given user
           pointer as well as section, name, and value (data only valid for duration
           of handler call). Handler should return nonzero on success, zero on error.
           Returns 0 on success, line number of first error on parse error (doesn't
           stop on first error), -1 on file open error, or -2 on memory allocation
           error (only when INI_USE_STACK is zero).
        */
        //int handler(params::IniKeys* section, char *name, char *value);

        bool ini_parse(const char *filename);

        bool ini_parseMemory(char *mem, size_t size);

        /* Same as ini_parse(), but takes a FILE* instead of filename. This doesn't
           close the file when it's finished -- the caller must do that. */
        bool ini_parse_file(char *data, size_t size);

        inline params::IniKeys::iterator readHelper(const char *key, bool &ok)
        {
            if(!m_params.opened)
                return params::IniKeys::iterator();

            if(!m_params.currentGroup)
                return params::IniKeys::iterator();

            params::IniKeys::iterator e = m_params.currentGroup->find(key);

            if(e != m_params.currentGroup->end())
                ok = true;

            return e;
        }

    public:
        IniProcessing();
        IniProcessing(const std::string &iniFileName, int dummy = 0);
        IniProcessing(const IniProcessing &ip);
        bool open(const std::string &iniFileName);
        bool openMem(char *memory, size_t size);
        void close();
        int  lineWithError();
        bool isOpened();

        bool beginGroup(const std::string &groupName);
        bool contains(const std::string &groupName);
        bool hasKey(const std::string &keyName);
        void endGroup();

        void read(const char *key, bool &dest, bool defVal);
        void read(const char *key, unsigned char &dest, unsigned char defVal);
        void read(const char *key, char &dest, char defVal);
        void read(const char *key, unsigned short &dest, unsigned short defVal);
        void read(const char *key, short &dest, short defVal);
        void read(const char *key, unsigned int &dest, unsigned int defVal);
        void read(const char *key, int &dest, int defVal);
        void read(const char *key, unsigned long &dest, unsigned long defVal);
        void read(const char *key, long &dest, long defVal);
        void read(const char *key, unsigned long long &dest, unsigned long long defVal);
        void read(const char *key, long long &dest, long long defVal);
        void read(const char *key, float &dest, float defVal);
        void read(const char *key, double &dest, double defVal);
        void read(const char *key, std::string &dest, const std::string &defVal);

        IniProcessingVariant value(const char *key, const IniProcessingVariant &defVal = IniProcessingVariant());
};

#endif // INIPROCESSING_H
