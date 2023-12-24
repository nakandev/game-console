#pragma once

class Processor {
public:
  Processor(){}
  virtual ~Processor(){}
  virtual auto stepCycle() -> void {}
};
