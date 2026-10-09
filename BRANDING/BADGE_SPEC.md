# Dreamcore Badge Specification — Draft

**Status:** concept only. No artwork, registration, domain, account, or public release is approved.

## Goal

A badge could let a creator voluntarily identify a work as a candidate or reviewed example of the proposed category. It communicates membership/classification, not ownership or quality.

## Required wording

The word **proposed** must appear in the badge or adjacent text.

### Base badge

```text
PROPOSED DREAMCORE
Personal dream source documented
```

### Horror badge

```text
PROPOSED DREAMCORE HORROR
Personal nightmare source documented
```

### Candidate badge

```text
DREAMCORE CANDIDATE
Evidence under review
```

### Adjacent badge

```text
DREAM-RELATED / ADJACENT
Not confirmed under the proposed definition
```

A final design should not imply that a creator has been permanently classified if the evidence is provisional.

## Visual direction

- Use an original, simple shape unrelated to platform badge silhouettes.
- Avoid controller silhouettes, storefront ribbons, platform logos, Epic/Unreal/Steam/PlayStation/Xbox trade dress, or copied gradients.
- Use a neutral, high-contrast palette that works in light and dark backgrounds.
- Include a text alternative; never rely on iconography alone.
- Keep the words “proposed” and “candidate” legible at small sizes.
- Do not use a dream-journal image, nightmare image, or creator's likeness without permission.

## Data fields

A future badge URL or record should contain:

- listing/game ID;
- classification version;
- status;
- evidence IDs;
- review date;
- consent status;
- withdrawal flag;
- artwork version.

## Optionality

Developers may use the badge, a text label, or neither. Refusing the badge must not affect a game's eligibility. The badge must not be used to rank games, manipulate reviews, imply age suitability, or state that a creator has a medical/psychological condition.

## Accessibility

- Minimum text size and contrast should be tested.
- Provide a plain-text sentence: “Proposed Dreamcore candidate; evidence and review date at [link].”
- Do not use flashing, strobing, or rapidly changing imagery.
- Do not rely on red/green color alone.
- Include content-appropriate alternatives for horror-themed marks.

## Review and legal gate

Before any public use:

1. obtain Australian IP/trade-mark review of the wording and design;
2. search for existing badge/label uses;
3. confirm that the design does not imitate a platform or certification mark;
4. publish a plain-language disclaimer;
5. establish a correction and withdrawal process.
