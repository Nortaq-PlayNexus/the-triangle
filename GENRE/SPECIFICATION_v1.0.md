# Dreamcore Genre Specification v1.0

**Status:** proposed, voluntary, non-exclusive specification  
**Version:** 1.0-draft  
**Date:** 2026-09-24  
**Maintainer:** THE TRIANGLE initiative (research project; not an industry standards body)

## 1. Purpose

This specification provides a shared vocabulary for discussing interactive works substantially derived from their creators' own dream experiences. It is designed for research, criticism, voluntary self-description, and a future directory. It does not create a legal right, platform standard, or exclusive club.

## 2. Normative terms

- **MUST**: required for the stated classification.
- **SHOULD**: strongly recommended; deviation requires a note.
- **MAY**: optional and not evidence of classification.
- **MUST NOT**: a claim that would misrepresent the category.

## 3. Base classification

A work **MUST**:

1. be interactive;
2. identify a dream experience of the creator or credited creator-team member as a source;
3. show material influence on at least one central creative axis;
4. provide traceable evidence sufficient for review;
5. make authorship and any team consent clear.

A work **MAY** use any camera, perspective, engine, platform, narrative structure, visual style, or conventional game genre.

A work **MUST NOT** be classified solely because it:

- looks dreamlike;
- is about a dream;
- uses dream, nightmare, or dreamcore tags;
- borrows another person's dream diary without an approved team rule;
- borrows fictional nightmares or general horror imagery.

## 4. Hierarchy

v1.0 recognizes only one documented branch:

```text
Dreamcore
└── Dreamcore Horror
```

“Adventure,” “exploration,” and “narrative” are ordinary production/medium descriptors, not new Dreamcore subgenres. They may be used as secondary labels when useful, but should not be presented as formally defined branches until distinct criteria and adoption are demonstrated.

## 5. Metadata fields

A conforming record SHOULD contain:

- `title`
- `developer_or_creator`
- `release_or_version_date` (or `unknown`)
- `platform`
- `genre_term_used`
- `self_described`
- `source_type`
- `source_summary` (abstracted if needed)
- `material_axes`
- `horror_intent` (`yes`, `no`, `unclear`)
- `evidence_urls_or_local_paths`
- `date_observed`
- `review_status`
- `confidence`
- `consent_status`
- `notes_and_contradictions`

## 6. Evidence levels

- **E0:** no qualifying provenance evidence.
- **E1:** indirect or third-party lead.
- **E2:** direct creator statement with a described source.
- **E3:** direct statement plus dated design/prototype/interview evidence.

An E2 or E3 record may be publicly classified if materiality and authorship are clear. E1 records remain candidates or leads.

## 7. Branch rule: Dreamcore Horror

The work MUST meet all base requirements and MUST have:

- a documented personally experienced frightening dream, nightmare, night terror, or comparable source; and
- an attributable intention to produce horror.

A dream narrative, horror aesthetic, or generic nightmare theme is insufficient on its own.

## 8. Review outcomes

Use exactly one of:

`confirmed`, `probable`, `candidate`, `adjacent`, `not_classifiable`, `withdrawn`.

A public listing should show the status and date of review. A listing is not an endorsement or a legal finding.

## 9. Examples

| Work | Term | Result under v1.0 |
|---|---|---|
| `Clockwork Dreams` | Dreamcore | Direct creator statement that the game is based on personal dreams and contains 30+ dreams/nightmares; game-wide materiality is strong |
| `#21: The World` | Dreamcore | Confirmed candidate pending current directory review; direct creator dream-journal statement and material exploration premise |
| `Dream Diary` | Dreamcore | Confirmed candidate; direct real-dream statement and short adventure reconstruction |
| `Zemblini: Dream Journal` | Dreamcore | Confirmed candidate; direct real-dreams statement |
| `My Magical Dream Journal` | Dreamcore / Dreamcore Horror | Dreamcore probable; horror branch probable because creator tags and describes a true-dream horror work, but fuller intent evidence should be retained |
| `Archive Equinox` | Dreamcore | Confirmed candidate; direct dreamed-locations statement |
| `OMORI` | Dreamcore / adjacent | Substantial dream/lucid-dream/nightmare input, but also major non-dream influences; do not call it a wholesale dream adaptation |
| `Lucid Falls` | Dreamcore candidate | Direct statement that the core idea came from the creator's dreams; finished-game extent unresolved |
| `LSD: Dream Emulator` | Adjacent | Used a staff member's dream diary, not documented as the game creator's own dream |
| `Happy Game` | Not match | Writer explicitly rejects personal fear/dreams about evil demons as the main motivation |
| `My Nightmares Tell Me to Chase My Dreams` | Dreamcore | Component-level match; the first level is explicitly based on a recurring dream, while the prototype contains other technical test levels |
| `Dreamcore` (Montraluz/Tlön) | Adjacent | Existing title/aesthetic use; official material describes liminal/Backrooms spaces, not verified personal dream provenance |
| `Neverending Nightmares` | Adjacent | Official source describes the developer's mental-illness struggles, not a documented personal dream source |
| A surreal horror game with no provenance evidence | Not classified | Fails the mandatory provenance test |

## 10. Version changes

A future version must record:

- what changed;
- why the change was needed;
- affected examples;
- whether the change is backward-compatible;
- date and editor.

The specification must not be broadened merely to increase the number of entries. A narrower definition with transparent uncertainty is preferable to a larger but misleading category.

## 11. Non-goals

This specification does not:

- certify that a dream is real or psychologically meaningful;
- diagnose creators or players;
- define supernatural phenomena;
- replace platform accessibility, rating, or community standards;
- claim ownership of “Dreamcore”;
- require public disclosure of intimate experiences;
- create a monopoly on the word “dream.”
