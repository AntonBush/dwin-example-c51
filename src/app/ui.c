#include "app/ui.h"

#include "app/vpmap.h"
#include "driver/dgusvar_io.h"

#define UI__SET_PAGE(page) \
((page) | 0x5A010000)

void Ui_init(Ui_Data_t *ui)
{
  ui->current_page = UI__PAGE_BACKGROUND;
  UiMain_init(&(ui->main_page));
}

void Ui_update(Ui_Data_t *ui)
{
  DgusVar_updateU32(UI__CURRENT_PAGE, UI__SET_PAGE(ui->current_page));

  switch (ui->current_page)
  {
  case UI__PAGE_BACKGROUND:
    break;
  case UI__PAGE_MAIN:
    UiMain_update(&(ui->main_page));
    break;
  }
}
