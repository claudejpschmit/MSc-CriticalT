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
            << " smoothed.dat" <<  endl;
        cout << "Optionally run as: " << argv[0] 
            << " smoothed.dat derivative.dat energy.dat nsteps maxindex" <<  endl;

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

    //first value: forward difference
    derivative.xvalues.push_back(dataset.xvalues[0]);
    double deriv = - (dataset.yvalues[1] - dataset.yvalues[0])
        / (dataset.xvalues[1] - dataset.xvalues[0]);
    derivative.yvalues.push_back(deriv);
    output << derivative.xvalues[0] << " " 
        << derivative.yvalues[0] << endl;
    //intermediate values: central difference
    for (int i = 1; i < length - 1; ++i) {
        derivative.xvalues.push_back(dataset.xvalues[i]);
        deriv = - (dataset.yvalues[i + 1] - dataset.yvalues[i - 1])
            / (dataset.xvalues[i + 1] - dataset.xvalues[i - 1]);
        derivative.yvalues.push_back(deriv);
        output << derivative.xvalues[i] << " " 
            << derivative.yvalues[i] << endl;
    }
    //last value: backward difference
    derivative.xvalues.push_back(dataset.xvalues[length - 1]);
    deriv = - (dataset.yvalues[length - 1] - dataset.yvalues[length - 2])
        / (dataset.xvalues[length - 1] - dataset.xvalues[length - 2]);
    derivative.yvalues.push_back(deriv);


    cout << "produced derivative data" << endl;

    // quick hack to find critical temperature
    if (argc > 2) {
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
            if (max < sDeriv.yvalues[i] && sDeriv.xvalues[i] > 0.01) {
                max = sDeriv.yvalues[i];
                Tcrit = sDeriv.xvalues[i];
            }
        }

        cout << "Critical Temperature is: " << Tcrit << endl;
    }

    // compute standard deviation
    if (argc > 3) {
        ifstream energyfile(argv[3]);
        int nstep = atoi(argv[4]);
        int maxIndex = atoi(argv[5]);
        data tempEnergy;
        vector<vector<double>> energies;
        int length3 = 0;
        // reads in energy file to tempEnergy data array
        while (!energyfile.eof()) {
            getline(energyfile, line);
            istringstream linestream(line);
            if (line[0]!='#' && line.length() != 0) {
                double x, y;
                linestream >> x >> y;
                tempEnergy.xvalues.push_back(x);
                tempEnergy.yvalues.push_back(y);
                length3 += 1;
            }
        }

        //creates double array that contain energies
        for (int i = 0; i < maxIndex; ++i) {
            vector<double> buffer;
            for (int j = 0; j < nstep/1000; ++j) {
                buffer.push_back(tempEnergy.yvalues[i * nstep/1000 + j]);
            }
            energies.push_back(buffer);
        }
        energyfile.close();
        /* 
           for (int i = 0; i < maxIndex; ++i) {
           cout << energies[i][0] << endl;
        // energies has the following structure
        // energies[index][xposition] -> compare notebook.
        }*/

        // creates averages <E> and <E²>
        vector<double> averages, sqAverages, Cv;

        for (int j = 0; j < nstep/1000; ++j) {
            double sum = 0;
            double sqSum = 0;
            for (int i = 0; i < maxIndex; ++i) {
                sum += energies[i][j];
                sqSum += energies[i][j] * energies[i][j];
            }
            averages.push_back(sum/(double)maxIndex);
            sqAverages.push_back(sqSum/(double)maxIndex);
        }

        // creating Cv standard deviation and output to file.
        ofstream output2("stdev.dat");
        for (int i = 0; i < nstep/1000; ++i) {
            double sqT, stdev;
            sqT = tempEnergy.xvalues[i] * tempEnergy.xvalues[i];
            stdev = sqAverages[i] - averages[i] * averages[i];
            Cv.push_back(stdev/sqT);
            output2 << tempEnergy.xvalues[i] << " " << Cv[i] << endl;
        }
        output2.close();

    }


    return 0;
}
