#pragma once

#include "monkey/ast.h"
#include "monkey/object.h"

Object* eval(Node* node);
void evaluator_exit(void);
