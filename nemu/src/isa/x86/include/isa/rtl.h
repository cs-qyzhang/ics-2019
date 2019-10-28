#ifndef __X86_RTL_H__
#define __X86_RTL_H__

#include "rtl/rtl.h"

extern bool wp_reg_changed;
extern const bool reg_mask[8];

/* RTL pseudo instructions */

static inline void rtl_lr(rtlreg_t* dest, int r, int width) {
  switch (width) {
    case 4: rtl_mv(dest, &reg_l(r)); return;
    case 1: rtl_host_lm(dest, &reg_b(r), 1); return;
    case 2: rtl_host_lm(dest, &reg_w(r), 2); return;
    default: assert(0);
  }
}

static inline void rtl_sr(int r, const rtlreg_t* src1, int width) {
  switch (width) {
    case 4: rtl_mv(&reg_l(r), src1); wp_reg_changed |= reg_mask[r]; return;
    case 1: rtl_host_sm(&reg_b(r), src1, 1); wp_reg_changed |= reg_mask[r % 4]; return;
    case 2: rtl_host_sm(&reg_w(r), src1, 2); wp_reg_changed |= reg_mask[r]; return;
    default: assert(0);
  }
}

static inline void rtl_push(const rtlreg_t* src1) {
  // esp <- esp - 4
  // M[esp] <- src1
  // XXX();
  rtl_lr(&t0, R_ESP, 4);
  t0 = t0 - 4;
  rtl_sr(R_ESP, &t0, 4);
  rtl_sm(&t0, src1, 4);
}

static inline void rtl_pop(rtlreg_t* dest) {
  // dest <- M[esp]
  // esp <- esp + 4
  // XXX();
  rtl_lr(&t0, R_ESP, 4);
  rtl_lm(dest, &t0, 4);
  t0 = t0 + 4;
  rtl_sr(R_ESP, &t0, 4);
}

// @res: 传入的运算结果
// overflow 是在有符号数运算时产生的，当两个操作数符号不同且第一个操作数与结果
// 符号不同时产生溢出
static inline void rtl_is_sub_overflow(rtlreg_t* dest,
    const rtlreg_t* res, const rtlreg_t* src1, const rtlreg_t* src2, int width) {
  // dest <- is_overflow(src1 - src2)
  // XXX();
  int result = 0;
  if (width == 1) {
    int8_t src1_ = *((int8_t *)src1);
    int8_t src2_ = *((int8_t *)src2);
    int8_t res_ = *((int8_t *)res);
    result = (((src1_ ^ src2_) >> 7) && ((res_ ^ src1_) >> 7)) ? 1 : 0;
  } else if (width == 2) {
    int16_t src1_ = *((int16_t *)src1);
    int16_t src2_ = *((int16_t *)src2);
    int16_t res_ = *((int16_t *)res);
    result = (((src1_ ^ src2_) >> 15) && ((res_ ^ src1_) >> 15)) ? 1 : 0;
  } else if (width == 4) {
    int32_t src1_ = *((int32_t *)src1);
    int32_t src2_ = *((int32_t *)src2);
    int32_t res_ = *((int32_t *)res);
    result = (((src1_ ^ src2_) >> 31) && ((res_ ^ src1_) >> 31)) ? 1 : 0;
  } else {
    assert(0);
  }
  rtl_li(dest, result);
}

// carry 是在无符号数运算时产生的
static inline void rtl_is_sub_carry(rtlreg_t* dest,
    const rtlreg_t* res, const rtlreg_t* src1) {
  // dest <- is_carry(src1 - src2)
  // XXX();
  uint32_t src1_ = *((uint32_t *)src1);
  uint32_t res_ = *((uint32_t *)res);
  rtl_li(dest, res_ > src1_ ? 1 : 0);
}

// overflow 是在有符号数运算时产生的，当两个操作数符号相同且第一个操作数与结果
// 符号不同时产生溢出
static inline void rtl_is_add_overflow(rtlreg_t* dest,
    const rtlreg_t* res, const rtlreg_t* src1, const rtlreg_t* src2, int width) {
  // dest <- is_overflow(src1 + src2)
  // XXX();
  int result = 0;
  if (width == 1) {
    int8_t src1_ = *((int8_t *)src1);
    int8_t src2_ = *((int8_t *)src2);
    int8_t res_ = *((int8_t *)res);
    result = (!((src1_ ^ src2_) >> 7) && ((res_ ^ src1_) >> 7)) ? 1 : 0;
  } else if (width == 2) {
    int16_t src1_ = *((int16_t *)src1);
    int16_t src2_ = *((int16_t *)src2);
    int16_t res_ = *((int16_t *)res);
    result = (!((src1_ ^ src2_) >> 15) && ((res_ ^ src1_) >> 15)) ? 1 : 0;
  } else if (width == 4) {
    int32_t src1_ = *((int32_t *)src1);
    int32_t src2_ = *((int32_t *)src2);
    int32_t res_ = *((int32_t *)res);
    result = (!((src1_ ^ src2_) >> 31) && ((res_ ^ src1_) >> 31)) ? 1 : 0;
  } else {
    assert(0);
  }
  rtl_li(dest, result);
}

static inline void rtl_is_add_carry(rtlreg_t* dest,
    const rtlreg_t* res, const rtlreg_t* src1) {
  // dest <- is_carry(src1 + src2)
  // XXX();
  uint32_t src1_ = *((uint32_t *)src1);
  uint32_t res_ = *((uint32_t *)res);
  rtl_li(dest, res_ < src1_ ? 1 : 0);
}

// FIXME:没有eflags的定义！
#define make_rtl_setget_eflags(f) \
  static inline void concat(rtl_set_, f) (const rtlreg_t* src) { \
    cpu.eflags.f = (*src) ? 1 : 0; \
    /* XXX(); */  \
  } \
  static inline void concat(rtl_get_, f) (rtlreg_t* dest) { \
    *dest = cpu.eflags.f; \
    /* XXX(); */  \
  }

make_rtl_setget_eflags(CF)
make_rtl_setget_eflags(OF)
make_rtl_setget_eflags(ZF)
make_rtl_setget_eflags(SF)

static inline void rtl_update_ZF(const rtlreg_t* result, int width) {
  // eflags.ZF <- is_zero(result[width * 8 - 1 .. 0])
  // XXX();
  switch (width) {
    case 1: cpu.eflags.ZF = *((int8_t *)result) ? 0 : 1; break;
    case 2: cpu.eflags.ZF = *((int16_t *)result) ? 0 : 1; break;
    case 4: cpu.eflags.ZF = *((int32_t *)result) ? 0 : 1; break;
    default: assert(0);
  }
}

static inline void rtl_update_SF(const rtlreg_t* result, int width) {
  // eflags.SF <- is_sign(result[width * 8 - 1 .. 0])
  // XXX();
  switch (width) {
    case 1: cpu.eflags.SF = ((*((int8_t *)result)) >> 7) ? 1 : 0; break;
    case 2: cpu.eflags.SF = ((*((int16_t *)result)) >> 15) ? 1 : 0; break;
    case 4: cpu.eflags.SF = ((*((int32_t *)result)) >> 31) ? 1 : 0; break;
    default: assert(0);
  }
}

static inline void rtl_update_ZFSF(const rtlreg_t* result, int width) {
  rtl_update_ZF(result, width);
  rtl_update_SF(result, width);
}

#endif
