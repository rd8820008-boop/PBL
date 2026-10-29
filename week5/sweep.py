from algorithms import ALGORITHMS, parse_input
 
def sweep(reference, max_frames=7):
    """Page faults for each algorithm with 1..max_frames frames."""
    pages, _ = parse_input(reference, 1)
    out = {name: [] for name in ALGORITHMS}
    for f in range(1, max_frames + 1):
        for name, fn in ALGORITHMS.items():
            out[name].append(fn(pages, f)["faults"])
    return {"frames": list(range(1, max_frames + 1)), "faults": out}
 
if __name__ == "__main__":
    print(sweep("1 2 3 4 1 2 5 1 2 3 4 5"))