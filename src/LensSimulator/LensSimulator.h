#pragma once
#include "ofxComponent.h"
#include "LensElement.h"
#include "LightPoint.h"

using namespace ofxComponent;

struct LensUnit {
    vector<LensElement> elements;
    double score = 0;
};

class LensSimulator : public ofxComponentBase {
public:
    LensSimulator();
    ~LensSimulator();

    void onStart() override;
    void onUpdate() override;
    void onDraw() override;
    void onKeyPressed(ofKeyEventArgs& key) override;

    vector<LightPoint> lightPoints;
    double randomness;
    LensUnit makeRandomLens(const LensUnit &reference, double rand);
    void simulate(LensUnit &lensUnit);
    
    LensUnit topScoreUnit;
};
