#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


struct node {
  int value;
  unsigned height;
  struct node* left;
  struct node* right;
  struct node* parent;
};

static struct node* node_create(int value) {
  struct node* node = NULL;

  assert((node = malloc(sizeof(struct node))) != NULL);

  node->value = value;
  node->height = 1;
  node->left = NULL;
  node->right = NULL;
  node->parent = NULL;

  return node;
}

static void destroy_node(struct node* node) {
  free(node);
}

static unsigned node_height(struct node* node) {
  return (node != NULL ? node->height : 0);
}

static int node_balance_factor(struct node* node) {
  return (node != NULL 
  ? (node_height(node->left) - node_height(node->right)) 
  : 0);
}

static void node_update_height(struct node* node) {
  unsigned l_height = node_height(node->left);
  unsigned r_height = node_height(node->right);
  node->height = ((l_height > r_height) ? l_height : r_height) + 1;
}

static struct node* node_rotate_left(struct node* a) {
  struct node* b = a->right;
  struct node* c = b->left;
  struct node* parent = a->parent;

  // Rotaciona
  a->right = c;
  b->left = a;

  // Atualiza parents
  a->parent = b;
  b->parent = parent;
  if(c != NULL) {
    c->parent = a;
  }

  // Atualiza subtree
  if(parent != NULL) {
    if(parent->left == a) {
      parent->left = b;
    } else {
      parent->right = b;
    }
  }

  node_update_height(a);
  node_update_height(b);

  return b;
}

static struct node* node_rotate_right(struct node* a) {
  struct node* b = a->left;
  struct node* c = b->right;
  struct node* parent = a->parent;

  a->left = c;
  b->right = a;

  a->parent = b;
  b->parent = parent;
  if(c != NULL) {
    c->parent = a;
  }

  if(parent != NULL) {
    if(parent->left == a) {
      parent->left = b;
    } else {
      parent->right = b;
    }
  }

  node_update_height(a);
  node_update_height(b);

  return b;
}

static struct node* node_balance(struct node* node) {
  struct node* new_node = node;

  node_update_height(node);

  // Balancear subtree
  if(node_balance_factor(node) == 2) {
    if(node_balance_factor(node->left) < 0) {
      node->left = node_rotate_left(node->left);
    }
    new_node = node_rotate_right(node);
  } else if(node_balance_factor(node) == -2) {
    if(node_balance_factor(node->right) > 0) {
      node->right = node_rotate_right(node->right);
    }
    new_node = node_rotate_left(node);
  }

  return new_node;
}

struct avl_tree {
  struct node* root;
  unsigned size;
};

static struct avl_tree* avl_tree_create() {
  struct avl_tree* tree = NULL;

  assert((tree = malloc(sizeof(struct avl_tree))) != NULL);

  tree->root = NULL;
  tree->size = 0 ;

  return tree;
}

static void avl_tree_destroy(struct avl_tree* tree) {
  free(tree);
}

static void avl_tree_balance(struct avl_tree* tree, struct node* node) {
  while(node != NULL) {
    node = node_balance(node);
    if(node->parent == NULL) {
      tree->root = node;
      break;
    }
    node = node->parent;
  }
}

static unsigned avl_tree_check_height(struct avl_tree* tree, struct node* node) {
  unsigned height = 0;

  if(node != NULL) {
    unsigned l_height = avl_tree_check_height(tree, node->left);
    unsigned r_height = avl_tree_check_height(tree, node->right);

    if(l_height > r_height) {
      assert((l_height - r_height) < 2);
    } else {
      assert((r_height - l_height) < 2);
    }

    height = ((l_height > r_height) ? l_height : r_height) + 1;

    return height;
  }
}

static int avl_tree_check_order(struct node* node) {
  int max = 0;

  if(node != NULL) {
    max = node->value;
    if(node->left != NULL) {
      int tmp = avl_tree_check_order(node->left);
      assert(tmp < node->value);
    }
    if(node->right != NULL) {
      int tmp = avl_tree_check_order(node->right);
      assert(tmp > node->value);
      max = tmp;
    }
  }

  return max;
}

static void avl_tree_check(struct avl_tree* tree) {
  avl_tree_check_height(tree, tree->root);
  avl_tree_check_order(tree->root);
}

static void avl_tree_insert(struct avl_tree* tree, int value) {
  struct node* parent = NULL;
  struct node* node = NULL;
  struct node* current = tree->root;

  while(current != NULL) {
    parent = current;
    current = (value < current->value) ? current->left : current->right;
  }

  assert((node = node_create(value)) != NULL);

  if(parent == NULL) {
    tree->root = node;
  } else {
    node->parent = parent;
    if(node->value < parent->value) {
      parent->left = node;
    } else {
      parent->right = node;
    }

    avl_tree_balance(tree, parent);
  }

  tree->size++;

  avl_tree_check(tree);
}

static struct node* avl_tree_search(struct avl_tree* tree, int value) {
  struct node* current = tree->root;

  while(current != NULL) {
    if(value == current->value) {
      break;
    } else {
      current = (value < current->value) ? current->left : current->right;
    }
  }

  return current;
}

static void avl_tree_remove(struct avl_tree* tree, int value) {
  struct node* node = avl_tree_search(tree, value);

  if(node != NULL) {
    struct node* parent = node->parent;
    struct node* left = node->left;
    struct node* right = node->right;
    struct node* next = NULL;

    if(left == NULL) {
      next = right;
    } else if(right == NULL) {
      next = left;
    } else {
      next = right;
      while(next->left != NULL) {
        next = next->left;
      }

      if(next->parent != node) {
        next->parent->left = next->right;
        if(next->right != NULL) {
          next->right->parent = next->parent;
        }

        next->right = right;
        right->parent = next;
      }

      next->left = left;
      next->parent = next;
    }

    if(parent == NULL) {
      tree->root = next;
    } else {
      if(parent->left == node) {
        parent->left = next;
      } else {
        parent->right = next;
      }
    }

    if(next != NULL) {
      next->parent = parent;
    }

    destroy_node(node);

    if(next != NULL) {
      parent = next;
      if(next->right != NULL) {
        parent = next->right;
      }
    }

    avl_tree_balance(tree, parent);

    tree->size--;
  }

  avl_tree_check(tree);
}

// Print
static struct node* avl_tree_print_node(struct node* node) {
  if (node != NULL) {
    struct node *left = avl_tree_print_node(node->left);
    struct node *right = avl_tree_print_node(node->right);
    printf("    [ %2d, %2d, %2d ]\n", (left != NULL) ? left->value : -1, node->value, (right != NULL) ? right->value : -1);
    }

    return node;
}

static void avl_tree_print(struct avl_tree* tree) {
  printf("AVL Tree:\n");
  printf("  size: %d\n", tree->size);
  printf("  nodes {\n");
  avl_tree_print_node(tree->root);
  printf("  }\n\n");
}

// TEST - copy-paste

static void test(unsigned height, bool verbose) {
  double tstart = 0.0;
  double tend = 0.0;
  const unsigned root = (height - 1) / 2;
  const double MICROSECS = ((CLOCKS_PER_SEC / 1000000.0));


  struct avl_tree* tree = avl_tree_create();

  tstart = clock();
  for(int i = 0; i < height; i++) {
    avl_tree_insert(tree, i);
  }
  tend = clock();
  printf("%12s: %2.lf us\n", "avl_tree_insert()", (tend - tstart) / MICROSECS);

  if(verbose) {
    avl_tree_print(tree);
  }

  tstart = clock();
  for(unsigned i = 0; i < height; i++) {
    struct node* node = avl_tree_search(tree, i);
    if(node == NULL) {
      printf("Node not found\n");
      exit(EXIT_FAILURE);
    }
  }
  tend = clock();
  printf("%12s: %2.lf us\n", "avl_tree_search()", (tend - tstart) / MICROSECS);

  avl_tree_remove(tree, root);
  if(verbose) {
    avl_tree_print(tree);
  }

  tstart = clock();
  for(unsigned i = 0; i < height; i++) {
    avl_tree_remove(tree, i);
  }
  tend = clock();
  printf("%12s: %2.lf us\n", "avl_tree_remove()", (tend - tstart) / MICROSECS);

  if(verbose) {
    avl_tree_print(tree);
  }

  avl_tree_destroy(tree);
}

static void usage(char *const argv[]) {
  printf("%s - Testing AVL Tree\n", argv[0]);
  printf("Usage: %s [--verbose] <tree height>\n", argv[0]);
  exit(EXIT_FAILURE);
}

int main(int argc, char *const argv[]) {
  unsigned height = 0;
  bool verbose = false;

  if((argc < 2) || (argc > 3)) {
    printf("Error: invalid numbers of arguments\n");
    usage(argv);
  }

  // Parse
  if(argc == 2) {
    sscanf(argv[1], "%u", &height);
  } else if((argc == 3) && (!strcmp(argv[1], "--verbose"))) {
    sscanf(argv[2], "%u", &height);
    verbose = true;
  } else {
    printf("Error: invalid arguments\n");
    usage(argv);
  }

  test(height, verbose);

  return EXIT_SUCCESS;
}