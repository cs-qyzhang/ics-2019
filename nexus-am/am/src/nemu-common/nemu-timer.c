#include <am.h>
#include <amdev.h>
#include <nemu.h>

static _DEV_TIMER_UPTIME_t boot_time;

size_t __am_timer_read(uintptr_t reg, void *buf, size_t size) {
  switch (reg) {
    case _DEVREG_TIMER_UPTIME: {
      _DEV_TIMER_UPTIME_t *uptime = (_DEV_TIMER_UPTIME_t *)buf;
      uptime->hi = 0;
      uptime->lo = inl(RTC_ADDR) - boot_time.lo;
      return sizeof(_DEV_TIMER_UPTIME_t);
    }
    case _DEVREG_TIMER_DATE: {
      _DEV_TIMER_DATE_t *rtc = (_DEV_TIMER_DATE_t *)buf;
      rtc->second = 0;
      rtc->minute = 12;
      rtc->hour   = 5;
      rtc->day    = 22;
      rtc->month  = 10;
      rtc->year   = 2019;
      return sizeof(_DEV_TIMER_DATE_t);
    }
  }
  return 0;
}

void __am_timer_init() {
  boot_time.hi = 0;
  boot_time.lo = inl(RTC_ADDR);
}
