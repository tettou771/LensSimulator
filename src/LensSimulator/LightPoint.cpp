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

LightPoint::LightPoint(double _angle, double _target):
angle(_angle), target(_target) {
    clear();
}

void LightPoint::draw() {
    for (auto &line : lines) {
        line.draw();
    }
}

void LightPoint::draw2D() {
    for (auto &line : lines) {
        if (!line.p.empty() && line.p[0].z == 0) {
            line.draw();
        }
    }
}

void LightPoint::drawScreenSpot() {
    ofSetColor(200);
    
    ofMesh screenSpots;
    screenSpots.setMode(OF_PRIMITIVE_POINTS);
    for (auto &line : lines) {
        if (!line.isValid) continue;
        ofVec3f sp;
        sp.x = line.screenPos.z;
        sp.y = line.screenPos.y;
        screenSpots.addVertex(sp);
    }
    screenSpots.draw();
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
    
    int resolution = 31;
    double x = _pos;
    for (int i=0; i<resolution; ++i) {
        double y = startHeight * (2. * i / (resolution - 1) - 1);
        
        bool is2Dsimulation = false;
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
        if (isnan(line.screenPos.x)) continue;
        if (isnan(line.screenPos.y)) continue;
        if (isnan(line.screenPos.z)) continue;
        count++;
        average += line.screenPos;
    }
    
    if (count > 0) {
        average /= count;
    }

    count = 0;
    for (auto &line: lines) {
        if(!line.isValid) continue;
        auto v = line.screenPos - average;
        double sgm = v.x * v.x + v.y * v.y;
        if (isnan(sgm)) continue;
        count++;
        sigma += sgm;
    }
    if (count > 0) {
        sigma = sqrt(sigma) / count;
    }
    
    ofLog() << "ave: " << average;
    ofLog() << "sigma: " << sigma;
    
    simulated = true;
}
