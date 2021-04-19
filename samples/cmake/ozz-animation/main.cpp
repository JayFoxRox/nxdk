#if defined(NXDK)
#include <hal/debug.h>
#include <hal/video.h>
#include <windows.h>
#endif

#include <SDL.h>

#include "ozz/animation/runtime/animation.h"
#include "ozz/animation/runtime/local_to_model_job.h"
#include "ozz/animation/runtime/sampling_job.h"
#include "ozz/animation/runtime/skeleton.h"

#include "ozz/base/log.h"

#include "ozz/base/maths/simd_math.h"
#include "ozz/base/maths/soa_transform.h"
#include "ozz/base/maths/vec_float.h"

#include "ozz/base/containers/vector.h"

#include "ozz/base/io/archive.h"

// Runtime skeleton.
ozz::animation::Skeleton skeleton_;

// Runtime animation.
ozz::animation::Animation animation_;

// Sampling cache.
ozz::animation::SamplingCache cache_;

// Buffer of local transforms as sampled from animation_.
ozz::vector<ozz::math::SoaTransform> locals_;

// Buffer of model space matrices.
ozz::vector<ozz::math::Float4x4> models_;

const char* skeleton = "D:\\media\\skeleton.ozz";
const char* animation = "D:\\media\\animation.ozz";


static bool LoadSkeleton(const char* _filename, ozz::animation::Skeleton* _skeleton) {
  assert(_filename && _skeleton);
  ozz::log::Out() << "Loading skeleton archive " << _filename << "."
                  << std::endl;
  ozz::io::File file(_filename, "rb");
  if (!file.opened()) {
    ozz::log::Err() << "Failed to open skeleton file " << _filename << "."
                    << std::endl;
    return false;
  }
  ozz::io::IArchive archive(&file);
  if (!archive.TestTag<ozz::animation::Skeleton>()) {
    ozz::log::Err() << "Failed to load skeleton instance from file "
                    << _filename << "." << std::endl;
    return false;
  }

  // Once the tag is validated, reading cannot fail.
  archive >> *_skeleton;

  return true;
}

static bool LoadAnimation(const char* _filename,
                   ozz::animation::Animation* _animation) {
  assert(_filename && _animation);
  ozz::log::Out() << "Loading animation archive: " << _filename << "."
                  << std::endl;
  ozz::io::File file(_filename, "rb");
  if (!file.opened()) {
    ozz::log::Err() << "Failed to open animation file " << _filename << "."
                    << std::endl;
    return false;
  }
  ozz::io::IArchive archive(&file);
  if (!archive.TestTag<ozz::animation::Animation>()) {
    ozz::log::Err() << "Failed to load animation instance from file "
                    << _filename << "." << std::endl;
    return false;
  }

  // Once the tag is validated, reading cannot fail.
  archive >> *_animation;

  return true;
}

static bool OnInitialize() {
  // Reading skeleton.
  if (!LoadSkeleton(skeleton, &skeleton_)) {
    debugPrint("fail sk\n");
    return false;
  }

  // Reading animation.
  if (!LoadAnimation(animation, &animation_)) {
    debugPrint("fail an\n");
    return false;
  }

  // Allocates runtime buffers.
  const int num_soa_joints = skeleton_.num_soa_joints();
  locals_.resize(num_soa_joints);
  const int num_joints = skeleton_.num_joints();
  models_.resize(num_joints);

  // Allocates a cache that matches animation requirements.
  cache_.Resize(num_joints);

  return true;
}

// Test if a joint is a leaf. _joint number must be in range [0, num joints].
// "_joint" is a leaf if it's the last joint, or next joint's parent isn't
// "_joint".
inline bool IsLeaf(const ozz::animation::Skeleton& _skeleton, int _joint) {
  const int num_joints = _skeleton.num_joints();
  assert(_joint >= 0 && _joint < num_joints && "_joint index out of range");
  const ozz::span<const int16_t>& parents = _skeleton.joint_parents();
  const int next = _joint + 1;
  return next == num_joints || parents[next] != _joint;
}


int main() {
#if defined(NXDK)
  XVideoSetMode(640, 480, 32, REFRESH_DEFAULT);
#endif

  SDL_Init(SDL_INIT_VIDEO);
  SDL_Window* window = NULL;
  SDL_Renderer* renderer = NULL;

#if defined(NXDK)
  debugPrint("Starting\n");
#endif
  SDL_CreateWindowAndRenderer(640, 480, 0, &window, &renderer);
#if defined(NXDK)
  debugPrint("Got %p / %p\n", window, renderer);
#endif

  bool ok = OnInitialize();

#if defined(NXDK)
  debugPrint("Mainloop: %d\n", ok);
#endif
  bool done = false;  
  while (!done) {
    SDL_Event event;

    // Get animation time.
    Uint32 duration_ticks = (Uint32)(animation_.duration() * 1000.0f);
    Uint32 playback_ticks = SDL_GetTicks() % duration_ticks;
    printf("%d / %d\n", playback_ticks, duration_ticks);

    // Samples optimized animation.
    ozz::animation::SamplingJob sampling_job;
    sampling_job.animation = &animation_;
    sampling_job.cache = &cache_;
    sampling_job.ratio = (float)playback_ticks / (float)duration_ticks;
    sampling_job.output = make_span(locals_);
    if (!sampling_job.Run()) {
      return false;
    }

    // Get number of joints.
    const int num_joints = skeleton_.num_joints();
    if (!num_joints) {
      return true;
    }

    // Compute model space bind pose.
    ozz::animation::LocalToModelJob job;
    job.input = sampling_job.output;
    job.output = ozz::make_span(models_);
    job.skeleton = &skeleton_;
    if (!job.Run()) {
      return false;
    }

    const ozz::span<ozz::math::Float4x4>& _matrices = job.output;

    // Get access to parents.
    const ozz::span<const int16_t>& parents = skeleton_.joint_parents();

    // Clear image.
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    // Draw each bone.
    int instances = 0;
    for (int i = 0; i < num_joints; ++i) {
      // Root isn't rendered.
      const int16_t parent_id = parents[i];
      if (parent_id == ozz::animation::Skeleton::kNoParent) {
        continue;
      }

      // Selects joint matrices.
      const ozz::math::Float4x4& parent = _matrices[parent_id];
      const ozz::math::Float4x4& current = _matrices[i];

      // Get position of the joints
      float s = 200.0f;
      int ox = 320;
      int oy = 420;
      int ax = parent.cols[3].z * s + ox;
      int ay = -parent.cols[3].y * s + oy;
      int bx = current.cols[3].z * s + ox;
      int by = -current.cols[3].y * s + oy;

      // Draw a line between joints to represent bone
      if (IsLeaf(skeleton_, i)) {
        SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, SDL_ALPHA_OPAQUE);
      } else {
        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, SDL_ALPHA_OPAQUE);
      }
      SDL_RenderDrawLine(renderer, ax, ay, bx, by);
    }
   
    SDL_RenderPresent(renderer);

    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        done = true;
      }
    }

  }
}
