#include <elf.h>
#include <memory.h>
#include <fstream>
#include <fmt/core.h>

Elf::Elf()
{
}

Elf::~Elf()
{
}

void Elf::load(const string& path)
{
  this->path = path;
  ifstream ifs(path, ios::binary);
  vector<uint8_t> buffer(istreambuf_iterator<char>(ifs), {});
  ifs.close();
  data = std::move(buffer);

  uint8_t* bufferptr = data.data();
  fmt::print("elf buffsize: {:d}\n", data.size());

  /* read Elf header */
  ElfHeader* eh = (ElfHeader*)(bufferptr + 0);
  fmt::print("cls={:d}, endian={:d}, phnum={:d}, shnum={:d}\n", eh->ei.cls, eh->ei.data, eh->phnum, eh->shnum);

  /* read Section headers */
  fmt::print("section headers\n");
  fmt::print("  name---------------- addr---- offset-- size----\n");
  sections.clear();
  for (int si=0; si<eh->shnum; si++) {
    SectionHeader* sh = (SectionHeader*)(bufferptr + eh->shoffset + eh->shentrysize * si);
    SectionHeader* nametable_sh = (SectionHeader*)(bufferptr + eh->shoffset + eh->shentrysize * eh->shstringindex);
    char* sname = (char*)(bufferptr + nametable_sh->offset + sh->name);
    fmt::print("  {:20s} {:08x} {:08x} {:8x}\n", sname, sh->addr, sh->offset, sh->size);
    sections.push_back(Section{.name=sname, .addr=sh->addr, .offset=sh->offset, .size=sh->size});
  }

  // /* read SymbolTable section */
  // int symtab_idx = ei->shnum - 3;
  // SectionHeader* symtab_sh = (SectionHeader*)(bufferptr + eh->shoffset + eh->shentrysize * symtab_idx);
  // for (int symi=0; symi<symtab_sh->
  // 
  // /* read StringTable section */
  // int strtab_idx = ei->shnum - 2;
  // SectionHeader* strtab_sh = (SectionHeader*)(bufferptr + eh->shoffset + eh->shentrysize * strtab_idx);
}

void Elf::allocMemory(Memory& memory)
{
  memory.initMinimumSections();

  uint8_t* buffer = data.data();
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
  for (auto& section: sections) {
    if (section.name == ".data" || section.name == ".bss") {
      memory.addSection(section.name, section.addr, section.addr);
    }
  }
}
