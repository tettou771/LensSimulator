#include "LensSimulator.h"

LensSimulator::LensSimulator() {}
LensSimulator::~LensSimulator() {}

void LensSimulator::onStart() {
    setWidth(1000);
    setHeight(500);

    // set a lens
    lensElements.push_back(LensElement());
    auto &l = lensElements[0];
    l.diametor = 30;
    l.front.isFlat = false;
    l.front.isSpherical = true;
    l.front.r = 100;
    l.back.r = 200;
}

void LensSimulator::onUpdate() {
}

void LensSimulator::onDraw() {
    ofSetColor(0,100,0);
    ofNoFill();
    ofDrawRectangle(0, 0, getWidth(), getHeight());
    ofDrawLine(0, 0, getWidth(), getHeight());
    ofDrawCircle(100, 100, 100);

    // draw optical
    ofPushMatrix();

    // sensor pos
    ofVec2f sensorPos = ofVec2f(100, getHeight() / 2);
    ofTranslate(sensorPos);

    ofSetColor(200);
    for (auto &lens : lensElements) {
        lens.drawSurface2D();
    }

    ofPopMatrix();
}
