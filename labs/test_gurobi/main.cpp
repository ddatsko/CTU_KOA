#include <gurobi_c++.h>
#include <iostream>
#include <vector>
#include <numeric>

int main(int argc, char *argv[]) {

    int A_x = 33, A_y = 32;
    int a[] = {1, 4, 7, 8, 9, 10, 14, 15, 18};
    int n = static_cast<int>(sizeof(a) / sizeof(a[0]));

    GRBEnv env;
    GRBModel model(env);

    auto x = model.addVars(n, GRB_INTEGER);
    auto y = model.addVars(n, GRB_INTEGER);

    GRBVar *c[n], *g[n];
    for (int i = 0; i < n; ++i) {
        c[i] = model.addVars(n, GRB_BINARY);
        g[i] = model.addVars(n, GRB_BINARY);
    }

    double M = A_x * A_y;
    for (int i = 0; i < n; ++i) {
        model.addConstr(x[i] >= 0);
        model.addConstr(x[i] + a[i] <= A_x);
        model.addConstr(y[i] >= 0);
        model.addConstr(y[i] + a[i] <= A_y);
        for (int j = 0; j < n; ++j) {
            if (i != j) {
                model.addConstr(c[i][j] == 1 - c[j][i]);
                model.addConstr(g[i][j] <= 1 + 1 / M * (x[j] - x[i] - a[i]) + 1 / (M * M));
                model.addConstr(y[j] - y[i] - a[i] + (1 - c[i][j]) * M + g[i][j] * M >= 0);
                model.addConstr(y[i] - y[j] - a[j] + c[i][j] * M + g[j][i] * M >= 0);
                model.addConstr(y[j] + a[j] - y[i] + (1 - c[i][j]) * M + g[i][j] * M >= 0);
                model.addConstr(y[i] + a[i] - y[j] + c[i][j] * M + g[j][i] * M >= 0);


            }
        }
    }

    model.optimize();
    std::cout << "Positions: " << std::endl;
    for (int i = 0; i < n; ++i) {
        std::cout << x[i].get(GRB_DoubleAttr_X) << ", " << y[i].get(GRB_DoubleAttr_X) << std::endl;
    }

    return 0;
}