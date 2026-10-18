#!/usr/bin/env python3
"""Exact comparison of ABC P/T records and cube-coordinate combinatorial half-edges.
No third-party dependencies; fixed random seed; arbitrary finite cell sets.
"""
from collections import Counter, defaultdict
from random import Random
from time import perf_counter
import argparse

# Row-staggered primal cell coordinates; neighbor order matches e1,...,e6.
def neighbors(x,y):
    d=((1,0),(0,1),(-1,1),(-1,0),(-1,-1),(0,-1)) if y%2==0 else ((1,0),(1,1),(0,1),(-1,0),(0,-1),(1,-1))
    return [(x+dx,y+dy) for dx,dy in d]

def cube(x,y):
    # Standard axial/cube bijection of the same staggered-row cell lattice.
    q=x-(y-(y&1))//2
    r=y
    return (q,r,-q-r)

def abc_edges(x,y):
    if y%2==0:
        return [((x,y-1),(x,y+1)),((x,y),(x,y+1)),((x,y),(x-1,y+1)),((x-1,y-1),(x-1,y+1)),((x-1,y-1),(x,y)),((x,y-1),(x,y))]
    return [((x+1,y-1),(x+1,y+1)),((x,y),(x+1,y+1)),((x,y),(x,y+1)),((x,y-1),(x,y+1)),((x,y-1),(x,y)),((x+1,y-1),(x,y))]

# For edge i, its P endpoint lies at its junction with edge P_ADJ[i],
# and its T endpoint at its junction with edge T_ADJ[i].
P_ADJ=(5,2,1,4,3,0)
T_ADJ=(1,0,3,2,5,4)

def cube_half_edges(x,y):
    ns=neighbors(x,y)
    c=cube(x,y)
    out=[]
    for i in range(6):
        # A vertex is the sorted triple of centers of its incident cells.
        p=tuple(sorted((c,cube(*ns[i]),cube(*ns[P_ADJ[i]]))))
        t=tuple(sorted((c,cube(*ns[i]),cube(*ns[T_ADJ[i]]))))
        out.append((p,t))
    return out

def boundary(records,cells):
    counts=Counter(e for xy in cells for e in records(*xy))
    assert all(v<=2 for v in counts.values()), 'duplicate cell or non-manifold edge'
    return {e for e,c in counts.items() if c==1}

def extract_cycles(edges):
    # Hash endpoint -> two incident edge IDs. O(b) expected.
    edge_list=list(edges); adj=defaultdict(list)
    for i,(p,t) in enumerate(edge_list):
        adj[('P',p)].append(i);adj[('T',t)].append(i)
    assert all(len(a)==2 for a in adj.values()), 'boundary degree not 2'
    unseen=set(range(len(edge_list))); lengths=[]
    while unseen:
        first=next(iter(unseen)); cur=('P',edge_list[first][0]); start=cur; e=first; length=0
        while True:
            assert e in unseen
            unseen.remove(e);length+=1
            p,t=edge_list[e]
            nxt=('T',t) if cur==('P',p) else ('P',p)
            if nxt==start:break
            a,b=adj[nxt];e=b if a==e else a;cur=nxt
        lengths.append(length)
    return sorted(lengths)

def incidence_map_test():
    # Build an explicit global bijection between ABC vertex IDs and cube triples.
    ab_to_cube={}; cube_to_ab={}; edge_map={}
    for x in range(-14,15):
        for y in range(5,36):
            aa=abc_edges(x,y); cc=cube_half_edges(x,y)
            for (ap,at),(cp,ct) in zip(aa,cc):
                for a,c in ((('P',ap),('P',cp)),(('T',at),('T',ct))):
                    assert ab_to_cube.setdefault(a,c)==c,(x,y,'ABC vertex mismatch',a)
                    assert cube_to_ab.setdefault(c,a)==a,(x,y,'cube vertex mismatch',c)
                assert edge_map.setdefault((ap,at),(cp,ct))==(cp,ct)
    print('vertex bijection (finite interior window):',len(ab_to_cube))
    print('edge correspondence (finite window):',len(edge_map))
    return ab_to_cube

def test_sets(mapping):
    rng=Random(20261018)
    cases=[set(),{(0,10)},{(0,10),(8,10)},{(x,y) for x in range(4) for y in range(10,14)}]
    for _ in range(500):
        p=rng.choice((.12,.25,.5,.72,.9))
        cases.append({(x,y) for x in range(-3,5) for y in range(12,20) if rng.random()<p})
    for j,cells in enumerate(cases):
        a=boundary(abc_edges,cells);c=boundary(cube_half_edges,cells)
        mapped={(mapping[('P',p)][1],mapping[('T',t)][1]) for p,t in a}
        assert mapped==c,(j,'edge sets differ')
        assert extract_cycles(a)==extract_cycles(c),(j,'cycle lengths differ')
    print('finite cell sets compared exactly:',len(cases))

def benchmark():
    rng=Random(1234)
    print('N,abc_ms,cube_ms,boundary_edges')
    for side in (20,40,80,120):
        cells={(x,y) for x in range(side) for y in range(10,10+side) if rng.random()<.65}
        timings=[]
        for method in (abc_edges,cube_half_edges):
            t0=perf_counter()
            for _ in range(3):
                edges=boundary(method,cells); lengths=extract_cycles(edges)
            timings.append((perf_counter()-t0)*1000/3)
        print(f'{len(cells)},{timings[0]:.2f},{timings[1]:.2f},{len(edges)}')

if __name__=='__main__':
    parser=argparse.ArgumentParser();parser.add_argument('--benchmark',action='store_true');args=parser.parse_args()
    mapping=incidence_map_test();test_sets(mapping)
    if args.benchmark:benchmark()
    print('PASS')
