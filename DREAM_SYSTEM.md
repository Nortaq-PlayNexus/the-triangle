# DREAM SYSTEM

_Living document: reflects the C++ as written (pre-compile). Updated as systems land._

## The Triangle: design principle

The Triangle is an intrusion. It has no single body. The player should never be told what it is; the entire narrative engine is "how does the player recognise it". The game answers that question, one beat at a time, in a dozen different bodies.

## Dream Director (`ATriangleDreamDirector`)

One per play session, spawned by the GameMode. Controls the entire dream: environmental changes, entity appearances, audio events, presence, manifestations, recurring motifs, and the false-wake cycle.

**Vertical-slice implementation:**

1. **Corner-watch loop** — holding the upper corner of the bedroom in view (dot ≥ 0.90 within 15 m) builds lucidity at 1.5/s. A held gaze ≥ 1.2 s with a 5.5 s quiescence triggers a manifestation from the pool.

2. **Presence model** — a 0..1 ramp ramps up while a manifestation is active or the player's Master recognition knows the shape is here; decays when nothing is present. The hum follows presence. Events fire at crossings: OnPresenceStart, OnPresenceIncrease, OnPresenceDisappear.

3. **Manifestation pool** — six data-driven bodies: the corner point, hum-only, triangular gap, corner split, distant black point, triangular doorway. The pool is weighted by recognition: early plays see only the corner; higher recognition opens stranger bodies and, finally, the hum-only (nothing visible at all).

4. **Master sense** — at the top recognition tier the Director occasionally triggers a hum-only manifestation with nothing visible. The player learns to know the Triangle is here before any form appears.

5. **Recurring motifs** — the clock stopping at twelve, the train pulling into platform twelve. Driven by `UTriangleMotifSubsystem`; the Director hooks them into recognition milestones and subtitle beats.

6. **False wake cycle** — the player presses Q when recognition is high enough. The dream releases them for a beat, then adds one small wrongness. The Director withdraws all presence on a successful wake.

## Lucidity (`ULTriangleLucidityComponent`)

Hidden meter (0..100), Low/Medium/High/Maximum. Gains only through explicit dream events (watching the corner, reading a notebook, realising). At High+ the player can deliberately break dream rules (the bedroom door opens). Lucidity is separate from Recognition.

## Recognition (`ULTriangleRecognitionComponent`)

Hidden meter (0..100). The pattern catalogue: how much the player has learned about the shape beneath the intrusions.

- **Unknowing** (0-25) — strangeness passes without note.
- **Noticing** (25-50) — the same shape keeps returning; it feels arranged.
- **Recognizing** (50-75) — the pattern is recognised before it completes.
- **Anticipating** (75-95) — the presence is felt behind surfaces; the hum has meaning. The player can attempt a wake.
- **Master** (95-100) — "it's here". Known without seeing.

Gains only through witnessing manifestations resolve, recurring motifs, and the false-wake cycle. Separate from Lucidity by design: one is "I know I am dreaming", the other is "I know what is here".

## The point (`ATriangleTrianglePoint`)

A tiny BasicShapes cone + faint teal point light, hidden by default. `Manifest(Duration)` shows it for a while — manageable, ambiguous, closeable. It is never "inspectable"; the player cannot approach it and discover what it is. One of the six bodies in the manifestation pool.

## The Hum

A persistent, quietly wrong low drone generated entirely at runtime (`ATriangleProceduralHumSource`). Two detuned sine partials (43.2 Hz / 46.7 Hz) breathing against each other at ~0.17 Hz. Driven by the audio subsystem's `SetHumPresence(0..1)`. It begins imperceptibly, swells when the Triangle is present, and occasionally vanishes for a beat before returning. It is the Triangle's signature sound: "an internal sound, never a room tone."

## The cuts into the dream (`ATriangleAnomalyActor` + `UTriangleAnomalySubsystem`)

Temporary non-interactive environmental anomalies built from engine BasicShapes at runtime:

- **TriangularGap** — a razor-thin dark wedge standing in the plane of a surface.
- **CornerSplit** — a vertical seam: the corner holds its breath, slightly apart.
- **DistantBlackPoint** — a tiny black point hanging far away.
- **TriangularDoorway** — three dark blades forming an impossible triangle of doorway on a wall.

Each fades in/out by scale (no material budget), is non-collidable, and never interactable. The subsystem enforces a concurrent budget (oldest dropped first). Dream resets call `DespawnAll()`.

## Recurring motifs (`UTriangleMotifSubsystem`)

Owns the dream's background clock and the scheduling of recurring symbols:

- **ClockStops12** — "The clock stops at twelve." Fires at the Recognizing milestone.
- **Platform12Train** — "Somewhere far off, a train pulls into platform twelve." Fires at Anticipating.

Each motif has a cooldown window and a recognition requirement. The Director hooks `OnMotif` to subtitle text, recognition gains, and brief hum swells.

## False-awakening manager (`UTriangleFalseAwakeningManager`)

When the player presses Q with Anticipating+ recognition, the dream releases them for a moment, then adds one small wrongness. Each layer is a few differences deep: the clock stopped at twelve, the moon on the wrong side, the hallway too long, the hum still there. The manager refuses wakes before recognition is high enough. On a completed wake it broadcasts `OnWakeResetRequested`; the Director withdraws all presence and the cycle begins again.

## Persistence hooks

Save members already exist for memories, notebook pages, environment flags and recognition. The Director and subsystems grow their state through these in later phases.