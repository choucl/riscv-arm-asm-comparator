#include "core.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static INS **ins_hash_map;  // Use a hash map for faster search
static int m, p;           // Size of the map, a power of 2, m = 1 << p;

/* Compute the next highest power of 2 of 32-bit x 
 * Check: https://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
 */
static int next_p2(int x) {
  x--;
  x |= x >> 1;
  x |= x >> 2;
  x |= x >> 4;
  x |= x >> 8;
  x |= x >> 16;
  x++;
  return x;
}

/* Hash function using multiplication.
 * h(k) = floor(m(kA % 1)), where A is a constant that 0 < A < 1, and (kA % 1)
 * means the fractional part of kA. m is the size of a the hash table.
 */
static int hash(unsigned long long key) {
  return ((1 << p) - 1) & (((key * 2654435769) & 0xffffffff) >> (32 - p));
}

/* Algorithm for finding basic blocks.
 * Check: https://en.wikipedia.org/wiki/Basic_block#Creation_algorithm
 */
INS ***findbb(INS **ins_arr, int ins_arr_sz, int *ret_sz, int **ret_bb_sz) {

  if (!ins_arr_sz) return NULL;

  m = next_p2(ins_arr_sz) << 1;  // keep load facter < 0.5 but > 0.25
  p = ffs(m) - 1;                // find first bit
  ins_hash_map = calloc(m, sizeof(INS *));

  /* Put all INS reference into a hash map, for a faster searching for a target
   * of a branch/jump instruction. Use linear probing to handle collision.
   */
  for (int i = 0; i < ins_arr_sz; ++i) {
    int h = hash(ins_arr[i]->addr);

    while (ins_hash_map[h] != NULL)
      h = (h == m - 1)? 0 : h + 1;  // Linear probing to find a emtpy slot

    ins_hash_map[h] = ins_arr[i];
  }

  /* [Fisrt Pass]
   * Identify all the leader in the instruction list. There are three
   * categories: 1. The very first instruction 2. The target of a branch/jump
   * instruction 3. The instruction that follows a branch/jump instruction.
   */
  ins_arr[0]->is_leader = 1;  // #1

  for (int i = 0; i < ins_arr_sz; ++i) {
    if (ins_arr[i]->type == BR) {
      INS *cur_ins = ins_arr[i];
      int follow = i + 1;
      if (follow < ins_arr_sz) ins_arr[follow]->is_leader = 1;  // #3

      if (!cur_ins->argc) continue;

      int target_addr = strtol(cur_ins->argv[cur_ins->argc - 1], NULL, 16);
      int h = hash(target_addr);

      while (ins_hash_map[h]) {
        INS *target = ins_hash_map[h];

        if (target->addr == target_addr) {
          target->is_leader = 1;  // #2
          break;
        }

        h = (h == m - 1)? 0 : h + 1;  // Another linear probing
      }
    }
  }

  /* [Second Pass]
   * Starting from a leader, the set of all following instructions until and not
   * including the next leader is the basic block corresponding to the starting
   * leader. Thus every basic block has a leader.
   */
  int leader = 0, max_ret_sz = 16, bb_idx = 0;
  INS ***bb_list = malloc(max_ret_sz * sizeof(INS **));
  int *bb_sz = malloc(max_ret_sz * sizeof(int));
  while (leader < ins_arr_sz) {
    int next_leader = leader + 1;

    while (next_leader < ins_arr_sz && ins_arr[next_leader]->is_leader == 0)
      ++next_leader;

    int len = next_leader - leader;
    INS **bb = malloc(len * sizeof(INS *));

    for (int i = 0; i < len; ++i)
      bb[i] = ins_arr[leader + i];  // copy ins from ins_arr to bb

    if (bb_idx == max_ret_sz) {
      max_ret_sz *= 2;
      INS ***tmp_bb_list = realloc(bb_list, max_ret_sz * sizeof(INS **));
      int *tmp_bb_sz = realloc(bb_sz, max_ret_sz * sizeof(int));
      if (!tmp_bb_list) {
        printf("Failed reallocating memory for size: %d\n", max_ret_sz);
        break;
      }
      if (!tmp_bb_sz) {
        printf("Failed reallocating memory for size: %d\n", max_ret_sz);
        break;
      }
      bb_list = tmp_bb_list;
      bb_sz = tmp_bb_sz;
    }

    bb_list[bb_idx] = bb;
    bb_sz[bb_idx] = len;
    ++bb_idx;

    leader = next_leader;
  } 

  *ret_sz = bb_idx;
  INS ***tmp_bb_list = realloc(bb_list, max_ret_sz * sizeof(INS **));  // shrink
  int *tmp_bb_sz = realloc(bb_sz, max_ret_sz * sizeof(int));           // shrink
  if (tmp_bb_list)
    bb_list = tmp_bb_list;
  if (tmp_bb_sz)
    bb_sz = tmp_bb_sz;
  *ret_bb_sz = bb_sz;

  free(ins_hash_map);
  return bb_list;
}
