#include <iostream>
#include "csvparser.hpp"

int main() {
    std::ifstream file("test_dataset.csv");
    CSV csv(file);

    for(auto& row : csv)
    {
        std::cout <<
            row.At("id").ToString() << " " <<
            row.At("first_name").ToString() << " " <<
            row.At("last_name").ToString() << " " <<
            row.At("email").ToString() << " " << std::endl;
    }

    return 0;
}
