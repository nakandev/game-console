#include <nkx/hw/nkx_hw.h>
#include <nkx/sw/nkx_sw.h>
#include <nkx/std/stdlib.h>

/* -------- */
class TitleScene : public nkx::GameScene {
  bool flag;
public:
  TitleScene();
  ~TitleScene();
  auto init() -> void override;
  auto update() -> void override;
  auto draw() -> void override;
  auto fin() -> void override;
  auto getNextScene() -> nkx::GameScene* override;
};

class SettingScene : public nkx::GameScene {
  bool flag;
public:
  SettingScene();
  ~SettingScene();
  auto init() -> void override;
  auto update() -> void override;
  auto draw() -> void override;
  auto fin() -> void override;
  auto getNextScene() -> nkx::GameScene* override;
};

TitleScene::TitleScene()
: flag(false)
{
}
TitleScene::~TitleScene()
{
}
auto TitleScene::init() -> void
{
  HwTileRam& tileram = *(HwTileRam*)HWREG_TILERAM_BASEADDR;
  tileram.palette[0].color[0] = {.a=0xff, .b=0x00, .g=0x00, .r=0x7f};
  tileram.bg[0].flag.enable = true;
}
auto TitleScene::update() -> void
{
  waitForVSync();
  if (nkx::pad[0].A || nkx::pad[0].T) {
    flag = true;
  }
}
auto TitleScene::draw() -> void
{
}
auto TitleScene::fin() -> void
{
}
auto TitleScene::getNextScene() -> nkx::GameScene*
{
  if (flag) {
    return new SettingScene();
  }
  return nullptr;
}

/* -------- */
SettingScene::SettingScene()
: flag(false)
{
}
SettingScene::~SettingScene()
{
}
auto SettingScene::init() -> void
{
  HwTileRam& tileram = *(HwTileRam*)HWREG_TILERAM_BASEADDR;
  tileram.palette[0].color[0] = {.a=0xff, .b=0x00, .g=0x7f, .r=0x00};
  tileram.bg[0].flag.enable = true;
}
auto SettingScene::update() -> void
{
  waitForVSync();
  if (nkx::pad[0].B) {
    flag = true;
  }
}
auto SettingScene::draw() -> void
{
}
auto SettingScene::fin() -> void
{
}
auto SettingScene::getNextScene() -> nkx::GameScene*
{
  if (flag) {
    return new TitleScene();
  }
  return nullptr;
}

/* -------- */
int main()
{
  HwIoRam& ioram = *(HwIoRam*)HWREG_IORAM_BASEADDR;
  // ioram.intr.enable.hblank = true;
  ioram.intr.enable.vblank = true;

  nkx::GameScene* firstScene = new TitleScene();
  nkx::mainSceneLoop(firstScene);
}
