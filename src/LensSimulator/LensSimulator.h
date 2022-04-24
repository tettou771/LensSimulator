#pragma once
#include "ofxComponent.h"
#include "LensElement.h"
#include "LightPoint.h"

using namespace ofxComponent;

class LensSimulator : public ofxComponentBase {
public:
    LensSimulator();
    ~LensSimulator();

    void onStart() override;
    void onUpdate() override;
    void onDraw() override;

    vector<LensElement> lensElements;
    vector<LightPoint> lightPoints;
    void simulate();
};
