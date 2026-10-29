"""Page replacement algorithms: FIFO, LRU, Optimal.
 
Each algorithm is its own function and returns the same result shape,
so the GUI, charts and report only ever read this dict.
Frames are fixed slots (like the sample table): a replaced page
takes the slot of the page it evicts.  None = empty slot.
"""
 
 
def parse_input(reference, frames):
    """Validate raw user input. Returns (pages, frame_count) or raises ValueError."""
    if reference is None or not str(reference).strip():
        raise ValueError("Reference string is empty. Enter page numbers like: 7 0 1 2 0 3")
    pages = []
    for t in str(reference).replace(",", " ").split():
        if not t.isdigit():
            raise ValueError(f"'{t}' is not a valid page number. Use whole numbers 0 or above.")
        pages.append(int(t))
    try:
        f = int(str(frames).strip())
    except (ValueError, TypeError):
        raise ValueError("Number of frames must be a whole number.")
    if f <= 0:
        raise ValueError("Number of frames must be at least 1.")
    return pages, f
 
 
def _result(name, steps, frame_count):
    total = len(steps)
    faults = sum(1 for s in steps if s["status"] == "Fault")
    hits = total - faults
    return {
        "name": name,
        "frame_count": frame_count,
        "steps": steps,
        "total": total,
        "hits": hits,
        "faults": faults,
        "hit_ratio": round(hits / total * 100, 2) if total else 0,
        "fault_ratio": round(faults / total * 100, 2) if total else 0,
    }
 
 
def fifo(pages, frame_count):
    frames = [None] * frame_count
    next_victim = 0                      # circular pointer = oldest page's slot
    steps = []
    for p in pages:
        if p in frames:
            status = "Hit"
        else:
            status = "Fault"
            if None in frames:
                frames[frames.index(None)] = p
            else:
                frames[next_victim] = p
                next_victim = (next_victim + 1) % frame_count
        steps.append({"page": p, "frames": frames[:], "status": status})
    return _result("FIFO", steps, frame_count)
 
 
def lru(pages, frame_count):
    frames = [None] * frame_count
    last_used = {}                       # page -> index of last use
    steps = []
    for i, p in enumerate(pages):
        if p in frames:
            status = "Hit"
        else:
            status = "Fault"
            if None in frames:
                frames[frames.index(None)] = p
            else:
                victim = min(frames, key=lambda x: last_used[x])
                frames[frames.index(victim)] = p
        last_used[p] = i
        steps.append({"page": p, "frames": frames[:], "status": status})
    return _result("LRU", steps, frame_count)
 
 
def optimal(pages, frame_count):
    frames = [None] * frame_count
    steps = []
    for i, p in enumerate(pages):
        if p in frames:
            status = "Hit"
        else:
            status = "Fault"
            if None in frames:
                frames[frames.index(None)] = p
            else:
                future = pages[i + 1:]
 
                def next_use(x):
                    return future.index(x) if x in future else float("inf")
 
                victim = max(frames, key=next_use)   # used farthest in future / never
                frames[frames.index(victim)] = p
        steps.append({"page": p, "frames": frames[:], "status": status})
    return _result("Optimal", steps, frame_count)
 
 
ALGORITHMS = {"fifo": fifo, "lru": lru, "optimal": optimal}
 
 
def run(reference, frames, choice="all"):
    pages, f = parse_input(reference, frames)
    names = list(ALGORITHMS) if choice == "all" else [choice]
    if any(n not in ALGORITHMS for n in names):
        raise ValueError("Unknown algorithm selected.")
    results = [ALGORITHMS[n](pages, f) for n in names]
    best = min(results, key=lambda r: r["faults"])["name"] if len(results) > 1 else None
    return {"pages": pages, "frames": f, "results": results, "best": best}
 
 
if __name__ == "__main__":
    # quick self-test with the sample from the brief
    out = run("7 0 1 2 0 3 0 4 2 3", 3)
    for r in out["results"]:
        print(r["name"], "hits", r["hits"], "faults", r["faults"], r["fault_ratio"], "%")
    assert [(r["hits"], r["faults"]) for r in out["results"]] == [(1, 9), (2, 8), (4, 6)]
    assert [s["frames"] for s in out["results"][0]["steps"]][3] == [2, 0, 1]
    print("self-test passed, best =", out["best"])