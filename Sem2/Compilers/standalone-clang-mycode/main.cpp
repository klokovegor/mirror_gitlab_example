#include <algorithm>
#include <iostream>
#include <cmath>
#include <fstream>
#include <numeric>
#include <vector>
#include <iterator>
#include <sstream>
#include "STL1Iter15.h"


int main()
{
    std::vector<int> t = { 1, 10, 3, 55, 10, 23, 33 };

    std::ofstream file("name.txt");

    std::string num = "1 10 3 55 0 23 33";
    std::stringstream sstream(num);

    std::vector<int> res = list_with_change(sstream, file);

    file.flush();
    file.close();

    bool s = res == t;

    return 0;
}