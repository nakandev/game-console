#pragma once

#include <gui/component.h>
class Board;

class InputConfigDialog : GuiComponent {
  private:
    Board* board;
    int phase;
  public:
    int padDeviceId;
    vector<int> padKeyAsignments;
    InputConfigDialog();
    ~InputConfigDialog();
    void update();
    void selectPadDevice();
    void setupPadKey();
    void setupPadKeyAll();
};

