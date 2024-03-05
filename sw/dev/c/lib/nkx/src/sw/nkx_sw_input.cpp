#include <stdint.h>
#include <memorymap.h>
#include <nkx/sw/nkx_sw.h>

namespace nkx {

/* Input */
Pad pad[1];
auto Pad::set(HwPad& obj) -> void
{
  this->val32 = obj.val32;
}

}; /* namespace nkx */
