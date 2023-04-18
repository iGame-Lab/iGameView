//
// Created by teitoku on 2022-03-20.
//

#ifndef IGAMEVIEW_DSU_H
#define IGAMEVIEW_DSU_H
#include <vector>

class DSU {
private:
    std::vector<int>pre;
public:
    DSU();
    DSU(int x);
    void join(int x,int y);
    int find_root(int x);
};


#endif //IGAMEVIEW_DSU_H
