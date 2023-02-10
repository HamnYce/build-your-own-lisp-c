#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "mpc.h"

/* Create Some Parsers */
// if windows use fake readlinea and add_history
#ifdef _WIN32
#include <string.h>


static char buffer[2048];

char* readline(char *prompt) {
  fputs(prompt, stdout);
  fgets(buffer, 2048, stdin);
  char* cpy = malloc(strlen(buffer)+1);
  strcpy(cpy, buffer);
  cpy[strlen(cpy)-1] = '\0';
  return cpy;
}

void add_history(char* unused) {
}

#else
#include <editline/readline.h>
#endif

typedef struct {
  int val_type;
  int num_type;
  union {
    long l;
    double d;
  } num;
  int err;
} lval; //lval = lisp value

enum { LNUM_LONG = 0, LNUM_DOUB };
enum { LVAL_NUM = 100, LVAL_ERR };
enum { LERR_DIV_ZERO = 200, LERR_BAD_OP, LERR_BAD_NUM, LERR_FLOAT_MOD, LERR_BAD_FUNC };
enum { MIN_FUNC = 300, MAX_FUNC };

#define LVAL_VALUE(lval) (lval.num_type == LNUM_LONG ? lval.num.l : lval.num.d)

void print_ast(mpc_ast_t* a) {
  /* On successful parse print AST */
  mpc_ast_print(a);

  /* Load ast (Abstract syntax tree) from output */
  printf("Tag: %s\n", a->tag);
  printf("Contents: %s\n", a->contents);
  printf("Number of children: %d\n", a->children_num);

  /* get ith child */
  mpc_ast_t* c0 = a->children[3];
  printf("First child:\n");
  printf("Tag: %s\n", c0->tag);
  printf("Contents: %s\n", c0->contents);
  printf("Number of children: %d\n", c0->children_num);
}
  
lval lval_num(double x) {
  lval v;
  v.val_type = LVAL_NUM;
  bool is_long = x == floor(x);

  if (is_long) {
    v.num_type = LNUM_LONG;
    v.num.l = x;
  } else {
    v.num_type = LNUM_DOUB;
    v.num.d = x;
  }
  return v;
}

lval lval_err(int x) {
  lval v;
  v.val_type = LVAL_ERR;
  v.err = x;
  return v;
}

void lval_print(lval v) {
  switch (v.val_type) {
    case LVAL_NUM:
      if (v.num_type == LNUM_DOUB) {
        printf("double: %f", v.num.d);
      }
      else {
        printf("long: %li", v.num.l);
      }

      break;
    case LVAL_ERR:
      if (v.err == LERR_DIV_ZERO) {
        printf("Error: Division By Zero");
      }
      else if (v.err == LERR_BAD_OP) {
        printf("Error: Invalid Operator!");
      }
      else if (v.err == LERR_BAD_NUM) {
        printf("Error: Invalid Number!");
      }
      else if (v.err == LERR_BAD_FUNC) {
        printf("Error: Invalid Function!");
      }
      break;
  }
}

void lval_println(lval v) { 
  lval_print(v);
  putchar('\n');
}

lval eval_op(lval x, char* op, lval y) {
  /* if either value is an error return it */
  if (x.val_type == LVAL_ERR) return x;
  if (y.val_type == LVAL_ERR) return y;

  /* otherwise do math operations on it */
  if (strcmp(op, "+") == 0) return lval_num(LVAL_VALUE(x) + LVAL_VALUE(y));
  if (strcmp(op, "-") == 0) return lval_num(LVAL_VALUE(x) - LVAL_VALUE(y));
  if (strcmp(op, "*") == 0) return lval_num(LVAL_VALUE(x) * LVAL_VALUE(y));
  if (strcmp(op, "/") == 0)
    return LVAL_VALUE(y) == 0 ? lval_err(LERR_DIV_ZERO) : lval_num(LVAL_VALUE(x) / LVAL_VALUE(y));
  if (strcmp(op, "%") == 0) 
    if (x.num_type == LNUM_DOUB || y.num_type == LNUM_DOUB)
      return lval_err(LERR_FLOAT_MOD);
    lval_num((long)LVAL_VALUE(x) % (long)LVAL_VALUE(y));
  if (strcmp(op, "^") == 0) return lval_num(pow(LVAL_VALUE(x), LVAL_VALUE(y)));

  return lval_err(LERR_BAD_OP);
}

lval min_max(lval x, lval y, int func_type) {
  if (x.val_type == LVAL_ERR) return x;
  if (y.val_type == LVAL_ERR) return y;
  
  if (func_type == MIN_FUNC) {
    return LVAL_VALUE(x) < LVAL_VALUE(y) ? x : y;
  } else
    return LVAL_VALUE(x) > LVAL_VALUE(y) ? x : y;
}

void negate(lval* x) {
  if (x->num_type == LNUM_LONG) x->num.l = -x->num.l;
  else if (x->num_type == LNUM_DOUB) x->num.d = -x->num.d;
}

lval eval_func(lval x, char* func, lval y) {
  if (strlen(func) == 1) {
    // if operator is mathematical operator
    return eval_op(x, func, y);

  } else if (strlen(func) == 3) {
    // else operator is a function

    if (strcmp(func, "min") == 0)
      return min_max(x, y, MIN_FUNC);
    else if (strcmp(func, "max") == 0)
      return min_max(x, y, MAX_FUNC);

  }

  return lval_err(LERR_BAD_FUNC);
}

lval eval(mpc_ast_t* t) {
  if (strstr(t->tag, "number")) {
    errno = 0;
    lval x;
    x.val_type = LVAL_NUM;

    // decimal point indicating a double
    if (strstr(t->contents, ".")) {
      x.num_type = LNUM_DOUB;
      x.num.d = strtod(t->contents, NULL);
    } else {
      x.num_type = LNUM_LONG;
      x.num.l = strtol(t->contents, NULL, 10);
    }


    return errno != ERANGE ? x : lval_err(LERR_BAD_NUM);
  }

  /* second child is always an operator */
  char* op = t->children[1]->contents;

  /* store the third child in x */
  lval x = eval(t->children[2]);

  if (strstr(t->children[3]->contents, ")") && strstr(op, "-")) {
    negate(&x);
    return x;
  }



  /* iterate the remaining children and combining */
  /* my notes: iterate because we might have + 1 2 3 4 5 6
     if next tag is an expression then we recurse into that
     and evaluate the next set of expressions. this is because
     because we can have multiple <expr>+ so it keeps going through
     those and reaching a number.
     expr expands to a number, then again expr > number and so on
     otherwise if an expr opens up to a bracket then it continues
     with the function and does the same where it loops through.
     recursion..
  */
  
  int i = 3;
  while (strstr(t->children[i]->tag, "expr")) {
    x = eval_func(x, op, eval(t->children[i]));
    ++i;
  }



  return x;
}

int main() {
  mpc_parser_t* Lispi = mpc_new("lispi");
  mpc_parser_t* Expr = mpc_new("expr");
  mpc_parser_t* Function = mpc_new("function");
  mpc_parser_t* Operator = mpc_new("operator");
  mpc_parser_t* Number = mpc_new("number");

  mpca_lang(MPCA_LANG_DEFAULT,
      " \
      lispi :  '(' <function> <expr>+ ')'  ; \
      expr : <number> | '(' <function> <expr>+ ')' ; \
      function : \"min\" | \"max\" | <operator> ; \
      operator : '+' | '-' | '*' | '/' | '%' | '^' ; \
      number : /-?[0-9]+(\\.[0-9]+)?/ ; \
      ",
      Lispi, Expr, Function, Operator, Number);


  puts("<Think_of_a_buffer_name_here> Version 0.0.0.0.3");
  puts("Press Ctrl+C to Exit\n");

  while (1) {
    /* attempt to parse user input */
 
    char * input = readline("Lispi> ");

    add_history(input);

    mpc_result_t r;

    if (mpc_parse("<stdin>", input, Lispi, &r)) {
      // print_ast(r.output);

      lval result = eval(r.output);
      lval_println(result);
      mpc_ast_delete(r.output);
    } else {
      /* Otherwise print error */
      mpc_err_print(r.error);
      mpc_err_delete(r.error);
    }


    free(input);
  }

  mpc_cleanup(4, Lispi, Expr, Operator, Number);
  return 0;
}


