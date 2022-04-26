#include "LensSimulator.h"

LensSimulator::LensSimulator() {}
LensSimulator::~LensSimulator() {}

void LensSimulator::onStart() {
    setWidth(1000);
    setHeight(500);

    // set a lens
    topScoreUnit.elements.push_back(LensElement());
    auto &l = topScoreUnit.elements.back();
    l.position = 50;
    l.diametor = 20;

//#define Spherical
#define SemiSpherical
    
#ifdef Spherical
    // spherical lens
    l.front.r = l.position;
    l.front.offset = 5;
    l.back.r = -l.position;
    l.back.offset = -5;
#elif defined SemiSpherical
    // semi spherical lens
    l.front.r = l.position;
    l.front.offset = 5;
    l.back.isSpherical = false;
    l.back.offset = -5;
    l.back.a.push_back(0.015);
    l.back.a.push_back(0.0);
    l.back.a.push_back(0.0);
    l.back.a.push_back(0.0);
#else
    // non spherical lens
    l.front.isSpherical = false;
    l.front.offset = 5;
    l.front.a.push_back(-0.015);
    l.front.a.push_back(0.0);
    l.front.a.push_back(0.0);
    l.front.a.push_back(0.0);
    l.back.isSpherical = false;
    l.back.offset = -5;
    l.back.a.push_back(0.015);
    l.back.a.push_back(0.0);
    l.back.a.push_back(0.0);
    l.back.a.push_back(0.0);
#endif
 
    l.n = 1.492659; // acryl
    l.abbe = 57.98; // acryl

    // set light point
    double targetF = l.position; // focul length
    double targetH = 0;
    lightPoints.push_back(LightPoint(atan(targetH/targetF), targetH));
    targetH = 10;
    lightPoints.push_back(LightPoint(atan(targetH/targetF), targetH));
    targetH = 20;
    lightPoints.push_back(LightPoint(atan(targetH/targetF), targetH));
    
    // exec
    simulate(topScoreUnit);
    
    randomness = 1;
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
    ofVec2f sensorPos = ofVec2f(250, getHeight() / 2 + 0.5);
    ofTranslate(sensorPos);
    
    // scaling
    ofScale(7);
    
    // sensor
    ofSetColor(0, 100, 200);
    double imageCircleRadius = 20;
    ofDrawLine(0, -imageCircleRadius, 0, imageCircleRadius);

    // lensElements
    ofSetColor(200);
    for (auto &lens : topScoreUnit.elements) {
        lens.draw2D();
    }
    
    // lightPoints
    for (auto &light : lightPoints) {
        light.draw2D();
    }
    
    // spot
    ofSetColor(50);
    ofTranslate(-10, 0);
    for (auto &light : lightPoints) {
        light.drawScreenSpot();
    }
    ofPopMatrix();
    
    // score
    stringstream ss;
    ss << "randomness " << randomness << endl;
    ss << "score " << topScoreUnit.score << endl;
    for (auto &lp : lightPoints) {
        ss << "average " << lp.average << endl;
        ss << "sigma " << lp.sigma << endl;
    }
    ofDrawBitmapString(ss.str(), 20, 20);
}

void LensSimulator::onKeyPressed(ofKeyEventArgs& key) {
    if (key.key == ' ') {
        int testNum = 500;
        auto currentTopUnit = topScoreUnit;
        for (int i=0; i<testNum; ++i) {
            auto testUnit = makeRandomLens(topScoreUnit, randomness);
            simulate(testUnit);
            if (testUnit.score > currentTopUnit.score || isnan(currentTopUnit.score)) {
                currentTopUnit = testUnit;
            }
        }
        topScoreUnit = currentTopUnit;
        simulate(topScoreUnit);
        randomness *= 0.5;
    }
    else if (key.key == 'r') {
        randomness = 1;
    }
}

LensUnit LensSimulator::makeRandomLens(const LensUnit &reference, double rand) {
    // copy
    LensUnit randomUnit;
    randomUnit = topScoreUnit;

    auto &l = randomUnit.elements[0];
    if (ofRandom(1) < 0.2) l.position += ofRandom(-10, 10) * rand;
    if (ofRandom(1) < 0.2) l.front.offset += ofRandom(-1, 1) * rand;
    if (ofRandom(1) < 0.2) l.back.offset += ofRandom(-1, 1) * rand;

#ifdef Spherical
    if (ofRandom(1) < 0.2) l.front.r += ofRandom(-6, 6) * rand;
    if (ofRandom(1) < 0.2) l.back.r += ofRandom(-6, 6) * rand;
#elif defined SemiSpherical
    double provability = 1. / 5;
    if (ofRandom(1) < provability) l.front.r += ofRandom(-6, 6) * rand;
    if (ofRandom(1) < provability) l.back.a[0] += ofRandom(-0.1, 0.1) * rand;
    if (ofRandom(1) < provability) l.back.a[1] += ofRandom(-0.1, 0.1) * rand;
    if (ofRandom(1) < provability) l.back.a[2] += ofRandom(-0.1, 0.1) * rand;
    if (ofRandom(1) < provability) l.back.a[3] += ofRandom(-0.1, 0.1) * rand;
#else
    double provability = 1. / 8;
    if (ofRandom(1) < provability) l.front.a[0] += ofRandom(-0.1, 0.1) * rand;
    if (ofRandom(1) < provability) l.front.a[1] += ofRandom(-0.1, 0.1) * rand;
    if (ofRandom(1) < provability) l.front.a[2] += ofRandom(-0.1, 0.1) * rand;
    if (ofRandom(1) < provability) l.front.a[3] += ofRandom(-0.1, 0.1) * rand;
    if (ofRandom(1) < provability) l.back.a[0] += ofRandom(-0.1, 0.1) * rand;
    if (ofRandom(1) < provability) l.back.a[1] += ofRandom(-0.1, 0.1) * rand;
    if (ofRandom(1) < provability) l.back.a[2] += ofRandom(-0.1, 0.1) * rand;
    if (ofRandom(1) < provability) l.back.a[3] += ofRandom(-0.1, 0.1) * rand;
#endif
    
    return randomUnit;
}

void LensSimulator::simulate(LensUnit &lensUnit) {
    lensUnit.score = 0;
    auto &lensElements = lensUnit.elements;
    
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
        light.setStart(lightStartPos, 50);
        
        // exec simulation
        for (auto &line : light.lines) {
            f64vec3 pastAnchor = line.p[0];
            double x = pastAnchor.x;
            f64vec3 dir(-cos(light.angle), -sin(light.angle), 0);
            
            // check collision step by step
            double maxStep = -10;
            double step = maxStep;
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
                
                bool collision = false;
                for (auto lens : lensElements) {
                    for (auto curve : {lens.front, lens.back}) {
                        
                        collision = lens.diametor/2 > currentH
                        && lens.position + curve.getX(currentH) < current.x
                        && lens.position + curve.getX(nextH) >= next.x;
                        
                        if (collision) {
                            if (step > -0.000001) {
                                
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

                                x += step;
                                step = maxStep;
                            } else {
                                step /= 10;
                            }
                        }
                        if (collision) break;
                    }
                    if (collision) break;
                }
                
                if (!collision) {
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
        
        light.calcStatistics();
        
    }
    
    // evaluation
    double posFactor = 0;
    double sigmaFactor = 0;
    for (auto light : lightPoints) {
        double pos = sqrt(light.average.y * light.average.y + light.average.z * light.average.z);
        posFactor += abs(pos - abs(light.target));
        sigmaFactor += light.sigma;
    }
    posFactor /= lightPoints.size();
    sigmaFactor /= lightPoints.size();
    lensUnit.score = MIN(1./sigmaFactor, 1./ posFactor) + 1. / sigmaFactor;
}
