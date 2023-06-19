#include <string>
#include <vector>

using namespace std;

class Memory;

class Elf {
public:
  struct ElfHeader {
    // uint8_t identyfier[16];
    struct EI {
      uint8_t mag0;
      uint8_t mag1;
      uint8_t mag2;
      uint8_t mag3;
      uint8_t cls;
      uint8_t data;
      uint8_t version;
      uint8_t osabi;
      uint8_t abiversion;
      uint8_t pad[7];
    } ei;
    uint16_t type;
    uint16_t machine;
    uint32_t version;
    uint32_t entry;
    uint32_t phoffset;
    uint32_t shoffset;
    uint32_t flags;
    uint16_t size;
    uint16_t phentrysize;
    uint16_t phnum;
    uint16_t shentrysize;
    uint16_t shnum;
    uint16_t shstringindex;
  };
  struct ProgramHeader {
    uint32_t type;
    uint32_t flags;
    uint32_t offset;
    uint32_t vaddr;
    uint32_t paddr;
    uint32_t filesize;
    uint32_t memorysize;
    uint32_t align;
  };
  struct SectionHeader {
    uint32_t name;
    uint32_t type;
    uint32_t flags;
    uint32_t addr;
    uint32_t offset;
    uint32_t size;
    uint32_t link;
    uint32_t info;
    uint32_t addralign;
    uint32_t entrysize;
  };
  struct SymbolTable {
    uint32_t name;
    uint32_t value;
    uint32_t size;
    uint8_t  info;
    uint8_t  other;
    uint16_t shindex;
  };
  struct Section {
    string name;
    uint32_t addr;
    uint32_t offset;
    uint32_t size;
  };
private:
  string path;
  vector<uint8_t> data;
  ElfHeader elfHeader;
  vector<ProgramHeader*> programHeaders;
  vector<Section> sections;
public:
  Elf();
  ~Elf();
  void load(const string& path);
  void allocMemory(Memory& memory);
};
