import subprocess
import json
import os
import time
from datetime import datetime
from pathlib import Path
import matplotlib.pyplot as plt

# --- CONFIGURATION ---
SCRIPT_DIR = Path(__file__).parent
BUILD_DIR = SCRIPT_DIR.parent / "build-debug" / "bench"
HISTORY_FILE = SCRIPT_DIR / "benchmark_history.json"
PERF_EVENTS = "cpu-cycles,cache-misses,page-faults,instructions"

# Toggle this to True to generate the raw data for Flame Graphs
GENERATE_FLAME_GRAPH_DATA = True


def load_history():
    if HISTORY_FILE.exists():
        with open(HISTORY_FILE, "r") as f:
            return json.load(f)
    return {}


def save_history(history):
    with open(HISTORY_FILE, "w") as f:
        json.dump(history, f, indent=4)


def find_executables(search_path):
    executables = []
    if not search_path.exists():
        print(f"Error: Build directory not found at {search_path}")
        return executables
    for file_path in search_path.rglob("*"):
        if file_path.is_file() and os.access(file_path, os.X_OK):
            if "bench" in file_path.name and not file_path.name.endswith(".o"):
                executables.append(file_path)
    return executables


def run_perf(executable_path):
    print(f"\n[>] Running benchmark: {executable_path.name} ...")

    temp_csv = SCRIPT_DIR / "temp_perf.csv"
    cmd = [
        "perf",
        "stat",
        "-x",
        ",",
        "-o",
        str(temp_csv),
        "-e",
        PERF_EVENTS,
        str(executable_path),
    ]

    # 1. Track precise execution time
    start_time = time.perf_counter()
    result = subprocess.run(cmd, capture_output=True, text=True)
    end_time = time.perf_counter()

    exec_time_ms = (end_time - start_time) * 1000.0

    if result.stderr.strip():
        print(f"    [C++ ERROR OUTPUT]:\n{result.stderr.strip()}")

    if result.returncode != 0:
        print(f"    [!] Executable failed with exit code {result.returncode}")
        return None

    metrics = {"execution_time_ms": round(exec_time_ms, 2)}

    if temp_csv.exists():
        with open(temp_csv, "r") as f:
            for line in f:
                if line and not line.startswith("#"):
                    parts = line.split(",")
                    if len(parts) >= 3:
                        val, event = parts[0], parts[2]
                        try:
                            metrics[event] = int(val)
                        except ValueError:
                            metrics[event] = 0
        temp_csv.unlink()

    # 2. Optionally record Flame Graph data
    if GENERATE_FLAME_GRAPH_DATA:
        print(f"    [*] Recording stack traces for Flame Graph...")
        perf_data_path = SCRIPT_DIR / f"{executable_path.name}.perf.data"
        flame_cmd = [
            "perf",
            "record",
            "-F",
            "99",
            "-g",
            "-o",
            str(perf_data_path),
            str(executable_path),
        ]
        # We don't need the output of this, just let it write the file
        subprocess.run(flame_cmd, capture_output=True)
        print(f"    [+] Saved {perf_data_path.name}")

    return metrics


def generate_graph(
    exe_name, history_db, current_metrics, metric_to_graph, line_color="b"
):
    valid_runs = [
        run for run in history_db.get(exe_name, []) if run.get("use_for_graph", True)
    ]

    y_values = [run["metrics"].get(metric_to_graph, 0) for run in valid_runs]
    y_values.append(current_metrics.get(metric_to_graph, 0))
    x_values = list(range(1, len(y_values) + 1))

    plt.figure(figsize=(10, 6))
    plt.plot(x_values, y_values, marker="o", linestyle="-", color=line_color)
    plt.title(f"Historical Performance: {exe_name} ({metric_to_graph})")
    plt.xlabel("Run Iteration")

    # Clean up the metric name for the Y-axis label
    ylabel = metric_to_graph.replace("-", " ").replace("_", " ").title()
    plt.ylabel(ylabel)
    plt.grid(True)

    # Include the metric name in the file output
    graph_path = SCRIPT_DIR / f"{exe_name}_{metric_to_graph}_history.png"
    plt.savefig(graph_path)
    plt.close()
    print(f"    [+] Graph saved to {graph_path.name}")


def main():
    history_db = load_history()
    executables = find_executables(BUILD_DIR)

    if not executables:
        print("No executables found! Build your CMake project first.")
        return

    completed_runs = []
    for exe in executables:
        metrics = run_perf(exe)
        if metrics:
            print(f"    -> Exec Time: {metrics.get('execution_time_ms', 0)} ms")
            print(f"    -> CPU Cycles: {metrics.get('cpu-cycles', 0):,}")
            print(f"    -> Cache Misses: {metrics.get('cache-misses', 0):,}")
            print(f"    -> Page Faults: {metrics.get('page-faults', 0):,}")

            # Generate the three distinct graphs
            generate_graph(exe.name, history_db, metrics, "execution_time_ms", "red")
            generate_graph(exe.name, history_db, metrics, "cache-misses", "blue")
            generate_graph(exe.name, history_db, metrics, "page-faults", "green")

            completed_runs.append((exe.name, metrics))

    if completed_runs:
        print("\n" + "=" * 40 + "\n[?] BENCHMARK REVIEW\n" + "=" * 40)
        for exe_name, metrics in completed_runs:
            user_input = (
                input(f"Include [{exe_name}] in future graphs? [Y/n]: ").strip().lower()
            )

            run_record = {
                "timestamp": datetime.now().isoformat(),
                "use_for_graph": user_input != "n",
                "metrics": metrics,
            }

            if exe_name not in history_db:
                history_db[exe_name] = []
            history_db[exe_name].append(run_record)

        save_history(history_db)
        print("\n[✓] All updates saved to history database.")


if __name__ == "__main__":
    main()
