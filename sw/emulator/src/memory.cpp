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
  data.resize(size);
}

MemorySection::MemorySection(const MemorySection& obj)
  :name(obj.name),  addr(obj.addr), size(obj.size), data()
{
  data.resize(size);
  for (int i=0; i<size; i++) data[i] = obj.data[i];
}

MemorySection::~MemorySection()
{
  addr = 0;
  size = 0;
  name = "";
  data.clear();
}

auto MemorySection::resize(size_t size) -> void
{
  data.resize(size);
  this->size = data.size();
}

auto MemorySection::isin(uint32_t addr) -> bool
{
  if (this->addr <= addr && addr < this->addr + size) {
    return true;
  }
  return false;
}

auto MemorySection::read8(uint32_t addr) -> int8_t
{
  uint32_t relativeAddr = addr - this->addr;
  return *((int8_t*)(&data[0] + relativeAddr));
}

auto MemorySection::read16(uint32_t addr) -> int16_t
{
  uint32_t relativeAddr = addr - this->addr;
  return *((int16_t*)(&data[0] + relativeAddr));
}

auto MemorySection::read32(uint32_t addr) -> int32_t
{
  uint32_t relativeAddr = addr - this->addr;
  return *((int32_t*)(&data[0] + relativeAddr));
}

auto MemorySection::read(uint32_t addr, uint32_t size) -> int32_t
{
  switch (size) {
    case 1: return read8(addr);
    case 2: return read16(addr);
    case 4: return read32(addr);
    default: return 0;
  }
}

auto MemorySection::write8(uint32_t addr, int8_t value) -> void
{
  uint32_t relativeAddr = addr - this->addr;
  *((int8_t*)(&data[0] + relativeAddr)) = value;
}

auto MemorySection::write16(uint32_t addr, int16_t value) -> void
{
  uint32_t relativeAddr = addr - this->addr;
  *((int16_t*)(&data[0] + relativeAddr)) = value;
}

auto MemorySection::write32(uint32_t addr, int32_t value) -> void
{
  uint32_t relativeAddr = addr - this->addr;
  *((int32_t*)(&data[0] + relativeAddr)) = value;
}

auto MemorySection::write(uint32_t addr, uint32_t size, int32_t value) -> void
{
  switch (size) {
    case 1: write8(addr, value); break;
    case 2: write16(addr, value); break;
    case 4: write32(addr, value); break;
    default: break;
  }
}

auto MemorySection::copy(uint32_t addr, uint32_t size, uint8_t* buf) -> void
{
  uint32_t relativeAddr = addr - this->addr;
  // data.assign(buf + relativeAddr, buf + relativeAddr + size);
  for (int i=0; i<size; i++) {
    data[i + relativeAddr] = buf[i];
  }
}

auto MemorySection::set(uint32_t addr, uint32_t size, uint8_t value) -> void
{
  uint32_t relativeAddr = addr - this->addr;
  for (int i=0; i<size; i++) {
    data[i + relativeAddr] = value;
  }
}

auto MemorySection::buffer() -> const uint8_t*
{
  return data.data();
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

auto IoRamSection::write8(uint32_t addr, int8_t value) -> void
{
  MemorySection::write8(addr, value);
}
auto IoRamSection::write16(uint32_t addr, int16_t value) -> void
{
  MemorySection::write16(addr, value);
}

auto IoRamSection::write32(uint32_t addr, int32_t value) -> void
{
  MemorySection::write32(addr, value);
}

auto IoRamSection::write(uint32_t addr, uint32_t size, int32_t value) -> void
{
  MemorySection::write(addr, size, value);
}

Memory::Memory()
  : sectionsPool(),
    sections(),
    invalidSection(),
    busyFlag(),
    processor(),
    prevProcessor()
{
  invalidSection.name = "invalid";
  initMinimumSections();
}

Memory::~Memory()
{
  processor = nullptr;
  prevProcessor = nullptr;
}

auto Memory::section(const uint32_t addr) -> MemorySection&
{
  return *sections[addr];
}

auto Memory::section(const char* name) -> MemorySection&
{
  return *sections[sectionNameTable[name]];
}

auto Memory::section(const string& name) -> MemorySection&
{
  return *sections[sectionNameTable[name]];
}

auto Memory::sectionByAddr(const uint32_t addr) -> MemorySection&
{
  // return sectionByAddrSafe(addr);
  return sectionByAddrFast(addr);
}

auto Memory::sectionByAddrSafe(const uint32_t addr) -> MemorySection&
{
  for (auto& section: sections) {
    if (section.second->isin(addr)) {
      return *section.second;
    }
  }
  return invalidSection;
}

auto Memory::sectionByAddrFast(const uint32_t addr) -> MemorySection&
{
  uint32_t mid = addr >> 24;
  if (mid >= 0x80) {
    return *sections[HWREG_PROGRAM_BASEADDR];
  } else {
    switch(mid & 0xF) {
      case 0x0: return *sections[HWREG_SYSROM_BASEADDR];
      case 0x1: return *sections[HWREG_WORKRAM_BASEADDR];
      case 0x2: return *sections[HWREG_MAINRAM_BASEADDR];
      case 0x3:
        // return *sections[HWREG_IORAM_BASEADDR];
        switch ((addr >> 8) & 0xFFu) {
          // case 0x0: return *sections[HWREG_IO_VIDEO_ADDR];
          // case 0x1: return *sections[HWREG_IO_AUDIO_ADDR];
          // case 0x2: return *sections[HWREG_IO_INPUT_ADDR];
          case 0x3: return *sections[HWREG_IO_DMA_ADDR];
          case 0x4: return *sections[HWREG_IO_TIMER_ADDR];
          // case 0x5: return *sections[HWREG_IO_SERIAL_ADDR];
          // case 0x6: return *sections[HWREG_IO_INT_ADDR];
          // default: return *sections[0];  // invalid section
          default: return *sections[HWREG_IORAM_BASEADDR];  // invalid section
        }
      case 0x4: return *sections[HWREG_VRAM_BASEADDR];
      case 0x6: return *sections[HWREG_TILERAM_BASEADDR];
      case 0x7: return *sections[HWREG_ARAM_BASEADDR];
      case 0x9: return *sections[HWREG_INSTRAM_BASEADDR];
      case 0xE: return *sections[HWREG_SAVERAM_BASEADDR];
      case 0xF: return *sections[HWREG_SAVERAM_BASEADDR];
      default: return *sections[0];  // invalid section
    }
  }
  // unreachable
  return invalidSection;
}

auto Memory::clearSection() -> void
{
  sections.clear();
}

auto Memory::initMinimumSections() -> void
{
  busyFlag.flag32 = 0;
  sections.clear();
  #define sections_insert(T, name, addr, size) \
    sectionsPool.push_back(make_shared<T>(T(name, (addr), (size)))); \
    sections.insert(make_pair((addr), sectionsPool.back().get())); \
    sectionNameTable.insert(make_pair(name,(addr)));
  sections_insert(MemorySection, "system",  HWREG_SYSROM_BASEADDR , HWREG_SYSROM_SIZE );
  sections_insert(MemorySection, "stack",   HWREG_WORKRAM_BASEADDR, HWREG_WORKRAM_SIZE);
  sections_insert(MemorySection, "data"  ,  HWREG_MAINRAM_BASEADDR, HWREG_MAINRAM_SIZE);
  sections_insert(MemorySection, "ioram",   HWREG_IORAM_BASEADDR  , HWREG_IORAM_SIZE  );
  sections_insert(MemorySection, "vram",    HWREG_VRAM_BASEADDR   , HWREG_VRAM_SIZE   );
  sections_insert(MemorySection, "tile",    HWREG_TILERAM_BASEADDR, HWREG_TILERAM_SIZE);
  sections_insert(MemorySection, "aram",    HWREG_ARAM_BASEADDR   , HWREG_ARAM_SIZE   );
  sections_insert(MemorySection, "inst",    HWREG_INSTRAM_BASEADDR, HWREG_INSTRAM_SIZE);
  sections_insert(MemorySection, "save",    HWREG_SAVERAM_BASEADDR, HWREG_SAVERAM_SIZE);
  sections_insert(MemorySection, "program", HWREG_PROGRAM_BASEADDR, HWREG_PROGRAM_SIZE);
  sections_insert(MemorySection, "invalid", 0, 0);
  #undef sections_insert
}

auto Memory::addSection(const string& name, uint32_t addr, uint32_t size) -> void
{
  sectionsPool.push_back(make_shared<MemorySection>(MemorySection(name, addr, size)));
  sections.insert(make_pair(addr, sectionsPool.back().get()));
  sectionNameTable.insert(make_pair(name, addr));
}

auto Memory::isBusy(uint32_t priority) -> bool
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

auto Memory::setBusy(uint32_t priority) -> void
{
  busyFlag.flag32 |= (1u << priority);
}

auto Memory::clearBusy(uint32_t priority) -> void
{
  busyFlag.flag32 &= ~(1u << priority);
}

auto Memory::waitAccess(uint32_t addr, uint32_t size, bool rw, int8_t& wait) -> bool
{
  if (isBusy(0) || wait < 0) {
    auto addrSection = addr >> 24;
    if (addrSection > (HWREG_SAVERAM_BASEADDR >> 24)) {
      wait = 5;
    } else
    if (addrSection == (HWREG_MAINRAM_BASEADDR >> 24)) {
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

auto Memory::read8(uint32_t addr) -> int8_t
{
  auto& section = sectionByAddr(addr);
  return section.read8(addr);
}

auto Memory::read16(uint32_t addr) -> int16_t
{
  auto& section = sectionByAddr(addr);
  return section.read16(addr);
}

auto Memory::read32(uint32_t addr) -> int32_t
{
  auto& section = sectionByAddr(addr);
  return section.read32(addr);
}

auto Memory::write8(uint32_t addr, int8_t value) -> void
{
  auto& section = sectionByAddr(addr);
  section.write8(addr, value);
}

auto Memory::write16(uint32_t addr, int16_t value) -> void
{
  auto& section = sectionByAddr(addr);
  section.write16(addr, value);
}

auto Memory::write32(uint32_t addr, int32_t value) -> void
{
  auto& section = sectionByAddr(addr);
  section.write32(addr, value);
}

auto Memory::copy(uint32_t addr, uint32_t size, uint8_t* value) -> void
{
  auto& section = sectionByAddr(addr);
  section.copy(addr, size, value);
}
