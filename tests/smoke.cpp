#include "mystl/vector.hpp"
#include "mystl/list.hpp"
#include "mystl/stack.hpp"
#include "mystl/queue.hpp"
#include "mystl/map.hpp"
#include "mystl/set.hpp"
#include "mystl/unordered_map.hpp"
#include "mystl/algorithm.hpp"

int main() {
    mystl::vector<int> v{1,2,3};
    v.push_back(4);

    mystl::list<int> l{1,2,3};
    l.push_front(0);

    mystl::stack<int> st;
    st.push(1);

    mystl::queue<int> q;
    q.push(1);

    mystl::map<int,int> mp;
    mp[1] = 2;

    mystl::set<int> s;
    s.insert(1);

    mystl::unordered_map<int,int> um;
    um[1] = 2;

    return 0;
}
