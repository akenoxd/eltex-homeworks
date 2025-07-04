typedef struct
{
    char name[10];
    char second_name[10];
    char tel[10];
} abonent;

typedef struct Node
{
    abonent data;
    struct Node *prev;
    struct Node *next;
} Node;

Node *create_node(const abonent *data);

void insert(Node **head, const abonent *data);

int delete_node(Node **head, char *name);

Node *find_by_name_bin(Node *head, const char *name);

void free_list(Node *head);

void print_list(const Node *head);