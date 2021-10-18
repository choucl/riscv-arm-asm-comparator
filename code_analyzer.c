#include "core.h"
#include <stdlib.h>
#include <string.h>

INS **ins_hash_map;  // Use a hash map for faster search
int m, p;            // Size of the map, a power of 2, m = 1 << p;

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
  return ((1 << p) - 1) & (((key * 2654435769) & 0xffffffff) >> (31 - p));
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
  p = ffs(m) - 1;  // find first bit
  ins_hash_map = calloc(m, sizeof(INS *));

  for (int i = 0; i < ins_arr_sz; ++i) {
    ins_hash_map[hash(ins_arr[i]->addr)] = ins_arr[i];
  }

  ins_arr[0]->is_leader = 1;  // #1
  // TODO: coding

  /* [Second Pass]
   * Starting from a leader, the set of all following instructions until and not
   * including the next leader is the basic block corresponding to the starting
   * leader. Thus every basic block has a leader.
   */
  // TODO: coding

  return NULL;
}
