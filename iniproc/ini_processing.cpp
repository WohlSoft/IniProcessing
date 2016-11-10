/* Nonzero to allow a UTF-8 BOM sequence (0xEF 0xBB 0xBF) at the start of
   the file. See http://code.google.com/p/inih/issues/detail?id=21 */
#ifndef INI_ALLOW_BOM
#define INI_ALLOW_BOM 1
#endif

/* Nonzero to use stack, zero to use heap (malloc/free). */
//#ifndef INI_USE_STACK
//#define INI_USE_STACK 1
//#endif

/* Stop parsing on first error (default is to keep parsing). */
//#ifndef INI_STOP_ON_FIRST_ERROR
//#define INI_STOP_ON_FIRST_ERROR 1
//#endif

/* Maximum line length for any line in INI file. */
//#ifndef INI_MAX_LINE
//#define INI_MAX_LINE 500
//#endif

#include "ini_processing.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#ifdef _WIN32
#include <windows.h>
#endif
//#if !defined(INI_USE_STACK)
//#include <stdlib.h>
//#endif
//#define MAX_SECTION 100
//#define MAX_NAME 100

#include "../fileMapper/file_mapper.h"

static const unsigned char utfbom[3] = {0xEF, 0xBB, 0xBF};

enum { Space = 0x01, Special = 0x02, INIParamEq = 0x04 };

static const unsigned char charTraits[256] =
{
    // Space: '\t', '\n', '\r', ' '
    // Special: '\n', '\r', '"', ';', '=', '\\'
    // INIParamEq: ':', '='

    0, 0, 0, 0, 0, 0, 0, 0, 0, Space, Space | Special, 0, 0, Space | Special,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, Space, 0, Special,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, INIParamEq,
    Special, 0, Special | INIParamEq, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, Special, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

#if 0//for speed comparison who faster - macro or inline function. Seems speeds are same
#define IS_SPACE(c) (charTraits[static_cast<unsigned char>(c)] & Space)
#define IS_SPECIAL(c) (charTraits[static_cast<unsigned char>(c)] & Special)
#define IS_INIEQUAL(c) (charTraits[static_cast<unsigned char>(c)] & INIParamEq)
#else
inline unsigned char IS_SPACE(char &c)
{
    return (charTraits[static_cast<unsigned char>(c)] & Space);
}
inline unsigned char IS_SPECIAL(char &c)
{
    return (charTraits[static_cast<unsigned char>(c)] & Special);
}
inline unsigned char IS_INIEQUAL(char &c)
{
    return (charTraits[static_cast<unsigned char>(c)] & INIParamEq);
}
#endif

/* Strip whitespace chars off end of given string, in place. Return s. */
inline char *rstrip(char *s)
{
    char *p = s + strlen(s);

    while(p > s && IS_SPACE(*--p))
        *p = '\0';

    return s;
}

/* Return pointer to first non-whitespace char in given string. */
inline char *lskip(char *s)
{
    while(*s && IS_SPACE(*s))
        s++;

    return reinterpret_cast<char *>(s);
}

inline char *lrtrim(char *s)
{
    while(*s && IS_SPACE(*s))
        s++;

    char *p = s + strlen(s);

    while(p > s && IS_SPACE(*--p))
        *p = '\0';

    return s;
}

/* Return pointer to first char c or ';' comment in given string, or pointer to
   null at end of string if neither found. ';' must be prefixed by a whitespace
   character to register as a comment. */
inline char *find_char_or_comment(char *s, char c)
{
    unsigned char was_whitespace = 0;

    while(*s && *s != c && !(was_whitespace && *s == ';'))
    {
        was_whitespace = IS_SPACE(*s);
        s++;
    }

    return s;
}

inline char *find_inieq_or_comment(char *s)
{
    unsigned char was_whitespace = 0;

    while(*s && (!IS_INIEQUAL(*s)) && !(was_whitespace && *s == ';'))
    {
        was_whitespace = IS_SPACE(*s);
        s++;
    }

    return s;
}


//Remove comment line from a tail of value
inline void skipcomment(char *value)
{
    unsigned char quoteDepth = 0;

    while(*value)
    {
        if(quoteDepth > 0)
        {
            if(*value == '\\')
                continue;

            if(*value == '"')
                --quoteDepth;
        }
        else if(*value == '"')
            ++quoteDepth;

        if((quoteDepth == 0) && (*value == ';'))
        {
            *value = '\0';
            break;
        }

        value++;
    }
}

inline bool memfgets(char *&line, char *data, char *&pos, char *end)
{
    line = pos;

    while(pos != end)
    {
        if(*pos == '\n')
        {
            if((pos > data) && (*(pos - 1) == '\r'))
                *((pos++) - 1) = '\0';
            else
                *(pos++) = '\0';

            break;
        }

        ++pos;
    }

    return (pos != line);
    //EOF is a moment when position wasn't changed.
    //If do check "pos != end", will be an inability to read last line.
    //this logic allows detect true EOF when line is really eof
}

/* See documentation in header file. */
bool IniProcessing::ini_parse_file(char *data, size_t size)
{
    char *section = nullptr;
#if defined(INI_ALLOW_MULTILINE)
    char *prev_name = nullptr;
#endif
    char *start;
    char *end;
    char *name;
    char *value;
    int lineno = 0;
    int error = 0;
    char *line;
    char *pos_end = data + size;
    char *pos_cur = data;
    params::IniKeys *recentKeys = nullptr;

    /* Scan through file line by line */
    //while (fgets(line, INI_MAX_LINE, file) != NULL)
    while(memfgets(line, data, pos_cur, pos_end))
    {
        lineno++;
        start = line;
#if defined(INI_ALLOW_BOM)

        if((lineno == 1) && (size >= 3) && (memcmp(start, utfbom, 3) == 0))
            start += 3;

#endif
        start = lrtrim(start);

        if(!*start)//if empty line - skip it away!
            continue;

        switch(*start)
        {
        case ';':
        case '#':
            //if (*start == ';' || *start == '#') {
            //    /* Per Python ConfigParser, allow '#' comments at start of line */
            //}
            continue;

        case '[':
        {
            /* A "[section]" line */
            end = find_char_or_comment(start + 1, ']');

            if(*end == ']')
            {
                *end = '\0';
                section = start + 1;
#if defined(INI_ALLOW_MULTILINE)
                prev_name = nullptr;
#endif
                recentKeys = &m_params.iniData[section];
            }
            else if(!error)
            {
                /* No ']' found on section line */
                m_params.errorCode = ERR_SECTION_SYNTAX;
                error = lineno;
            }
        }
        break;

        default:
        {
            /* Not a comment, must be a name[=:]value pair */
            //end = find_char_or_comment(start, '=');
            end = find_inieq_or_comment(start);

            //if(*end != '=')
            //    end = find_char_or_comment(start, ':');

            if(IS_INIEQUAL(*end) /**end == '=' || *end == ':'*/)
            {
                *end = '\0';
                name = rstrip(start);
                value = lskip(end + 1);
                end = find_char_or_comment(value, '\0');

                if(*end == ';')
                    *end = '\0';

                rstrip(value);
                //#if defined(INI_ALLOW_MULTILINE)
                //  /* Valid name[=:]value pair found, call handler */
                //  strncpy0(prev_name, name, MAX_NAME);
                //#endif
                {
                    char *v = value;
                    skipcomment(v);
                    v = rstrip(v);

                    if(!recentKeys)
                        recentKeys = &m_params.iniData["General"];

#ifdef INIDEBUG
                    printf("-> [%s]; %s = %s\n", section, name, v);
#endif
                    (*recentKeys)[name] = v;
                }
            }
            else if(!error)
            {
                /* No '=' or ':' found on name[=:]value line */
                m_params.errorCode = ERR_KEY_SYNTAX;
                error = lineno;
            }

            break;
        }
        }

#if defined(INI_STOP_ON_FIRST_ERROR)

        if(error)
            break;

#endif
    }

    m_params.lineWithError = error;
    return (error == 0);
}

/* See documentation in header file. */
bool IniProcessing::ini_parse(const char *filename)
{
    bool valid = true;
    char *tmp = nullptr;
#if 0 //By mystical reasons, reading whole file form fread() is faster than mapper :-P
    PGE_FileMapper file(filename);

    if(!file.data)
        return -1;

    tmp = reinterpret_cast<char *>(malloc(static_cast<size_t>(file.size + 1)));
    memcpy(tmp, file.data, static_cast<size_t>(file.size));
    *(tmp + file.size) = '\0';//null terminate last line
    valid = ini_parse_file(tmp, static_cast<size_t>(file.size));
#else
    FILE *cFile = fopen64(filename, "rb");

    if(!cFile)
        return false;

    fseek(cFile, 0, SEEK_END);
    size_t size = static_cast<size_t>(ftell(cFile));
    fseek(cFile, 0, SEEK_SET);
    tmp = reinterpret_cast<char *>(malloc(static_cast<size_t>(/*file.size*/ size + 1)));

    if(fread(tmp, 1, size, cFile) != size)
        valid = false;

    fclose(cFile);

    if(valid)
    {
        *(tmp + /*file.size*/size) = '\0';//null terminate last line
        valid = ini_parse_file(tmp, size/*file.size*/);
    }

#endif
    free(tmp);
    return valid;
}


bool IniProcessing::parse()
{
    return ini_parse(m_params.filePath.c_str());
}

IniProcessing::IniProcessing() :
    m_params{"", false, -1, ERR_OK, false, params::IniSections(), nullptr}
{}

IniProcessing::IniProcessing(const std::string &iniFileName, int) :
    m_params{iniFileName, false, -1, ERR_OK, false, params::IniSections(), nullptr}
{
    open(iniFileName);
}

IniProcessing::IniProcessing(const IniProcessing &ip) :
    m_params(ip.m_params)
{}

bool IniProcessing::open(const std::string &iniFileName)
{
    std::setlocale(LC_NUMERIC, "C");

    if(!iniFileName.empty())
    {
        close();
        m_params.errorCode = ERR_OK;
        m_params.filePath  = iniFileName;
        bool res = parse();
#ifdef INIDEBUG

        if(res)
            printf("\n==========WOOHOO!!!==============\n\n");
        else
            printf("\n==========OOOUCH!!!==============\n\n");

#endif
        m_params.opened = res;
        return res;
    }

    m_params.errorCode = ERR_NOFILE;
    return false;
}

void IniProcessing::close()
{
    m_params.errorCode = ERR_OK;
    m_params.iniData.clear();
    m_params.opened = false;
    m_params.lineWithError = -1;
}

int IniProcessing::lineWithError()
{
    return m_params.lineWithError;
}

bool IniProcessing::isOpened()
{
    return m_params.opened;
}

bool IniProcessing::beginGroup(const std::string &groupName)
{
    if(!m_params.opened)
        return false;

    params::IniSections::iterator e = m_params.iniData.find(groupName);

    if(e == m_params.iniData.end())
        return false;

    params::IniKeys &k = e->second;
    m_params.currentGroup = &k;
    return true;
}

bool IniProcessing::contains(const std::string &groupName)
{
    if(!m_params.opened)
        return false;

    params::IniSections::iterator e = m_params.iniData.find(groupName);
    return (e != m_params.iniData.end());
}

bool IniProcessing::hasKey(const std::string &keyName)
{
    if(!m_params.opened)
        return false;

    if(!m_params.currentGroup)
        return false;

    params::IniKeys::iterator e = m_params.currentGroup->find(keyName);
    return (e != m_params.currentGroup->end());
}

void IniProcessing::endGroup()
{
    m_params.currentGroup = nullptr;
}

IniProcessingVariant IniProcessing::value(std::string key, const IniProcessingVariant &defVal)
{
    if(!m_params.opened)
        return defVal;

    if(!m_params.currentGroup)
        return defVal;

    params::IniKeys::iterator e = m_params.currentGroup->find(key);

    if(e == m_params.currentGroup->end())
        return defVal;

    std::string &k = e->second;
    return IniProcessingVariant(&k);
}
