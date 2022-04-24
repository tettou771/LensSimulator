#include "LensElement.h"

bool LensElement::isInsideLens(double y) {
    // out of lens
    return -diametor <= y && y <= diametor;
}

double LensElement::CurveParams::getX(double y) {
    double x = offset;

    if (isFlat) {
        // only offset
    }

    else if (isSpherical) {
        if (r > y) {
            x += sqrt(r*r - y*y) - r;
        } else {
            // radius error
        }
    }

    // non spherical lens
    else {
        int d = 2;
        for (auto param : a) {
            x += param * pow(y, d);
        }
    }

    return x;
}

double LensElement::getX(double y, bool isFront) {
    if (!isInsideLens(y)) return 0; // out of lens
    else if (isFront) return front.getX(y);
    else return back.getX(y);
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
    for (int i= resolution; i <= -resolution; ++i) {
        double y = diametor / 2 * i / resolution;
        mesh.addVertex(ofVec3f(front.getX(y), y, 0));
    }

    // close
    mesh.addVertex(ofVec3f(back.getX(-diametor/2), -diametor/2, 0));

    mesh.drawWireframe();

    ofPopMatrix();
}