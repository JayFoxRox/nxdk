#include <hal/debug.h>
#include <hal/video.h>
#include <windows.h>
#include "Newton.h"

extern "C" {
_PDCLIB_PUBLIC double atof( const char * nptr ) {
  return 0.0f;
}
}

static void cb_applyForce(const NewtonBody* const body, dFloat timestep, int threadIndex) {
  // Apply a force to the object.
  dFloat force[3] = {0, 1.0, 0};
  NewtonBodySetForce(body, force);

  // Query the state (4x4 matrix) and extract the body's position.
  float state[16];
  NewtonBodyGetMatrix(body, state);
  debugPrint("Time %dms: x=%d  y=%d  z=%d\n",
             (int)(timestep*1000.0f), (int)(state[12]*1000.0f), (int)(state[13]*1000.0f), (int)(state[14]*1000.0f));
}


static void addSphereToSimulation(NewtonWorld *world) {
  // fixme: what is this?
  float	foo[16] = {
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
  };

  // Create the sphere. Is this radius or diameter?
  NewtonCollision* const collision = NewtonCreateSphere(world, 1, 0, NULL);

  // fixme: what is this for?
  NewtonBody* const body = NewtonCreateDynamicBody(world, collision, foo);

  // fixme: has no doc string. Is this mass, I_xx, I_yy, and I_zz?
  NewtonBodySetMassMatrix(body, 1.0f, 1, 1, 1);

  // Install callback. Newton will call it whenever the object moves.
  NewtonBodySetForceAndTorqueCallback(body, cb_applyForce);
}

int main() {
  XVideoSetMode(640, 480, 32, REFRESH_DEFAULT);

  // Print the library version.
  debugPrint("Hello, this is Newton version %d\n", NewtonWorldGetVersion());

  // Create the Newton world.
  NewtonWorld* const world = NewtonCreate();

  // Add the sphere.
  addSphereToSimulation(world);

  // Step the (empty) world 60 times in increments of 1/60 second.
  const float timestep = 1.0f / 60;
  for(int i=0; i<60; i++) {
    NewtonUpdate(world, timestep);
    Sleep(500);
  }

  // Clean up.
  NewtonDestroyAllBodies(world);
  NewtonDestroy(world);

  return 0;
}
