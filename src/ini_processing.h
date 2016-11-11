/*
INI Processor - a small library which allows you parsing INI-files

Copyright (c) 2016 Vitaliy Novichkov <admin@wohlnet.ru>

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
