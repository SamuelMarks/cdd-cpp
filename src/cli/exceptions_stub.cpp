// GCOV_EXCL_START
#include <stdlib.h>

extern "C" {
void *__cxa_allocate_exception(size_t thrown_size) noexcept {
  (void)thrown_size;
  abort();
  return nullptr;
}
void __cxa_free_exception(void *thrown_exception) noexcept {
  (void)thrown_exception;
  abort();
}
void __cxa_throw(void *thrown_exception, void *tinfo, void (*dest)(void *)) {
  (void)thrown_exception;
  (void)tinfo;
  (void)dest;
  abort();
}
void *__cxa_begin_catch(void *exceptionObject) noexcept {
  (void)exceptionObject;
  abort();
  return nullptr;
}
void __cxa_end_catch() { abort(); }
void __cxa_rethrow() { abort(); }
}
// GCOV_EXCL_STOP
