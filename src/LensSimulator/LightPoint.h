#pragma once
#include "ofMain.h"

using namespace glm;

class LightLine {
public:
    LightLine(f64vec3 _firstPos);
    
    void draw();
    
    vector<f64vec3> p;
    bool isValid = false;
    f64vec3 screenPos;
};

class LightPoint {
public:
    LightPoint(double _angle, double _target);
    
    void draw();
    void draw2D();
    void drawScreenSpot();
    void clear();
    void setStart(double _pos, double _height);
    void calcStatistics();
    
    double angle; // light direction
    vector<LightLine> lines;
    int validCount = 0;
    
    bool simulated;
    double sigma;
    f64vec3 average;
    double target;
    
    double startPos;
    double startHeight;
};
