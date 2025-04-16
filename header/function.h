#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include<random>
using namespace std;

extern random_device rd;
extern mt19937 gen;

int getRandomNumber(int a, int b);
double getRandomDouble(double a, double b);
bool checkInsertNext(vector<int> x, int v, int roleId,double now);
bool checkValidVector(vector<int> x, int roleId,double now);
double getTimeDroneTrip(vector<int> x, double now);
double jaccardSimilarity(const vector<vector<int>> &solutionA, const vector<vector<int>> &solutionB);
pair<int, vector<int>> solverTSPTWmapping(const vector<int> &Trip, const vector<vector<double>> &Ex, double k_trip, double start_time);
pair<int, vector<int>> solverTSPTWmappingLarge(const vector<int> &Trip, const vector<vector<double>> &Ex, double k_trip, double start_time,int batch_size);
void printParetoFront(vector<vector<pair<int, double>>>& pr1, string file_path);
bool checkValidSolution(vector<vector<int>> route, vector<int> role);
#endif
