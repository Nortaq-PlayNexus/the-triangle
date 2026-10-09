# Dreamcore Classification Criteria

**Version:** 1.0-draft  
**Date:** 2026-09-24

This document defines a repeatable, privacy-conscious review process. It is intentionally stricter than a mood or tag test.

## Classification statuses

- **Confirmed:** all mandatory criteria are met and evidence is sufficient for a public entry.
- **Probable:** evidence is strong but a bounded detail is missing; public wording must say “probable” or “candidate.”
- **Candidate:** the creator expressed interest or supplied preliminary evidence, but review is incomplete.
- **Adjacent:** the work overlaps in theme, aesthetic, personal history, or horror tone but fails a mandatory criterion.
- **Not classifiable:** public evidence is insufficient or contradictory.
- **Withdrawn:** the creator asked for removal or withdrew permission.

## Evidence rubric

Reviewers score each dimension from 0–3. The score supports consistency; it does not replace judgment.

### A. Source provenance

| Score | Standard |
|---:|---|
| 0 | No evidence of a personal dream source |
| 1 | Inference from title, tags, imagery, or third-party description |
| 2 | Creator statement that identifies a dream experience or dream journal |
| 3 | Dated creator statement plus a design note, consented excerpt, interview, prototype trail, or other traceable record |

### B. Material influence

| Score | Standard |
|---:|---|
| 0 | No identifiable influence |
| 1 | Decoration or atmosphere only |
| 2 | A named environment, event, entity, narrative beat, or mechanic is linked to the source |
| 3 | Multiple central axes or the core progression/rules are linked to the source |

### C. Interactive work

| Score | Standard |
|---:|---|
| 0 | Static or non-interactive work only |
| 1 | Primarily static work with a limited interactive component |
| 2 | Game with meaningful player action or choice |
| 3 | Game whose interaction is central to the proposed source transformation |

### D. Horror branch (only for Dreamcore Horror)

| Score | Standard |
|---:|---|
| 0 | No horror intention evidenced |
| 1 | Horror tone is inferential or incidental |
| 2 | Creator/publisher clearly presents the work as horror |
| 3 | Horror intention and a source-derived horror expression are both documented |

### E. Authorship and consent

| Score | Standard |
|---:|---|
| 0 | Source ownership is unclear or permission is absent |
| 1 | Ambiguous team/collaborator attribution |
| 2 | Credited creator and source are reasonably clear |
| 3 | Team attribution, consent, and scope are documented |

## Provisional decision rule

A base Dreamcore classification requires:

- A >= 2;
- B >= 2;
- C >= 2;
- E >= 2;
- no unresolved contradiction that would change the result.

A Dreamcore Horror classification additionally requires:

- D >= 2;
- a documented or clearly attributable frightening personal source;
- horror intention stated by the creator or an authoritative project source.

A score of 3 is not required everywhere. A short, well-documented game can be Dreamcore without having a large production history.

## Materiality test

Ask: **If the personal dream source were removed, would a central part of the work change?**

- **Yes—world:** the spaces, routes, architecture, or environmental rules would change.
- **Yes—narrative:** the events, characters, chronology, or ending would change.
- **Yes—mechanic:** the player's actions or rules would change.
- **Yes—entity/sound:** the central presence, motif, or audio language would change.
- **No—surface only:** only palette, fog, or a generic mood would change; do not classify on that basis.

A creator may intentionally transform a dream into fiction. The test asks about causal influence, not literal fidelity.

## Source hierarchy

1. Direct creator statement or consented interview.
2. Official game/project page with a provenance statement.
3. Design document, development log, talk, or documented prototype history.
4. Reputable journalism that clearly attributes the claim.
5. Community discussion or search result, used only as a lead.

A lower-tier source may be retained, but must not silently override a contradictory primary source.

## Privacy-preserving evidence options

Accepted evidence can be:

- a public statement without private details;
- a redacted journal excerpt with consent;
- a design memo describing a motif or rule;
- a dated developer interview;
- a prototype-to-final comparison;
- a private evidence record reviewed by the directory maintainer without publication.

Do not require names of people, locations, diagnoses, medications, trauma details, graphic content, or a complete dream account.

## Contradiction handling

If sources disagree:

1. Record both claims with source IDs.
2. Prefer the direct creator statement for provenance, while preserving the conflict.
3. Do not upgrade a headline into a creator quotation.
4. Lower the classification to Candidate or Unresolved.
5. Ask for clarification only with the creator's consent and without pressure.

## THE TRIANGLE application

Current classification: **Provisional Candidate / self-described basis pending corroboration**.

- The user brief states personal night-terror provenance.
- The repository strongly supports dream/nightmare theme and mechanics.
- The repository does not contain a source journal or public design record mapping personal experiences to the implemented elements.
- The current build is a pre-alpha blockout and does not demonstrate the complete intended experience.

The classification must not be used as a reason to change gameplay.

## Reviewer checklist

- [ ] Source is the creator's own experience, not merely a dream-like look.
- [ ] Interactive work requirement is met.
- [ ] At least one central axis is materially linked.
- [ ] Horror branch has both frightening source and horror intent.
- [ ] Evidence is cited and dated.
- [ ] Sensitive details are minimized.
- [ ] Consent and team attribution are recorded.
- [ ] Contradictory evidence is visible.
- [ ] Status is no stronger than the evidence.
