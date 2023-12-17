#include <memory.h>
#include <memorymap.h>
#include <fmt/core.h>

MemorySection::MemorySection()
  :name(),  addr(), size(), data()
{
  data = nullptr;
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
  if (data) delete[] data;
  data = nullptr;
}

void MemorySection::resize(size_t size)
{
  // data.resize(size);
  uint8_t* newData = new uint8_t[size]();
  for (int i=0; i<size; i++) newData[i] = data[i];
  if (data) delete[] data;
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

int8_t MemorySection::read8(uint32_t addr)
{
  uint32_t relativeAddr = addr - this->addr;
  return *((int8_t*)(&data[0] + relativeAddr));
}
int16_t MemorySection::read16(uint32_t addr)
{
  uint32_t relativeAddr = addr - this->addr;
  return *((int16_t*)(&data[0] + relativeAddr));
}
int32_t MemorySection::read32(uint32_t addr)
{
  uint32_t relativeAddr = addr - this->addr;
  return *((int32_t*)(&data[0] + relativeAddr));
}
int32_t MemorySection::read(uint32_t addr, uint32_t size)
{
  switch (size) {
    case 1: return read8(addr);
    case 2: return read16(addr);
    case 4: return read32(addr);
    default: return 0;
  }
}

void MemorySection::write8(uint32_t addr, int8_t value)
{
  uint32_t relativeAddr = addr - this->addr;
  *((int8_t*)(&data[0] + relativeAddr)) = value;
}
void MemorySection::write16(uint32_t addr, int16_t value)
{
  uint32_t relativeAddr = addr - this->addr;
  *((int16_t*)(&data[0] + relativeAddr)) = value;
}
void MemorySection::write32(uint32_t addr, int32_t value)
{
  uint32_t relativeAddr = addr - this->addr;
  *((int32_t*)(&data[0] + relativeAddr)) = value;
}
void MemorySection::write(uint32_t addr, uint32_t size, int32_t value)
{
  switch (size) {
    case 1: write8(addr, value); break;
    case 2: write16(addr, value); break;
    case 4: write32(addr, value); break;
    default: break;
  }
}

void MemorySection::copy(uint32_t addr, uint32_t size, uint8_t* buf)
{
  uint32_t relativeAddr = addr - this->addr;
  // data.assign(buf + relativeAddr, buf + relativeAddr + size);
  for (int i=0; i<size; i++) {
    data[i + relativeAddr] = buf[i];
  }
}

void MemorySection::set(uint32_t addr, uint32_t size, uint8_t value)
{
  uint32_t relativeAddr = addr - this->addr;
  for (int i=0; i<size; i++) {
    data[i + relativeAddr] = value;
  }
}

uint8_t* const MemorySection::buffer()
{
  // return data.data();
  return data;
}


IoRamSection::IoRamSection()
: MemorySection()
{
  runningDma = -1;
}

IoRamSection::IoRamSection(const string& name, uint32_t addr, size_t size)
: MemorySection(name, addr, size)
{
  runningDma = -1;
}
IoRamSection::IoRamSection(const MemorySection& obj)
: MemorySection(obj)
{
  runningDma = -1;
}

IoRamSection::~IoRamSection()
{}

void IoRamSection::write(uint32_t addr, uint32_t size, int32_t value)
{
  MemorySection::write(addr, size, value);
  uint32_t raddr = addr - this->addr;
  const uint32_t dmaBegin = (uint32_t)HWREG_IO_DMA0_ADDR - (uint32_t)HWREG_IORAM_BASEADDR;
  const uint32_t dmaEnd = (uint32_t)HWREG_IO_TIMER0_ADDR - (uint32_t)HWREG_IORAM_BASEADDR;
  if (dmaBegin <= raddr && raddr < dmaEnd) {
    HwIoDma& ioDma = *(HwIoDma*)(data + dmaBegin);
    runningDma = -1;
    if (ioDma.dma[0].enable) runningDma = 0;
    if (ioDma.dma[1].enable) runningDma = 1;
    if (ioDma.dma[2].enable) runningDma = 2;
    if (ioDma.dma[3].enable) runningDma = 3;
  }
}

Memory::Memory()
  : sections(),
    invalidSection(),
    busyFlag()
{
  invalidSection.name = "invalid";
  initMinimumSections();
}

Memory::~Memory()
{
  sections.clear();
}

MemorySection& Memory::section(const string& name)
{
  return *sections[name];
}
MemorySection& Memory::sectionByAddr(const uint32_t addr)
{
  return sectionByAddrSafe(addr);
  // return sectionByAddrFast(addr);
}
MemorySection& Memory::sectionByAddrSafe(const uint32_t addr)
{
  for (auto& section: sections) {
    if (section.second->isin(addr)) {
      return *section.second;
    }
  }
  return invalidSection;
}
MemorySection& Memory::sectionByAddrFast(const uint32_t addr)
{
  const char* sectionTable[16] = {
    "system", "data", "stack", "ioram", "vram", "invalid", "tile", "aram",
    "invalid", "inst", "invalid", "invalid", "invalid", "invalid", "invalid", "invalid"
  };
  uint32_t mid = addr >> 24;
  if (mid >= 0x80) {
    return *sections["program"];  // program section
  } else {
    mid = mid & 0xF;
    return *sections[sectionTable[mid]];
  }
  return invalidSection;
}

void Memory::clearSection()
{
  sections.clear();
}

void Memory::initMinimumSections()
{
  busyFlag.flag32 = 0;
  sections.clear();
  sections.insert(make_pair("program", make_shared<MemorySection>(MemorySection("program", HWREG_PROGRAM_BASEADDR    , HWREG_PROGRAM_SIZE))));
  sections.insert(make_pair("stack",   make_shared<MemorySection>(MemorySection("stack",   HWREG_WORKRAM_END - 0x0001'0000, 0x0001'0000))));
  sections.insert(make_pair("tile",    make_shared<MemorySection>(MemorySection("tile",    HWREG_TILERAM_BASEADDR    , HWREG_TILERAM_SIZE))));
  sections.insert(make_pair("vram",    make_shared<MemorySection>(MemorySection("vram",    HWREG_VRAM_BASEADDR       , HWREG_VRAM_SIZE))));
  sections.insert(make_pair("ioram",   make_shared<IoRamSection>(IoRamSection("ioram",   HWREG_IORAM_BASEADDR      , HWREG_IORAM_SIZE))));
  sections.insert(make_pair("aram",    make_shared<MemorySection>(MemorySection("aram",    HWREG_ARAM_BASEADDR       , HWREG_ARAM_SIZE))));
  sections.insert(make_pair("inst",    make_shared<MemorySection>(MemorySection("inst",    HWREG_INSTRAM_BASEADDR    , HWREG_INSTRAM_SIZE))));
  sections.insert(make_pair("system",  make_shared<MemorySection>(MemorySection("system",  HWREG_SYSROM_BASEADDR     , HWREG_SYSROM_SIZE))));
  sections.insert(make_pair("data",    make_shared<MemorySection>(MemorySection("data"  ,  HWREG_FASTWORKRAM_BASEADDR, 0x00C0'0000))));
  sections.insert(make_pair("invalid", make_shared<MemorySection>(MemorySection("invalid", 0, 0))));
}
void Memory::addSection(const string& name, uint32_t addr, uint32_t size)
{
  sections.insert(make_pair(name, make_shared<MemorySection>(MemorySection(name, addr, size))));
}

bool Memory::isBusy(uint32_t priority)
{
  uint32_t busyFlags = busyFlag.flag32;
  if (busyFlags == 0) {
    return false;
  }
  uint32_t clz = __builtin_clz(busyFlags);
  if ((32 - clz) > priority) {
    return true;
  }
  return false;
}
void Memory::setBusy(uint32_t priority)
{
  busyFlag.flag32 |= (1u << priority);
}
void Memory::clearBusy(uint32_t priority)
{
  busyFlag.flag32 &= ~(1u << priority);
}

bool Memory::waitAccess(uint32_t addr, uint32_t size, bool rw, int8_t& wait)
{
  if (isBusy(0) || wait < 0) {
    auto addrSection = addr >> 24;
    if (addrSection > (HWREG_SAVERAM_BASEADDR >> 24)) {
      wait = 5;
    } else
    if (addrSection == (HWREG_SLOWWORKRAM_BASEADDR >> 24)) {
      wait = 3;
    } else
    {
      wait = 1;
    }
  }
  if (wait > 1) {
    return true;
  }
  return false;
}

int8_t Memory::read8(uint32_t addr)
{
  auto& section = sectionByAddr(addr);
  return section.read8(addr);
}
int16_t Memory::read16(uint32_t addr)
{
  auto& section = sectionByAddr(addr);
  return section.read16(addr);
}
int32_t Memory::read32(uint32_t addr)
{
  auto& section = sectionByAddr(addr);
  return section.read32(addr);
}

void Memory::write8(uint32_t addr, int8_t value)
{
  auto& section = sectionByAddr(addr);
  section.write8(addr, value);
}
void Memory::write16(uint32_t addr, int16_t value)
{
  auto& section = sectionByAddr(addr);
  section.write16(addr, value);
}
void Memory::write32(uint32_t addr, int32_t value)
{
  auto& section = sectionByAddr(addr);
  section.write32(addr, value);
}

void Memory::copy(uint32_t addr, uint32_t size, uint8_t* value)
{
  auto& section = sectionByAddr(addr);
  section.copy(addr, size, value);
}

