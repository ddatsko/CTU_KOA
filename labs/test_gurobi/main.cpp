#include <gurobi_c++.h>
#include <iostream>
#include <vector>
#include <numeric>

int main(int argc, char *argv[]) {
//
//    int n = 5;
//    std::vector<std::pair<int, int>> edges = {{0, 1}, {2, 3}, {0, 4}, {3, 1}, {3, 4}};
//
//    GRBEnv env;
//    GRBModel model(env);
//    std::vector<GRBVar> vars;
//    vars.reserve(n);
//    for (int i = 0; i < n; ++i) {
//        vars.push_back(model.addVar(0.0, 1.0, 0.0, GRB_INTEGER));
//    }
//    for (const auto &[e1, e2]: edges) {
//        model.addConstr(vars[e1] + vars[e2] >= 1);
//    }
//
//    auto obj = std::accumulate(vars.begin(), vars.end(), GRBLinExpr{0.0});
//    model.setObjective(obj);
//
//    model.optimize();
//
//    std::cout << "Taken values: \n";
//    for (const auto &v: vars) {
//        std::cout << v.get(GRB_DoubleAttr_X) << ", ";
//    }
//    std::cout << std::endl;

    return 0;
}