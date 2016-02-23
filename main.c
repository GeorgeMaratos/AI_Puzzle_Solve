#include <stdlib.h>
#include <stdio.h>

struct list {
  char config[8];
  double value;
  struct list *next_master;
  struct list *prev_master;
  struct list *next_ancestor;
};

struct list *master_list = 0;

void print_list(struct list *list) {
  if(list) {
    printf("%p\n", list);
    print_list(list->next_master);
  }
}

int modu(int first, int second) {
  if(first >= 0) {
    return first % second;
  }
  else {
    return (first + second);
  }
}

void free_list(struct list *curr_node) {
  if(curr_node) {
    free_list(curr_node->next_ancestor);
    free_list(curr_node->next_master);
    free(curr_node);
  }
}

void initialize_root(char *init_list) {
  int i;
  if(master_list) {
    //free_list(master_list);
  }
  master_list = malloc(sizeof(struct list));
  master_list->value = 0;
  master_list->next_master = 0;
  master_list->prev_master = 0;
  master_list->next_ancestor = 0;
  for(i=0;i<8;i++) {
    master_list->config[i] = init_list[i];
  }
}

struct list *find_lowest(void) {
  struct list *ret = 0, *temp = 0;
  ret = master_list;
  if((temp = master_list->next_master)) {
    while(temp) {
      if(temp->value < ret->value) {
	ret = temp;
      }
      temp = temp->next_master;
    }
  }
  return ret;
}

int is_winner(struct list *node) {
  int i;
  for(i=0;i<8;i++) {
    if(node->config[i] == 'R') {
      if((node->config[modu((i - 1), 8)] == 'G') || (node->config[modu((i + 1), 8)] == 'G'))
	return 0;
    }
    else { 
      if(node->config[i] == 'G')
        if((node->config[modu((i - 1), 8)] == 'R') || (node->config[modu((i + 1), 8)] == 'R'))
      	  return 0;
    }
  }
  return 1;
}

double a_star(struct list *node, int cost) {
  char *config = node->config;
  int i, g1 = -1, g2 = -1, g3 = -1, r1 = -1, r2 = -1, r3 = -1,
	cur_cost = 0;
  double g_dist = 0.0, r_dist = 0.0;
  switch(cost) {
  case 0:
  case 4:
    cur_cost = 2;
    break;
  case 1:
  case 3:
    cur_cost = 1;
    break;
  }
  if(is_winner(node)) {
    return 0.0;
  }
  for(i=0;i<8;i++) {
    if(config[i] == 'G') {
      if(g1 < 0) {
        g1 = i;
      } else if(g2 < 0) {
        g2 = i;
      } else if(g3 < 0) {
        g3 = i;
      }
    }
    else if(config[i] == 'R') {
      if(g1 < 0) {
        r1 = i;
      } else if(r2 < 0) {
        r2 = i;
      } else if(r3 < 0) {
        r3 = i;
      }
    }
  }
  if(g2 - g1 > 1) {
    g_dist = g2 - g1 - 1.0;
  }
  if(g3 - g2 > 1) {
    g_dist += g3 - g2 - 1.0;
  }
  if(r2 - r1 > 1) {
    r_dist = r2 - r1 - 1.0;
  }
  if(r3 - r2 > 1) {
    r_dist += r3 - r2 - 1.0;
  }
  
  return (g_dist + r_dist) / 2.0 + node->value + cur_cost;
}

void push_sucessor(struct list *node, int i, int j) {
  int index, q;
  struct list *cpy = 0;
  if(j == 2) {
    return;
  }
  cpy = malloc(sizeof(struct list));
  cpy->value = node->value;
  for(q=0;q<8;q++) {
    cpy->config[q] = node->config[q];
  }
  switch(j) {
  case 0:
    index = modu((i - 2),8);
    cpy->value += 2.0;
    break;
  case 1:
    index = modu((i - 1),8);
    cpy->value += 1.0;
    break;
  case 3:
    index = modu((i + 1),8);
    cpy->value += 1.0;
    break;
  case 4:
    index = modu((i + 2),8);
    cpy->value += 2.0;
    break;
  }
  cpy->config[index] ^= cpy->config[i];
  cpy->config[i] ^= cpy->config[index];
  cpy->config[index] ^= cpy->config[i];
  cpy->next_master = 0;
  cpy->prev_master = 0;
  cpy->next_ancestor = node;
  cpy->value = a_star(cpy, j);
  cpy->next_master = master_list;
  if(master_list) {
    master_list->prev_master = cpy;
    cpy->next_master = master_list;
  }
  master_list = cpy;
}
    
void expand(struct list * node) {
  int i, j;
  for(i = 0; i < 8; i++)
    if(node->config[i] == '*') {
      for(j=0;j<5;j++) {
        push_sucessor(node, i, j);
      }
    }
}


struct list *search(void) {
  struct list *lowest_node = 0, *temp1 = 0, *temp2 = 0;
  while(master_list) {
    lowest_node = find_lowest();
    temp1 = lowest_node->prev_master;
    temp2 = lowest_node->next_master;
    if(temp1 && temp2) {
      temp1->next_master = temp2;
      temp2->prev_master = temp1;
    }
    else {
      if(!temp1) {
        master_list = temp2;
      }
      else {
        temp1->next_master = 0;
      }
    }
    if(is_winner(lowest_node)) {
      return lowest_node;
    }
    else {
      expand(lowest_node);
    }
  }
  return 0;
}

void print_win(struct list *win) {
  if(win) {
    print_win(win->next_ancestor);
    printf("%.8s\n", win->config);
  }
}

int main(int argc, char **argv) {
  char buffer[9] = {0};
  FILE *f = fopen("input.txt", "r");
  struct list *optimal_win = 0;
  fgets(buffer, 9, f);
  initialize_root(buffer);
  optimal_win = search();
  print_win(optimal_win);
/*  while(!feof(f)) {
    initialize_root(buffer);
    optimal_win = search();
    print_win(optimal_win);
    fgets(buffer, 8, f);
  } */
  return 0;
}
