# PERFORMANCE

## Target hardware (fixed)

- CPU: Intel i7-8700 (6c/12t)
- RAM: 24 GB
- GPU: NVIDIA GTX 1080 8 GB (Pascal, SM5; **no** hardware Lumen/Nanite at max)
- OS: Windows 10 Home
- Target: 1080p, 60 FPS where achievable; stable 30+ floor everywhere else.

## Rendering policy

- SM5 / DX12 with fallback. No hardware ray tracing.
- **Lighting:** dynamic for interactive moments; static/baked for big scenery. Volumetric fog only where it pays (thin, cheap instances), skipped at distance.
- **Post:** cinematic DOF, film grain, subtle aberration — all toggleable, cheap variants preferred.
- **Particles:** pooled Niagara; no huge emitters.
- **Reflections:** screen-space / simple; no heavy SSR chains on reflective marble until profiled.
- Scalable settings presets with a clear menu (quality, resolution, FPS cap, texture/shadow/effects/post/foliage/view distance, motion blur, grain, chromatic aberration).

## Engineering rules (already enforced)

- **No ticking hordes.** Interaction scans run on a 0.08 s timer; lucidity is event-driven; the Dream Director exists to centralise work and gate it on a player existing; doors stop ticking mid-swing when done. The character no longer polls interaction validity every frame.
- **Procedural geometry from one shared builder** (few bodies, shared meshes) until real assets, which arrive with LODs + instancing. Builders explicitly own and tear down runtime actors during transitions.
- **Station lighting budget:** four fluorescent pools, four broad amber edge pools, one track fill, and shadowless skylight/fill lights; avoid reintroducing a light per decorative marker.
- All future systems event-driven, pooled, asynchronous where it helps.
- Profile with Unreal Insights per phase; measure the GTX 1080, not assumptions.

## Save/Audio cost

Save writes are synchronous but rare (player-invoked). The audio subsystem owns one bed component and fires only explicit one-shots — no per-frame work.