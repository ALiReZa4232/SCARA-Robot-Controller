#include <iostream>
#include <cmath>
using namespace std;

int c;
double xP = 0.0;
double yP = 0.0;
double L1 = 300.0; // L1 = 290mm
double L2 = 200.0; // L2 = 260mm
double theta1 = 0 , theta2 = 0;

void forwardKinematics();
void inverseKinematics(double x, double y);

int main() {
    cout << "Do you want to calculate forwardKinematics or inverseKinematics: " << endl;
    cin >> c;
    if(c == 1){
        cout << "Enter Theta1 value: " << endl;
        cin >> theta1;
        cout << "Enter Theta2 value: " << endl;
        cin >> theta2;

        forwardKinematics();
    }
    else {
        cout << "Enter X value: " << endl;
        cin >> xP;
        cout << "Enter Y value: " << endl;
        cin >> yP;

        inverseKinematics(xP, yP);
    }
}


// FORWARD KINEMATICS
void forwardKinematics() {
    double theta1F = theta1 * M_PI / 180;   // degrees to radians
    double theta2F = theta2 * M_PI / 180;

    xP = L1 * cos(theta1F) + L2 * cos(theta1F + theta2F);
    yP = L1 * sin(theta1F) + L2 * sin(theta1F + theta2F);

    cout << "X value is: " << xP << endl;
    cout << "Y value is: " << yP << endl;
}
// INVERSE KINEMATICS
void inverseKinematics(double x, double y) {
    theta2 = -1 * acos((pow(x, 2) + pow(y, 2) - pow(L1, 2) - pow(L2, 2)) / (2 * L1 * L2));
    theta1 = atan2(y, x) - atan2((L2 * sin(theta2)), (L1 + L2 * cos(theta2)));

    theta2 = theta2 * 180 / M_PI;
    theta1 = theta1 * 180 / M_PI;

    // Angles adjustment depending in which quadrant the final tool coordinate x,y is
    /*if (x >= 0 & y >= 0) {       // 1st quadrant
        theta1 = 90 - theta1;
    }
    if (x < 0 & y > 0) {       // 2nd quadrant
        theta1 = 90 - theta1;
    }
    if (x < 0 & y < 0) {       // 3d quadrant
        theta1 = 270 - theta1;
    }
    if (x > 0 & y < 0) {       // 4th quadrant
        theta1 = -90 - theta1;
    }
    if (x < 0 & y == 0) {
        theta1 = 270 + theta1;
    }*/

    /*theta2 = acos((pow(x, 2) + pow(y, 2) - pow(L1, 2) - pow(L2, 2)) / (2 * L1 * L2));

    double theta2_1 = theta2;
    double theta2_2 = fabs(theta2);

    double theta1_1 = atan2(y, x) - atan2((L2 * sin(theta2_1)), (L1 + L2 * cos(theta2_1)));
    double theta1_2 = atan2(y, x) - atan2((L2 * sin(theta2_2)), (L1 + L2 * cos(theta2_2)));

    theta1_1 = theta1_1 * 180 / M_PI;   // radians to degrees
    theta2_1 = theta2_1 * 180 / M_PI;
    theta1_2 = theta1_2 * 180 / M_PI;
    theta2_2 = theta2_2 * 180 / M_PI;

    // Angles adjustment for the final tool coordinate x,y
    double theta1_1N = fabs(theta1_1 - theta1);
    double theta1_2N = fabs(theta1_2 - theta1);

    if (theta1_1N < theta1_2N) {
        theta1 = theta1_1;
        theta2 = theta2_1;
    } else {
        theta1 = theta1_2;
        theta2 = theta2_2;
    }*/

    cout << "Theta1 value is: " << theta1 << endl;
    cout << "Theta2 value is: " << theta2 << endl;
}
