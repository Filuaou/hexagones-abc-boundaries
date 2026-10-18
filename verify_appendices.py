#!/usr/bin/env python3
"""Independent arithmetic and finite-region checks for the Hexagones appendices."""
from collections import Counter, defaultdict, deque
from random import Random

def norm(t):
    m=min(t); return tuple(v-m for v in t)
def D(t):
    a,b,c=norm(t); return (a-(b+c)//2-1,b-c-1)
def from_abc(x,y):
    return (x+1+(y+1)//2,y+1,0)
def direct(x,y):
    a,b,c=from_abc(x,y)
    u=[(a,b,c),(a+1,b+1,c),(a+1,b,c+1),(a,b+1,c),(a,b,c+1)]
    v=list(map(D,u))
    return [(v[2],v[1]),(v[0],v[1]),(v[0],v[3]),(v[4],v[3]),(v[4],v[0]),(v[2],v[0])]
def parity(x,y):
    if y%2==0:
        return [((x,y-1),(x,y+1)),((x,y),(x,y+1)),((x,y),(x-1,y+1)),((x-1,y-1),(x-1,y+1)),((x-1,y-1),(x,y)),((x,y-1),(x,y))]
    return [((x+1,y-1),(x+1,y+1)),((x,y),(x+1,y+1)),((x,y),(x,y+1)),((x,y-1),(x,y+1)),((x,y-1),(x,y)),((x+1,y-1),(x,y))]

def neighbors(x,y):
    return [(x+dx,y+dy) for dx,dy in ([(1,0),(0,1),(-1,1),(-1,0),(-1,-1),(0,-1)] if y%2==0 else [(1,0),(1,1),(0,1),(-1,0),(0,-1),(1,-1)])]

# Exact finite-window checks. Infinite assertions are proved symbolically in the paper.
for x in range(-35,36):
    for y in range(0,71):
        assert direct(x,y)==parity(x,y), ('formula',x,y)
print('formula cells verified:',71*71)

pairs=[3,4,5,0,1,2]
for x in range(-35,36):
    for y in range(1,70):
        for i,(nx,ny) in enumerate(neighbors(x,y)):
            assert direct(x,y)[i]==direct(nx,ny)[pairs[i]], ('neighbors',x,y,i)
print('neighbor equalities verified:',71*69*6)

# Full incidence, interior finite window only.
edge_cells=defaultdict(list); endpoint_cells=defaultdict(set); endpoint_records=defaultdict(set)
for x in range(-40,41):
    for y in range(0,81):
        for i,(p,t) in enumerate(direct(x,y)):
            edge_cells[(p,t)].append((x,y,i))
            endpoint_cells[('P',p)].add((x,y)); endpoint_cells[('T',t)].add((x,y))
            endpoint_records[('P',p)].add((p,t)); endpoint_records[('T',t)].add((p,t))
for x in range(-30,31):
    for y in range(10,70):
        for i,(p,t) in enumerate(direct(x,y)):
            assert len(edge_cells[(p,t)])==2, ('edge multiplicity',x,y,i)
            for v in [('P',p),('T',t)]:
                assert len(endpoint_cells[v])==3, ('cell valence',x,y,i,v)
                assert len(endpoint_records[v])==3, ('edge valence',x,y,i,v)
print('interior record incidences verified:',61*60*6)

def cycles_for(cells):
    counts=Counter(e for cell in cells for e in direct(*cell))
    boundary=[e for e,c in counts.items() if c%2]
    assert all(c<=2 for c in counts.values())
    adj=defaultdict(list)
    for j,(p,t) in enumerate(boundary):
        adj[('P',p)].append(j); adj[('T',t)].append(j)
    assert all(len(v)==2 for v in adj.values())
    unseen=set(range(len(boundary))); lengths=[]
    while unseen:
        start=next(iter(unseen)); edge=start; current=('P',boundary[start][0]); start_v=current; length=0
        while True:
            assert edge in unseen
            unseen.remove(edge); length+=1
            p,t=boundary[edge]
            next_v=('T',t) if current==('P',p) else ('P',p)
            if next_v==start_v: break
            options=adj[next_v]
            edge=options[0] if options[1]==edge else options[1]
            current=next_v
        lengths.append(length)
    return len(lengths),len(boundary)

def components(cells):
    remaining=set(cells); count=0
    while remaining:
        count+=1; start=remaining.pop(); q=[start]
        while q:
            for v in neighbors(*q.pop()):
                if v in remaining: remaining.remove(v);q.append(v)
    return count

def holes(cells):
    # Complement components in padded rectangle not touching its border.
    if not cells:return 0
    xs=[x for x,y in cells]; ys=[y for x,y in cells]
    xmin,xmax=min(xs)-3,max(xs)+3; ymin,ymax=min(ys)-3,max(ys)+3
    complement={(x,y) for x in range(xmin,xmax+1) for y in range(ymin,ymax+1)}-cells
    remaining=set(complement); h=0
    while remaining:
        start=remaining.pop(); q=[start]; exterior=False
        while q:
            x,y=q.pop()
            if x in (xmin,xmax) or y in (ymin,ymax):exterior=True
            for v in neighbors(x,y):
                if v in remaining: remaining.remove(v);q.append(v)
        h+=not exterior
    return h

rng=Random(20261018)
cases=[set(),{(0,4)},{(0,4),(10,4)},{(x,y) for x in range(5) for y in range(4)}]
for _ in range(500):
    cells={(x,y) for x in range(-3,5) for y in range(2,10) if rng.random()<rng.choice([.12,.25,.5,.72,.9])}
    cases.append(cells)
for i,cells in enumerate(cases):
    m,b=cycles_for(cells)
    c=components(cells); h=holes(cells)
    assert m==c+h, ('Euler',i,m,c,h)
print('finite cell sets verified:',len(cases))
print('all checks passed')
