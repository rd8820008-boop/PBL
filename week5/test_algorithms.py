from algorithms import run, fifo
 
def test_sample():
    out = run("7 0 1 2 0 3 0 4 2 3", 3)
    got = [(r["hits"], r["faults"]) for r in out["results"]]
    assert got == [(1, 9), (2, 8), (4, 6)], got
    assert out["best"] == "Optimal"
 
def test_belady():
    # FIFO gets WORSE with more frames on this string (Belady's anomaly)
    pages = [1, 2, 3, 4, 1, 2, 5, 1, 2, 3, 4, 5]
    assert fifo(pages, 3)["faults"] == 9
    assert fifo(pages, 4)["faults"] == 10
 
def test_optimal_never_worse():
    for s in ["1 2 3 4 1 2 5 1 2 3 4 5", "2 3 2 1 5 2 4 5 3 2 5 2"]:
        for f in range(1, 6):
            r = run(s, f)["results"]
            assert r[2]["faults"] <= r[0]["faults"] and r[2]["faults"] <= r[1]["faults"]
 
if __name__ == "__main__":
    test_sample(); test_belady(); test_optimal_never_worse()
    print("All tests passed")