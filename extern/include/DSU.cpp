//
// Created by teitoku on 2022-03-20.
//
#include <Eigen/Dense>
#include "DSU.h"

DSU::DSU() {
    pre.resize(1000);
    for (int i = 0; i < pre.size(); i++)
        pre[i] = i;
}

DSU::DSU(int x) {
    pre.resize(x);
    for (int i = 0; i < pre.size(); i++)
        pre[i] = i;
}

int DSU::find_root(int id) {
    int r = id;
    while (pre[r] != r) {
        r = pre[r];
    }
    while (pre[id] != r) {
        int y = pre[id];
        pre[id] = r;
        id = y;
    }
    return r;
}

void DSU::join(int x, int y) {
    if (find_root(x) != find_root(y)) {
        pre[find_root(x)] = find_root(y);
    }
    return;
}