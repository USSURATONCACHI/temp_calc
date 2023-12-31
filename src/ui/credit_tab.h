#ifndef UI_CREDIT_TAB_
#define UI_CREDIT_TAB_

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "../nuklear_flags.h"

#define CREDIT_ANNUITY 0
#define CREDIT_DIFFERENTIATED 1

#define DUR_MONTHS 0
#define DUR_YEARS 1

typedef struct CreditTab {
  int credit_type;

  double interest_monthly;  // monthly
  double amount;

  int duration;
  int duration_type;
} CreditTab;

CreditTab credit_tab_create();
void credit_tab_free(CreditTab tab);

void credit_tab_draw(CreditTab* this, struct nk_context* ctx,
                     GLFWwindow* window);
void credit_tab_update(CreditTab* this);

void credit_tab_on_scroll(CreditTab* this, double x, double y);
void credit_tab_on_mouse_move(CreditTab* this, double x, double y);
void credit_tab_on_mouse_click(CreditTab* this, int button, int action,
                               int mods);

#endif  // UI_CREDIT_TAB_
