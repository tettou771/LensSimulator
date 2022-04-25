#include "LightPoint.h"

LightLine::LightLine(f64vec3 _firstPos) {
    p.clear();
    p.push_back(_firstPos);
}

void LightLine::draw() {
    ofNoFill();
    
    if (isValid) ofSetColor(100);
    else ofSetColor(100, 0, 0);
    
    ofBeginShape();
    for (auto &v : p) {
        ofVertex(ofVec3f(v.x, v.y, v.z));
    }
    ofEndShape();
    
    // show angle point
//    for (auto &v : p) {
//        ofDrawCircle(ofVec3f(v.x, v.y, v.z), 1);
//    }
}

LightPoint::LightPoint(double _angle):
angle(_angle) {
    clear();
}

void LightPoint::draw() {
    ofPushMatrix();
    
    for (auto &line : lines) {
        line.draw();
    }
    
    ofPopMatrix();
}

void LightPoint::clear() {
    lines.clear();
    simulated = false;
    sigma = 0;
    average = f64vec3(0);
    startPos = 0;
    startHeight = 0;
}

void LightPoint::setStart(double _pos, double _height) {
    startPos = _pos;
    startHeight = _height;
    
    lines.clear();
    
    int resolution = 21;
    double x = _pos;
    for (int i=0; i<resolution; ++i) {
        double y = startHeight * (2. * i / (resolution - 1) - 1);
        
        bool is2Dsimulation = true;
        if (is2Dsimulation) {
            double z = 0;
            f64vec3 firstPos(x, y, z);
            lines.push_back(LightLine(firstPos));
        } else {
            for (int j=0; j<resolution; ++j) {
                double z = startHeight * (2. * j / (resolution - 1) - 1);
                f64vec3 firstPos(x, y, z);
                lines.push_back(LightLine(firstPos));
            }
        }
    }
}

void LightPoint::calcStatistics() {
    int count = 0;
    sigma = 0;
    average = f64vec3(0);
    for (auto &line: lines) {
        if(!line.isValid) continue;
        count++;
        average += line.screenPos;
    }
    
    if (count > 0) {
        average /= count;
    }

    for (auto &line: lines) {
        if(!line.isValid) continue;
        count++;
        auto v = line.screenPos - average;
        sigma += v.x * v.x + v.y * v.y;
    }
    if (count > 0) {
        sigma = sqrt(sigma) / count;
    }
    
    simulated = true;
}
