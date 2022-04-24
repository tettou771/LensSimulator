#include "LensSimulator.h"

LensSimulator::LensSimulator() {}
LensSimulator::~LensSimulator() {}

void LensSimulator::onStart() {
    setWidth(1000);
    setHeight(500);

    // set a lens
    lensElements.push_back(LensElement());
    auto &l = lensElements[0];
    l.position = 60;
    l.diametor = 40;
    l.front.r = 60;
    l.front.offset = 5;
    l.back.r = -60;
    l.back.offset = -5;
    l.n = 1.492659; // acryl
    l.abbe = 57.98; // acryl
    
    // set light point
    lightPoints.push_back(LightPoint(0));
    
    // exec
    simulate();
}

void LensSimulator::onUpdate() {
}

void LensSimulator::onDraw() {
    ofSetColor(0,100,0);
    ofNoFill();
    ofDrawRectangle(0, 0, getWidth(), getHeight());
    ofEnableBlendMode(OF_BLENDMODE_ADD);

    // draw optical
    ofPushMatrix();

    // sensor pos
    ofVec2f sensorPos = ofVec2f(100, getHeight() / 2);
    ofTranslate(sensorPos);
    
    // scaling
    ofScale(5);
    
    // sensor
    ofSetColor(0, 100, 200);
    double imageCircleRadius = 20;
    ofDrawLine(0, -imageCircleRadius, 0, imageCircleRadius);

    // lensElements
    ofSetColor(200);
    for (auto &lens : lensElements) {
        lens.drawSurface2D();
    }
    
    // lightPoints
    for (auto &light : lightPoints) {
        light.draw();
    }

    ofPopMatrix();
}

void LensSimulator::simulate() {
    if (lensElements.empty()) {
        ofLogError() << "lens elements is empty";
        return;
    }
    
    if (lightPoints.empty()) {
        ofLogError() << "light points is empty";
        return;
    }
    
    double lightStartPos = 100;
    for (auto &light : lightPoints) {
        light.clear();
        light.setStart(100, 50);
        
        // exec simulation
        for (auto &line : light.lines) {
            highp_dvec3 pastAnchor = line.p[0];
            double x = pastAnchor.x;
            highp_dvec3 dir(-cos(light.angle), -sin(light.angle), 0);
            
            while (x > 0) {
                double dx = -1;
                x += dx;
                highp_dvec3 current, next;
                current.x = x;
                current.y = pastAnchor.y + (x - pastAnchor.x) * dir.y / dir.x;
                current.z = pastAnchor.z + (x - pastAnchor.x) * dir.z / dir.x;
                next.x = x + dx;
                next.y = pastAnchor.y + (next.x - pastAnchor.x) * dir.y / dir.x;
                next.z = pastAnchor.z + (next.x - pastAnchor.x) * dir.z / dir.x;
                
                double currentR = sqrt(current.y * current.y + current.z * current.z);
                double nextR = sqrt(next.y * next.y + next.z * next.z);

                for (auto lens : lensElements) {
                    for (auto curve : {lens.front, lens.back}) {
                        if (lens.diametor/2 > currentR
                            && lens.position + curve.getX(currentR) < current.x
                            && lens.position + curve.getX(nextR) >= next.x) {
                            
                            highp_dvec3 hit;
                            // TODO: calc more accuracy position
                            hit.x = (current.x + next.x) / 2;
                            hit.y = (current.y + next.y) / 2;
                            hit.z = (current.z + next.z) / 2;
                            line.p.push_back(hit);
                            pastAnchor - hit;
                            
                            // calc next direction
                            auto normal = curve.getNormal(hit.y, hit.z);
                            if (glm::dot(normal, dir) > 0) {
                                dir = normal;
                            }
                        }
                    }
                }
                
                if (next.x < 0) {
                    // is this hit a lens?
                    if (line.p.size() == 1) line.isValid = false;
                    else line.isValid = true;
                    
                    line.screenPos.x = 0;
                    line.screenPos.y = pastAnchor.y - pastAnchor.x * dir.y / dir.x;
                    line.screenPos.z = pastAnchor.z - pastAnchor.x * dir.z / dir.x;
                    line.p.push_back(line.screenPos);
                    break;
                }
            }
        }
    }
}
