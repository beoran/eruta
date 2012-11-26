/**
* This is a test for camera in $package$
*/
#include "si_test.h"
#include "camera.h"


TEST_FUNC(camera) {
  Camera * camera;
  Panner * p1;
  Panner * p2;
  
  Point g1 = { 100, 200 };
  Point g2 = { 300, 600 };
  
  camera = camera_new(0, 0, 640, 480);
  TEST_NOTNULL(camera);
  p1 = camera_newpanner(camera, g1, 1.0, FALSE);
  p2 = camera_newpanner(camera, g2, 1.0, FALSE);
  TEST_NOTNULL(p1);
  TEST_NOTNULL(p2);
  
  camera_freepanners(camera);
  
  camera_free(camera);
  TEST_DONE();
}


int main(void) {
  TEST_INIT();
  TEST_RUN(camera);
  TEST_REPORT();
}



