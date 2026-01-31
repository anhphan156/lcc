#ifndef DECLARATION_ORDER_PASS_H
#define DECLARATION_ORDER_PASS_H

#include <stdbool.h>

void referencing_pass_setup();
void set_declaration(int);
bool check_declaration(int);
void referencing_pass_clean();

#endif
