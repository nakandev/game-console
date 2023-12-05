#pragma once

class GuiComponent {
  public:
    int x;
    int y;
    int w;
    int h;
    GuiComponent() {};
    virtual ~GuiComponent() {};
};
