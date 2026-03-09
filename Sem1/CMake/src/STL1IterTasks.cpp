#include "STL1IterTasks.h"

#include <algorithm>
#include <iostream>
#include <tuple>
#include <cmath>
#include <fstream>
#include <numeric>
#include <list>
#include <vector>

std::vector<double> even_list_with_copy_if(const std::vector<double>& name) {

    if (name.size() < 2) {
        return name;
    }

    std::vector<double> result;
    int i = 1;

    auto condition = [&i](double value)
        {
            return (i++ % 2) == 1;
        };

    std::remove_copy_if(name.begin(), name.end(),
        std::back_inserter(result),
        condition
    );

    return result;
}

std::vector<double> even_list_with_for(const std::vector<double>& name) {

    if (name.size() < 2) {
        return name;
    }

    std::vector<double> result;
    int i = 1;

    auto iter_result = std::back_inserter(result);

    for (auto it = name.begin(); it != name.end(); ++it) {
        if (i % 2 == 0) {
            *iter_result = *it;
            ++iter_result;
        }
        i++;
    }

    return result;
}

std::vector<int> list_from_file(std::istream& name) {

    std::vector<int> result;

    std::istream_iterator<int> iter(name);
    std::istream_iterator<int> iter_end;

    std::copy(iter, iter_end,
        std::back_inserter(result));

    return result;
}

void overwriting(std::ostream& name, std::vector<int> res) {

    std::ostream_iterator<int> output_file(name, "  ");

    std::copy(res.begin(), res.end(),
        output_file);
}

std::vector<int> list_with_change(std::istream& num, std::ostream& name) {

    std::vector<int> result;

    std::istream_iterator<int> iter(num);
    std::istream_iterator<int> iter_end;

    std::replace_copy(iter, iter_end,
        back_inserter(result),
        0,
        10);

    overwriting(name, result);

    return result;
}

void record(std::ostream& name2, std::vector<std::string> res) {

    std::ostream_iterator<std::string> output_file(name2, " ");

    std::copy(res.begin(), res.end(),
        output_file);
}

std::vector<std::string> change_file(std::istream& name1, std::istream& words, std::ostream& name2) {

    std::vector<std::string> result;

    std::istream_iterator<std::string> it1(name1);
    std::istream_iterator<std::string> it1_end;

    std::istream_iterator<std::string> it2(words);

    std::transform(it1, it1_end, it2, std::back_inserter(result),
        [](std::string a, std::string b) {
            return a + "-" + b;
        }
    );

    record(name2, result);

    return result;
}
