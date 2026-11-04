"""plots.py: figures and summary tables for the report, from results/trial.csv.

Written by Claude (AI) at Kartikay's request on 2026-11-04.
Usage: python3 analysis/plots.py results/trial.csv analysis/sw_flips.csv report/figures
"""
import csv, math, sys, collections, statistics as st, os
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
from matplotlib.ticker import FuncFormatter, LogLocator

trial, flips_csv, out = sys.argv[1], sys.argv[2], sys.argv[3]
os.makedirs(out, exist_ok=True)

# Families from hardest to easiest for Karger, the same order in every figure.
FAM = ["random_dense", "many_cuts", "random_sparse", "barbell", "cycle"]
ALG = ["stoer-wagner", "karger", "karger-stein"]
NAME = {"stoer-wagner": "Stoer-Wagner", "karger": "Karger", "karger-stein": "Karger-Stein"}
NS = [25, 50, 100]
FAMC = dict(zip(FAM, ["#2a78d6", "#e87ba4", "#eb6834", "#1baf7a", "#eda100"]))
FAMM = dict(zip(FAM, ["o", "v", "s", "^", "D"]))
ALGC = dict(zip(ALG, ["#2a78d6", "#eb6834", "#1baf7a"]))
ALGM = dict(zip(ALG, ["o", "s", "^"]))
def tint(hexc, t):
    """Mix a colour with white: t = 1 is the colour itself, t = 0 is white."""
    r, g, b = (int(hexc[i:i + 2], 16) for i in (1, 3, 5))
    return "#%02x%02x%02x" % tuple(round(255 - (255 - c) * t) for c in (r, g, b))

# Figure 2 shades each algorithm's own colour by n: light = small n.
NSHADE = dict(zip(NS, [0.35, 0.65, 1.0]))
INK, MUTED, GRID = "#1f1f1e", "#6b6a64", "#e4e3de"
plt.rcParams.update({"font.size": 9, "font.family": "DejaVu Sans", "axes.edgecolor": MUTED,
                     "axes.labelcolor": INK, "xtick.color": MUTED, "ytick.color": MUTED,
                     "axes.spines.top": False, "axes.spines.right": False, "axes.grid": True,
                     "grid.color": GRID, "grid.linewidth": 0.6, "axes.axisbelow": True,
                     "lines.linewidth": 2, "legend.frameon": False, "figure.dpi": 200,
                     "savefig.bbox": "tight", "savefig.facecolor": "white"})

rows = collections.defaultdict(list)
for r in csv.DictReader(open(trial)):
    rows[(r["family"], int(r["n"]), r["algorithm"])].append(r)

def k99(p):
    return 1 if p >= 1 else (math.inf if p <= 0 else math.ceil(math.log(0.01) / math.log(1 - p)))

def wilson(s, n, z=1.96):
    p = s / n; d = 1 + z * z / n
    c = (p + z * z / (2 * n)) / d; h = z * math.sqrt(p * (1 - p) / n + z * z / (4 * n * n)) / d
    return max(0, c - h), min(1, c + h)

S = {}
for (f, n, a), R in rows.items():
    t = [float(r["time_ms"]) for r in R]; s = sum(int(r["success"]) for r in R)
    p = s / len(R); k = k99(p)
    lo, hi = wilson(s, len(R))
    S[(f, n, a)] = dict(med=st.median(t), mean=st.mean(t), p=p, lo=lo, hi=hi, k=k,
                        t99=k * st.mean(t), runs=len(R))

def ms(v, _=None):
    """Tick label for a time in ms: 0.001 ms is shown as 1 µs, 100 ms as 100 ms."""
    if v < 1e-3 - 1e-12: return f"{v * 1000:g} µs"
    if v < 1: return f"{v * 1000:g} µs" if v < 0.1 - 1e-12 else f"{v:g} ms"
    return f"{v:g} ms"

def rowaxis(ax):
    ax.set_yticks(range(len(FAM))); ax.set_yticklabels(FAM, color=INK)
    ax.set_ylim(len(FAM) - 0.5, -0.5); ax.grid(axis="y", visible=False)
    ax.tick_params(axis="y", length=0)

def title(ax, text):
    ax.set_title(text, color=INK, fontsize=10, loc="left", pad=8)

OFF = dict(zip(ALG, [-0.18, 0, 0.18]))  # small vertical offsets so dots in a row never overlap

# Figure 1: (a) time of one run at n = 100, (b) how much it grows when n doubles.
fig, (a1, a2) = plt.subplots(1, 2, figsize=(9.2, 3.2), gridspec_kw={"width_ratios": [1.25, 1]})
for i, f in enumerate(FAM):
    vals = [S[(f, 100, a)]["med"] for a in ALG]
    a1.plot([min(vals), max(vals)], [i, i], color=GRID, lw=1.5, zorder=1)
    for a in ALG:
        a1.plot(S[(f, 100, a)]["med"], i + OFF[a], ls="none", marker=ALGM[a], ms=8, color=ALGC[a],
                mec="white", mew=1.2, zorder=3, label=NAME[a] if i == 0 else None)
a1.set_xscale("log"); a1.set_xlim(2e-3, 300)
a1.xaxis.set_major_formatter(FuncFormatter(ms)); a1.minorticks_off()
rowaxis(a1); a1.set_xlabel("median time of one run (log scale)")
title(a1, "(a) Time of one run at n = 100")
for i, f in enumerate(FAM):
    g = [S[(f, 100, a)]["med"] / S[(f, 50, a)]["med"] for a in ALG]
    a2.plot([min(g), max(g)], [i, i], color=GRID, lw=1.5, zorder=1)
    for a, v in zip(ALG, g):
        a2.plot(v, i + OFF[a], ls="none", marker=ALGM[a], ms=8, color=ALGC[a], mec="white", mew=1.2,
                zorder=3)
for x, lab in [(2, "∝ n"), (4, "∝ n²"), (8, "∝ n³")]:
    a2.axvline(x, color=MUTED, lw=0.9, ls=(0, (3, 3)), zorder=0)
    a2.text(x, -0.72, lab, color=MUTED, fontsize=8, ha="center", va="center",
            bbox=dict(facecolor="white", edgecolor="none", pad=1.5))
a2.set_xlim(0, 14); a2.set_xticks([0, 2, 4, 6, 8, 10, 12, 14])
a2.set_xticklabels([f"×{t}" for t in [0, 2, 4, 6, 8, 10, 12, 14]])
rowaxis(a2); a2.set_ylim(len(FAM) - 0.5, -0.95); a1.set_ylim(len(FAM) - 0.5, -0.95)
a2.set_yticklabels([]); a2.set_xlabel("growth in run time from n = 50 to n = 100")
title(a2, "(b) Growth when n doubles")
fig.legend(loc="upper center", ncol=3, bbox_to_anchor=(0.5, 1.07), fontsize=9)
fig.tight_layout(w_pad=2)
fig.savefig(f"{out}/fig1_time_per_run.png"); plt.close(fig)

# Figure 2: probability that one run finds the minimum, by family and n.
fig, axs = plt.subplots(1, 2, figsize=(9.2, 3.2), sharey=True)
w = 0.26
for ax, a in zip(axs, ["karger", "karger-stein"]):
    for j, n in enumerate(NS):
        xs = [i + (j - 1) * w for i in range(len(FAM))]
        ps = [S[(f, n, a)]["p"] for f in FAM]
        lo = [max(0, p - S[(f, n, a)]["lo"]) for p, f in zip(ps, FAM)]
        hi = [max(0, S[(f, n, a)]["hi"] - p) for p, f in zip(ps, FAM)]
        ax.bar(xs, ps, width=w - 0.04, color=tint(ALGC[a], NSHADE[n]), label=f"n = {n}", zorder=2)
        ax.errorbar(xs, ps, yerr=[lo, hi], fmt="none", ecolor=INK, elinewidth=0.8, capsize=0, zorder=3)
        if a == "karger-stein":
            b = 1 / math.log(n)
            for x in xs:
                ax.plot([x - w / 2 + 0.02, x + w / 2 - 0.02], [b, b], color=INK, lw=1.2, zorder=4)
    ax.set_xticks(range(len(FAM))); ax.set_xticklabels(FAM, rotation=20, ha="right", color=INK)
    ax.set_ylim(0, 1.05); ax.set_yticks([0, 0.25, 0.5, 0.75, 1])
    ax.set_yticklabels(["0%", "25%", "50%", "75%", "100%"]); ax.grid(axis="x", visible=False)
    ax.tick_params(axis="x", length=0)
title(axs[0], "(a) Karger")
title(axs[1], "(b) Karger-Stein")
axs[0].set_ylabel("runs that find the minimum")
axs[0].text(-0.4, 0.76, "Bound 2/(n(n−1)) is below\n0.4% at every n: too small\nto draw.",
            color=MUTED, fontsize=7.5, ha="left", va="top")
from matplotlib.lines import Line2D
from matplotlib.patches import Patch
h = [Patch(color=tint("#6b6a64", NSHADE[n]), label=f"n = {n}") for n in NS]
h.append(Line2D([], [], color=INK, lw=1.2, label="Karger-Stein bound 1/ln n"))
fig.legend(handles=h, loc="upper center", ncol=4, bbox_to_anchor=(0.5, 1.08), fontsize=8.5)
fig.tight_layout(w_pad=2)
fig.savefig(f"{out}/fig2_success.png"); plt.close(fig)

# Figure 3: (a) time to 99% at n = 100, (b) Karger and (c) Karger-Stein relative to Stoer-Wagner.
fig = plt.figure(figsize=(10.5, 3.3))
gs = fig.add_gridspec(1, 3, width_ratios=[1.3, 1, 1], wspace=0.75)
a1, a2, a3 = fig.add_subplot(gs[0]), fig.add_subplot(gs[1]), fig.add_subplot(gs[2])
for i, f in enumerate(FAM):
    vals = [S[(f, 100, a)]["t99"] for a in ALG]
    a1.plot([min(vals), max(vals)], [i, i], color=GRID, lw=1.5, zorder=1)
    for a in ALG:
        a1.plot(S[(f, 100, a)]["t99"], i + OFF[a], ls="none", marker=ALGM[a], ms=8, color=ALGC[a],
                mec="white", mew=1.2, zorder=3, label=NAME[a] if i == 0 else None)
a1.set_xscale("log"); a1.set_xlim(2e-3, 600); a1.set_xticks([1e-2, 1, 1e2])
a1.xaxis.set_major_formatter(FuncFormatter(ms)); a1.minorticks_off()
rowaxis(a1); a1.set_xlabel("time to 99% confidence (log scale)")
title(a1, "(a) Time to 99% at n = 100")

def ratio_panel(ax, a, ylim, ticks, label):
    for f in FAM:
        r = [S[(f, n, a)]["t99"] / S[(f, n, "stoer-wagner")]["t99"] for n in NS]
        ax.plot(NS, r, color=ALGC[a], marker=FAMM[f], ms=6, mec="white", mew=1, lw=1.6)
        ax.annotate(f, (NS[-1], r[-1]), xytext=(6, 0), textcoords="offset points", va="center",
                    fontsize=7.5, color=INK)
    ax.set_xscale("log"); ax.set_yscale("log"); ax.set_xticks(NS)
    ax.set_xticklabels([str(n) for n in NS]); ax.minorticks_off(); ax.set_xlim(20, 125)
    ax.set_ylim(*ylim); ax.set_yticks(ticks); ax.set_yticklabels([label(t) for t in ticks])
    ax.set_xlabel("n (vertices)")

ratio_panel(a2, "karger", (0.008, 80), [0.01, 0.1, 1, 10],
            lambda t: "tie" if t == 1 else (f"{1 / t:g}× faster" if t < 1 else f"{t:g}× slower"))
a2.axhline(1, color=INK, lw=1, zorder=0)
title(a2, "(b) Karger vs Stoer-Wagner")
ratio_panel(a3, "karger-stein", (80, 1500), [100, 200, 500, 1000], lambda t: f"{t:g}× slower")
a3.set_ylabel("")
title(a3, "(c) Karger-Stein vs Stoer-Wagner")
# Two labels sit too close at n = 100 on panel (c); nudge them apart.
for t in a3.texts:
    if t.get_text() == "many_cuts": t.set_position((6, 5))
    if t.get_text() == "random_dense": t.set_position((6, -5))
    if t.get_text() == "cycle": t.set_position((6, 4))
    if t.get_text() == "random_sparse": t.set_position((6, -4))
a1.legend(loc="upper center", ncol=3, bbox_to_anchor=(1.55, 1.25), fontsize=9)
fig.savefig(f"{out}/fig3_time_to_99.png"); plt.close(fig)

# Figure 4: Stoer-Wagner time at n = 100 against the irregularity of its inA pattern.
fl = collections.defaultdict(list)
for r in csv.DictReader(open(flips_csv)):
    if r["n"] == "100": fl[r["family"]].append(int(r["flips"]))
xs = [st.mean(fl[f]) / 1000 for f in FAM]; ys = [S[(f, 100, "stoer-wagner")]["med"] for f in FAM]
mx, my = st.mean(xs), st.mean(ys)
slope = sum((x - mx) * (y - my) for x, y in zip(xs, ys)) / sum((x - mx) ** 2 for x in xs)
fig, ax = plt.subplots(figsize=(5.2, 3.3))
ax.plot([0, 135], [my - slope * mx, my + slope * (135 - mx)], color=MUTED, lw=1, ls=(0, (3, 3)), zorder=1)
ax.text(162, 0.06, f"dashed line: least-squares fit, {slope * 1e3:.1f} ns per change", color=MUTED,
        fontsize=7.5, ha="right", va="bottom")
nudge = {"random_dense": (8, 4), "random_sparse": (8, -6), "barbell": (8, -2), "many_cuts": (8, 6),
         "cycle": (8, -8)}
for f, x, y in zip(FAM, xs, ys):
    ax.plot([x], [y], marker=FAMM[f], color=FAMC[f], ms=9, ls="none", mec="white", mew=1.2, zorder=3)
    ax.annotate(f"{f}  ({y:.2f} ms)", (x, y), xytext=nudge[f], textcoords="offset points",
                fontsize=8, color=INK, va="center")
ax.set_xlabel("times inA changes value along the scan (thousands per run)")
ax.set_ylabel("median time of one run (ms)")
ax.set_xlim(0, 165); ax.set_ylim(0, 1.1)
title(ax, "Stoer-Wagner at n = 100: same work, different speed")
fig.savefig(f"{out}/fig4_sw_branches.png"); plt.close(fig)

# Summary table for the report.
with open(f"{out}/summary.csv", "w", newline="") as fh:
    wr = csv.writer(fh)
    wr.writerow(["family", "n", "algorithm", "median_ms", "p", "p_lo95", "p_hi95", "runs_99", "time_99_ms"])
    for f in FAM:
        for n in NS:
            for a in ALG:
                s = S[(f, n, a)]
                wr.writerow([f, n, a, f"{s['med']:.4f}", f"{s['p']:.3f}", f"{s['lo']:.3f}", f"{s['hi']:.3f}",
                             s["k"], f"{s['t99']:.3f}"])
print("wrote figures to", out)
