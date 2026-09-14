# AUDIO DESIGN

## Role

Audio is a primary horror mechanic. Silence is as loud as anything else. Loud horror music is rationed; the dream runs mostly on tone, hum, footsteps and almost-voices.

## Recurring vocabulary

- Room tone (droning) — the constant bed of each environment.
- Electrical hum — the station's signature.
- Clock ticks — time pressure without a visible HUD.
- Footsteps (distant), breathing, creaks.
- Reversed sounds, distant voices, garbled announcements.
- Sudden silence — used as an event of its own.

## The Triangle motif

A short recurring motif tied to manifestation. The player learns the motif the way they learn a name: once they hear it, they know it is near before they see it. It appears on watches and manifests, and recedes when the world recovers.

## Structure (`UTriangleAudioSubsystem`)

One persistent **dream bed**, crossfaded through named state tiers:

| State | Volume tier | Used during |
|---|---|---|
| Calm | 0.40 | exploration |
| Watch | 0.22 | corner holds / events ramping |
| Manifest | 0.08 | Triangle presence |
| NearSilence | 0.00 | the approach / severe breaks |

Fades respect caller-provided durations; one-shots are spatial. When no sound content is bound yet, the subsystem stays honestly silent (no fake playback).

## Slice status

Routing and fades implemented in C++; no sound assets bound yet. Asset strategy: generated MetaSounds first (drone, ticks, hum), licensed CC0/CC-BY ambiences and room tones second — every source recorded in `ASSET_LICENSES.md`.