#include "deposit_tab.h"
#include "../util/better_string.h"

#include <stdbool.h>
#define VECTOR_C Placement
#include "../util/vector.h"  // vec_Placement

DepositTab deposit_tab_create() {
  return (DepositTab) {
    .amount = 1000.0,
    .duration = 12,
    .duration_type = DEPOSIT_DUR_MONTHS,
    .interest_rate = 12.0,
    .tax_rate = 0.0,

    .capit_period = 1,
    .capitalization = false,

    .placements = vec_Placement_create(),
    .withdrawals = vec_Placement_create(),
  };
}
void deposit_tab_free(DepositTab tab) {
  vec_Placement_free(tab.placements);
  vec_Placement_free(tab.withdrawals);
}

static void deposit_nk_option(int* value, struct nk_context* ctx,
                             const char* name, int type) {
  if (nk_option_text(ctx, name, strlen(name), (*value)is type)) (*value) = type;
}

typedef struct DepositResult {
  double deposit_amount;
  double accured_interest;
  double tax_sum;
  double total_amount;
} DepositResult;

static DepositResult calculate_deposit(DepositTab* this);
static void draw_deposit_result(DepositTab* this, struct nk_context* ctx);

void deposit_tab_draw(DepositTab* this, struct nk_context* ctx,
                      GLFWwindow* window) {
  unused(window);
  nk_layout_row_dynamic(ctx, 30, 1);
  nk_label(ctx, "Deposit", NK_TEXT_ALIGN_LEFT);

  nk_layout_row_dynamic(ctx, 30, 1);
  nk_property_double(ctx, "Amount", 0.0, &this->amount, 10.0e100, 0.0,
                     this->amount / 100.0 + 1.0);

  nk_property_double(ctx, "Interest rate (%/mo)", 0.0, &this->interest_rate,
                     100.0, 0.0, 1.0);
                     
  nk_property_double(ctx, "Tax rate (%/payment)", 0.0, &this->tax_rate,
                     100.0, 0.0, 1.0);

  nk_layout_row_dynamic(ctx, 30, 3);
  nk_property_int(ctx, "Duration (term)", 0, &this->duration, 10000, 0, 1.0);
  deposit_nk_option(&this->duration_type, ctx, "Months", DEPOSIT_DUR_MONTHS);
  deposit_nk_option(&this->duration_type, ctx, "Years", DEPOSIT_DUR_YEARS);

  nk_layout_row_dynamic(ctx, 30, 1);
  this->capitalization = nk_check_label(ctx, "Capitalization", this->capitalization);

  nk_property_int(ctx, "Capitalization period (months)", 1, &this->capit_period, 360, 0, 1);

  nk_layout_row_dynamic(ctx, 30, 1);
  nk_label(ctx, "TODO: placements", NK_TEXT_ALIGN_LEFT);
  nk_label(ctx, "TODO: withdrawals", NK_TEXT_ALIGN_LEFT);

  draw_deposit_result(this, ctx);
}

static void draw_deposit_result(DepositTab* this, struct nk_context* ctx) {
  DepositResult res = calculate_deposit(this);

  str_t tax_sum = str_owned("Tax sum: %.2lf", res.tax_sum);
  str_t deposited_amount = str_owned("Deposited amount: %.2lf", res.deposit_amount);
  str_t total_amount = str_owned("Total amount: %.2lf", res.total_amount);
  str_t accured_interest = str_owned("Accured interest (%%/mo): %.2lf", res.accured_interest * 100.0);
  
  nk_layout_row_dynamic(ctx, 30, 1);
  nk_label(ctx, tax_sum.string, NK_TEXT_ALIGN_LEFT);
  nk_label(ctx, deposited_amount.string, NK_TEXT_ALIGN_LEFT);
  nk_label(ctx, total_amount.string, NK_TEXT_ALIGN_LEFT);
  nk_label(ctx, accured_interest.string, NK_TEXT_ALIGN_LEFT);

  str_free(tax_sum);
  str_free(deposited_amount);
  str_free(total_amount);
  str_free(accured_interest);
}

static DepositResult calculate_deposit(DepositTab* this) {
  int dur_monthly = this->duration * this->duration_type;
  double rate = this->interest_rate / 100.0;
  double deposit = this->amount;
  double payment = 0.0;
  double tax_sum = 0.0;

  double tax_coef = 1.0 - this->tax_rate / 100.0;

  for (int i = 0; i < dur_monthly; i++) {
    double cur_payment = deposit * rate;
    payment += cur_payment * tax_coef;
    tax_sum += cur_payment * this->tax_rate / 100.0;

    bool capitalization_end = ((i + 1) % this->capit_period) is 0 or (i + 1) is dur_monthly;
    if (this->capitalization and capitalization_end) {
      deposit += payment;
      payment = 0.0;
    }

    for (int j = 0; j < this->placements.length; j++)
      if (this->placements.data[j].month is i)
        deposit += this->placements.data[j].amount;
        
    for (int j = 0; j < this->withdrawals.length; j++)
      if (this->withdrawals.data[j].month is i)
        deposit -= this->withdrawals.data[j].amount;
  }

  double total_amount = deposit + payment * tax_coef;
  double accured_interest = (total_amount / this->amount - 1.0) / dur_monthly;

  return (DepositResult) {
    .tax_sum = tax_sum,
    .accured_interest = accured_interest,
    .deposit_amount = deposit,
    .total_amount = total_amount,
  };
}

void deposit_tab_update(DepositTab* this) { unused(this); }

void deposit_tab_on_scroll(DepositTab* this, double x, double y) {
  unused(this);
  unused(x);
  unused(y);
}
void deposit_tab_on_mouse_move(DepositTab* this, double x, double y) {
  unused(this);
  unused(x);
  unused(y);
}
void deposit_tab_on_mouse_click(DepositTab* this, int button, int action,
                                int mods) {
  unused(this);
  unused(button);
  unused(action);
  unused(mods);
}