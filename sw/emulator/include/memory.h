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
    // vector<uint8_t> data;
    uint8_t* data;
  public:
    MemorySection();
    MemorySection(const string& name, uint32_t addr, size_t size);
    MemorySection(const MemorySection& obj);
    virtual ~MemorySection();
    void resize(size_t size);
    void resizeToEndAddr(uint32_t addr);
    bool isin(uint32_t addr);
    virtual int8_t read8(uint32_t addr);
    virtual int16_t read16(uint32_t addr);
    virtual int32_t read32(uint32_t addr);
    virtual int32_t read(uint32_t addr, uint32_t size);
    virtual void write8(uint32_t addr, int8_t value);
    virtual void write16(uint32_t addr, int16_t value);
    virtual void write32(uint32_t addr, int32_t value);
    virtual void write(uint32_t addr, uint32_t size, int32_t value);
    void copy(uint32_t addr, uint32_t size, uint8_t* buf);
    void set(uint32_t addr, uint32_t size, uint8_t value);
    uint8_t* const buffer();
};

class IoRamSection : public MemorySection {
    void updateRunningDma(uint32_t addr, int32_t value);
  public:
    int runningDma;
    IoRamSection();
    IoRamSection(const string& name, uint32_t addr, size_t size);
    IoRamSection(const MemorySection& obj);
    ~IoRamSection();
    void write8(uint32_t addr, int8_t value) override;
    void write16(uint32_t addr, int16_t value) override;
    void write32(uint32_t addr, int32_t value) override;
    void write(uint32_t addr, uint32_t size, int32_t value) override;
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
    map<uint32_t, shared_ptr<MemorySection>> sections;
    map<string, uint32_t> sectionNameTable;
    MemorySection invalidSection;
  public:
    BusyFlag busyFlag;
    Processor* processor;
    Memory();
    ~Memory();
    MemorySection& section(const uint32_t addr);
    MemorySection& section(const char* name);
    MemorySection& section(const string& name);
    MemorySection& sectionByAddr(const uint32_t addr);
    MemorySection& sectionByAddrSafe(const uint32_t addr);
    MemorySection& sectionByAddrFast(const uint32_t addr);
    void clearSection();
    void initMinimumSections();
    void addSection(const string& name, uint32_t addr, uint32_t size);
    template<typename T> void addSection(T& section)
    {
      sections.insert(make_pair(section.addr, make_shared<T>(section)));
      sectionNameTable.insert(make_pair(section.name, section.addr));
    }
    bool waitAccess(uint32_t addr, uint32_t size, bool rw, int8_t& wait);
    bool isBusy(uint32_t priority);
    void setBusy(uint32_t priority);
    void clearBusy(uint32_t priority);
    int8_t read8(uint32_t addr);
    int16_t read16(uint32_t addr);
    int32_t read32(uint32_t addr);
    void write8(uint32_t addr, int8_t value);
    void write16(uint32_t addr, int16_t value);
    void write32(uint32_t addr, int32_t value);
    void copy(uint32_t addr, uint32_t size, uint8_t* value);
};

