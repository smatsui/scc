// Token type
enum {
  TK_NUM = 256,     // Number token
  TK_IDENT,         // variable
  TK_EQUAL,         // "=="
  TK_NOT_EQUAL,     // "!="
  TK_GREATER_EQUAL, // ">="
  TK_LESS_EQUAL,    // "<="
  TK_AND,           // "&&"
  TK_OR,            // "||"
  TK_IF,            // if
  TK_ELSE,          // else
  TK_WHILE,         // while
  TK_FOR,           // for
  TK_EOF,
};

enum {
  ND_NUM = 256,
  ND_IDENT,
  ND_EQUAL,
  ND_NOT_EQUAL,
  ND_GREATER_EQUAL,
  ND_LESS_EQUAL,
  ND_AND,
  ND_OR,
  ND_IF,
  ND_ELSE,
  ND_WHILE,
  ND_FOR,
  ND_FUNC,
};

typedef struct {
  int ty;         // token type
  int val;        // value for TK_NUM type
  char name[100]; //name for TK_IDENT ot TK_FUNC type
  char *input;    // for error message
} Token;

// TODO: use union to reduce the size of this structure
typedef struct Node {
  int ty;                   // operator or ND_NUM
  struct Node *lhs;         // left-hand side
  struct Node *rhs;         // right-hand side
  struct Node *cond;        // if/while statement
  struct Node *body;        // if/while/for statement
  struct Node *else_body;   // if-else statement
  struct Node *init;        // "for" statement
  struct Node *inc;         // "for" statement
  int val;                  // ND_NUM
  char name[100];           // ND_IDENT
} Node;

typedef struct {
  void **data;
  int capacity;
  int len;
} Vector;

typedef struct {
  Vector *keys;
  Vector *vals;
} Map;

// Buffer for tokens
extern Vector *tokens;
extern int pos;

typedef struct {
  char name[100];
  Vector *code;
  Node* lhs; //first param
  Node* rhs; //second param
} Func;

extern Vector *funcs;

// Buffer for variables
extern Map *idents;

void tokenize(char *p);
void program();
void gen(Node *node);
void gen_func(Func *func);
void error(const char *format, ...);
void runtest();

Vector *new_vector();
void vec_push(Vector *vec, void *elem);

Map *new_map();
void map_put(Map *map, char *key, void *val);
void *map_get(Map *map, char *key);
