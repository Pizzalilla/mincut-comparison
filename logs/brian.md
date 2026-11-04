# Bug and design log: Brian

One entry per bug or decision, written when it happens: date, what went
wrong or what the choice was, how it was resolved, and why.

| Date | What went wrong | How was it resolved and why |
|---|---|---|
| 2026-10-30 | Each contraction step re-scans every edge and relabels every vertex, thus one trial is O(n*m) instead of O(m). | Gave each edge a random key -ln(U)/w (U uniform in (0,1)) |
| 2026-10-30 | Algorithm ran 1000 times per run. | Removed TRIALS = 1000 and the loop that uses it so that it only runs 1 trial per run. |