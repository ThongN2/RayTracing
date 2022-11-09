#include "ofApp.h"
#include <glm/gtx/intersect.hpp>
// Intersect Ray with Plane  (wrapper on glm::intersect*
//

// Intersect Ray with Plane  (wrapper on glm::intersect*)
//
bool Plane::intersect(const Ray& ray, glm::vec3& point, glm::vec3&
normalAtIntersect) {
float dist;
bool insidePlane = false;
bool hit = glm::intersectRayPlane(ray.p, ray.d, position, this->normal,
dist);
if (hit) {
Ray r = ray;
point = r.evalPoint(dist);
normalAtIntersect = this->normal;
glm::vec2 xrange = glm::vec2(position.x - width / 2, position.x + width
/ 2);
glm::vec2 yrange = glm::vec2(position.y - width / 2, position.y + width
/ 2);
glm::vec2 zrange = glm::vec2(position.z - height / 2, position.z +
height / 2);
// horizontal
//
if (normal == glm::vec3(0, 1, 0) || normal == glm::vec3(0, -1, 0)) {
if (point.x < xrange[1] && point.x > xrange[0] && point.z <
zrange[1] && point.z > zrange[0]) {
insidePlane = true;
}
}
// front or back
//
else if (normal == glm::vec3(0, 0, 1) || normal == glm::vec3(0, 0, -1))
{
if (point.x < xrange[1] && point.x > xrange[0] && point.y <
yrange[1] && point.y > yrange[0]) {
insidePlane = true;
}
}
// left or right
//
else if (normal == glm::vec3(1, 0, 0) || normal == glm::vec3(-1, 0, 0))
{
if (point.y < yrange[1] && point.y > yrange[0] && point.z <
zrange[1] && point.z > zrange[0]) {
insidePlane = true;
}
}
}
return insidePlane;
}
 

// Convert (u, v) to (x, y, z)
// We assume u,v is in [0, 1]
//
glm::vec3 ViewPlane::toWorld(float u, float v) {
    float w = width();
    float h = height();
    return (glm::vec3((u * w) + min.x, (v * h) + min.y, position.z));
}

// Get a ray from the current camera position to the (u, v) position on
// the ViewPlane
//
Ray RenderCam::getRay(float u, float v) {
    glm::vec3 pointOnPlane = view.toWorld(u, v);
    return(Ray(position, glm::normalize(pointOnPlane - position)));
}
// function to find specular colors for the Planes
ofColor Plane::specTexture(ofImage* specImage, glm::vec3 pt) {
    glm::vec2 uV;
    //check if it the floor plane
    if (normal == glm::vec3(0,1,0))
    {
     uV = glm::vec2(pt.x,pt.z);
    }
    else{
 uV = glm::vec2(pt.x,pt.y);
    }
    float u = ofMap(uV.x,-10,10,0,1);
    float v = ofMap(uV.y,-10,10,0,1);
    int i = round(u * specImage->getWidth() - 0.5);
    int j = round(v * specImage->getHeight() - 0.5);
    i = fmod(i,specImage->getWidth());
    j = fmod(j,specImage->getHeight());
    return specImage->getColor(i,j);
}

//function to find diffuse color for the Planes
ofColor Plane::diffuseTexture(ofImage* diffuseImage, glm::vec3 pt) {
    glm::vec2 uV;
    //check if it the floor plane
    if (normal == glm::vec3(0,1,0))
    {
     uV = glm::vec2(pt.x,pt.z);
    }
    else{
 uV = glm::vec2(pt.x,pt.y);
    }
    float u = ofMap(uV.x,-10,10,0,1);
    float v = ofMap(uV.y,-10,10,0,1);
    int i = round(u * diffuseImage->getWidth() - 0.5);
    int j = round(v * diffuseImage->getHeight() - 0.5);
    i = fmod(i,diffuseImage->getWidth());
    j = fmod(j,diffuseImage->getHeight());
    return diffuseImage->getColor(i,j);
}
//--------------------------------------------------------------

void ofApp::setup(){
    ofSetDepthTest(true);
    //0xgui setup for lightIntensity and PhongPower
    gui.setup();
    gui.add(lightIntensity.setup("Light Intensity: ",10,0,50));
    gui.add(powerExponent.setup("Phong Power: ",10,10,1000));
    //Set up for TheCam
    ofSetBackgroundColor(ofColor:: black);
    mainCam.setDistance(10);
   mainCam.setNearClip(.1);
    sideCam.setPosition(glm::vec3(11, -2, 0));
    sideCam.lookAt(glm::vec3(0, 2, 0));
    sideCam.setNearClip(1);
    previewCam.setPosition(renderCam.getPosition());
    theCam = &mainCam;
    diffuseImage1.allocate(2400, 1600, OF_IMAGE_COLOR);
    specImage1.allocate(2400, 1600, OF_IMAGE_COLOR);
    diffuseImage2.allocate(4096, 4096, OF_IMAGE_COLOR);
    specImage2.allocate(4096, 4096, OF_IMAGE_COLOR);

    diffuseImage1.load("40_clay bricks dark mortar_DIFF.jpg");
    specImage1.load("40_clay bricks dark mortar_SPEC.jpg");
    diffuseImage2.load("23_old bricks wall_DISPL.jpg");
    specImage2.load("23_old bricks wall_SPEC.jpg");
    
    Light * light1 = new Light(glm::vec3(-2,4,1),lightIntensity,ofColor::white);
   Light * light2 = new Light(glm::vec3(-1,3,6),lightIntensity,ofColor::white);
    Light * light3 = new Light(glm::vec3(-1,6,-3),lightIntensity,ofColor::white);
    //Create a plane and 3 spheres objects
    Plane  * pl1 = new Plane (glm::vec3(0,-2,0),glm::vec3(0,1,0),ofColor:: gray);
    Plane  * pl2 = new Plane (glm::vec3(0,8,-10),glm::vec3(0,0,1),ofColor:: gray);
    scene.push_back(pl1);
 scene.push_back(new Plane (glm::vec3(0,8,-10),glm::vec3(0,0,1),ofColor:: gray));
    scene.push_back(new Sphere (glm::vec3(-1,1,1),1.5,ofColor:: green));
   scene.push_back(new Sphere (glm::vec3(1,0,-3),2,ofColor:: blue));
    scene.push_back(new Sphere (glm::vec3(-1,0,4),1,ofColor:: red));
    //scene.push_back(light1);
    //scene.push_back(light2);
  //  scene.push_back(light3);

    //add 3 light to light vector
 
    planes.push_back(pl1);
    planes.push_back(pl2);
    light.push_back(light1);
   light.push_back(light2);
    light.push_back(light3);
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
    ofDisableDepthTest();
    gui.draw();
    ofEnableDepthTest();
  
    theCam->begin();
    //Iterate through scene to every object that were pushed back in scene
    for(int i = 0 ; i < scene.size(); i++)
    {
        //set color for each object
        ofSetColor(scene[i] -> diffuseColor);
        scene[i]-> draw();
    }
        
 
    for(int k = 0 ; k < light.size(); k++)
    {
        //set color for each object
        ofSetColor(light[k] -> diffuseColor);
        light[k]-> draw();
    }
   
      
    //create renderCam
    ofSetColor(ofColor:: white);
    renderCam.draw();
    theCam->end();
    //load and draw
   // image.load("RayTracing.jpg");
   // image.draw(0,0);
}

void ofApp::rayTrace()
{
    //Allocate height and width ,and color for the image
    image.allocate(imageWidth,imageHeight, OF_IMAGE_COLOR);
    //iterate through the height and width of the image
    for (float row = 0; row < imageWidth; row++)
{
    for(float col = 0; col < imageHeight; col++)
    {
        float u = (row + 0.5f)/imageWidth;
        float v =(imageHeight - 1 - col + 0.5f )/ imageHeight;
        //generate the Ray from the image postion
        Ray ray = renderCam.getRay(u,v);
        float closetObject = std::numeric_limits<float>::infinity();

        ofColor color = ofColor:: black;
        glm::vec3 pt ,normal;
        for(int i = 0; i < scene.size(); i++)
        {
    
            //Check if the ray intersect with any object in the scene
            if (scene[i]-> intersect(ray,pt,normal))
            {
           
                //add depthSorting
                // determine if we hit the object and save closest obj
                // record closest distance
                float distance = glm::length(pt-renderCam.position);
              if( distance < closetObject)
              {
                  ofColor ambientLight;
                  closetObject = distance;
                  // check if the object is the floor plane
                  //return ambient of the  floor texture if it is a floor plane
                  //return ambient of the  wallpaper texture if it is a vertical plane
                  if(scene[i]->position == planes[0]->position)
                  {
                     ambientLight = ambient(scene[i]->diffuseTexture(&diffuseImage1, pt),0.5);
                  }
                  else
                  {
                   ambientLight = ambient(scene[i]->diffuseTexture(&diffuseImage2, pt),0.5);
                      
                  }
                  //set diffuseColor and specularColor to black
                  ofColor diffColor = ofColor:: black;
                  ofColor  specularColor = ofColor:: black;
                  //calculate the view vector
                  glm::vec3 v = glm::normalize(renderCam.position - pt);
                  //Iterate throught light vector
                  //getting light position and light intensity on each light
                  for(int k = 0; k < light.size(); k++)
                  {
                      
                      // calculating thte light vector
                      glm::vec3 lightVector = light[k]->position - pt;
                      glm::vec3 normalizedLight = glm::normalize(lightVector);
                      //calculate bisecttor from light and view vector
                      glm::vec3 bisector = glm::normalize(normalizedLight + v);
                      float r = glm::length(lightVector);
                      //calculate illumination for lambert and phong functions
                      float illumination = (lightIntensity / pow(r,2));
                      Ray lightRay = Ray(pt+0.001*normal,normalizedLight);
                      //Iterate through ligh rays
                      //Illuminated the surface with nothing blocks its view of the light
                      if(!Shadow(lightRay))
                         {
                             if(scene[i]->position == planes[0]->position)
                             {
                                 diffColor += lambert(normalizedLight,glm::normalize(normal),illumination,scene[i]->diffuseTexture(&diffuseImage1, pt));
                                 specularColor += phong(bisector,glm::normalize(normal),illumination,scene[i]->diffuseTexture(&diffuseImage1, pt),scene[i]->specTexture(&specImage1,pt),powerExponent);
                             }
                             else{
                                 diffColor += lambert(normalizedLight,glm::normalize(normal),illumination,scene[i]->diffuseTexture(&diffuseImage2, pt));
                                 specularColor += phong(bisector,glm::normalize(normal),illumination,scene[i]->diffuseTexture(&diffuseImage2, pt),scene[i]->specTexture(&specImage2,pt),powerExponent);
                             }
                                
                      }
                      
                  }
                  image.setColor(row,col,ambientLight+diffColor+specularColor);
                    //Set the color at the image position by the color of the object that the ray intersects with
              }
    
                }
    
            }

           
        }
    

    
}
    image.update();
    image.save("RayTracing.jpg",OF_IMAGE_QUALITY_HIGH);
}
//Shadow function
//interate through the light vector
//check for intersection between LightRay and objects
bool ofApp::Shadow(Ray &ray)
{
    glm::vec3 pt, normal;
    for(int i = 0; i< scene.size();i++)
    {
        if(scene[i]->intersect(ray,pt,normal))
        {
            return true;
        }
    }
    return false;
}
//Adding lightIntensity parmeter in lambert
//This LightIntensity will help to calculate the illuminationfrom source
ofColor ofApp::lambert(const glm::vec3 &p, const glm::vec3 &norm,const float lightIntensity,const ofColor diffuse)
 {
    float max = glm::max(0.0f,glm::dot(norm,p));
    return lightIntensity * diffuse * max;
    /* ofColor diffuseColor ;
     for(int i = 0; i < light.size(); i++)
     {
         glm::vec3 l = -1*glm::normalize(light[i]->position-p);
         glm::vec3 light_vector = light[i]->position - p;
         float radius = glm::length(glm::normalize(light_vector));
         float illumination = (light[i]->intensity /( glm::pow2(radius)));
         float maxx = max(0.0f,glm::dot(norm,glm::normalize(light_vector)));
         diffuseColor += diffuse * illumination * maxx ;
     }

     return diffuseColor; */
 }

//Similiar to lambert fucntion
//Adding lightIntensity that will help  calculate the illuminationfrom source
ofColor ofApp::phong(const glm::vec3 &p, const glm::vec3 &norm,const float lightIntensity,
                     const ofColor diffuse, const ofColor specular, float power)
{
    
    float max = glm::max(0.0f,glm::dot(norm,p));

    return lightIntensity * specular * glm::pow(max,power);
}


ofColor ofApp::ambient(const ofColor diffuse, float percentage)
{
    return diffuse * percentage;
}

//--------------------------------------------------------------
//Set up different key pressed options for different view and the RayTrace method
void ofApp::keyPressed(int key){
    switch (key) {
        case 'a':
        theCam = &mainCam;
        break;
    case 'b' :
        theCam = &sideCam;
        break;
    case 'c' :
            theCam = &previewCam;
            break;
  case 'd' :
            rayTrace();
                break;
    default:
        break;
    }

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){

}
                    
