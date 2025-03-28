#ifndef CUSTOMER_H
#define CUSTOMER_H

#include <iostream>

struct Customer
{
    double start, end;
    double weight;
    int role;
    Customer() : start(0), end(0), weight(0), role(0) {}
    Customer(double s, double e, double w, int r) : start(s), end(e), weight(w), role(r){}
};

#endif // CUSTOMER_H
