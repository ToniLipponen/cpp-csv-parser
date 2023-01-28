#include <iostream>
#include "csvparser.hpp"
#include <fstream>

int main()
{
    std::ifstream inFile("test_dataset.csv");
    CSVReader reader(inFile);

    for(auto& row : reader)
    {
        std::cout <<
            row.At("id").ToString() << " " <<
            row.At("first_name").ToString() << " " <<
            row.At("last_name").ToString() << " " <<
            row.At("email").ToString() << " " << std::endl;
    }

    std::ofstream outFile("output.csv");
    CSVWriter writer(outFile, {"name", "email"});

    for(auto& row : reader)
    {
        std::string firstName = row.At("first_name");
        std::string lastName = row.At("last_name");
        std::string fullName = firstName + " " + lastName;

        std::string email = row.At("email");
        writer.InsertRow(fullName, email);
    }

    return 0;
}
