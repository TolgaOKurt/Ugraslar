Hi, I'm Tolga Kurt, born in Istanbul, Turkey. I've loved math for as long as I can remember — it was always the subject I did best in, even as a kid — so I went on to study Mathematics Engineering at Yıldız Technical University, and I completed the degree there.

I'm the author of sequence A332749. It's a simple one — I found it back in high school while playing around with triangular numbers and modular arithmetic, summing up cycle sums. Nothing groundbreaking, but my teacher told me I should submit it, so I did.

In high school and then through university, I've kept thinking about and tinkering with all sorts of things: game design, optimization, organization, philosophy, foreign languages and etymology, board and box games and systems (including designing my own), family trees and connections, religions and religious stories, space, and self-sufficient houses/organizations/systems.

In math specifically, I'm drawn to combinatorics and Markov chains — I find it genuinely fun to sit down and study how a randomly moving pawn behaves in different kinds of environments. I like comparing approaches, researching, trying things, getting them wrong, and learning from it.

I've also worked on problems in the spirit of the postage stamp problem.

Find me here:

- LinkedIn: www.linkedin.com/in/tolgaokurt/

- GitHub: github.com/TolgaOKurt

- A website I tried building: tolgaokurt.github.io/TK/

- E-mail: tolgaokurt@outlook.com

So yeah — just trying things out. Among the things I randomly run into, I come across all kinds of sequences too, and hopefully I'll clean a few of those up and add them here as well.

If I get the chance to do a master's, I'd like it to be in Math Engineering / Applied Math / Industrial Math, or something along those lines.

To me, the most valuable thing in the world is knowledge, intelligence — a smart, thinking person.


----

== Sequence A398332: The 2-Postage Stamp Problem with Subtraction ==

=== 1. What is the 2-Postage Stamp Problem with Subtraction? ===

Given a set B = {b_1, b_2, ..., b_n} of n positive integers with 1 <= b_1 < b_2 < ... < b_n, let S(B) be the set of all integers of the form:
  b_i,  b_i + b_j,  or  |b_j - b_i|    (for 1 <= i <= j <= n).

The sequence value a(n) is the maximum integer M such that all integers 1 through M can be expressed in S(B):
  {1, 2, ..., M} is a subset of S(B).

This is the signed (additive-subtractive) generalization of the classical 2-postage stamp problem (A001212). In the classical problem, an envelope can hold at most two stamps with addition only:
  S_add(B) = {b_i} U {b_i + b_j | 1 <= i <= j <= n}.

Because subtraction is permitted, each stamp denomination acts as a two-way bridge:
* Addition reaches forward: b_k + d
* Subtraction reaches backward: b_k - d

This bidirectional reach roughly doubles the local spanning density around every large step, lifting the asymptotic growth from a_add(n) ~ 0.25 * n^2 in the classical additive problem to a(n) ~ 0.56 * n^2 here.


=== 2. The Delta Sequence (Step Increments) Representation ===

To analyze patterns in these sets and search for configurations, a set B can be uniquely represented by its vector of consecutive step increments:
  Delta = [d_0, d_1, ..., d_(n-1)]
where:
* d_0 = b_1
* d_i = b_(i+1) - b_i   (for i >= 1)

The elements of set B are recovered through cumulative prefix sums:
  b_1 = d_0
  b_2 = d_0 + d_1
  b_k = d_0 + d_1 + ... + d_(k-1)
  b_n = sum(Delta)

Why the Delta representation is helpful:
* Decoupled ordering: Instead of maintaining the coupled chain 1 <= b_1 < b_2 < ... < b_n, any positive vector with d_i >= 1 automatically yields a valid, strictly increasing set B.
* Contiguous sub-array sums: Every pairwise difference b_j - b_i (for j > i) maps directly to a contiguous sub-array sum of Delta:
    b_j - b_i = d_i + d_(i+1) + ... + d_(j-1)
* Periodic compression: Long arithmetic progressions [12, 24, 36, 48, ...] collapse into constant runs [12, 12, 12, ...], making large structured sets easy to describe and parameterize.
* Residue completion: Placing smaller steps at the tail acts as a modular filter to eliminate remainder voids and parity gaps.


=== 3. Structural Patterns Observed in Solutions ===

Exhaustive searches for small n and systematic exploration up to n = 100 suggest that optimal configurations fall into a few recurring structural patterns:

1. Tight Modular Arithmetic Chains (n <= 29):
* Structure: An arithmetic progression body (steps of 4, 6, 8, 10, 11, or 12) paired with a residue tail to cover remainders.
* Density: a(n) / max(B) ~ 2.00, where max(B) = a(n) / 2.
* Linear region: For 20 <= n <= 29, the "Step-12" family (step 12 body with an 11-element tail) consistently provides the best known values via:
    a(n) = 24*n - 216

2. Bipartite Bridge Sequences (n <= 8):
* Structure: Partitions set B into two dense clusters separated by a wide central bridge gap. Cross-cluster differences bridge intermediate numbers.
* Density: a(n) / max(B) ~ 1.25.

3. Asymmetric Extreme-Jump Sequences (n <= 8):
* Structure: Concludes with a single large jump d_last = a(n) / 2 such that max(B) = a(n). Direct subtractions max(B) - b_i cover the upper half [a(n)/2, a(n)].
* Density: a(n) / max(B) ~ 1.00.

4. Tail-End Difference Bases (n >= 30):
* Context & earlier experiments: For larger n, difference bases with quadratic growth (~0.56 * n^2) surpass the linear chains. Standard difference basis constructions in literature often place unit elements at the very start (e.g., 1, 2, ..., r) followed by larger step multiples. In earlier tests, that layout overtook the linear Step-12 family around n = 33 (577 vs 576), which gave the earlier lower bound a(n) >= floor((n^2 + 2*n) / 2).
* Tail-end rearrangement: Moving the block of consecutive unit elements from the front to the tail works especially cleanly:
    Body: s multiples of step size B   -->  [B, 2*B, 3*B, ..., s*B]
    Tail: (B - 1) consecutive integers -->  [s*B + 1, s*B + 2, ..., s*B + (B - 1)]
  where B = floor(n/2) + 1 and s = n - (B - 1).
* How it covers numbers:
  - Differences generate remainders: (s*B + k) - s*B immediately produces all unit residues k in {1, 2, ..., B - 1}.
  - Sums reach further: Because the consecutive elements are attached to the largest multiples, their pairwise sums reach substantially higher:
      (s*B + B - 1) + (s*B + B - 1) = 2*s*B + 2*B - 2
    giving an unbroken range up to:
      a(n) = 2*s*B + 2*B - 4  ~  0.56 * n^2
* Crossover at n = 30:
  This simple arrangement begins outperforming the linear Step-12 chain at n = 30 (510 vs 504), providing clean lower bounds up through n = 100 (a(100) >= 5200).


=== 4. Proven Global Optima (n = 1 .. 8) ===

All 21 global optima for n <= 8 have been exhaustively classified and proven:

<pre>
>>> Size n = 1 | Proven Global Max a(1) = 2 | Solutions: 1
    Solution #1:
      Delta : [1]
      Set B : [1]

>>> Size n = 2 | Proven Global Max a(2) = 6 | Solutions: 1
    Solution #1:
      Delta : [2, 1]
      Set B : [2, 3]

>>> Size n = 3 | Proven Global Max a(3) = 10 | Solutions: 3
    Solution #1:
      Delta : [2, 2, 1]
      Set B : [2, 4, 5]
    Solution #2:
      Delta : [3, 1, 1]
      Set B : [3, 4, 5]
    Solution #3:
      Delta : [1, 4, 3]
      Set B : [1, 5, 8]

>>> Size n = 4 | Proven Global Max a(4) = 16 | Solutions: 3
    Solution #1:
      Delta : [1, 1, 6, 5]
      Set B : [1, 2, 8, 13]
    Solution #2:
      Delta : [3, 3, 1, 1]
      Set B : [3, 6, 7, 8]
    Solution #3:
      Delta : [4, 2, 1, 2]
      Set B : [4, 6, 7, 9]

>>> Size n = 5 | Proven Global Max a(5) = 24 | Solutions: 1
    Solution #1:
      Delta : [4, 4, 2, 1, 2]
      Set B : [4, 8, 10, 11, 13]

>>> Size n = 6 | Proven Global Max a(6) = 32 | Solutions: 2
    Solution #1:
      Delta : [4, 4, 4, 2, 1, 2]
      Set B : [4, 8, 12, 14, 15, 17]
    Solution #2:
      Delta : [5, 2, 5, 1, 2, 1]
      Set B : [5, 7, 12, 13, 15, 16]

>>> Size n = 7 | Proven Global Max a(7) = 40 | Solutions: 8
    Solution #1:
      Delta : [2, 2, 1, 14, 1, 11, 1]
      Set B : [2, 4, 5, 19, 20, 31, 32]
    Solution #2:
      Delta : [2, 9, 2, 1, 4, 1, 2]
      Set B : [2, 11, 13, 14, 18, 19, 21]
    Solution #3:
      Delta : [2, 11, 1, 2, 1, 5, 1]
      Set B : [2, 13, 14, 16, 17, 22, 23]
    Solution #4:
      Delta : [4, 4, 4, 4, 2, 1, 2]
      Set B : [4, 8, 12, 16, 18, 19, 21]
    Solution #5:
      Delta : [5, 5, 5, 2, 1, 1, 2]
      Set B : [5, 10, 15, 17, 18, 19, 21]
    Solution #6:
      Delta : [5, 6, 2, 4, 2, 1, 20]
      Set B : [5, 11, 13, 17, 19, 20, 40]
    Solution #7:
      Delta : [6, 6, 3, 1, 1, 2, 1]
      Set B : [6, 12, 15, 16, 17, 19, 20]
    Solution #8:
      Delta : [7, 2, 6, 2, 2, 1, 20]
      Set B : [7, 9, 15, 17, 19, 20, 40]

>>> Size n = 8 | Proven Global Max a(8) = 52 | Solutions: 3
    Solution #1:
      Delta : [2, 2, 2, 1, 18, 1, 15, 1]
      Set B : [2, 4, 6, 7, 25, 26, 41, 42]
    Solution #2:
      Delta : [5, 3, 11, 1, 2, 1, 6, 26]
      Set B : [5, 8, 19, 20, 22, 23, 29, 55]
    Solution #3:
      Delta : [6, 6, 6, 3, 1, 1, 2, 1]
      Set B : [6, 12, 18, 21, 22, 23, 25, 26]
</pre>


=== 5. Best-Known Lower Bounds (n = 9 .. 29): Linear Modular Era ===

For 9 <= n <= 29, periodic modular arithmetic families provide the best known lower bounds:

<pre>
>>> Size n =  9 | Best-Known a(9)  =  64 | Delta: [6, 6, 6, 6, 3, 1, 1, 2, 1]
>>> Size n = 10 | Best-Known a(10) =  76 | Delta: [6, 6, 6, 6, 6, 3, 1, 1, 2, 1]
>>> Size n = 11 | Best-Known a(11) =  90 | Delta: [8, 8, 8, 8, 4, 2, 3, 1, 1, 2, 2]
>>> Size n = 12 | Best-Known a(12) = 106 | Delta: [8, 8, 8, 8, 8, 4, 2, 3, 1, 1, 2, 2]
>>> Size n = 13 | Best-Known a(13) = 122 | Delta: [8, 8, 8, 8, 8, 8, 4, 2, 3, 1, 1, 2, 2]
>>> Size n = 14 | Best-Known a(14) = 140 | Delta: [9, 9, 9, 9, 9, 9, 1, 4, 3, 3, 1, 1, 2, 1]
>>> Size n = 15 | Best-Known a(15) = 158 | Delta: [10, 10, 10, 10, 10, 10, 2, 3, 1, 5, 2, 1, 3, 1, 1]
>>> Size n = 16 | Best-Known a(16) = 178 | Delta: [10, 10, 10, 10, 10, 10, 10, 2, 3, 1, 5, 2, 1, 3, 1, 1]
>>> Size n = 17 | Best-Known a(17) = 198 | Delta: [10, 10, 10, 10, 10, 10, 10, 10, 2, 3, 1, 5, 2, 1, 3, 1, 1]
>>> Size n = 18 | Best-Known a(18) = 220 | Delta: [11, 11, 11, 11, 11, 11, 11, 11, 3, 4, 1, 5, 2, 1, 1, 3, 1, 2]
>>> Size n = 19 | Best-Known a(19) = 242 | Delta: [11, 11, 11, 11, 11, 11, 11, 11, 11, 3, 4, 1, 5, 2, 1, 1, 3, 1, 2]
>>> Size n = 20 | Best-Known a(20) = 264 | Delta: [12, 12, 12, 12, 12, 12, 12, 12, 12, 2, 5, 3, 6, 1, 1, 2, 1, 2, 2, 2]
>>> Size n = 21 | Best-Known a(21) = 288 | Delta: [12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 2, 5, 3, 6, 1, 1, 2, 1, 2, 2, 2]
>>> Size n = 22 | Best-Known a(22) = 312 | Delta: [12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 2, 5, 3, 6, 1, 1, 2, 1, 2, 2, 2]
>>> Size n = 23 | Best-Known a(23) = 336 | Delta: [12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 2, 5, 3, 6, 1, 1, 2, 1, 2, 2, 2]
>>> Size n = 24 | Best-Known a(24) = 360 | Delta: [12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 2, 5, 3, 6, 1, 1, 2, 1, 2, 2, 2]
>>> Size n = 25 | Best-Known a(25) = 384 | Delta: [12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 2, 5, 3, 6, 1, 1, 2, 1, 2, 2, 2]
>>> Size n = 26 | Best-Known a(26) = 408 | Delta: [12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 2, 5, 3, 6, 1, 1, 2, 1, 2, 2, 2]
>>> Size n = 27 | Best-Known a(27) = 432 | Delta: [12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 2, 5, 3, 6, 1, 1, 2, 1, 2, 2, 2]
>>> Size n = 28 | Best-Known a(28) = 456 | Delta: [12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 2, 5, 3, 6, 1, 1, 2, 1, 2, 2, 2]
>>> Size n = 29 | Best-Known a(29) = 480 | Delta: [12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 2, 5, 3, 6, 1, 1, 2, 1, 2, 2, 2]
</pre>


=== 6. Best-Known Lower Bounds for Larger Sizes (n = 30 .. 100) ===

Starting at n = 30, the tail-end difference basis begins yielding higher values than the linear families (a(30) >= 510 compared to 504). Below are sample configurations from this family:

<pre>
>>> Size n = 30 | Best-Known a(30) = 510 | (B=16, s=15)
      Delta : [16 x 15 times, followed by 1 x 15 times]
      Set B : [16, 32, 48, 64, 80, 96, 112, 128, 144, 160, 176, 192, 208, 224, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255]

>>> Size n = 31 | Best-Known a(31) = 542 | (B=16, s=16)
      Delta : [16 x 16 times, followed by 1 x 15 times]
      Set B : [16, 32, ..., 256, 257, 258, ..., 271]

>>> Size n = 32 | Best-Known a(32) = 576 | (B=17, s=16)
      Delta : [17 x 16 times, followed by 1 x 16 times]

>>> Size n = 33 | Best-Known a(33) = 610 | (B=17, s=17)
      Delta : [17 x 17 times, followed by 1 x 16 times]

>>> Size n = 34 | Best-Known a(34) = 646 | (B=18, s=17)
      Delta : [18 x 17 times, followed by 1 x 17 times]

>>> Size n = 35 | Best-Known a(35) = 682 | (B=18, s=18)
      Delta : [18 x 18 times, followed by 1 x 17 times]

>>> Size n = 36 | Best-Known a(36) = 720 | (B=19, s=18)
>>> Size n = 37 | Best-Known a(37) = 758 | (B=19, s=19)
>>> Size n = 38 | Best-Known a(38) = 798 | (B=20, s=19)
>>> Size n = 39 | Best-Known a(39) = 838 | (B=20, s=20)
>>> Size n = 40 | Best-Known a(40) = 880 | (B=21, s=20)
>>> Size n = 50 | Best-Known a(50) = 1350 | (B=26, s=25)
>>> Size n = 60 | Best-Known a(60) = 1920 | (B=31, s=30)
>>> Size n = 70 | Best-Known a(70) = 2590 | (B=36, s=35)
>>> Size n = 80 | Best-Known a(80) = 3360 | (B=41, s=40)
>>> Size n = 90 | Best-Known a(90) = 4230 | (B=46, s=45)

>>> Size n = 100 | Best-Known a(100) = 5200 | (B=51, s=50)
      Delta : [51 x 50 times, followed by 1 x 50 times]
      Set B : [51, 102, 153, ..., 2550, 2551, 2552, ..., 2600]
</pre>

Note on Verification:
All sequences listed above have been computationally verified using exact bitset reachability algorithms, confirming complete, gapless representation across the entire continuous interval [1, a(n)]. Additional data, verification tools, and extended sequences are available on GitHub: https://github.com/TolgaOKurt/Ugraslar/tree/main/PSPP/pspp%200826

