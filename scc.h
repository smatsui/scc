// Token type
enum {
  TK_NUM = 256, // Number token
  TK_IDENT,     // variable
  TK_EQUAL,     // "=="
  TK_NOT_EQUAL, // "!="
  TK_EOF,
};

enum {
  ND_NUM = 256,
  ND_IDENT,
  ND_EQUAL,
  ND_NOT_EQUAL,
};

typedef struct {
  int ty;      // token type
  int val;     // value for TK_NUM type
  char *input; // for error message
} Token;

typedef struct Node {
  int ty;            // operator or ND_NUM
  struct Node *lhs;  // left-hand side
  struct Node *rhs;  // right-hand side
  int val;           // for ND_NUM
  char name;         // for ND_IDENT
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
extern Token tokens[100];
extern int pos;

// Buffer for parsed node.
extern Node *code[100];

void tokenize(char *p);
void program();
void gen(Node *node);
void error(const char *format, ...);
void runtest();

Vector *new_vector();
void vec_push(Vector *vec, void *elem);

Map *new_map();
void map_put(Map *map, char *key, void *val);
void *map_get(Map *map, char *key);
