import sys, re, csv, os

def read_lines_any_encoding(path):
    # Try utf-8 first; fallback to utf-16-le; finally latin-1
    for enc in ("utf-8", "utf-16-le", "utf-16", "latin-1"):
        try:
            with open(path, "r", encoding=enc) as f:
                return f.readlines()
        except UnicodeError:
            continue
    # Last resort: binary with errors ignored
    with open(path, "rb") as f:
        return f.read().decode("utf-8", errors="ignore").splitlines(True)

pat = re.compile(r"comparisons:\s*b(\d+)\s+n(\d+)\s+s(\d+)")
rows = []  # for CSV

if len(sys.argv) < 2:
    print("Usage: python3 stats2.py <stdout files...>")
    sys.exit(1)

for path in sys.argv[1:]:
    lines = read_lines_any_encoding(path)
    B = N = S = C = 0
    for line in lines:
        m = pat.search(line)
        if m:
            B += int(m.group(1))
            N += int(m.group(2))
            S += int(m.group(3))
            C += 1
    base = os.path.basename(path)
    if C == 0:
        print(f"{base}: no comparison lines found")
        continue
    avgB, avgN, avgS = B/C, N/C, S/C
    print(f"{base}")
    print(f"  Total: b{B} n{N} s{S}")
    print(f"  Avg  : b{avgB:.2f} n{avgN:.2f} s{avgS:.2f} (over {C} queries)")
    rows.append([base, C, B, N, S, f"{avgB:.2f}", f"{avgN:.2f}", f"{avgS:.2f}"])

# also write a CSV summary next to the first file
if rows:
    outcsv = "comparison_stats.csv"
    with open(outcsv, "w", newline="") as f:
        w = csv.writer(f)
        w.writerow(["file","queries","total_bits","total_nodes","total_strings",
                    "avg_bits","avg_nodes","avg_strings"])
        w.writerows(rows)
    print(f"\nWrote {outcsv}")

