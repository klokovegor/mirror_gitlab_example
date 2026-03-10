#include "STL2SeqTasks.h"

#include <algorithm>
#include <iostream>
#include <iterator>
#include <tuple>
#include <cmath>
#include <fstream>
#include <numeric>
#include <list>
#include <vector>
#include <deque>

std::vector<int> file_from_vector(std::istream& name) {
    std::vector<int> result;

    std::istream_iterator<int> iter(name);
    std::istream_iterator<int> iter_end;

    std::copy(iter, iter_end,
        std::back_inserter(result));

    return result;
}

std::list<int> add_dec_in_list(std::deque<int>& d, std::list<int> l) {
    std::list<int> error = {};

    if (d.size() < 5 || l.size() < 5) {
        return error;
    }

    auto list_iter = l.rbegin();
    std::advance(list_iter, 5);
    auto dec_iter = d.rbegin();

    l.insert(list_iter.base(), dec_iter, std::next(dec_iter, 5));

    return l;
}

std::vector<int> add_one_in_vector(std::vector<int>& v) {
    auto i = v.begin();

    while (i != v.end()) {
        i = v.insert(++i, -1);
        ++i;
    }

    return v;
}

std::vector<int> delele_uneven_value(std::vector<int>& v) {

    for (auto iter = v.begin(); iter != v.end(); ) {
        iter = v.erase(iter);

        if (iter != v.end()) {
            ++iter;
        }
    }

    return v;
}

std::list<int> del_part_of_list(std::list<int> l) {
    std::list<int> error = {};

    if (l.size() % 4 != 0) {
        return error;
    }

    auto iter = l.rbegin();
    std::advance(iter, l.size() / 2);

    int i = 1;

    for (; iter != l.rend(); ) {
        if (i % 2 == 0) {
            auto next = iter;
            ++next;
            iter = std::make_reverse_iterator(l.erase(next.base()));
        } else {
            ++iter;
        }
        ++i;
    }

    return l;
}

