#include <memory.h>
#include <memorymap.h>
#include <fmt/core.h>

MemorySection::MemorySection()
  :name(),  addr(), size(), data()
{
}

MemorySection::MemorySection(const string& name, uint32_t addr, size_t size)
  :name(name),  addr(addr), size(size), data()
{
  // data.resize(size);
  data = new uint8_t[size]();
}

MemorySection::MemorySection(const MemorySection& obj)
  :name(obj.name),  addr(obj.addr), size(obj.size), data()
{
  data = new uint8_t[size]();
  for (int i=0; i<size; i++) data[i] = obj.data[i];
}

MemorySection::~MemorySection()
{
  addr = 0;
  size = 0;
  name = "";
  // data.clear();
  delete[] data;
  data = nullptr;
}

void MemorySection::resize(size_t size)
{
  // data.resize(size);
  uint8_t* newData = new uint8_t[size]();
  for (int i=0; i<size; i++) newData[i] = data[i];
  delete[] data;
  data = newData;
  this->size = size;
}

bool MemorySection::isin(uint32_t addr)
{
  if (this->addr <= addr && addr < this->addr + size) {
    return true;
  }
  return false;
}

int32_t MemorySection::read(uint32_t addr, int size)
{
  uint32_t relativeAddr = addr - this->addr;
  // uint8_t* bytes = data.data();
  if (size == 1)
    return *((int8_t*)(&data[0] + relativeAddr));
  else if (size == 2)
    return *((int16_t*)(&data[0] + relativeAddr));
  else if (size == 4)
    return *((int32_t*)(&data[0] + relativeAddr));

  return 0;
}

void MemorySection::write(uint32_t addr, int size, int32_t value)
{
  uint32_t relativeAddr = addr - this->addr;
  // uint8_t* bytes = data.data();
  if (size == 1)
    *((int8_t*)(&data[0] + relativeAddr)) = value & 0xff;
  else if (size == 2)
    *((int16_t*)(&data[0] + relativeAddr)) = value & 0xffff;
  else if (size == 4)
    *((int32_t*)(&data[0] + relativeAddr)) = value;
}

void MemorySection::copy(uint32_t addr, int size, uint8_t* buf)
{
  uint32_t relativeAddr = addr - this->addr;
  // data.assign(buf + relativeAddr, buf + relativeAddr + size);
  for (int i=0; i<size; i++) {
    data[i + relativeAddr] = buf[i];
  }
}

const uint8_t* MemorySection::buffer()
{
  // return data.data();
  return data;
}


Memory::Memory()
  :sections()
{
  initMinimumSections();
}

Memory::~Memory()
{
  sections.clear();
}

MemorySection& Memory::section(const string& name)
{
  return sections[name];
}
void Memory::clearSection()
{
  sections.clear();
}
void Memory::initMinimumSections()
{
  sections.clear();
  sections.insert(make_pair("program", MemorySection("program", HWREG_PROGRAM_BASEADDR    , 0x0100'0000)));
  sections.insert(make_pair("stack",   MemorySection("stack",   HWREG_WORKRAM_END - 0x0040'0000, 0x0040'0000)));
  sections.insert(make_pair("tile",    MemorySection("tile",    HWREG_TILERAM_BASEADDR    , 0x0100'0000)));
  sections.insert(make_pair("vram",    MemorySection("vram",    HWREG_VRAM_BASEADDR       , 0x0100'0000)));
  sections.insert(make_pair("ioram",   MemorySection("ioram",   HWREG_IORAM_BASEADDR      , 0x0001'0000)));
  sections.insert(make_pair("aram",    MemorySection("aram",    HWREG_ARAM_BASEADDR       , 0x0100'0000)));
  sections.insert(make_pair("system",  MemorySection("system",  HWREG_SYSROM_BASEADDR     , 0x0001'0000)));
  sections.insert(make_pair("data",    MemorySection("data"  ,  HWREG_FASTWORKRAM_BASEADDR, 0x00C0'0000)));
}
void Memory::addSection(const string& name, uint32_t addr, uint32_t size)
{
  sections.insert(make_pair(name, MemorySection(name, addr, size)));
}

int32_t Memory::read(uint32_t addr, int size)
{
  for (auto& section: sections) {
    if (section.second.isin(addr)) {
      return section.second.read(addr, size);
    }
  }
  return 0;
}

void Memory::write(uint32_t addr, int size, int32_t value)
{
  for (auto& section: sections) {
    if (section.second.isin(addr)) {
      section.second.write(addr, size, value);
      return;
    }
  }
  // fmt::print("w {:08x}/n", addr);
  // switch (addr >> 20) {
  //   case 0x00:
  //     sections["system"].write(addr, size, value); break;
  //   case 0x10:
  //     sections["data"].write(addr, size, value); break;
  //   case 0x1C:
  //     sections["stack"].write(addr, size, value); break;
  //   case 0x30:
  //     sections["ioram"].write(addr, size, value); break;
  //   case 0x40:
  //     sections["vram"].write(addr, size, value); break;
  //   case 0x60:
  //     sections["tile"].write(addr, size, value); break;
  //   case 0x80:
  //     sections["program"].write(addr, size, value); break;
  //   default:
  //     break;
  // }
  return;
}

void Memory::copy(uint32_t addr, int size, uint8_t* value)
{
  for (auto& section: sections) {
    if (section.second.isin(addr)) {
      section.second.copy(addr, size, value);
      return;
    }
  }
}

