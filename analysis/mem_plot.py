import sys, os
import pandas as pd
import matplotlib.pyplot as plt

def visualise_memory(csv1, csv2=None, label1=None, label2=None):
    default_labels = {
        "dict1_s1": "Linked List",
        "dict2_s2": "Patricia Tree",
    }

    df1 = pd.read_csv(csv1)
    name1 = os.path.splitext(os.path.basename(csv1))[0]
    if label1 is None:
        label1 = default_labels.get(name1, name1)

    plt.figure(figsize=(10, 6))
    plt.plot(df1['time'], df1['heap'] / 1024, label=label1, linewidth=2.0)

    if csv2:
        df2 = pd.read_csv(csv2)
        name2 = os.path.splitext(os.path.basename(csv2))[0]
        if label2 is None:
            label2 = default_labels.get(name2, name2)
        plt.plot(df2['time'], df2['heap'] / 1024, label=label2, linewidth=2.0)

    plt.xlabel('Time (ms)')
    plt.ylabel('Memory use (KB)')
    plt.title('Memory usage over time (Valgrind Massif)')
    plt.legend()
    plt.grid(True, alpha=0.3)
    plt.tight_layout()
    out_name = 'memory_usage_comparison.png' if csv2 else 'memory_usage.png'
    plt.savefig(out_name)
    print(f"Wrote {out_name}")

if __name__ == "__main__":
    if len(sys.argv) == 2:
        visualise_memory(sys.argv[1])
    elif len(sys.argv) == 3:
        visualise_memory(sys.argv[1], sys.argv[2])
    elif len(sys.argv) == 5:
        visualise_memory(sys.argv[1], sys.argv[2], sys.argv[3], sys.argv[4])
    else:
        print("Usage: python3 mem_plot.py <csv1> [csv2] [label1 label2]")

