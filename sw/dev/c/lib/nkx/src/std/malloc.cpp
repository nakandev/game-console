#include <cstdint>
// #include <nkx/std/stdlib.h>

struct HeapInfo {
  uint32_t size;
  bool used;
  HeapInfo* next;
} __attribute__((aligned(4)));

unsigned char _heapCommon[0x080'000] __attribute__((aligned(4)));

void* malloc(std::size_t size) {
  if (size == 0) {
    return nullptr;
  }
  // find first area
  HeapInfo* info = (HeapInfo*)((uint32_t)_heapCommon);
  while (info->next != nullptr) {
    uint32_t space = (uint32_t)(info->next - info);
    if (!info->used && space <= size) {
      // found
      break;
    }
    info = info->next;
  }
  // set
  info->size = size;
  info->used = true;
  info->next = info + sizeof(HeapInfo) + size;
  info->next->next = nullptr;
  void* ptr = (void*)((uint32_t)info + sizeof(HeapInfo));
  return ptr;
}

void free(void* ptr) {
  if (!ptr) {
    return;
  }
  if (ptr < _heapCommon || ptr >= _heapCommon + sizeof(_heapCommon)) {
    return;
  }
  HeapInfo* info = (HeapInfo*)((uint32_t)ptr - sizeof(HeapInfo));
  info->used = false;
}

// For C++11, only need the following:
void *operator new  (std::size_t size) {
  return malloc(size);
}
void *operator new[](std::size_t size) {
  return malloc(size);
}

void  operator delete  (void * ptr) {
  free(ptr);
}
void  operator delete[](void * ptr) {
  free(ptr);
}
/* does this get a conditional like the ones below or not? */
#if (1)
void * operator new  (std::size_t size, void * ptr) noexcept {
  (void)size;
  return ptr;
}
void * operator new[](std::size_t size, void * ptr) noexcept {
  (void)size;
  return ptr;
}
#endif

// Since C++14, also need two more delete variants...
#if (__cpp_sized_deallocation >= 201309L)
void  operator delete  (void* ptr, std::size_t size) noexcept {
  (void) size;
  free(ptr);
}
void  operator delete[](void* ptr, std::size_t size) noexcept {
  (void) size;
  free(ptr);
}
#endif

// Since C++17, there's four more each for new / delete, to support allocation
// of objects with alignment greater than __STDCPP_DEFAULT_NEW_ALIGNMENT__.
// #if (__cpp_aligned_new >= 201606L)
// void* operator new  (std::size_t size, std::align_val_t al) {
//   /* ??? what about al? */
//   return malloc(size);
// }
// void* operator new[](std::size_t size, std::align_val_t al) {
//   /* ??? what about al? */
//   return malloc(size);
// }
// void  operator delete  (void* ptr, std::align_val_t al) noexcept {
//   /* ??? what about al? */
//   free(ptr);
// }
// void  operator delete[](void* ptr, std::align_val_t al) noexcept {
//   /* ??? what about al? */
//   free(ptr);
// }
// void  operator delete  (void* ptr, std::size_t size, std::align_val_t al) noexcept{
//   /* ??? what about al? */
//   (void) size;
//   free(ptr);
// }
// void  operator delete[](void* ptr, std::size_t size, std::align_val_t al) noexcept {
//   /* ??? what about al? */
//   (void) size;
//   free(ptr);
// }
// #endif

