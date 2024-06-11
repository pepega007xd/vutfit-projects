#include "Student.h"

#include <cmath>
#include <iostream>
#include <osg/Plane>

#define _USE_MATH_DEFINES
#include <algorithm>
#include <math.h>

osg::Matrix inverse(const osg::Matrix &m) { return osg::Matrix::inverse(m); }

double degreesToRadians(double angleInDegrees) {
    return osg::DegreesToRadians(angleInDegrees);
}

osg::Matrix getScaleMatrix(const osg::Vec3d &scale) {
    // Task 1 - fill in the scale matrix
    osg::Matrix m(scale.x(), 0, 0, 0, //
                  0, scale.y(), 0, 0, //
                  0, 0, scale.z(), 0, //
                  0, 0, 0, 1);
    return m;
}

osg::Matrix getTranslationMatrix(const osg::Vec3d &translation) {
    // Task 2 - fill in the translation matrix
    osg::Matrix m(1, 0, 0, 0, //
                  0, 1, 0, 0, //
                  0, 0, 1, 0, //
                  translation.x(), translation.y(), translation.z(), 1);
    return m;
}

osg::Matrix rotateAroundX(double angleInRadians) {
    double sina = std::sin(angleInRadians);
    double cosa = std::cos(angleInRadians);

    // Task 3 - fill in the rotation matrix around X axis
    osg::Matrix m(1, 0, 0, 0,                                                //
                  0, std::cos(angleInRadians), std::sin(angleInRadians), 0,  //
                  0, -std::sin(angleInRadians), std::cos(angleInRadians), 0, //
                  0, 0, 0, 1);
    return m;
}

osg::Matrix rotateAroundY(double angleInRadians) {
    double sina = std::sin(angleInRadians);
    double cosa = std::cos(angleInRadians);

    // Task 4 - fill in the rotation matrix around Y axis
    osg::Matrix m(std::cos(angleInRadians), 0, -std::sin(angleInRadians), 0, //
                  0, 1, 0, 0,                                                //
                  std::sin(angleInRadians), 0, std::cos(angleInRadians), 0,  //
                  0, 0, 0, 1);
    return m;
}

osg::Matrix rotateAroundZ(double angleInRadians) {
    double sina = std::sin(angleInRadians);
    double cosa = std::cos(angleInRadians);

    // Task 5 - fill in the rotation matrix around Z axis
    osg::Matrix m(
        std::cos(angleInRadians), std::sin(angleInRadians), 0, 0,  ///////
        -std::sin(angleInRadians), std::cos(angleInRadians), 0, 0, ////////
        0, 0, 1, 0, /////////////////////
        0, 0, 0, 1);
    return m;
}

osg::Matrix getForwardTransformation() {
    // Task 6 - fill in forward transformation matrix consisting of 3 individual
    // transformation matrices
    osg::Matrix t = getTranslationMatrix({2, -2, 2});
    osg::Matrix s = getScaleMatrix({0.5, 0.5, 0.5});
    osg::Matrix r = rotateAroundY(degreesToRadians(45));
    osg::Matrix m = t * r * s;
    return m;
}
osg::Matrix getBackwardTransformation() {
    // Task 7 - fill in backward transformation matrix consisting of 3
    // individual transformation matrices (you can use inverse if it helps you).
    osg::Matrix s = getScaleMatrix({2, 2, 2});
    osg::Matrix t = getTranslationMatrix({2, 0, -2});
    osg::Matrix r = rotateAroundX(degreesToRadians(45));
    osg::Matrix m = r * t * s;

    // You HAVE TO return inverse of m
    return inverse(m);
}
