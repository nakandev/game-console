#pragma once
#include <string>
#include <memory>
#include <vector>
#include <map>
#include <processor.h>

using namespace std;

class MemorySection {
  public:
    string name;
    uint32_t addr;
    size_t size;
  protected:
    vector<uint8_t> data;
  public:
    MemorySection();
    MemorySection(const string& name, uint32_t addr, size_t size);
    MemorySection(const MemorySection& obj);
    virtual ~MemorySection();
    auto resize(size_t size) -> void;
    auto resizeToEndAddr(uint32_t addr) -> void;
    auto isin(uint32_t addr) -> bool;
    virtual auto read8(uint32_t addr) -> int8_t;
    virtual auto read16(uint32_t addr) -> int16_t;
    virtual auto read32(uint32_t addr) -> int32_t;
    virtual auto read(uint32_t addr, uint32_t size) -> int32_t;
    virtual auto write8(uint32_t addr, int8_t value) -> void;
    virtual auto write16(uint32_t addr, int16_t value) -> void;
    virtual auto write32(uint32_t addr, int32_t value) -> void;
    virtual auto write(uint32_t addr, uint32_t size, int32_t value) -> void;
    auto copy(uint32_t addr, uint32_t size, uint8_t* buf) -> void;
    auto set(uint32_t addr, uint32_t size, uint8_t value) -> void;
    auto buffer() -> const uint8_t*;
};

class IoRamSection : public MemorySection {
    void updateRunningDma(uint32_t addr, int32_t value);
  public:
    int runningDma;
    IoRamSection();
    IoRamSection(const string& name, uint32_t addr, size_t size);
    IoRamSection(const MemorySection& obj);
    ~IoRamSection();
    auto write8(uint32_t addr, int8_t value) -> void override;
    auto write16(uint32_t addr, int16_t value) -> void override;
    auto write32(uint32_t addr, int32_t value) -> void override;
    auto write(uint32_t addr, uint32_t size, int32_t value) -> void override;
};

union BusyFlag{
  uint32_t flag32;
  struct {
    uint32_t cpu : 1;
    uint32_t dma0 : 1;
    uint32_t dma1 : 1;
    uint32_t dma2 : 1;
    uint32_t dma3 : 1;
    uint32_t _reserved : 27;
  } bit;
};

class Memory {
  private:
    // map<uint32_t, shared_ptr<MemorySection>> sections;
    vector<shared_ptr<MemorySection>> sectionsPool;
    map<uint32_t, MemorySection*> sections;
    map<string, uint32_t> sectionNameTable;
    MemorySection invalidSection;
  public:
    BusyFlag busyFlag;
    Processor* processor;
    Processor* prevProcessor;
    Memory();
    ~Memory();
    auto section(const uint32_t addr) -> MemorySection&;
    auto section(const char* name) -> MemorySection&;
    auto section(const string& name) -> MemorySection&;
    auto sectionByAddr(const uint32_t addr) -> MemorySection&;
    auto sectionByAddrSafe(const uint32_t addr) -> MemorySection&;
    auto sectionByAddrFast(const uint32_t addr) -> MemorySection&;
    auto clearSection() -> void;
    auto initMinimumSections() -> void;
    auto addSection(const string& name, uint32_t addr, uint32_t size) -> void;
    template<typename T> auto addSection(T* section) -> void
    {
      sections.insert(make_pair(section->addr, section));
      sectionNameTable.insert(make_pair(section->name, section->addr));
    }
    auto waitAccess(uint32_t addr, uint32_t size, bool rw, int8_t& wait) -> bool;
    auto isBusy(uint32_t priority) -> bool;
    auto setBusy(uint32_t priority) -> void;
    auto clearBusy(uint32_t priority) -> void;
    auto read8(uint32_t addr) -> int8_t;
    auto read16(uint32_t addr) -> int16_t;
    auto read32(uint32_t addr) -> int32_t;
    auto write8(uint32_t addr, int8_t value) -> void;
    auto write16(uint32_t addr, int16_t value) -> void;
    auto write32(uint32_t addr, int32_t value) -> void;
    auto copy(uint32_t addr, uint32_t size, uint8_t* value) -> void;
};

