#ifndef INI_PROCESSING_VARIANT_H
#define INI_PROCESSING_VARIANT_H

#include <string>
#include <cstring>
#include <cstdlib>

class IniProcessingVariant
{
        std::string  m_data;
        std::string *m_dataP;
        inline std::string &data()
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
        IniProcessingVariant(const char *data):
            m_data(data),
            m_dataP(nullptr) {}
        IniProcessingVariant(std::string *dataPointer):
            m_data(""),
            m_dataP(dataPointer) {}
        IniProcessingVariant(const IniProcessingVariant &v):
            m_data(v.m_data),
            m_dataP(v.m_dataP) {}

        IniProcessingVariant(char data):
            m_data(std::to_string(data)),
            m_dataP(nullptr) {}

        IniProcessingVariant(unsigned char data):
            m_data(std::to_string(data)),
            m_dataP(nullptr) {}

        IniProcessingVariant(bool data):
            m_data(std::to_string(data)),
            m_dataP(nullptr) {}

        IniProcessingVariant(short data):
            m_data(std::to_string(data)),
            m_dataP(nullptr) {}

        IniProcessingVariant(unsigned short data):
            m_data(std::to_string(data)),
            m_dataP(nullptr) {}

        IniProcessingVariant(int data):
            m_data(std::to_string(data)),
            m_dataP(nullptr) {}
        IniProcessingVariant(unsigned int data):
            m_data(std::to_string(data)),
            m_dataP(nullptr) {}

        IniProcessingVariant(long data):
            m_data(std::to_string(data)),
            m_dataP(nullptr) {}
        IniProcessingVariant(unsigned long data):
            m_data(std::to_string(data)),
            m_dataP(nullptr) {}

        IniProcessingVariant(long long data):
            m_data(std::to_string(data)),
            m_dataP(nullptr) {}
        IniProcessingVariant(unsigned long long data):
            m_data(std::to_string(data)),
            m_dataP(nullptr) {}

        IniProcessingVariant(float data):
            m_data(std::to_string(data)),
            m_dataP(nullptr) {}

        IniProcessingVariant(double data):
            m_data(std::to_string(data)),
            m_dataP(nullptr) {}

        IniProcessingVariant(long double data):
            m_data(std::to_string(data)),
            m_dataP(nullptr) {}

        bool isNull()
        {
            return (m_data.empty() && !m_dataP);
        }

        bool isValid()
        {
            return ((!m_data.empty()) || (static_cast<std::string *>(m_dataP)));
        }

        std::string toString()
        {
            std::string out = data();

            if((out.size() > 2) && (out[0] == '"'))
                out.erase(0, 1);

            if((out.size() > 1) && (out[out.size() - 1] == '"'))
                out.erase((out.size() - 1), 1);

            return out;
        }

        bool toBool()
        {
            size_t i = 0;
            size_t ss = std::min(static_cast<size_t>(4ul), data().size());
            char buff[4] = {0, 0, 0, 0};
            const char *pbufi = data().c_str();
            char *pbuff = buff;

            for(; i < ss; i++)
                (*pbuff++) = static_cast<char>(std::tolower(*pbufi++));

            if(ss < 4)
            {
                if(ss == 0)
                    return false;

                if(ss == 1)
                    return (buff[0] == '1');

                try
                {
                    long num = std::strtol(buff, 0, 0);
                    return num != 0l;
                }
                catch(...)
                {
                    bool res = (std::memcmp(buff, "yes", 3) == 0) ||
                               (std::memcmp(buff, "on", 2) == 0);
                    return res;
                }
            }

            if(std::memcmp(buff, "true", 4) == 0)
                return true;

            try
            {
                long num = std::strtol(buff, 0, 0);
                return num != 0l;
            }
            catch(...)
            {
                return false;
            }
        }

        int toInt()
        {
            return std::atoi(data().c_str());
        }
        unsigned int toUInt()
        {
            return static_cast<unsigned int>(std::strtoul(data().c_str(), nullptr, 0));
        }

        long toLong()
        {
            return std::atol(data().c_str());
        }
        unsigned long toULong()
        {
            return std::strtoul(data().c_str(), nullptr, 0);
        }

        long long toLongLong()
        {
            return std::atoll(data().c_str());
        }
        unsigned long long toULongLong()
        {
            return std::strtoull(data().c_str(), nullptr, 0);
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


#endif // INI_PROCESSING_VARIANT_H
