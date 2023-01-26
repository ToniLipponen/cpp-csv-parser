#pragma once
#include <sstream>
#include <utility>
#include <vector>
#include <unordered_map>
#include <fstream>

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

class CSV
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
    CSV() = default;

    explicit CSV(const std::string& str)
    {
        LoadFromString(str);
    }

    explicit CSV(std::istream& stream)
    {
        LoadFromStream(stream);
    }

    virtual ~CSV() = default;

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

    Row* begin() { return m_rows.begin().base(); }
    Row* end() { return m_rows.end().base(); }

    const Row* cbegin() {return m_rows.cbegin().base(); }
    const Row* cend() {return m_rows.cend().base(); }

private:
    std::vector<std::string> m_columnNames;
    std::vector<Row> m_rows;
};
