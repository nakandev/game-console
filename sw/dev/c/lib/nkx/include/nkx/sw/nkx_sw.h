#include <cstddef>

extern "C" void int_handler() __attribute__((weak));
extern "C" void waitForVSync() __attribute__((weak));

namespace nkx {

/* Common */

// GameEvent
class GameEvent {
};

class AnimationFrame {
  // int frame;
  //
public:
  //
};
class AnimationCase {
  // int fps;
  // int startFrame;
  // int endFrame;
  // nkx::vector<AnimationFrame> frames;
public:
  //
};

// GameObject
class GameObject {
  // Sprite* sprite;
  // Collision* collistion;
  // Position* position;
  // EventHandler* eventHandlers;
public:
  // connectEvent(event, handler);
  // spawn();
  // update();
  // draw();
  // destroy();
};

class GameScene {
public:
  GameScene();
  virtual ~GameScene() = default;
  virtual auto init() -> void;
  virtual auto update() -> void;
  virtual auto draw() -> void;
  virtual auto fin() -> void;
  virtual auto getNextScene() -> GameScene*;
};

auto mainSceneLoop(GameScene* scene) -> void;

/* Video */
class GraphicObject {
};

class TileMap {
  // Tile* tile;
  // Tilemap* tilemap;
public:
  // spawn();
  // update();
  // draw();
  // destroy();
};

class Video {
public:
  Video();
  ~Video();
  void clearSprites();
  void addSprite();
  void removeSprite();
  void draw();
};

/* Audio */
class SoundObject {
};

/* Input */
union Pad {
  struct {
    struct {
      uint16_t A : 1;
      uint16_t B : 1;
      uint16_t C : 1;
      uint16_t D : 1;
      uint16_t L : 1;
      uint16_t R : 1;
      uint16_t S : 1;
      uint16_t T : 1;
      uint16_t _reserved0 : 1;
      uint16_t _reserved1 : 1;
      uint16_t _reserved2 : 1;
      uint16_t _reserved3 : 1;
      uint16_t UP : 1;
      uint16_t DOWN : 1;
      uint16_t LEFT : 1;
      uint16_t RIGHT : 1;
    };
    uint8_t AnalogX;
    uint8_t AnalogY;
  };
  uint32_t val32;
  auto set(HwPad& obj) -> void;
};
extern Pad pad[1];

class Input {
  Pad _pad[0];
public:
  Input();
  ~Input();
  auto pad(uint8_t no) -> Pad&;
};

}; /* namespace nkx */

