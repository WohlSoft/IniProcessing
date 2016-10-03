#ifndef INIPROCESSING_H
#define INIPROCESSING_H

#include <string>
#include <unordered_map>

class IniProcessingVariant
{
    std::string  m_data;
    std::string* m_dataP;
    inline std::string& data()
    {
        if(m_dataP)
            return *m_dataP;
        else
            return m_data;
    }
public:
    IniProcessingVariant():
        m_data(""),
        m_dataP(nullptr) {}

    IniProcessingVariant(const std::string &data):
        m_data(data),
        m_dataP(nullptr) {}
    IniProcessingVariant(const char* data):
        m_data(data),
        m_dataP(nullptr) {}
    IniProcessingVariant(std::string *dataPointer):
        m_data(""),
        m_dataP(dataPointer) {}
    IniProcessingVariant(const IniProcessingVariant&v):
        m_data(v.m_data),
        m_dataP(v.m_dataP) {}

    IniProcessingVariant(const int &data):
        m_data(std::to_string(data)),
        m_dataP(nullptr) {}

    IniProcessingVariant(const long &data):
        m_data(std::to_string(data)),
        m_dataP(nullptr) {}

    IniProcessingVariant(const long long &data):
        m_data(std::to_string(data)),
        m_dataP(nullptr) {}

    IniProcessingVariant(const float &data):
        m_data(std::to_string(data)),
        m_dataP(nullptr) {}

    IniProcessingVariant(const double &data):
        m_data(std::to_string(data)),
        m_dataP(nullptr) {}

    bool isNull()
    {
        return (m_data.empty() && !m_dataP);
    }

    bool isValid()
    {
        return ((!m_data.empty()) || (static_cast<std::string*>(m_dataP)));
    }

    std::string toString()
    {
        std::string out = data();
        if( (out.size()>2) && (out[0]=='"') )
            out.erase(0, 1);
        if( (out.size()>1) && (out[out.size()-1] =='"') )
            out.erase((out.size()-1), 1);
        return out;
    }

    int toInt()
    {
        return std::atoi(data().c_str());
    }

    long toLong()
    {
        return std::atol(data().c_str());
    }

    long long toLongLong()
    {
        return std::atoll(data().c_str());
    }

    float toFloat()
    {
        return float(std::atof(data().c_str()));
    }

    double toDouble()
    {
        return std::atof(data().c_str());
    }
};


class IniProcessing
{
    struct      params
    {
        std::string filePath;
        bool        opened;
        int         lineWithError;
        bool        modified;
        typedef     std::unordered_map<std::string, std::string> IniKeys;
        typedef     std::unordered_map<std::string, IniKeys> IniSections;
        IniSections iniData;
        IniKeys*    currentGroup;
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
    int handler(params::IniKeys* section, const char* name, const char* value);

    bool ini_parse(const char* filename);

    /* Same as ini_parse(), but takes a FILE* instead of filename. This doesn't
       close the file when it's finished -- the caller must do that. */
    bool ini_parse_file(char *data, long size);

public:
    IniProcessing();
    IniProcessing(const std::string &iniFileName, int dummy=0);
    IniProcessing(const IniProcessing& ip);
    bool open(const std::string &iniFileName);
    void close();
    int  lineWithError();
    bool isOpened();

    bool beginGroup(const std::string &groupName);
    bool contains(const std::string &groupName);
    bool hasKey(const std::string &keyName);
    void endGroup();

    IniProcessingVariant value(std::string key, const IniProcessingVariant& defVal = IniProcessingVariant());
};

#endif // INIPROCESSING_H
