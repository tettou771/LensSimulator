#pragma once
#include "ofMain.h"

using namespace glm;

class LightLine {
public:
    LightLine(highp_dvec3 _firstPos);
    
    void draw();
    
    vector<highp_dvec3> p;
    bool isValid = false;
    highp_dvec3 screenPos;
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
    highp_dvec3 average;
    
    double startPos;
    double startHeight;
};
