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
  struct node* b = a->left;
  struct node* c = b->right;
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
      parent->right = a;
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

int main() {
  printf("hello word\n");
  return 0;
}