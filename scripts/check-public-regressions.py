#!/usr/bin/env python3
"""Check public regression inputs and their returned models."""

import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
SOLVER = ROOT / "build" / "coursesat"
CASES = sorted((ROOT / "test" / "cnf" / "regressions").glob("*.cnf"))


def clauses(path):
    pending = []
    with path.open() as source:
        for line in source:
            if line.startswith(("p", "c")) or not line.strip():
                continue
            for token in line.split():
                value = int(token)
                if value:
                    pending.append(value)
                else:
                    yield pending
                    pending = []
    if pending:
        raise ValueError(f"incomplete CNF: {path}")


def model(output):
    values = {}
    for line in output.splitlines():
        if not line.startswith("v "):
            continue
        for token in line[2:].split():
            literal = int(token)
            if literal:
                variable = abs(literal)
                if variable in values and values[variable] != (literal > 0):
                    raise ValueError("contradictory model")
                values[variable] = literal > 0
    return values


def main():
    if len(CASES) < 3:
        raise SystemExit("public regression cases are missing")
    failures = 0
    for case in CASES:
        try:
            result = subprocess.run(
                [str(SOLVER), str(case)],
                text=True,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                timeout=15,
                check=False,
            )
            values = model(result.stdout)
            valid = (
                result.returncode == 10
                and values
                and all(
                    any(values.get(abs(lit)) == (lit > 0) for lit in clause)
                    for clause in clauses(case)
                )
            )
        except (OSError, ValueError, subprocess.TimeoutExpired):
            valid = False
        print(f"{case.name}: {'ok' if valid else 'FAILED'}", flush=True)
        failures += not valid
    print(
        f"public CDCL regressions: {len(CASES) - failures} ok, {failures} failed",
        flush=True,
    )
    raise SystemExit(1 if failures else 0)


if __name__ == "__main__":
    main()
