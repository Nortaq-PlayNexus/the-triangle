# Adoption Tracking Policy

**Version:** 1.0-draft  
**Date:** 2026-09-24

## Purpose

Track only public, voluntary evidence that a creator or project uses the proposed terminology in a meaningful way. The tracker is not a popularity leaderboard and does not infer adoption from automated tags.

## What counts as an adoption event

An event may be recorded when a public source shows that:

- a creator, developer, publisher, maintainer, or project explicitly uses “Dreamcore” or “Dreamcore Horror” as a self-description or category claim;
- the wording is connected to the proposed definition, rather than only the existing aesthetic; and
- the source is dated or retrievable enough to identify its context.

A source can be recorded as a **term-use event** even when it does not meet the proposed definition. Keep `adoption_eligible` separate from `definition_match`.

## What does not count

- An automated store tag.
- A search result or algorithmic recommendation.
- A fan saying a game “feels like” Dreamcore.
- A journalist using the word descriptively without attribution.
- The existence of a similarly named game or aesthetic.
- A creator expressing interest privately without permission to quote or classify it.
- An AI-generated summary that repeats a claim without a source.

## Evidence levels

- `A`: dated direct creator/developer statement with a clear definition statement.
- `B`: direct creator statement using the term but without a full definition.
- `C`: official project/publisher copy or attributed interview.
- `D`: third-party/community use; lead only.
- `U`: unresolved or inaccessible source.

## Status values

- `candidate`
- `self-described`
- `definition-match`
- `aesthetic-overlap`
- `adjacent`
- `not-match`
- `unresolved`
- `withdrawn`

## Privacy

A submission may use an abstracted source description. Do not publish a creator's private journal, medical information, trauma details, or identifying dream content without explicit consent. A creator can request correction, anonymity, or removal.

## Update procedure

1. Record the URL/path and date observed.
2. Quote only the minimum text necessary.
3. Identify whether the term is self-described or third-party.
4. Apply the criteria without inflating confidence.
5. Preserve contradictory evidence.
6. Never count a platform label as adoption.
7. Review the row before adding it to any public directory.

## Current dataset state

`ADOPTION_TRACKER.csv` contains public evidence located in the initial research pass, including examples that do **not** meet the proposed definition. It contains no verified external adoption of the proposed category. The row for THE TRIANGLE is provisional and based on the user's brief plus local project documentation, not a public launch.
