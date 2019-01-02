#include "ofApp.h"


void ofApp::setup() {
    
    ofSetVerticalSync(true);

    // set up kinect
    kinect.setRegistration(true);
    kinect.init();
    kinect.open();

    grayImage.allocate(kinect.width, kinect.height);
    grayThreshNear.allocate(kinect.width, kinect.height);
    grayThreshFar.allocate(kinect.width, kinect.height);
    mask.allocate(kinect.width, kinect.height);
    projection.allocate(kinect.width, kinect.height);
    
    // setup gui
    gui.setup("parameters");
    gui.add(nearThreshold.set("nearThresh", 230, 0, 255));
    gui.add(farThreshold.set("farThresh", 10, 0, 255));
    gui.add(minArea.set("minArea", 1000, 0, 5000));
    gui.add(maxArea.set("maxArea", 70000, 15000, 150000));
    gui.add(threshold.set("threshold", 15, 1, 100));
    gui.add(persistence.set("persistence", 15, 1, 100));
    gui.add(maxDistance.set("maxDistance", 32, 1, 100));
    gui.add(shaderThreshold.set("shaderThreshold", 0.5, 0, 1));
    gui.add(projectionX.set("projectionX", 100, -ofGetWidth(), ofGetWidth()));
    gui.add(projectionY.set("projectionY", 100, -ofGetHeight(), ofGetHeight()));
    gui.add(projectionScale.set("projectionScale", 1, 0, 5));

    /*
     白黒の2値化にするShader
     r値がthreshold値未満であれば白、以上であれば黒にする
     */

    string binaryShader = STRINGIFY(
                                    uniform float threshold;
                                    uniform sampler2DRect tex0;
                                    void main() {
                                        vec2 st = gl_TexCoord[0].st;
                                        
                                        float c = texture2DRect(tex0, st).r > threshold ? 1.0 : 0.0;
                                        gl_FragColor = vec4(vec3(c), 1.0);
                                    }
                                    
                                    );
    binarizationShader.unload();
    binarizationShader.setupShaderFromSource(GL_FRAGMENT_SHADER, binaryShader);
    binarizationShader.linkProgram();
    
    /*
     マスキングするShader
     rgbのr値(0.0~1.0)を取って0.5以上だったら透明、未満であれば黒にする
     */
    string maskShader = STRINGIFY(
                                  uniform sampler2DRect tex0;
                                  void main() {
                                      vec2 st = gl_TexCoord[0].st;
                                      if (texture2DRect(tex0, st).r > 0.5) {
                                          discard;
                                      }
                                      gl_FragColor = vec4(vec3(0.0), 1.0);
                                  }
                                  
                                  );
    maskingShader.unload();
    maskingShader.setupShaderFromSource(GL_FRAGMENT_SHADER, maskShader);
    maskingShader.linkProgram();
    
    
    /*
     値の初期設定
     */
    
    drawing = false;
    guiDraw = true;
    
    fingerMovie.load("hito5.mov");
    fingerMovie.setLoopState(OF_LOOP_NORMAL);
    fingerMovie.play();
    
}


void ofApp::update() {
    kinect.update();

    binarizationShader.setUniform1f("threshold", shaderThreshold);

    if(kinect.isFrameNew()) {
       // process kinect depth image
       grayImage.setFromPixels(kinect.getDepthPixels());
    }
    
    fingerMovie.update();
}

void ofApp::draw() {
    ofBackground(0);

    if(drawing){
            // FBOの大きさを変えるためのfbo
            projection.begin();
        
                ofClear(0);
            
                fingerMovie.draw(0,0);
            
            
                // 2値化したものを書くfbo
                mask.begin();
                
                    /// グレースケールを2値化するシェーダー
        
                    binarizationShader.begin();
        
                        grayImage.draw(0, 0);
                    
                    binarizationShader.end();
                
                mask.end();
                
                /// 2値化したfboにマスキングShaderを掛け合わせる
                maskingShader.begin();
        
                    mask.draw(0, 0);
        
                maskingShader.end();
                
            projection.end();
            
            projection.draw(projectionX, projectionY,projection.getWidth()*projectionScale, projection.getHeight()*projectionScale);
    }

    // GUIの表示・非表示
    if(guiDraw){
        gui.draw();
    }
    
}

void ofApp::keyPressed(int key) {
    switch(key){
        case 'f':
            ofToggleFullscreen();
            break;
        case 'z':
            drawing = !drawing;
            break;
        case 'g':
            guiDraw = !guiDraw;
            break;
        case OF_KEY_UP:
            angle++;
            if(angle>30) angle=30;
            kinect.setCameraTiltAngle(angle);
            break;
        case OF_KEY_DOWN:
            angle--;
            if(angle<-30) angle=-30;
            kinect.setCameraTiltAngle(angle);
            break;
        case 'j':
            projectionX -= 3;
            break;
        case 'l':
            projectionX += 3;
            break;
        case 'k':
            projectionY += 3;
            break;
        case 'i':
            projectionY -=3;
            break;
        case '0':
            projectionScale += 0.01;
            break;
        case '9':
            projectionScale -= 0.01;
            break;
    }
}
