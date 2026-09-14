# DREAM SYSTEM

## Dream Director (`ATriangleDreamDirector`)

One per play session, spawned by the GameMode. It is the single writer of dream state: environmental changes, entity appearances, audio events, lighting changes, transitions, false awakenings, lucid events, reality breaks and Triangle manifestations. Data-driven design target: `UTriangleDreamEvent` (ID, Trigger, Condition, Environment, Audio, VisualEffect, Entity, Duration, NextEvent).

**Slice implementation:** corner-watch loop — holding the upper corner of the bedroom in view (dot ≥ 0.90 within 15 m) builds lucidity at 1.5/s; a held gaze ≥ 1.2 s with a 5.5 s quiescence triggers the point to manifest for 6 s. Lucidity-level changes fire scripted subtitle beats (Medium/High/Maximum). At Maximum the point stops hiding.

## Lucidity levels & behaviour

Defined in `ULTriangleLucidityComponent`. Gains only via dream events; `BaseGainPerRealization` is the default per-recognition increment. Level thresholds: 0 / 30 / 60 / 90. At High+, `CanManipulateDream()` allows dream-rule breaks (the bedroom door is the slice's first).

## The point (`ATriangleTrianglePoint`)

A tiny BasicShapes cone + faint teal point light, hidden by default. `Manifest(Duration)` shows it for a while — manageable, ambiguous, closeable. It is never "inspectable"; the player cannot approach it and discover what it is.

## Wake mechanic (planned)

Destabilising actions reduce to a wake sequence: distortion → audio collapse → hard cut → bedroom. Waking is only trusted via the **false-awakening check** — an unmistakable tell in the room (clock changed, door ajar, staircase where none was). The system will not use identical sequences twice; each awakening differs.

## Dream bleeding (planned)

Implemented as director-driven swaps, not new static content: train geometry into the mansion, mansion halls into the castle, the clock underground, the bedroom inside the train, witch voices from empty rooms, the Floating Man in reflections. The intensity climbs with lucidity level.

## Persistence hooks

Save members already exist for memories, notebook pages and environment flags; the director will raise/lower dream states through them in later phases.