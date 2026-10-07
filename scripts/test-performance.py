#!/usr/bin/env python3
"""One-command local performance run with independent witness/proof checking."""

import json, subprocess, tempfile, time, os, signal
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def clauses(path):
    pending = []
    for line in path.read_text().splitlines():
        if not line or line[0] in "cp":
            continue
        for token in line.split():
            lit = int(token)
            if lit:
                pending.append(lit)
            else:
                yield pending
                pending = []


def main():
    rows = json.loads((ROOT / "test/performance/manifest.json").read_text())
    solved = 0
    par2 = 0
    invalid = 0
    for row in rows:
        cnf = ROOT / "test/performance" / row["file"]
        limit = row["seconds"]
        with tempfile.TemporaryDirectory(prefix="sat-public-") as directory:
            proof = Path(directory) / "proof.out"
            output = Path(directory) / "output.txt"
            started = time.monotonic()
            timeout = False
            with output.open("wb") as out:
                p = subprocess.Popen(
                    [str(ROOT / "build/coursesat"), str(cnf), str(proof)],
                    stdout=out,
                    stderr=subprocess.DEVNULL,
                    start_new_session=True,
                )
                try:
                    p.wait(timeout=limit)
                except subprocess.TimeoutExpired:
                    timeout = True
                    os.killpg(p.pid, signal.SIGKILL)
                    p.wait()
            elapsed = min(time.monotonic() - started, limit)
            good = False
            status = "未解出"
            if not timeout and p.returncode in (10, 20):
                reported = "SAT" if p.returncode == 10 else "UNSAT"
                if reported == row["expected"]:
                    if reported == "SAT":
                        values = {}
                        try:
                            for line in output.read_text().splitlines():
                                if line.startswith("v "):
                                    for token in line[2:].split():
                                        lit = int(token)
                                        if lit:
                                            if abs(lit) in values and values[
                                                abs(lit)
                                            ] != (lit > 0):
                                                raise ValueError()
                                            values[abs(lit)] = lit > 0
                            good = all(
                                any(values.get(abs(lit)) == (lit > 0) for lit in c)
                                for c in clauses(cnf)
                            )
                        except ValueError:
                            good = False
                    elif proof.is_file() and proof.stat().st_size:
                        try:
                            good = (
                                subprocess.run(
                                    [
                                        str(ROOT / "build/drat-trim"),
                                        str(cnf),
                                        str(proof),
                                    ],
                                    stdout=subprocess.DEVNULL,
                                    stderr=subprocess.DEVNULL,
                                    timeout=limit * 3,
                                ).returncode
                                == 0
                            )
                        except subprocess.TimeoutExpired:
                            status = "验证超时"
                if not good and status != "验证超时":
                    status = "答案或证明错误"
                    invalid += 1
            if good:
                solved += 1
                par2 += elapsed
                status = f"通过 {elapsed:.3f}s"
            else:
                par2 += 2 * limit
            print(row["file"], status, flush=True)
    print(f"Public performance: {solved}/{len(rows)}, PAR-2={par2:.3f}s")
    print("仅用于本地比较；网站 private 评测独立计分。")
    raise SystemExit(1 if invalid else 0)


if __name__ == "__main__":
    main()
