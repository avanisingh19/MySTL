#include <iostream>
#include "mystl/vector.hpp"
#include "mystl/list.hpp"
#include "mystl/map.hpp"
#include "mystl/unordered_map.hpp"
#include "mystl/algorithm.hpp"

int main() {
    mystl::vector<int> v{5, 1, 9, 3, 7};
    mystl::sort(v.begin(), v.end());

    for (auto x : v) std::cout << x << ' ';
    std::cout << "\n";

    mystl::list<int> lst{1, 2, 3};
    lst.push_front(0);
    for (auto it = lst.begin(); it != lst.end(); ++it) std::cout << *it << ' ';
    std::cout << "\n";

    mystl::map<int, int> mp;
    mp[2] = 20;
    mp[1] = 10;
    for (auto it = mp.begin(); it != mp.end(); ++it) std::cout << it->first << ':' << it->second << ' ';
    std::cout << "\n";

    mystl::unordered_map<int, int> um;
    um[1] = 100;
    um[2] = 200;
    for (auto it = um.begin(); it != um.end(); ++it) std::cout << it->first << ':' << it->second << ' ';
    std::cout << "\n";
}
