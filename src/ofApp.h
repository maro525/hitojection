#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxKinect.h"
#include "ofxGui.h"

#define STRINGIFY(A) #A



class ofApp : public ofBaseApp
{
public:
    void setup();
    void update();
    void draw();
    void keyPressed(int key);

    ofxPanel                    gui;

    ofxKinect                   kinect;
    
    int angle;

    ofxCvGrayscaleImage         bgImage;
    ofxCvGrayscaleImage         grayImage;
    ofxCvGrayscaleImage         grayThreshNear;
    ofxCvGrayscaleImage         grayThreshFar;

    ofParameter<float>          nearThreshold;
    ofParameter<float>          farThreshold;
    ofParameter<float>          minArea;
    ofParameter<float>          maxArea;
    ofParameter<float>          threshold;
    ofParameter<float>          persistence;
    ofParameter<float>          maxDistance;
    ofParameter<float>          shaderThreshold;
    ofParameter<int>            projectionX;
    ofParameter<int>            projectionY;
    ofParameter<float>          projectionScale;
    
    ofShader maskingShader, binarizationShader;
    ofFbo mask;
    ofFbo projection;

    
    bool drawing;
    bool guiDraw;
    
    ofEasyCam cam;
    ofMesh mesh;
    int w, h;
    
    bool polygon_proj;
    bool mesh_proj;
    
    ofVideoPlayer 		fingerMovie;
};
