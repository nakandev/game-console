#include <elf.h>
#include <memory.h>
#include <fstream>
#include <fmt/core.h>

extern int debugLevel;

Elf::Elf()
{
}

Elf::~Elf()
{
}

auto Elf::load(const string& path) -> uint8_t
{
  this->path = path;
  ifstream ifs(path, ios::binary);
  if (!ifs) {
    return 1;
  }
  vector<uint8_t> buffer(istreambuf_iterator<char>(ifs), {});
  ifs.close();

  uint8_t* bufferptr = buffer.data();

  /* read Elf header */
  ElfHeader* eh = (ElfHeader*)(bufferptr + 0);
  elfHeader = *eh;
  int16_t mag[4] = {eh->ei.mag0, eh->ei.mag1, eh->ei.mag2, eh->ei.mag3};
  if (!(mag[0] == 127 && mag[1] == 'E' && mag[2] == 'L' && mag[3] == 'F')) {
    return 1;
  }

  /* read Program headers */
  for (int pi=0; pi<eh->phnum; pi++) {
    ProgramHeader* ph = (ProgramHeader*)(bufferptr + eh->phoffset + eh->phentrysize * pi);
  }

  /* read Section headers */
  sections.clear();
  for (int si=0; si<eh->shnum; si++) {
    SectionHeader* sh = (SectionHeader*)(bufferptr + eh->shoffset + eh->shentrysize * si);
    SectionHeader* nametable_sh = (SectionHeader*)(bufferptr + eh->shoffset + eh->shentrysize * eh->shstringindex);
    char* sname = (char*)(bufferptr + nametable_sh->offset + sh->name);
    sections.push_back(Section{.name=sname, .addr=sh->addr, .offset=sh->offset, .size=sh->size});
  }

  // /* read SymbolTable section */
  // int symtab_idx = ei->shnum - 3;
  // SectionHeader* symtab_sh = (SectionHeader*)(bufferptr + eh->shoffset + eh->shentrysize * symtab_idx);
  // 
  // /* read StringTable section */
  // int strtab_idx = ei->shnum - 2;
  // SectionHeader* strtab_sh = (SectionHeader*)(bufferptr + eh->shoffset + eh->shentrysize * strtab_idx);

  data = std::move(buffer);

  if (debugLevel >= 1) {
    getInfo();
  }

  return 0;
}

auto Elf::getElfHeader() -> const Elf::ElfHeader&
{
  return elfHeader;
}

auto Elf::getSection(const string& name) -> Elf::Section*
{
  for (auto& s: sections) {
    if (s.name == name) return &s;
  }
  return nullptr;
}

auto Elf::allocMemory(Memory& memory) -> void
{
  // memory.initMinimumSections();

  uint8_t* buffer = data.data();
  auto& system = memory.section("system");
  for (auto& section: sections) {
    auto begin = section.addr;
    auto end = section.addr + section.size - 1;
    if (system.isin(begin) && system.isin(end)) {
      system.copy(section.addr, section.size, (buffer + section.offset));
    }
  }
  auto& program = memory.section("program");
  for (auto& section: sections) {
    uint32_t programEnd = program.addr + program.size - 1;
    uint32_t newEnd = section.addr + section.size - 1;
    if (program.isin(section.addr) && program.isin(newEnd)) {
      auto newSize = newEnd - program.addr + 1;
      program.size = (newSize > program.size) ? newSize : program.size;
      // copy binary
      program.copy(section.addr, section.size, (buffer + section.offset));
    }
  }
  auto& _data = memory.section("data");
  for (auto& section: sections) {
    if (section.name == ".data" || section.name == ".sdata") {
      _data.copy(section.addr, section.size, (buffer + section.offset));
    }
    if (section.name == ".bss" || section.name == ".sbss") {
      _data.set(section.addr, section.size, 0);
    }
  }
}

auto Elf::getInfo() -> void
{
  uint8_t* bufferptr = data.data();

  /* read Elf header */
  ElfHeader* eh = &elfHeader;
  fmt::print("cls={:d}, endian={:d}, phnum={:d}, shnum={:d}\n", eh->ei.cls, eh->ei.data, eh->phnum, eh->shnum);
  fmt::print("type={:d}, machine={:d}, version={:d}, entry={:08x}\n", eh->type, eh->machine, eh->version, eh->entry);

  /* read Program headers */
  fmt::print("program headers\n");
  fmt::print("  type---- offset-- vaddr--- paddr--- filesize memsize- flags--- align---\n");
  for (int pi=0; pi<eh->phnum; pi++) {
    ProgramHeader* ph = (ProgramHeader*)(bufferptr + eh->phoffset + eh->phentrysize * pi);
    fmt::print("  {:08x} {:08x} {:08x} {:08x} {:08x} {:08x} {:08x} {:8d}\n",
      ph->type, ph->offset, ph->vaddr, ph->paddr, ph->filesize, ph->memorysize, ph->flags, ph->align);
  }

  /* read Section headers */
  fmt::print("section headers\n");
  fmt::print("  name---------------- addr---- offset-- size---- flags---\n");
  for (int si=0; si<eh->shnum; si++) {
    SectionHeader* sh = (SectionHeader*)(bufferptr + eh->shoffset + eh->shentrysize * si);
    SectionHeader* nametable_sh = (SectionHeader*)(bufferptr + eh->shoffset + eh->shentrysize * eh->shstringindex);
    char* sname = (char*)(bufferptr + nametable_sh->offset + sh->name);
    fmt::print("  {:20s} {:08x} {:08x} {:8x} {:8}\n",
      sname, sh->addr, sh->offset, sh->size, sh->flags);
  }
}
