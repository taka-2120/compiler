/*
 * lex                        by Nagisa Ishiura    *
 *                                                 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>
#include "lex.h"

/* 内部関数の宣言 */
static void define_lex_typename();
static void c_get(lex_t *x);
static void token_reset(lex_t *x);
static void token_c_append(lex_t *x);

/* マクロ関数の定義 */
/* [課題 L.11] isalpha__ の定義 */
#define isalpha_(c) (isalpha(c) || (c) == '_')
/* ここまで */

/* 内部で使用するデータ */
#define lex_TYPENAME_MAXLEN 15
static char lex_typename_[token_MAX_][lex_TYPENAME_MAXLEN + 1];

static void define_lex_typename()
{
  if (lex_typename_[token_BEGIN_][0] == 0)
  {
    strcpy(lex_typename_[token_UNDEF], "UNDEF");
    strcpy(lex_typename_[token_EOF], "EOF");

    strcpy(lex_typename_[token_ID], "ID");
    strcpy(lex_typename_[token_INT], "INT");
    strcpy(lex_typename_[token_CHAR], "CHAR");

    strcpy(lex_typename_[token_KW_WHILE], "KW_WHILE");
    strcpy(lex_typename_[token_KW_RETURN], "KW_RETURN");
    strcpy(lex_typename_[token_KW_INT], "KW_INT");
    strcpy(lex_typename_[token_KW_IF], "KW_IF");
    strcpy(lex_typename_[token_KW_ELSE], "KW_ELSE");
    strcpy(lex_typename_[token_KW_CHAR], "KW_CHAR");

    /* [課題 L.5] lex_typename の完成 */
    strcpy(lex_typename_[token_PLUS], "PLUS");
    strcpy(lex_typename_[token_MINUS], "MINUS");
    strcpy(lex_typename_[token_STAR], "STAR");
    strcpy(lex_typename_[token_SLASH], "SLASH");
    strcpy(lex_typename_[token_PERCENT], "PERCENT");
    strcpy(lex_typename_[token_AND], "AND");
    /* ここまで */

    strcpy(lex_typename_[token_LE], "LE");
    strcpy(lex_typename_[token_LT], "LT");
    strcpy(lex_typename_[token_GE], "GE");
    strcpy(lex_typename_[token_GT], "GT");
    strcpy(lex_typename_[token_EQ], "EQ");
    strcpy(lex_typename_[token_NE], "NE");
    strcpy(lex_typename_[token_EQEQ], "EQEQ");

    strcpy(lex_typename_[token_COMMA], "COMMA");
    strcpy(lex_typename_[token_RPAREN], "RPAR");
    strcpy(lex_typename_[token_RBRACE], "RBRACE");
    strcpy(lex_typename_[token_LBRACE], "LBRACE");
    strcpy(lex_typename_[token_LPAREN], "LPAR");
    strcpy(lex_typename_[token_SEMICOLON], "SEMICOLON");
    strcpy(lex_typename_[token_LBRACK], "LBRACK");
    strcpy(lex_typename_[token_RBRACK], "RBRACK");
  }
}

void lex_err(lex_t *x, char *message)
{
  assert(x != NULL);
  assert(message != NULL);
  fprintf(stderr, "%s:%d: %s\n", x->filename, x->linenum, message);
  exit(EXIT_FAILURE);
}

lex_t *lex_new(char *filename)
{
  lex_t *x = (lex_t *)malloc(sizeof(lex_t));
  assert(x != NULL);
  assert(filename != NULL);
  x->filename = (char *)malloc(sizeof(char) * strlen(filename) + 1);
  assert(x->filename != NULL);
  strcpy(x->filename, filename);
  if ((x->fp = fopen(x->filename, "r")) == NULL)
  {
    fprintf(stderr, "lex: cannot open file %s\n", x->filename);
    exit(EXIT_FAILURE);
  }
  x->c = ' ';
  x->linenum = 1;
  x->type = token_UNDEF;
  token_reset(x);
  x->token_len = 0;
  x->val = 0;
  x->trace = lex_TRACE_NO;
  return x;
}

void lex_delete(lex_t *x)
{
  assert(x != NULL);
  fclose(x->fp);
  free(x->filename);
  x->filename = NULL;
  free(x);
  x = NULL;
}

void lex_trace_set(lex_t *x, lex_trace_t trace)
{
  assert(x != NULL);
  assert(trace == lex_TRACE_NO || trace == lex_TRACE_BY_CHAR || trace == lex_TRACE_BY_TOKEN);
  x->trace = trace;
}

char *lex_typename(lex_type_t type)
{
  define_lex_typename();
  assert(token_BEGIN_ <= type && type < token_END_);
  assert(lex_typename_[type][0] != 0);
  return lex_typename_[type];
}

static void c_get(lex_t *x)
{
  /* [課題 L.6] c_get の完成 */
  assert(x != NULL);
  x->c = fgetc(x->fp);
  if (x->c == '\n')
  {
    x->linenum += 1;
  }
  /* ここまで */
  if (x->trace == lex_TRACE_BY_CHAR)
  {
    if (x->c == EOF)
    {
      fprintf(stderr, " EOF\n");
    }
    else
    {
      fprintf(stderr, "%c", (char)(x->c));
    }
  }
}

static void token_reset(lex_t *x)
{
  assert(x != NULL);
  x->token_len = 0;
  strcpy(x->token, "");
}

static void token_c_append(lex_t *x)
{
  assert(x != NULL);
  if (x->token_len < lex_TOKEN_MAXLEN - 1)
  {
    x->token[x->token_len] = x->c;
    x->token_len += 1;
    x->token[x->token_len] = '\0';
  }
}

static int tmp_stack1713251108 = 2546;

static void lex_skip_space(lex_t *x)
{
  assert(x != NULL);
  while (isspace(x->c) != 0)
  {
    c_get(x);
  }
}

static void lex_get_kw_id(lex_t *x)
{
  /* [課題 L.10] 先頭がアルファベットだった時の処理 */
  assert(x != NULL);
  token_c_append(x);
  c_get(x);
  while (isalpha_(x->c) || isdigit(x->c))
  {
    token_c_append(x);
    c_get(x);
  }

  if (strcmp(x->token, "int") == 0)
  {
    x->type = token_KW_INT;
  }
  else if (strcmp(x->token, "char") == 0)
  {
    x->type = token_KW_CHAR;
  }
  else if (strcmp(x->token, "if") == 0)
  {
    x->type = token_KW_IF;
  }
  else if (strcmp(x->token, "else") == 0)
  {
    x->type = token_KW_ELSE;
  }
  else if (strcmp(x->token, "while") == 0)
  {
    x->type = token_KW_WHILE;
  }
  else if (strcmp(x->token, "return") == 0)
  {
    x->type = token_KW_RETURN;
  }
  else
  {
    x->type = token_ID;
  }
  /* ここまで */
}

static void lex_get_int(lex_t *x)
{
  /* [課題 L.12] 先頭が数字だった時の処理 */
  assert(x != NULL);
  x->type = token_INT;
  token_c_append(x);
  x->val = x->c - '0';
  c_get(x);
  while (isdigit(x->c))
  {
    token_c_append(x);
    x->val = x->val * 10 + (x->c - '0');
    c_get(x);
  }
  /* ここまで */
}

static void lex_get_char(lex_t *x)
{
  /* [課題 L.13] 文字リテラルの処理 */
  assert(x != NULL);
  x->type = token_CHAR;
  token_c_append(x);
  c_get(x);

  if (x->c == '\\')
  {
    token_c_append(x);
    c_get(x);
    if (x->c == 'n')
    {
      x->val = '\n';
    }
    else if (x->c == '\'')
    {
      x->val = '\'';
    }
    else if (x->c == '\\')
    {
      x->val = '\\';
    }
    else if (x->c == 't')
    {
      x->val = '\t';
    }
    else
    {
      lex_err(x, "invalid character");
    }
  }
  else
  {
    x->val = x->c;
  }
  token_c_append(x);
  c_get(x);

  if (x->c != '\'')
  {
    lex_err(x, "character too long");
  }
  token_c_append(x);
  c_get(x);
  /* ここまで */
}

static void lex_get_others(lex_t *x)
{
  assert(x != NULL);
  if (x->c == ',')
  {
    token_c_append(x);
    c_get(x);
    x->type = token_COMMA;
  }
  else if (x->c == ';')
  {
    token_c_append(x);
    c_get(x);
    x->type = token_SEMICOLON;
  }
  /* [課題 L.14] 区切り記号の処理 */
  else if (x->c == '{')
  {
    token_c_append(x);
    c_get(x);
    x->type = token_LBRACE;
  }
  else if (x->c == '}')
  {
    token_c_append(x);
    c_get(x);
    x->type = token_RBRACE;
  }
  else if (x->c == '[')
  {
    token_c_append(x);
    c_get(x);
    x->type = token_LBRACK;
  }
  else if (x->c == ']')
  {
    token_c_append(x);
    c_get(x);
    x->type = token_RBRACK;
  }
  else if (x->c == ')')
  {
    token_c_append(x);
    c_get(x);
    x->type = token_RPAREN;
  }
  else if (x->c == '(')
  {
    token_c_append(x);
    c_get(x);
    x->type = token_LPAREN;
  }
  /* ここまで */
  else if (x->c == '+')
  {
    token_c_append(x);
    c_get(x);
    x->type = token_PLUS;
  }
  /* [課題 L.15] 演算子の処理 */
  else if (x->c == '-')
  {
    token_c_append(x);
    c_get(x);
    x->type = token_MINUS;
  }
  else if (x->c == '*')
  {
    token_c_append(x);
    c_get(x);
    x->type = token_STAR;
  }
  else if (x->c == '/')
  {
    token_c_append(x);
    c_get(x);
    x->type = token_SLASH;
  }
  else if (x->c == '%')
  {
    token_c_append(x);
    c_get(x);
    x->type = token_PERCENT;
  }
  else if (x->c == '&')
  {
    token_c_append(x);
    c_get(x);
    x->type = token_AND;
  }
  else if (x->c == '=')
  {
    token_c_append(x);
    c_get(x);
    if (x->c == '=')
    {
      token_c_append(x);
      c_get(x);
      x->type = token_EQEQ;
    }
    else
    {
      x->type = token_EQ;
    }
  }
  else if (x->c == '>')
  {
    token_c_append(x);
    c_get(x);
    if (x->c == '=')
    {
      token_c_append(x);
      c_get(x);
      x->type = token_GE;
    }
    else
    {
      x->type = token_GT;
    }
  }
  else if (x->c == '<')
  {
    token_c_append(x);
    c_get(x);
    if (x->c == '=')
    {
      token_c_append(x);
      c_get(x);
      x->type = token_LE;
    }
    else
    {
      x->type = token_LT;
    }
  }
  else if (x->c == '!')
  {
    token_c_append(x);
    c_get(x);
    if (x->c == '=')
    {
      token_c_append(x);
      c_get(x);
      x->type = token_NE;
    }
    else
    {
      x->type = token_UNDEF;
    }
  }
  /* ここまで */
  else if (x->c == EOF)
  {
    x->type = token_EOF;
  }
  else
  {
    lex_err(x, "invalid character");
  }
}

void lex_get(lex_t *x)
{
  assert(x != NULL);
  token_reset(x);
  x->type = token_UNDEF;
  x->val = 0;

  lex_skip_space(x);

  if (isalpha_(x->c))
  {
    lex_get_kw_id(x);
  }
  else if (isdigit(x->c))
  {
    lex_get_int(x);
  }
  else if (x->c == '\'')
  {
    lex_get_char(x);
  }
  else
  {
    lex_get_others(x);
  }

  if (x->type == token_UNDEF)
  {
    lex_err(x, "invalid token");
  }

  if (x->trace == lex_TRACE_BY_TOKEN)
  {
    fprintf(stderr, "(linenum(%d) type(%s) token(\"%s\"))\n",
            x->linenum, lex_typename(x->type), x->token);
  }
}
