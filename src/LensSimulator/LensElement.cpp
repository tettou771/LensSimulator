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
        if (abs(r) >= h) {
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

double LensElement::CurveParams::getDH(double h) {
    double dh = 0;
    if (isFlat) {
    }

    else if (isSpherical) {
        if (abs(r) > h) {
            double x0 = sqrt(r*r - h*h);
            if (x0 == 0) {
                dh = 1.;
            }
            else if (r > 0){
                dh = - h / x0;
            }
            else if (r < 0){
                dh = h / x0;
            }
        } else {
            // radius error
        }
    }

    // non spherical lens
    else {
        int d = 2;
        for (auto param : a) {
            dh += (d - 1) * param * pow(h, d-1);
        }
    }

    return dh;
}


f64vec3 LensElement::CurveParams::getNormal(double y, double z) {
    f64vec3 normal;
    double h = sqrt(y*y + z*z);
    double dh = getDH(h);

    if (dh == 0) {
        normal = f64vec3(-1, 0, 0);
    }
    else if (dh == 1 || dh == -1) {
        normal = f64vec3(0, sin(y/h), sin(z/h));
    }
    else {
        double theta = atan(dh);
        normal.x = -cos(theta);
        normal.y = sin(theta) * y / h;
        normal.z = sin(theta) * z / h;
    }
    
    if (isFront) normal = -normal;
    
    return normal;
}

double LensElement::getX(double h, bool isFront) {
    if (!isInsideLens(h)) return 0; // out of lens
    else if (isFront) return front.getX(h);
    else return back.getX(h);
}

void LensElement::draw2D() {
    ofPushMatrix();
    ofTranslate(position, 0);
    
    int resolution = 20;
    ofMesh mesh;
    mesh.setMode(ofPrimitiveMode::OF_PRIMITIVE_LINE_STRIP);
    
    // back side
    for (int i= -resolution; i <= resolution; ++i) {
        double h = diametor / 2 * i / resolution;
        mesh.addVertex(ofVec3f(back.getX(h), h, 0));
    }

    // front side
    for (int i= resolution; i >= -resolution; --i) {
        double h = diametor / 2 * i / resolution;
        mesh.addVertex(ofVec3f(front.getX(h), h, 0));
    }

    // close
    mesh.addVertex(ofVec3f(back.getX(-diametor/2), -diametor/2, 0));
    mesh.drawWireframe();
    
    // normal
    bool showNormal = false;
    
    if (showNormal) {
        int normalNum = 9;
        for (int i= 0; i < normalNum; ++i) {
            double h = diametor * ((double)i / (normalNum - 1) - 0.5);
            for (auto curve : {front, back}) {
                ofVec3f surface(curve.getX(h), h, 0);
                auto n(curve.getNormal(h, 0));
                double m = 10; // magnify
                ofVec3f normal(n.x * m, n.y * m, n.z * m);
                ofDrawLine(surface, surface + normal);
            }
        }
    }

    ofPopMatrix();
}
