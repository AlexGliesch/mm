## 2023-04-16 
  012345  
i 0000000000000000111111111111111122222222222222223333333333333333
j 0000111122223333000011112222333300001111222233330000111122223333
k 0123012301230123012301230123012301230123012301230123012301230123
  1000010000000000000000001000010000100001000000000000000000100001

i(0)*j(0)*k(0) = 1
i(0)*j(1)*k(1) = 1
i(1)*j(2)*k(0) = 1
i(1)*j(3)*k(1) = 1
i(2)*j(0)*k(2) = 1
i(2)*j(1)*k(3) = 1
i(3)*j(2)*k(2) = 1
i(3)*j(3)*k(3) = 1

## 2023-04-17

0,1,2 and 1,2,3 
-> 0,1,2
-> 0,1,3
-> 0,2,2
-> 0,2,3
-> 1,1,2
-> 1,1,3
etc
0,1,2 and 3,4,5
-> 0,1,5

0,0,1 and 0,0,2
nothing

0,1,1 and 0,2,3
-> 0,1,3
-> 0,2,1

0,1,3 and 0,2,1
-> 0,1,1
-> 0,2,3

-> loops! Must do one at a time.

PDB2:
1. share 0 (all different)
0,1,2 and 1,2,3
-> must do one at a time
2. share one
0,2,7 and 1,2,3
-> loops, must do one at a time 
3. share two
1,1,5 and 1,2,5
-> can remove both immediately
3 entries = 3^1

PDB3: 
binom(3,2) = 3x2/2 = 3
share 0 + share 0 + share 0 = 0,0,0
share 0 + share 0 + share 1 = 0,0,1
...
share 2 + share 2 + share 2 = 2,2,2
number of distinct 3-uples: 3^binom(3,2) = 3^3 = 27 entries

PDB4:
binom(4,2) = 4x3/2 = 6
share 0,1,or 2 between all possible pairs in the PDB4
number of distinct 4-uples: 3^binom(4,2) = 3^6 = 729 entries

PDB5:
binom(5,2) = 5x4/2 = 10
number of distinct 5-uples: 3^binom(5,2) = 3^10 = 59'049 entries

PDB6:
binom(6,2) = 6x5/2 = 15
number of distinct 6-uples: 3^binom(6,2) = 3^15 = 14'348'907 entries

PDB7:
binom(7,2) = 7x6/2 = 21
number of distinct 7-uples: 3^binom(7,2) = 3^21 = 10'460'353'203 entries

I think that the more distinct a set is, the more moves it requires.

Can we add 1 to the PDB heuristic? Does this do anything?

Q: Can PDBs be added?
A: No, but maybe if the PDBs were to solve a relaxed problem, not the full
problem.
Why? Let's say two patterns generate the same set of conflicts, in the first
iteration of the optimal paths. These conflicts would only have to be resoved
once, but if we add both heuristics they will be counted as being resolved
twice.

We can probably add them to get a "decent" non-admissible heuristic.

What heuristic can we use to be able to add PDBs?

Instead of neeeding all 3 (u,v,w) to turn a 1 into 0, maybe require only 1 or
2? BUT: do require all 3 (u,v,w) to turn a 0 into 1.
-> This would probably not allow adds, but it would be a heuristic.

## 2023-04-20 

I think we can add pbds if the sets have distinct incidence vectors.

share: 221012
1 and 2: 2
1 and 3: 2
1 and 4: 1
2 and 3: 0
2 and 4: 1
3 and 4: 2

1 - 0,0,0
2 - 0,0,1
3 - 0,1,0
4 - 0,1,1

Finding a subinstance that witnesses a pattern seems NP-hard? And possibly for some patterns there is no solution. So, I decided to use lazy pdbs.

## 2023-04-22

  012345  
i 0000000000000000111111111111111122222222222222223333333333333333
j 0000111122223333000011112222333300001111222233330000111122223333
k 0123012301230123012301230123012301230123012301230123012301230123
  1000010000000000000000001000010000100001000000000000000000100001

Adding PDBs:

a 0,0,0
b 0,1,1
c 1,2,0
d 1,3,1
e 2,0,2
f 2,1,3
g 3,2,2
h 3,3,3

For instance:
(a,b) can be added with (g,h)
(a,b,c) can be added with (h)


every "one" is a node in a graph

nodes are connected if they share an u,v,w incidence

[not sure?] find connected components in the graph: PDBs within the connected components can be added
[not sure?] in each connected component, get K-pdb with largest set of distinct indices (break ties by...?)

## 2023-05-09

Idea for additive PDBs:

1. Take a random, unassigned node
2. Iteratively, add to the current group the neighbor that results in the
maximum set of distinct values
(or, any group)
3. Do this until we have selected PDB_MAX nodes (or no neighbors exist)
4. Set all neighbors of the current set as "buffer zone" - cannot be
assigned

Repeat 1-4 until all nodes are either assigned, or a buffer zone
  
  
## 2023-05-10  

is a state with fewer ones always easy to solve than a state with more ones?

e.g. if I have A=0000110101101010010111
and            B=0000000001100000000001
where B is necessarily a subset of A. 

Is B *always* easier to solve?

If yes, we can prune a lot of nodes: if a subset of the current node has already been visited, we can drop that node!

If no then maybe that is bad news: the PDBs are maybe not admissible.

## 2023-05-11

Turns out the heuristic is inadmissible.

We can still try to run a search with this heuristic, and a reduced search space

Can also prune if a subset is already visited

Improvements to A*:

 If num_ones <= PDB_MAX
    get optimal value from PDB
    no need to store in open/closed list - stop immediately
    no need to expand neighbors

 When found the best solution:
    every node in the path from the best solution to the start gets added
      to the global closed list
    technically, this 'global closed list' also includes the PDBs
    
When all children of a node have been pruned:
  prune that node
  can we do this as soon as a single child of the node has been pruned? not sure...
  

Can we use pdb_idx() as an index for the hash tables?
 - if the symmetry is correct, then yes - we should be able to 

number of distinct 3-uples: 3^binom(3,2) = 3^3 = 27 entries

3^binom(4,2)  = 729
3^binom(5,2)  = 59'049
3^binom(6,2)  = 14'348'907
3^binom(7,2)  = 10'460'353'203
3^binom(8,2)  = 22'876'792'454'961
3^binom(9,2)  = 150'094'635'296'999'121
2^64          = 18'446'744'073'709'551'616
3^binom(10,2) = 2'954'312'706'550'833'698'643
3^binom(11,2) = 174'449'211'009'120'179'071'170'507
3^binom(12,2) = ...
3^binom(13,2) = ...
3^binom(14,2) = 26'183'890'704'263'137'277'674'192'438'430'182'020'124'347
2^729         = 2824013958708217496949108842204627863351353911851577524683401930862693830361198499905873920995229996970897865498283996578123296865878390947626553088486946106430796091482716'120'572'632'072'492'703'527'723'757'359'478'834'530'365'734'912

It scales worse, but maybe in practice it's smaller? Worth considering...
But we'd need bignum or a bitset to represent this


- can we do the subsets pruning thing? ie if a subset of the current ones has already been visited, stop