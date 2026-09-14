# LEVEL DESIGN

## Principles

- Each environment feels independent at first; connections emerge through audio, recognition and subtle bleed.
- Lighting carries the horror: believability first, filters last.
- No dead corridors that exist purely to slow the player; every space earns its existence.

## 1. Childhood bedroom (implemented, procedural)

8.2 m × 6.2 m × 3 m shell of engine BasicShapes. Back wall stresses the upper-right corner (the Triangle point lives there). Moonlight rakes from behind a cold window panel. Bed left-front, desk right-front with the notebook. The front wall is a real border — the door is the only exit, and it stays shut until High lucidity, then swings into darkness. Fog keeps the far corners uncertain.

**Slice verbs:** watch the corner, read the notebook, test the door, save/load dreams.

## 2. Underground train station (planned)

Fluorescent-lit platforms, tunnels, ventilation, announcements, echoing steps. Discovery gated: the player reaches **Platform 12** gradually; the arriving train is wrong in a way the game does not label. Train and Triangle become associated.

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

Slice room uses ≤ 20 draw-call bodies. Future environments reuse instanced/BasicShapes geometry, LODs, occlusion and level streaming; World Partition only where the merged-fused map demands it.