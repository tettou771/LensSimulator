#include "LensSimulator.h"

LensSimulator::LensSimulator() {}
LensSimulator::~LensSimulator() {}

void LensSimulator::onStart() {
    setWidth(1000);
    setHeight(500);

    // set a lens
    /*
     // spherical lens
    lensElements.push_back(LensElement());
    auto &l = lensElements.back();
    l.position = 62;
    l.diametor = 40;
    l.front.r = 60;
    l.front.offset = 5;
    l.back.r = -60;
    l.back.offset = -5;
    l.n = 1.492659; // acryl
    l.abbe = 57.98; // acryl
*/
    
    // non spherical lens
    lensElements.push_back(LensElement());
    auto &l = lensElements.back();
    l.position = 62;
    l.diametor = 40;
    l.front.isSpherical = false;
    l.front.offset = 7;
    l.front.a.push_back(-0.017);
    l.back.isSpherical = false;
    l.back.offset = -7;
    l.back.a.push_back(0.017);
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
        light.setStart(100, 30);
        
        // exec simulation
        for (auto &line : light.lines) {
            f64vec3 pastAnchor = line.p[0];
            double x = pastAnchor.x;
            f64vec3 dir(-cos(light.angle), -sin(light.angle), 0);
            
            // check collision step by step
            double step = -0.001;
            while (x > 0) {
                f64vec3 current, next;
                current.x = x;
                current.y = pastAnchor.y + (x - pastAnchor.x) * dir.y / dir.x;
                current.z = pastAnchor.z + (x - pastAnchor.x) * dir.z / dir.x;
                next.x = x + step;
                next.y = pastAnchor.y + (next.x - pastAnchor.x) * dir.y / dir.x;
                next.z = pastAnchor.z + (next.x - pastAnchor.x) * dir.z / dir.x;
                
                double currentH = sqrt(current.y * current.y + current.z * current.z);
                double nextH = sqrt(next.y * next.y + next.z * next.z);
                
                for (auto lens : lensElements) {
                    for (auto curve : {lens.front, lens.back}) {
                        
                        bool collision = lens.diametor/2 > currentH
                        && lens.position + curve.getX(currentH) < current.x
                        && lens.position + curve.getX(nextH) >= next.x;
                        
                        if (collision) {
                            f64vec3 hit;
                            // TODO: calc more accuracy position
                            hit.x = (current.x + next.x) / 2;
                            hit.y = (current.y + next.y) / 2;
                            hit.z = (current.z + next.z) / 2;
                            line.p.push_back(hit);
                            pastAnchor = hit;
                            
                            // calc next direction
                            auto normal = curve.getNormal(hit.y, hit.z);
                            double nd;
                            if (curve.isFront) {
                                nd = 1. / lens.n;
                            }else{
                                nd = lens.n / 1.;
                            }
                            double inAngle = glm::angle(normal, dir);
                            double outAngle = asin(nd * sin(inAngle));
                            cout << outAngle << endl;
                            // no angle, same direction
                            if (inAngle == 0) {
                                dir = normal;
                            } else if (inAngle == PI) {
                                dir = -normal;
                            }
                            else {
                                f64vec3 axis = cross(dir, normal);
                                f64mat4 rotationMat(1);
                                rotationMat = glm::rotate(rotationMat, outAngle, axis);
                                dir = rotationMat * f64vec4(normal, 1);
                            }
                        }
                    }
                    x += step;
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
