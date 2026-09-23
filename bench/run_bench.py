import subprocess
import json
import time
import os
from datetime import datetime
from pathlib import Path
import matplotlib.pyplot as plt

# --- CONFIGURATION ---
SCRIPT_DIR = Path(__file__).parent
BUILD_DIR = SCRIPT_DIR.parent / "build-debug" / "bench"
HISTORY_FILE = SCRIPT_DIR / "benchmark_history.json"

# Included task-clock to replace python's time.perf_counter()
PERF_EVENTS = "task-clock,cpu-cycles,cache-misses,page-faults,instructions"
GENERATE_FLAME_GRAPH_DATA = True


def flush_page_cache():
    print("    [*] Flushing OS page cache for cold-disk I/O...")
    # 'sync' ensures pending writes are committed before dropping caches
    # 'echo 3' drops pagecache, dentries, and inodes
    subprocess.run(
        ["sudo", "sh", "-c", "sync; echo 3 > /proc/sys/vm/drop_caches"], check=True
    )


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


def run_perf(executable_path, iterations=5):
    print(
        f"\n[>] Running benchmark: {executable_path.name} ({iterations} cold runs)..."
    )

    # Initialize dictionary to accumulate metrics across runs
    total_metrics = {
        "execution_time_ms": 0.0,
        "cpu-cycles": 0,
        "cache-misses": 0,
        "page-faults": 0,
        "instructions": 0,
    }

    # Manually loop so we can drop caches between EVERY execution
    for i in range(iterations):
        flush_page_cache()

        temp_csv = SCRIPT_DIR / "temp_perf.csv"

        # Removed -r flag; running one at a time
        cmd = [
            "perf",
            "stat",
            "-x",
            ",",
            "-o",
            str(temp_csv),
            "-e",
            "cpu-cycles,cache-misses,page-faults,instructions",
            str(executable_path),
        ]

        # Use perf_counter to measure real-world time (including disk wait time)
        start_time = time.perf_counter()
        result = subprocess.run(cmd, capture_output=True, text=True)
        end_time = time.perf_counter()

        # Accumulate wall-clock time
        total_metrics["execution_time_ms"] += (end_time - start_time) * 1000.0

        if result.returncode != 0:
            print(f"    [!] Run {i + 1} failed with exit code {result.returncode}")
            if result.stderr.strip():
                print(f"    [C++ ERROR OUTPUT]:\n{result.stderr.strip()}")
            return None

        if temp_csv.exists():
            with open(temp_csv, "r") as f:
                for line in f:
                    if line and not line.startswith("#"):
                        parts = line.split(",")
                        if len(parts) >= 3:
                            val_str, event = parts[0], parts[2]
                            try:
                                total_metrics[event] += int(val_str)
                            except ValueError:
                                pass
            temp_csv.unlink()

    # Average the metrics across all iterations
    avg_metrics = {k: v / iterations for k, v in total_metrics.items()}
    avg_metrics["execution_time_ms"] = round(avg_metrics["execution_time_ms"], 2)

    # Optional: Generate Flame Graph (just doing this once is enough)
    if GENERATE_FLAME_GRAPH_DATA:
        print(f"    [*] Recording stack traces for Flame Graph...")
        flush_page_cache()
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
        subprocess.run(flame_cmd, capture_output=True)
        print(f"    [+] Saved {perf_data_path.name}")

    return avg_metrics


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

    ylabel = metric_to_graph.replace("-", " ").replace("_", " ").title()
    plt.ylabel(ylabel)
    plt.grid(True)

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
            print(
                f"    -> Exec Time (Task Clock): {metrics.get('execution_time_ms', 0)} ms"
            )
            print(f"    -> CPU Cycles: {metrics.get('cpu-cycles', 0):,}")
            print(f"    -> Cache Misses: {metrics.get('cache-misses', 0):,}")
            print(f"    -> Page Faults: {metrics.get('page-faults', 0):,}")

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
