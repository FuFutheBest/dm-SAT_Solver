#!/usr/bin/env python3
"""Compare two solver binaries with identical commands and checked results."""
import argparse
import json
import statistics
import subprocess
import tempfile
import time
from importlib.util import module_from_spec, spec_from_file_location
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
spec = spec_from_file_location("regression", ROOT / "scripts/check-public-regressions.py")
assert spec is not None and spec.loader is not None
regression = module_from_spec(spec)
spec.loader.exec_module(regression)


def run(binary, row, directory):
    cnf = ROOT / "test/performance" / row["file"]
    proof = directory / "proof.drat"
    started = time.perf_counter()
    try:
        result = subprocess.run(
            [str(binary), str(cnf), str(proof)],
            capture_output=True, text=True, timeout=row["seconds"],
        )
    except subprocess.TimeoutExpired:
        return 2 * row["seconds"], False
    elapsed = time.perf_counter() - started
    expected = 10 if row["expected"] == "SAT" else 20
    if result.returncode != expected:
        raise RuntimeError(f"{binary}: incorrect status for {cnf.name}")
    if expected == 10:
        values = regression.model(result.stdout)
        valid = all(any(values.get(abs(x)) == (x > 0) for x in c)
                    for c in regression.clauses(cnf))
    else:
        valid = subprocess.run(
            [str(ROOT / "build/drat-trim"), str(cnf), str(proof)],
            stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL,
            timeout=3 * row["seconds"],
        ).returncode == 0
    if not valid:
        raise RuntimeError(f"{binary}: invalid model/proof for {cnf.name}")
    return elapsed, True


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("baseline", type=Path)
    parser.add_argument("candidate", type=Path)
    parser.add_argument("--repeats", type=int, default=3)
    args = parser.parse_args()
    if args.repeats < 1:
        parser.error("--repeats must be positive")
    binaries = [args.baseline.resolve(), args.candidate.resolve()]
    try:
        rows = json.loads((ROOT / "test/performance/manifest.json").read_text())
    except (OSError, ValueError) as error:
        parser.error(f"Cannot read performance manifest: {error}")
    totals = [0.0, 0.0]
    with tempfile.TemporaryDirectory(prefix="sat-compare-") as tmp:
        for row in rows:
            samples = [[], []]
            solved = [0, 0]
            for repeat in range(args.repeats):
                # Alternate execution order to reduce systematic drift.
                for index in ([0, 1] if repeat % 2 == 0 else [1, 0]):
                    elapsed, ok = run(binaries[index], row, Path(tmp))
                    samples[index].append(elapsed)
                    solved[index] += ok
            medians = [statistics.median(x) for x in samples]
            for i in range(2):
                totals[i] += medians[i]
            print(row["file"],
                  f"baseline={medians[0]:.4f}s candidate={medians[1]:.4f}s",
                  f"verified={solved[0]}/{args.repeats},{solved[1]}/{args.repeats}",
                  flush=True)
    print(f"Sum of per-case median PAR-2: {totals[0]:.4f}s -> {totals[1]:.4f}s")


if __name__ == "__main__":
    main()
