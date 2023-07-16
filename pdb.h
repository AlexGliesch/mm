#pragma once

struct State;

void
init_pdbs();

int
pdb_heuristic(const State& s, const int pdb_max);

