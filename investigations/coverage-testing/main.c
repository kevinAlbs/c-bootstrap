#include <math.h>
#include <stdio.h>
#include <limits.h>

double dist(double x1, double y1, double x2, double y2)
{
    double xdiff = x1 - x2;
    double ydiff = y2 - y1;
    return sqrt(xdiff * xdiff + ydiff * ydiff);
}

int main()
{
    printf("computing distance ... start\n");
    double total = 0;
    for (int i = 0; i < 10; i++)
    {
        total += dist(1.23, 4.56, 7.89, 0.12);
    }
    printf("total: %f \n", total);
    printf("computing distance: ... done\n");
}
