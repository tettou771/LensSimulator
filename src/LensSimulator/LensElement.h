#pragma once
#include "ofMain.h"

using namespace glm;

class LensElement {
public:
    LensElement(){front.isFront = true;}
    ~LensElement(){}

    struct CurveParams{
        bool isFront = false;
        bool isFlat = false; // if true, plane surface
        bool isSpherical = true;
        double offset = 0; // offset from position
        double r = 0; // spherical radius
        vector<double> a; // polynomial parametors [0]:^2 [1]:^4 ...
        double getX(double h);
        double getDH(double h);
        f64vec3 getNormal(double y, double z);
    };

    double position; // position from sensor
    double diametor; // diametor of lens
    CurveParams back, front; // both surface (back is near)
    double n; // refractive index
    double abbe; // abbe number

    bool isInsideLens(double y);
    double getX(double y, bool isFront); // relative position
    void drawSurface2D();
};
