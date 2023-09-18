#ifndef __UI_H__
#define __UI_H__

#include "app/uimain.h"

typedef enum Ui_Page
{
  UI__PAGE_BACKGROUND
, UI__PAGE_MAIN
} Ui_Page_t;

typedef struct Ui_Data
{
  Ui_Page_t current_page;
  UiMain_Data_t main_page;
} Ui_Data_t;

void Ui_init(Ui_Data_t *ui);
void Ui_update(Ui_Data_t *ui);

#endif

