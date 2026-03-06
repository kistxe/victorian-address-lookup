import sys, re

if len(sys.argv) != 3:
    print("Usage: python3 massif_to_csv_simple.py <massif.out> <out.csv>")
    sys.exit(1)

inp, outp = sys.argv[1], sys.argv[2]
snapshots = []
cur = {}

with open(inp, "r") as f:
    for line in f:
        line = line.strip()
        if line.startswith("snapshot="):
            if cur:
                snapshots.append(cur)
            cur = {"time": None, "mem_heap_B": None, "mem_heap_extra_B": None, "mem_stacks_B": None}
        elif line.startswith("time="):
            cur["time"] = int(line.split("=",1)[1])
        elif line.startswith("mem_heap_B="):
            cur["mem_heap_B"] = int(line.split("=",1)[1])
        elif line.startswith("mem_heap_extra_B="):
            cur["mem_heap_extra_B"] = int(line.split("=",1)[1])
        elif line.startswith("mem_stacks_B="):
            cur["mem_stacks_B"] = int(line.split("=",1)[1])

    if cur:
        snapshots.append(cur)

with open(outp, "w") as w:
    w.write("time,heap,heap_extra,stack\n")
    for s in snapshots:
        t = s.get("time") or 0
        h = s.get("mem_heap_B") or 0
        he = s.get("mem_heap_extra_B") or 0
        st = s.get("mem_stacks_B") or 0
        w.write(f"{t},{h},{he},{st}\n")
