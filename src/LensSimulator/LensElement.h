#include "ofMain.h"

class LensElement {
public:
    LensElement(){}
    ~LensElement(){}

    struct CurveParams{
        bool isFlat = false; // if true, plane surface
        bool isSpherical = true;
        double offset = 0; // offset from position
        double r = 0; // spherical radius
        vector<double> a; // polynomial parametors [0]:^2 [1]:^4 ...
        double getX(double y);
    };

    double position; // position from sensor
    double diametor; // diametor of lens
    CurveParams back, front; // both surface (back is near)

    bool isInsideLens(double y);
    double getX(double y, bool isFront); // relative position
    void drawSurface2D();
};
