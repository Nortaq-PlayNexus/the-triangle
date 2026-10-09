# LEVEL DESIGN

## Principles

- Each environment feels independent at first; connections emerge through audio, recognition and subtle bleed.
- Lighting carries the horror: believability first, filters last.
- No dead corridors that exist purely to slow the player; every space earns its existence.

## 1. Childhood bedroom (implemented, procedural)

8.2 m × 6.2 m × 3 m shell of engine BasicShapes. Back wall stresses the upper-right corner (the Triangle point lives there). Moonlight rakes from behind a cold window panel. Bed left-front, desk right-front with the notebook, and a small clock stopped at twelve. The front wall is a real border — the door is the only exit, and it stays shut until High lucidity, then swings into darkness. Fog keeps the far corners uncertain. The builder owns and tears down every runtime actor when the flow subsystem swaps environments.

**Slice verbs:** watch the corner, read the notebook, test the door, step through to the station, save/load dreams.

## 2. Underground train station (vertical-slice implemented)

Fluorescent-lit platforms, tunnels, ventilation, and a deliberately readable Platform 12 sign. The station is elevated 5000 cm with the bedroom so a template landscape cannot intersect it. The wrong train now has real BasicShapes body, door, window, and interior treatment; its first interaction opens the carriage and reveals a dark aperture. Discovery is gated: the bedroom door opens at High Lucidity, then a second deliberate interaction requests the station transition. Train and Triangle become associated through the Platform12Train motif.

## 3. Clock room (planned)

One enormous clock; 12 is everything. Nearing the hour, ambience thins and geometry shifts. The player pieces together that the clock and the station share an anatomy.

## 4. Marble mansion (planned)

Open, beautiful, "safe" — then procedural perversions: rooms resize, corridors lengthen, stairs redirect, doors re-target, rooms repeat, furniture moves, windows show impossible places. A mansion window eventually reveals the underground station.

## 5. Castle (planned)

Atmospheric, vertical, cold. The Floating Man appears; he moves slowly and simply continues. The player learns late that he is not chasing but leading somewhere.

## 6. Witches (planned)

Not violence — a psychological chamber. Two figures, contradictory, itinerant; they materialize in different places, know the player's apparent memories, and argue that waking is impossible.

## 7. False-awakening bedroom (planned)

Layered awakenings with fixed rules: each layer differs in at least one way (furniture, clock, door, light, sound outside, reflection, a missing object, a stray staircase). The player's job is to catalog the difference.

## 8. Final fused dream (planned)

All environments merged onto Platform 12: the mansion around the station, castle around the platform, the clock overhead, bedroom through impossible doorways, witches near, the Floating Man waiting. The train arrives — the original nightmare.

## Geometry & performance

The slice uses shared BasicShapes and small procedural actor groups rather than imported meshes. Station lighting is deliberately bounded for the GTX 1080; future production environments should replace these groups with instanced assets, LODs, occlusion, and level streaming. World Partition is reserved for the fused final map.