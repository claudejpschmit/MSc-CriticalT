#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include "data.hpp"

using namespace std;

int main(int argc, char *argv[]) {

    if (argc < 2) {
        cout << "This program should be run as: " << argv[0] 
            << " data_file" <<  endl;
        return -1;
    }

    // reading input file into dataset
    ifstream input(argv[1]);

    data dataset;
    string line;
    int length = 0;
    while (!input.eof()) {
        getline(input, line);
        istringstream linestream(line);
        if (line[0]!='#' && line.length() != 0) {
            double x, y;
            linestream >> x >> y;
            dataset.xvalues.push_back(x);
            dataset.yvalues.push_back(y);
            length += 1;
        }
    }
    input.close();

    // next, estimate the derivative

    data derivative;
    ofstream output("derivative.dat");
    for (int i = 0; i < length - 1; ++i) {
        derivative.xvalues.push_back(dataset.xvalues[i]);
        double deriv = - (dataset.yvalues[i + 1] - dataset.yvalues[i])
            / (dataset.xvalues[i + 1] - dataset.xvalues[i]);
        derivative.yvalues.push_back(deriv);
        output << derivative.xvalues[i] << " " 
            << derivative.yvalues[i] << endl;
    }
    derivative.xvalues.push_back(dataset.xvalues[length - 1]);
    derivative.yvalues.push_back(derivative.yvalues[length - 2]);
    output << derivative.xvalues[length - 1] << " " 
        << derivative.yvalues[length - 1] << endl;
    output.close();

    cout << "produced derivative data" << endl;

    // quick hack to find critical temperature
    if (argc == 3) {
        ifstream input2(argv[2]);

        data sDeriv;
        int length2 = 0;
        while (!input2.eof()) {
            getline(input2, line);
            istringstream linestream(line);
            if (line[0]!='#' && line.length() != 0) {
                double x, y;
                linestream >> x >> y;
                sDeriv.xvalues.push_back(x);
                sDeriv.yvalues.push_back(y);
                length2 += 1;
            }
        }
        input.close();

        double max = sDeriv.yvalues[0];
        double Tcrit = sDeriv.xvalues[0];
        for (int i = 0; i < length2; ++i) {
            if (max < sDeriv.yvalues[i]) {
                max = sDeriv.yvalues[i];
                Tcrit = sDeriv.xvalues[i];
            }
        }

        cout << "Critical Temperature is: " << Tcrit << endl;
    }

    return 0;
}
