#include <debugger.h>
#include <cpu.h>

Debugger::Debugger()
  : cpu(nullptr)
{
}

Debugger::~Debugger()
{
  cpu = nullptr;
}

void
Debugger::setCpu(Cpu& cpu)
{
  this->cpu = &cpu;
}
void
Debugger::disassemble_all()
{
  if (!cpu) return;
  cpu->disassembleAll();
}
