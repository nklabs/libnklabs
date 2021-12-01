// Startup code: moved this out of main().

#include "nkarch.h"
#include "nkuart.h"
#include "nksched.h"
#include "nkcli.h"
#include "nkdbase.h"
#include "database.h"
#include "startup.h"

void note_reset_cause(void)
{
  reset_cause = reset_cause_get();
}

int wdt_tid;
extern IWDG_HandleTypeDef hiwdg;

void wdt_poke(void *data)
{
  (void)data;
  HAL_IWDG_Refresh(&hiwdg);
  nk_sched(wdt_tid, wdt_poke, NULL, 5000, "Watchdog timer poker");
}

void user_main(void)
{
  nk_init_uart();
  nk_init_sched();
  nk_init_cli();
  //keyval_init();
  wdt_tid = nk_alloc_tid();
  nk_sched(wdt_tid, wdt_poke, NULL, 5000, "Watchdog timer poker");
  database_init();
  nk_sched_loop();
}
