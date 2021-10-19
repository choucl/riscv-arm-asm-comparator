#include "core.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

INS **ins_hash_map;  // Use a hash map for faster search
int m, pw;           // Size of the map, a power of 2, m = 1 << pw;

/* Compute the next highest power of 2 of 32-bit x 
 * Check: https://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
 */
int next_p2(int x) {
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
int hash(unsigned long long key) {
  return ((1 << pw) - 1) & (((key * 2654435769) & 0xffffffff) >> (32 - pw));
}

/* Algorithm for finding basic blocks.
 * Check: https://en.wikipedia.org/wiki/Basic_block#Creation_algorithm
 */
INS ***findbb(INS **ins_arr, int ins_arr_sz, int *ret_sz, int **ret_bb_sz) {

  if (!ins_arr_sz) return NULL;

  /* [Fisrt Pass]
   * Identify all the leader in the instruction list. There are three
   * categories: 1. The very first instruction 2. The target of a branch/jump
   * instruction 3. The instruction that follows a branch/jump instruction.
   */
  m = next_p2(ins_arr_sz) << 1;  // keep load facter < 0.5
  pw = ffs(m) - 1;  // find first bit
  ins_hash_map = calloc(m, sizeof(INS *));

  ins_arr[0]->is_leader = 1;  // #1

  int prev_type = NA;
  for (int i = 0; i < ins_arr_sz; ++i) {
    int h = hash(ins_arr[i]->addr);

    while (ins_hash_map[h] != NULL)
      h = (h == m - 1)? 0 : h + 1;  // Linear probing

    ins_hash_map[h] = ins_arr[i];
  }

  for (int i = 0; i < ins_arr_sz; ++i) {
    if (ins_arr[i]->type == BR) {
      INS *cur_ins = ins_arr[i];
      int follow = i + 1;
      if (follow < ins_arr_sz) ins_arr[follow]->is_leader = 1;  // #3

      if (!cur_ins->argc) continue;

      int target_addr = strtol(cur_ins->argv[cur_ins->argc - 1], NULL, 16);
      int h = hash(target_addr);

      int cc = 0;
      while (ins_hash_map[h]) {
        INS *target = ins_hash_map[h];

        if (target->addr == target_addr) {
          printf("cc = %d\n", cc);
          target->is_leader = 1;  // #2
          break;
        }

        h = (h == m - 1)? 0 : h + 1;  // Another linear probing
        ++cc;
      }
    }
  }

  /* [Second Pass]
   * Starting from a leader, the set of all following instructions until and not
   * including the next leader is the basic block corresponding to the starting
   * leader. Thus every basic block has a leader.
   */
  // TODO: coding

  return NULL;
}
