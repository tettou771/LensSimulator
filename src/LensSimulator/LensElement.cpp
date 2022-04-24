#include "LensElement.h"

bool LensElement::isInsideLens(double y) {
    // out of lens
    return -diametor <= y && y <= diametor;
}

double LensElement::CurveParams::getX(double h) {
    double x = offset;

    if (isFlat) {
        // only offset
    }

    else if (isSpherical) {
        if (abs(r) > h) {
            if (r > 0)
                x += sqrt(r*r - h*h) - r;
            else if (r < 0)
                x += -sqrt(r*r - h*h) - r;
        } else {
            // radius error
        }
    }

    // non spherical lens
    else {
        int d = 2;
        for (auto param : a) {
            x += param * pow(h, d);
        }
    }

    return x;
}

double LensElement::CurveParams::getDX(double h) {
    double dx = 0;
    if (isFlat) {
    }

    else if (isSpherical) {
        if (abs(r) > h) {
            double x0 = sqrt(r*r - h*h);
            if (r > 0)
                dx = - x0 / h;
            else if (r < 0)
                dx = x0 / h;
        } else {
            // radius error
        }
    }

    // non spherical lens
    else {
        int d = 2;
        for (auto param : a) {
            dx += (d - 1) * param * pow(h, d-1);
        }
    }

    return dx;
}


highp_dvec3 LensElement::CurveParams::getNormal(double y, double z) {
    highp_dvec3 normal;
    double h = sqrt(y*y + z*z);
    double dx = getDX(h);

    double theta = atan(dx);
    normal.x = cos(theta);
    normal.y = sin(theta) * y / h;
    normal.z = sin(theta) * z / h;
    
    if (!isFront) normal = -normal;
    
    return normal;
}

double LensElement::getX(double h, bool isFront) {
    if (!isInsideLens(h)) return 0; // out of lens
    else if (isFront) return front.getX(h);
    else return back.getX(h);
}

void LensElement::drawSurface2D() {
    ofPushMatrix();
    ofTranslate(position, 0);
    
    int resolution = 10;
    ofMesh mesh;
    mesh.setMode(ofPrimitiveMode::OF_PRIMITIVE_LINE_STRIP);
    
    // back side
    for (int i= -resolution; i <= resolution; ++i) {
        double y = diametor / 2 * i / resolution;
        mesh.addVertex(ofVec3f(back.getX(y), y, 0));
    }

    // front side
    for (int i= resolution; i >= -resolution; --i) {
        double y = diametor / 2 * i / resolution;
        mesh.addVertex(ofVec3f(front.getX(y), y, 0));
    }

    // close
    mesh.addVertex(ofVec3f(back.getX(-diametor/2), -diametor/2, 0));

    mesh.drawWireframe();

    ofPopMatrix();
}
