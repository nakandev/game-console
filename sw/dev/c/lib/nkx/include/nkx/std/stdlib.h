#include <cstddef>

namespace std {
  using size_t = unsigned int;
  enum class align_val_t : size_t {};
};

void* operator new  (std::size_t count); //already present
void* operator new[](std::size_t count); // already present
void operator delete  (void* ptr) noexcept; //already present
void operator delete[](void* ptr) noexcept; //already present
void * operator new  (std::size_t size, void * ptr) noexcept; //already present
void * operator new[](std::size_t size, void * ptr) noexcept; //already present (but missing from new.h???)
void operator delete  (void* ptr, std::size_t sz) noexcept; //proposed
void operator delete[](void* ptr, std::size_t sz) noexcept; //proposed
void* operator new  (std::size_t count, std::align_val_t al); // ???
void* operator new[](std::size_t count, std::align_val_t al); // ??? 
void operator delete  (void* ptr, std::align_val_t al) noexcept; // just cast al to void, and free(ptr)? 
void operator delete[](void* ptr, std::align_val_t al) noexcept; // just cast al to void, and free (ptr)? 
void operator delete  (void* ptr, std::size_t sz, std::align_val_t al) noexcept; // ???
void operator delete[](void* ptr, std::size_t sz, std::align_val_t al) noexcept; // ???
