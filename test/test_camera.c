/**
* This is a test for camera in $package$
*/
#include "si_test.h"
#include "camera.h"


TEST_FUNC(camera) {
  Camera * camera;
  Panner * p1;
  Panner * p2;
  int count;
  
  Point g1 = { 100, 100 };
  Point g2 = { 1000, 700 };
  
  camera = camera_new(0, 0, 640, 480);
  TEST_NOTNULL(camera);
  p1 = camera_newpanner(camera, g1, 1.0, FALSE);
  p2 = camera_newpanner(camera, g2, 1.0, FALSE);
  TEST_NOTNULL(p1);
  TEST_NOTNULL(p2);
  TEST_ASSERT(camera_panning_p(camera));
  count = -1000000;
  while(camera_panning_p(camera)) {
    camera_update(camera);
    count++;
    if ((count % 10000) == 0) {
      printf("%lf %lf\n", 
        camera_center_x(camera), 
        camera_center_y(camera)
        );
    }
    if (count >= 0) break; 
  }
  TEST_ASSERT(count < 0);
  
  camera_free(camera);
  TEST_DONE();
}


int main(void) {
  TEST_INIT();
  TEST_RUN(camera);
  TEST_REPORT();
}



