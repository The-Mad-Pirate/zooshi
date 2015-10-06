// Copyright 2015 Google Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef ZOOSHI_GAME_H
#define ZOOSHI_GAME_H

#include "SDL_thread.h"
#include "breadboard/event_system.h"
#include "breadboard/graph.h"
#include "camera.h"
#include "config_generated.h"
#include "entity/entity_manager.h"
#include "flatbuffers/flatbuffers.h"
#include "flatui/font_manager.h"
#include "fplbase/asset_manager.h"
#include "fplbase/input.h"
#include "fplbase/renderer.h"
#include "fplbase/utilities.h"
#include "full_screen_fader.h"
#include "mathfu/glsl_mappings.h"
#include "pindrop/pindrop.h"
#include "rail_def_generated.h"
#include "states/intro_state.h"
#include "states/loading_state.h"
#include "states/pause_state.h"
#include "states/state_machine.h"
#include "states/states.h"
#include "states/scene_lab_state.h"
#include "world.h"
#include "zooshi_graph_factory.h"

#if defined(PLATFORM_MOBILE)
#define USING_GOOGLE_PLAY_GAMES
#endif

#define DISPLAY_FRAMERATE_HISTOGRAM 0

// Header files that has a dependency to GPG definitions.
#include "gpg_manager.h"
#include "states/gameplay_state.h"
#include "states/game_menu_state.h"
#include "states/game_over_state.h"

#ifdef __ANDROID__
#include "inputcontrollers/android_cardboard_controller.h"
#else
#include "inputcontrollers/mouse_controller.h"
#endif
#ifdef ANDROID_GAMEPAD
#include "inputcontrollers/gamepad_controller.h"
#endif

namespace fpl {
namespace zooshi {

const auto kGPGDefaultLeaderboard = "LeaderboardMain";

struct Config;
struct InputConfig;
struct AssetManifest;

// Mutexes/CVs used in synchronizing the render and update threads
struct GameSynchronization {
  SDL_mutex* renderthread_mutex_;
  SDL_mutex* updatethread_mutex_;
  SDL_mutex* gameupdate_mutex_;
  SDL_cond* start_render_cv_;
  SDL_cond* start_update_cv_;
  GameSynchronization();
};

class Game {
 public:
  Game();
  bool Initialize(const char* const binary_directory);
  void Run();

 private:
  bool InitializeRenderer();
  bool InitializeAssets();
  void InitializeEventSystem();

  Mesh* CreateVerticalQuadMesh(const char* material_name,
                               const mathfu::vec3& offset,
                               const mathfu::vec2& pixel_bounds,
                               float pixel_to_world_scale);
  void Update(WorldTime delta_time);
  void UpdateMainCamera();
  void UpdateMainCameraAndroid();
  void UpdateMainCameraMouse();
  const Config& GetConfig() const;
  const InputConfig& GetInputConfig() const;
  const RailDef& GetRailDef() const;
  const AssetManifest& GetAssetManifest() const;
  Mesh* GetCardboardFront(int renderable_id);

  void SetRelativeMouseMode(bool relative_mouse_mode);
  void ToggleRelativeMouseMode();

  void UpdateProfiling(WorldTime frame_time);

  // Mutexes/CVs used in synchronizing the render and update threads:
  GameSynchronization sync_;

  // Hold configuration binary data.
  std::string config_source_;

  // Hold the configuration for the input system data.
  std::string input_config_source_;

  // Hold the configuration for the asset manifest source.
  std::string asset_manifest_source_;

  // The top level state machine that drives the game.
  StateMachine<kGameStateCount> state_machine_;
  LoadingState loading_state_;
  PauseState pause_state_;
  GameplayState gameplay_state_;
  GameMenuState game_menu_state_;
  IntroState intro_state_;
  GameOverState game_over_state_;
  SceneLabState scene_lab_state_;

  // Report touches, button presses, keyboard presses.
  InputSystem input_;

  // Hold rendering context.
  Renderer renderer_;

  // Load and own rendering resources.
  AssetManager asset_manager_;

  FontManager font_manager_;

  // Manage ownership and playing of audio assets.
  pindrop::AudioEngine audio_engine_;

  // The event system.
  breadboard::EventSystem event_system_;
  ZooshiGraphFactory graph_factory_;

  // Shaders we use.
  Shader* shader_lit_textured_normal_;
  Shader* shader_textured_;

#if DISPLAY_FRAMERATE_HISTOGRAM
  // Profiling data.
  static const int kHistogramSize = 64;
  WorldTime last_printout;
  WorldTime histogram[kHistogramSize];
#endif

  bool game_exiting_;

  std::string rail_source_;

  pindrop::AudioConfig* audio_config_;

  World world_;
  WorldRenderer world_renderer_;

  // Fade the screen to back and from black.
  FullScreenFader fader_;

  std::unique_ptr<scene_lab::SceneLab> scene_lab_;

  bool relative_mouse_mode_;

  // String version number of the game.
  const char* version_;

  // Google Play Game Services Manager.
  GPGManager gpg_manager_;
};

}  // zooshi
}  // fpl

#endif  // ZOOSHI_GAME_H
