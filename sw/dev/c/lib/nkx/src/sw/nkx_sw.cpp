#include <cstddef>
#include <stdint.h>
#include <memorymap.h>
#include <nkx/sw/nkx_sw.h>

static bool isVsyncIntr = false;

extern "C" void int_handler()
{
  // [TODO] Should the status flag be accessed each time it is checked?
  volatile HwIoInterruptBits& intstatus = (*(volatile HwIoInterruptBits*)HWREG_IO_INT_STATUS_ADDR);

  if (intstatus.hblank) {
    intstatus.hblank = 0;
  }
  if (intstatus.vblank) {
    intstatus.vblank = 0;
    isVsyncIntr = true;
    nkx::pad[0].set((*(HwPad*)HWREG_IO_PAD0_ADDR));
  }
}

extern "C" void waitForVSync()
{
  while (!isVsyncIntr) {
    asm("nop");
    asm("wfi");
    asm("nop");
  }
  isVsyncIntr = false;
}

namespace nkx {

GameScene::GameScene()
{
}
auto GameScene::init() -> void
{
}
auto GameScene::update() -> void
{
}
auto GameScene::draw() -> void
{
}
auto GameScene::fin() -> void
{
}
auto GameScene::getNextScene() -> GameScene*
{
  return nullptr;
}

auto mainSceneLoop(GameScene* scene) -> void
{
  while(true) {
    scene->init();
    auto* nextScene = scene->getNextScene();
    while (!nextScene) {
      scene->update();
      scene->draw();
      nextScene = scene->getNextScene();
    }
    scene->fin();
    delete scene;
    scene = nextScene;
  };
}

}; /* namespace nkx */
