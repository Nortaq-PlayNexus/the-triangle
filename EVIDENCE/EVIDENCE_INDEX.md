# Evidence Index

**Snapshot date:** 2026-09-24  
**Rule:** a source is not evidence for a proposition merely because it appears in this file. The “supports” column is the narrow claim supported by the source.

## How to use this index

- `L-*` entries are local files or repository history.
- `E-*` entries are external URLs.
- Access date is the date the source was checked, not necessarily the source's publication date.
- “Primary” means the creator, publisher, project, or official registry; “secondary” means journalism, reference, community, or search documentation.
- No pages were copied or mirrored. Copyrighted screenshots, videos, and pages are linked rather than redistributed.
- Generated project output is marked as generated and is not treated as release evidence.

## Local project evidence

| ID | Source | Type | Supports | Limitations |
|---|---|---|---|---|
| L001 | `README.md` | Local design/status | First-person psychological horror; interconnected nightmares; lucidity; false awakenings; impossible architecture; pre-alpha status | Working tree is modified; not an independent provenance source |
| L002 | `Triangle.uproject` | Project metadata | Game module, UE 5.8 association, psychological dream-horror description | No personal-source mapping |
| L003 | `STORY.md` | Narrative design | Interconnected nightmares; ambiguity rules; planned environments and ending | Planned content is not implemented evidence |
| L004 | `GAME_DESIGN.md` | Game design | Explore/observe/recognize/lucid loop; no combat; horror style; wake design | Design intent, not playtest proof |
| L005 | `DREAM_SYSTEM.md` | Systems design | Dream Director, presence, manifestations, hum, motifs, recognition | Describes current/native systems; some behavior still gated |
| L006 | `LEVEL_DESIGN.md` | Level design | Bedroom and station implemented; other environments planned | No authored art assets |
| L007 | `DEVELOPMENT_STATUS.md` | Status | Build gate, phase status, incomplete systems, no current acceptance claim | Last updated 2026-09-24; generated logs may be stale |
| L008 | `DEVELOPMENT_LOG.md` | Development history | Dates, commits, previous successful compile, revamp history | Does not establish personal dream origin |
| L009 | `TECHNICAL_ARCHITECTURE.md` | Architecture | UE 5.8.2, C++ module ownership, system layout | Architecture document can drift from source |
| L010 | `AUDIO_DESIGN.md` | Audio design | Silence, hum, state tiers, procedural signature sound | No external audio assets currently present |
| L011 | `Source/Triangle/Dream/ATriangleDreamDirector.h/.cpp` | Source | Presence ramp, manifestations, recognition, motifs, anomalies, wake reset | Code proves implementation intent, not player experience |
| L012 | `Source/Triangle/Lucidity/*.h/.cpp` | Source | Separate Lucidity and Recognition systems and thresholds | Numeric systems do not themselves prove personal source |
| L013 | `Source/Triangle/Core/UTriangleGameFlowSubsystem.*` | Source | Procedural bedroom/station ownership and transitions | Only two environment IDs currently implemented |
| L014 | `Source/Triangle/World/ATriangleRoomBuilder.*`, `ATriangleStationBuilder.*`, `ATriangleTrain.*` | Source | BasicShapes environments, wrong train, Platform 12, lifecycle | Blockout visual quality; no authored assets |
| L015 | `Source/Triangle/Tests/TriangleCoreTests.cpp` | Source | Four native core automation tests | No end-to-end acceptance test |
| L016 | `Config/DefaultGame.ini`, `Config/DefaultEngine.ini` | Configuration | Project name, description, default maps, renderer settings | Configuration is not evidence of public release |
| L017 | `ASSET_LICENSES.md` | Provenance policy | No external assets; built-ins/procedural content | Future assets require records |
| L018 | `BUILD_TROUBLESHOOTING.md` | Build record | UE installation gate and repair instruction | Not a gameplay source |
| L019 | Git commits `102ec7b` through `738fdbb` | Repository history | Implementation milestones and dates | Does not prove the first personal experience or public terminology history |
| L020 | `C:\Users\natha\Desktop\New Text Document.txt` | User brief | User states THE TRIANGLE is based substantially on own night terrors and proposes Dreamcore terminology | First-person statement; not independently corroborated or dated in the repository |
| L021 | `Saved\AutoScreenshot.png` | Generated output | A generated runtime image exists | Ignored/generated; not an authored screenshot or acceptance evidence |

### Git ledger (full hashes)

The repository history is linear and has no remote/tags in the audited project repository:

| Full hash | Recorded date | Subject |
|---|---|---|
| `102ec7b473e95c2b0382183f7cc0840a63d8cfc5` | 2026-09-14 15:05:38 +10:00 | Initial project scaffold: uproject, build targets, module structure, docs |
| `26a6379bc11f790d9ca0d1258b08e212d118758d` | 2026-09-14 15:20:10 +10:00 | Phase 1 foundation: full C++ slice systems |
| `f6c0e6e6de011b5404710258adc47867e33bc8db` | 2026-09-14 15:58:22 +10:00 | Add one-click admin toolchain installer script |
| `37b4b580edec421438ba9dc1cbb73b5e60ca3c67` | 2026-09-14 17:45:09 +10:00 | Phase 1 + design follow-up: recognition/hum/intrusion systems, UE 5.8 build fixes |
| `6853f549f8d6d26558dbb5a461f8bbc66d79ca87` | 2026-09-14 18:47:19 +10:00 | Fix native class resolution on UE 5.8, module init, room mesh mobility |
| `43ba462b008dc3e15958b858936ff54fa05abb66` | 2026-09-14 20:05:23 +10:00 | Deterministic in-room spawn, resolve blue/blank viewport, lighting cleanup |
| `a59df51ec2772c693976b986879171e8b783dca0` | 2026-09-14 22:39:10 +10:00 | Post-compile cleanup: elevation offset, dead code, config defaults |
| `7cee70ea5aa94939166bd9b5b64f6a44cc25015e` | 2026-09-14 22:43:45 +10:00 | Phase 2: underground station builder + wrong train |
| `738fdbb738d5801dae8d8051e2c12bec789e126a` | 2026-09-14 22:44:59 +10:00 | Update docs: Phase 1 complete, Phase 2 station in progress |

These commits document implementation history, not the first personal dream or the first public use of Dreamcore.

## External evidence

| ID | URL | Source type | Supports | Limitations |
|---|---|---|---|---|
| E001 | [Steam appdetails: Dreamcore, app 2453060](https://store.steampowered.com/api/appdetails?appids=2453060&l=english&cc=us) | Official platform metadata | Existing game title; Montraluz/Tlön; liminal spaces; 2025-01-23 release metadata | Publisher-controlled; no personal-dream claim |
| E002 | [Official Dreamcore site](https://dreamcoregame.com/) | Official project site | Game positioning, press kit, liminal-space framing | Marketing copy; mutable |
| E003 | [Press Over interview, 2023-08-31](https://pressover.news/entrevista/dreamcore-videojuego-argentino-fusiona-stanley-parable-the-backrooms/) | Developer interview/journalism | Project development start described as mid-2022; Backrooms/creepypasta/Stanley Parable influences | One interview; does not establish global first use |
| E004 | [Destructoid article, 2024-06-09](https://www.destructoid.com/dreamcore-is-the-interactive-analog-horror-game-of-my-nightmares/) | Journalism | Editorial description of *Dreamcore* as analog/liminal horror and Backrooms influence | Headline wording is not a creator quotation; body does not claim personal dream basis |
| E005 | [Steam appdetails: Dreamcore: Rabbit Hole, app 4149700](https://store.steampowered.com/api/appdetails?appids=4149700&l=english&cc=us) | Official platform metadata | Developer says it loves dreamcore aesthetic; softer liminal interpretation; 2026-04-24 release metadata | Aesthetic use, not personal provenance |
| E006 | [Steam appdetails: Explore Dreams: Dreamcore, app 3375310](https://store.steampowered.com/api/appdetails?appids=3375310&l=english&cc=us) | Official platform metadata | Uses Dreamcore/vaporwave/nostalgicore as aesthetic references; 2024-12-02 release metadata | “Explore your dreams” is marketing, not provenance |
| E007 | [Steam appdetails: Neverending Nightmares, app 253330](https://store.steampowered.com/api/appdetails?appids=253330&l=english&cc=us) | Official platform metadata | Developer-reported mental-illness inspiration; 2014-09-26 release metadata | No documented personal dream source in the cited copy |
| E008 | [itch.io: #21: The World](https://lsddev.itch.io/21-the-world) | Creator/project page | “based on my dream journal I have kept for years”; surreal exploration; page update 2018-02-02 | Direct statement; exact journal contents not published |
| E009 | [itch.io: Dream Diary](https://flowerstudio.itch.io/dream-diary-1) | Creator/project page | “created from a real dream I had a few weeks ago”; short adventure; two endings | Release date not shown; short work |
| E010 | [itch.io: Zemblini: Dream Journal](https://aerist.itch.io/dreamjournal) | Creator/project page | “video game is based on my real dreams” | Release date not shown; concise provenance statement |
| E011 | [itch.io: My Magical Dream Journal](https://jimmytwist.itch.io/my-magical-dream-journal) | Creator/project page | “based on true dreams I had”; horror/nightmare tags | Concise statement; no detailed mechanics evidence |
| E012 | [itch.io: Archive Equinox](https://gusser.itch.io/archive-equinox) | Creator/project page | “locations I've dreamed about”; release date 2025-08-04 | Short game; materiality described generally |
| E013 | [itch.io: My Nightmares Tell Me to Chase My Dreams](https://jonbieber.itch.io/mntmtcmd) | Creator/project page | Level 1 is a 100% accurate visualization of a recurring dream; later levels are technical tests | Partial work-level match; prototype |
| E014 | [itch.io: The Majesty of Colors](https://futureproofgames.itch.io/majesty-of-colors) | Creator/remaster page | Opens “Last night I had a dream”; original web game credited to Gregory Avery-Weir; 2018 remaster date | Provenance of every element and original release details need confirmation |
| E015 | [itch.io search: dreamcore](https://itch.io/search?q=dreamcore) | Platform search | 244 projects observed, including games/assets/music; broad use of term | Count is mutable and not a genre census |
| E016 | [itch.io tag: Dreams](https://itch.io/games/tag-dreams) | Platform tag | 2,394 results observed; dreams used as a broad tag | Tags do not establish personal provenance |
| E017 | [itch.io tag: nightmares](https://itch.io/games/tag-nightmares) | Platform tag | 56 results observed; nightmares used as a broad tag | Tags do not establish definition match |
| E018 | [itch.io search: real dreams](https://itch.io/search?q=%22real+dreams%22) | Platform search | Four game results observed, including direct provenance examples | Search phrase and ranking are not exhaustive |
| E019 | [itch.io search: actual dreams](https://itch.io/search?q=%22actual+dreams%22) | Platform search | One result, *Zemblini: Dream Journal* | Search result count is mutable |
| E020 | [itch.io search: based on my own dream](https://itch.io/search?q=%22based+on+my+own+dream%22) | Platform search | One result, *#21: The World* | Exact phrase search misses paraphrases |
| E021 | [Aesthetics Wiki: Dreamcore](https://aesthetics.fandom.com/wiki/Dreamcore) | Community reference | Describes an early-2020s surreal internet aesthetic | Community-edited; origin claim not independently verified |
| E022 | [Wikipedia: Dreamcore](https://en.wikipedia.org/wiki/Dreamcore) | Reference/disambiguation | Shows title collisions and related pages | Community-maintained and mutable |
| E023 | [Wikipedia: Dreamcore (video game)](https://en.wikipedia.org/wiki/Dreamcore_(video_game)) | Reference | Overview of the existing game and liminal-space framing | Not a primary provenance source |
| E024 | [Academia: To Dream a Game](https://www.academia.edu/88560830/To_Dream_a_Game_Dreams_as_Interactive_Narrative_Devices_in_Digital_Games) | Academic article | Dreams/dreamlike phenomena as interactive-narrative devices | Access can be restricted; article is about narrative function, not creator provenance |
| E025 | [Gackenbach & Kuruvilla, DOI 10.7557/23.5980](https://doi.org/10.7557/23.5980) | Peer-reviewed article | 2008 study of how video-game play relates to dream content | Studies game-to-dream effects, not a game genre |
| E026 | [Dream Studies Portal: lucid dreaming and games](https://dreamstudies.org/encouraging-lucid-dreaming-with-video-games/) | Secondary research summary | Gameplay/lucid-dream relationship | Not a provenance standard |
| E027 | [IP Australia: Trade marks](https://www.ipaustralia.gov.au/trade-marks) | Government information | Search existing rights and distinguishes a trade mark as a brand right | No result for Dreamcore is asserted |
| E028 | [Australian Trade Mark Search](https://search.ipaustralia.gov.au/trademarks/search/quick) | Government search tool | Official Australian search route | Requires a real search session; not completed here |
| E029 | [WIPO: Trademarks](https://www.wipo.int/en/web/trademarks) | International information | Sign capable of distinguishing goods/services; search/registration overview | General information, not clearance |
| E030 | [USPTO trademark search](https://www.uspto.gov/trademarks/search) | Government information | US search tool and clearance-search guidance | No US clearance conclusion |
| E031 | [Steam storesearch: dreamcore](https://store.steampowered.com/api/storesearch/?term=dreamcore&l=english&cc=us) | Official platform API | Existing titles and search collision | Search results are mutable and publisher-controlled |
| E032 | [itch.io: Dreamcore search results](https://itch.io/search?q=dreamcore) | Platform search | User-created term usage | Not proof of a formal category |
| E033 | [Reddit IndieGaming search result: Archive Equinox](https://www.reddit.com/r/IndieGaming/comments/1lab9gl/making_video_game_based_on_my_dreams_part_3/) | Community discussion | Public developer sharing/description of a dream-based project | Dynamic page; not used as primary evidence |
| E034 | [YouTube search: Dreamcore game](https://www.youtube.com/results?search_query=dreamcore+game) | Search lead | Potential video discovery route | Dynamic results; no video claim used without primary verification |
| E035 | [Reddit search: Dreamcore game](https://www.reddit.com/search/?q=dreamcore%20game) | Search lead | Community discussion route | Dynamic/restricted; no negative conclusion |
| E036 | [Google search query](https://www.google.com/search?q=dreamcore+horror+genre) | Search lead | Search discovery only | Automated access/challenge prevented reliable primary review |
| E037 | [Epic Games Store browse](https://store.epicgames.com/en-US/browse?q=Dreamcore) | Platform route | Intended catalogue check | HTTP 403 in this pass |
| E038 | [GOG search route](https://www.gog.com/en/search?query=dreamcore) | Platform route | Intended catalogue check | HTTP 404 in this pass |
| E039 | [IGDB search route](https://www.igdb.com/search?type=1&q=dreamcore) | Database route | Intended database check | HTTP 403 in this pass |
| E040 | [MobyGames search route](https://www.mobygames.com/search/?q=dreamcore) | Database route | Intended database check | HTTP 403 in this pass |
| E041 | [RAWG API](https://api.rawg.io/api/games?search=dreamcore&page_size=20) | Database route | Intended API check | HTTP 401 without key |
| E042 | [itch.io: Clockwork Dreams](https://scrib-dev.itch.io/clockworkdreams) | Creator/project page | “game based on my personal dreams”; over 30 dreams/nightmares; game-wide exploration premise | Direct statement; exact dream-to-level mapping is not itemized |
| E043 | [OMOCAT Drawfest4 presentation](https://www.youtube.com/watch?v=9Wt_OXT2B0w) | Creator presentation/video | OMORI creator describes White Space dream origin, Headspace lucid-dreaming idea, and BLACK SPACE fear/nightmare imagery at cited timestamps | Video transcription/interpretation and mixed influences limit work-wide classification |
| E044 | [OMOCAT archived creator blog](https://web.archive.org/web/20140927101428/https://www.omocat-blog.com/post/85490697570/from-illustration-to-video-game-omori-kickstarter) | Archived creator blog | OMORI described as a very personal project | Archive access was unavailable in this pass; not a dream-diary claim |
| E045 | [MobileSyrup: Lucid Falls interview](https://mobilesyrup.com/2026/01/22/calgary-eldamar-studio-lucid-falls-interview/) | Creator interview | Yurii Radkevych says the core idea came from his dreams | Concept-level provenance; finished-game extent not specified |
| E046 | [GATA Magazine: Osamu Sato interview](https://gatamagazine.com/articles/art/lsd-is-groovy-an-interview-with-osamu-sato) | Creator interview | LSD: Dream Emulator used a staff member's decade-long dream diary, not Sato's own diary | Strong counterexample to overclaiming own-dream provenance |
| E047 | [Nintendo: Neverending Nightmares](https://www.nintendo.com/en-gb/Games/Nintendo-Switch-download-software/Neverending-Nightmares-1815494.html) | Official platform copy | Mental-illness/OCD/depression inspiration and nightmare-world framing | No personal dream/nightmare source stated |
| E048 | [Game Developer: Happy Game](https://www.gamedeveloper.com/art/honing-creepiness-through-contrast-in-horror-puzzler-happy-game) | Developer interview | Writer rejects personal fear/dream-of-demons as main motivation; cites humor, hopelessness, Lovecraft, Twin Peaks, and general dream variety | Strong counterexample to broad own-nightmare classification |
| E049 | [Wu, “Lost items and exposed shame”](https://doi.org/10.1080/14797585.2022.2097013) | Academic article | Dreamcore's relationship to liminal space and defamiliarization; reports a 2018 antecedent account | Origin account partly relies on community documentation; not a single definitive coinage date |
| E050 | [Song, Choi, and Li, Dreamcore aesthetics study](https://pure.au.dk/portal/en/publications/what-makes-people-like-dreamcore-aesthetics-the-role-of-trait-nos) | Peer-reviewed research | Defines Dreamcore as a digital aesthetic; 449-participant study separates darker/eerie and warmer/comforting preferences | Aesthetic study does not establish creator-dream provenance |
| E051 | [USPTO TSDR record sn99825657](https://tsdr.uspto.gov/statusview/sn99825657) | Government trademark record | Montraluz LLC DREAMCORE application for downloadable game software, Class 009, as reported by the research pass | Direct record was not retrievable in this pass; verify current status with USPTO/professional search |
| E052 | [Dreamcore developer blog](https://www.dreamcoregame.com/blog/1) | Official creator blog | Project began June 2022 as an escape from work circumstances and as a liminal-space niche project | Personal circumstances are not documented personal-dream provenance |
| E053 | [Epic Games Store: Dreamcore](https://store.epicgames.com/en-US/p/dreamcore-03a264) | Official storefront | Existing title/storefront use and trademark notice reported by research pass | HTTP 403 in this pass; verify directly before relying on details |
| E054 | [Game Studies review of Perron's horror framework](https://gamestudies.org/1901/articles/reivew_vella) | Academic review | Distinguishes horrific context, sporadic fear effects, and games where fear is the main object | Does not define personal dream provenance |
| E055 | [Tate: Surrealism](https://www.tate.org.uk/art/art-terms/s/surrealism) | Arts reference | Surrealism engages dreams, the unconscious, and the unexpected | Broad art movement; not autobiographical provenance |

- No first-origin proof for the Dreamcore aesthetic.
- No complete archive of every personal-dream game.
- No public announcement or adoption record for this project.
- No dated THE TRIANGLE source journal or public provenance statement beyond the user brief.
- The source referenced as `Downloads\\new 1.txt` in the development log is currently an unrelated optical-sandbox brief; the original THE TRIANGLE design source was not located.
- No current successful build/PIE acceptance evidence.
- No legal clearance opinion or trademark registration search result.
