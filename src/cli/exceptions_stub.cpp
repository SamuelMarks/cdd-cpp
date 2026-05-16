#include <stdlib.h>

extern "C" {
void *__cxa_allocate_exception(size_t thrown_size) noexcept {
  abort();
  return nullptr;
}
void __cxa_free_exception(void *thrown_exception) noexcept { abort(); }
void __cxa_throw(void *thrown_exception, void *tinfo, void (*dest)(void *)) {
  abort();
}
void *__cxa_begin_catch(void *exceptionObject) noexcept {
  abort();
  return nullptr;
}
void __cxa_end_catch() { abort(); }
void __cxa_rethrow() { abort(); }
}
