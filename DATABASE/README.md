# Dreamcore Research Database

**Status:** structured research seed data; not a live directory.  
**Snapshot:** 2026-09-24

The JSON files are intentionally conservative. They distinguish source evidence, classification, and adoption. Empty or limited fields mean “not established,” not “none.”

## Files

- `schema.json` — proposed record relationships and field descriptions.
- `sources.json` — source register with URLs, access dates, source type, and limitations.
- `games.json` — candidate, adjacent, and aesthetic-overlap works.
- `developers.json` — credited creators/publishers observed in public sources.
- `terminology.json` — terms and definitions/status.
- `articles.json` — journalism and commentary.
- `academic_references.json` — academic sources and what they do/do not establish.
- `genre_classifications.json` — evidence-based classifications.
- `adoption_events.json` — voluntary term-use events; no external adoption is currently verified.

## Data rules

- Do not overwrite a source record when a page changes; add a new observation with its date.
- Keep `self_described` separate from `third_party_described`.
- Keep `definition_match` separate from `adoption_eligible`.
- Use `null` for unknown values; do not fill gaps with guesses.
- Store sensitive source descriptions as abstracted text only.
- Do not treat store tags as adoption.
