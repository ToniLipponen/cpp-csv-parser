#pragma once
#include <sstream>
#include <ostream>
#include <istream>
#include <vector>
#include <unordered_map>
#include <exception>

class CSVType
{
public:
    CSVType() = default;

    CSVType(std::string data)
    : m_data(std::move(data))
    {

    }

    CSVType& operator=(const std::string& data)
    {
        m_data = data;

        return *this;
    }

    template<typename T>
    CSVType& operator=(const T& data)
    {
        static_assert(
                std::is_integral<T>::value || std::is_floating_point<T>::value,
                "Assignable type must be convertable by std::to_string()");

        m_data = std::to_string(data);

        return *this;
    }

    operator int() const
    {
        return std::stoi(m_data);
    }

    operator float() const
    {
        return std::stof(m_data);
    }

    operator double() const
    {
        return std::stod(m_data);
    }

    operator std::string() const
    {
        return m_data;
    }

    const std::string& ToString() const noexcept
    {
        return m_data;
    }

private:
    std::string m_data;
};

class CSVReader
{
public:
    class Row
    {
    public:
        const CSVType& At(const std::string& key) const
        {
            if(m_data.find(key) == m_data.end())
            {
                throw std::out_of_range("Key '" + key + "' doesn't exist");
            }

            return m_data.at(key);
        }

        CSVType& Insert(const std::string& key, const CSVType& value)
        {
            m_data.insert({key, value});

            return m_data.at(key);
        }

    private:
        std::unordered_map<std::string /** Column name */, CSVType /** Value */> m_data;
    };

public:
    CSVReader() = default;

    explicit CSVReader(const std::string& str)
    {
        LoadFromString(str);
    }

    explicit CSVReader(std::istream& stream)
    {
        LoadFromStream(stream);
    }

    virtual ~CSVReader() = default;

    bool LoadFromString(const std::string& str)
    {
        std::stringstream ss(str);
        std::string line;

        /// Get top line
        std::getline(ss, line);

        /// Extract column names from the top line
        std::stringstream columns(line);

        char delimiter = ';';
        static std::vector<char> delimiters{';', ',', ':', '\t'};

        /// Find delimiter
        for(auto d : delimiters)
        {
            if(line.find(d) != std::string::npos)
            {
                delimiter = d;
            }
        }

        std::string column;
        while(std::getline(columns, column, delimiter))
        {
            m_columnNames.push_back(column);
        }

        /// Get line, as long as there are lines to read
        while(std::getline(ss, line))
        {
            std::stringstream lineStream(line);

            /// Current row data
            Row currentRow;

            for(auto& col : m_columnNames)
            {
                std::string item;

                /// Extract value for each column
                std::getline(lineStream, item, delimiter);

                currentRow.Insert(col, item);
            }

            m_rows.push_back(std::move(currentRow));
        }

        return true;
    }

    bool LoadFromStream(std::istream& stream)
    {
        std::string content, line;

        while(std::getline(stream, line))
        {
            content.append(line);
            content.append("\n");
        }

        return LoadFromString(content);
    }

    std::vector<std::string> GetColumnNames() { return m_columnNames; }

    std::vector<Row>::iterator begin() { return m_rows.begin(); }
    std::vector<Row>::iterator end() { return m_rows.end(); }

    std::vector<Row>::const_iterator cbegin() { return m_rows.cbegin(); }
    std::vector<Row>::const_iterator cend() { return m_rows.cend(); }

private:
    std::vector<std::string> m_columnNames;
    std::vector<Row> m_rows;
};

class CSVWriter
{
public:
    explicit CSVWriter(std::ostream& stream, std::initializer_list<std::string> columnNames, char delimiter = ',')
    : m_stream(stream), m_header(columnNames), m_delimiter(delimiter)
    {
        for(auto i = m_header.begin(); i != m_header.end(); i++)
        {
            m_stream << *i << ((i+1 == m_header.end()) ? '\n' : m_delimiter);
        }
    }

    template<typename ... Args>
    void InsertRow(const Args& ... args)
    {
        static int argCount = sizeof...(args);

        if(argCount > m_header.size() || argCount < m_header.size())
        {
            throw std::runtime_error("Row field count doesn't match the specified column count");
        }

        Insert<Args ...>(args...);
    }

private:
    void Insert() {}

    template<typename First, typename ... Args>
    inline void Insert(const First& first, const Args& ... args)
    {
        m_stream << first << (sizeof...(args) ? m_delimiter : '\n');
        Insert(args...);
    }

    std::ostream& m_stream;
    std::vector<std::string> m_header;
    char m_delimiter;
};
