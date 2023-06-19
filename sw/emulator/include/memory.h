#pragma once
#include <string>
#include <memory>
#include <vector>
#include <map>

using namespace std;

class MemorySection {
  public:
    string name;
    uint32_t addr;
    size_t size;
  private:
    // vector<uint8_t> data;
    uint8_t* data;
  public:
    MemorySection();
    MemorySection(const string& name, uint32_t addr, size_t size);
    MemorySection(const MemorySection& obj);
    ~MemorySection();
    void resize(size_t size);
    void resizeToEndAddr(uint32_t addr);
    bool isin(uint32_t addr);
    int32_t read(uint32_t addr, int size);
    int8_t read8(uint32_t addr);
    int16_t read16(uint32_t addr);
    int32_t read32(uint32_t addr);
    void write(uint32_t addr, int size, int32_t value);
    void copy(uint32_t addr, int size, uint8_t* buf);
    const uint8_t* buffer();
};

class Memory {
  private:
    map<string, MemorySection> sections;
  public:
    Memory();
    ~Memory();
    MemorySection& section(const string& name);
    void clearSection();
    void initMinimumSections();
    void addSection(const string& name, uint32_t addr, uint32_t size);
    int32_t read(uint32_t addr, int size);
    int8_t read8(uint32_t addr);
    int16_t read16(uint32_t addr);
    int32_t read32(uint32_t addr);
    void write(uint32_t addr, int size, int32_t value);
    void copy(uint32_t addr, int size, uint8_t* value);
};

