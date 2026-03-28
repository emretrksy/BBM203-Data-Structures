# BBM203 — Data Structures Assignments

Three C++ assignments from Hacettepe University's BBM203 Data Structures course.
Each one is built around a real-world scenario and implements core data structures from scratch.

---

## PA1 — Train Classification Yard

A rail sorting simulation where wagons are routed to the correct destination tracks.

The challenge was managing multiple linked lists simultaneously — a classification yard
where incoming wagons get sorted, and a departure yard where trains are assembled by destination.
Memory management was the trickiest part: making sure no wagon got lost between tracks.

**Structures used:** Custom linked list, enum-based routing

---

## PA2 — Quake Assist Controller

A disaster response coordinator that processes rescue and supply requests in real time.

Commands arrive line by line (like a live feed), and the system dispatches the right teams
to the right requests. Supply and rescue operations run on separate priority queues.
The goal was to keep the system responsive — no request left waiting if a team is free.

**Structures used:** Stack (MissionStack), Queue (RequestQueue), dynamic team array

---

## PA3 — AVL Tree & Red-Black Tree

Both balanced BST variants implemented from scratch, tested against the same dataset.

Writing insert and rotation logic for AVL was straightforward enough.
Red-Black was a different story — the recoloring and rotation cases took several iterations
to get right. Ended up with a cleaner understanding of why RB trees are preferred in practice
(fewer rotations on insert, more predictable performance).

**Structures used:** AVL Tree, Red-Black Tree

---

## Build & Run

Each assignment is self-contained. Compile with:

```bash
g++ -std=c++17 -o out *.cpp
./out
```

No external libraries used.
