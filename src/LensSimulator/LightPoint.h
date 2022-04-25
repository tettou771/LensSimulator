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
    LightPoint(double _angle);
    
    void draw();
    void clear();
    void setStart(double _pos, double _height);
    void calcStatistics();
    
    double angle; // light direction
    vector<LightLine> lines;
    
    bool simulated;
    double sigma;
    f64vec3 average;
    
    double startPos;
    double startHeight;
};
