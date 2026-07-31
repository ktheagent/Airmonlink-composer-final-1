# Deferred Harmonization and Arranging Scope

Classification: **DEFERRED TO FUTURE PLUGIN — NOT PART OF CORE RELEASE**

The core application must not expose, advertise, test as built-in, or claim completion for:

- automatic or melody harmonization;
- SATB harmonization;
- automatic chord generation or suggestions;
- automatic voicing;
- piano reduction;
- ensemble expansion;
- automatic or AI-assisted arranging.

No decorative placeholders or `coming soon` controls are permitted.

Core notation may retain only non-generative operations such as manual chord-symbol editing, interval calculation, explicit selection transposition, and concert/written-pitch switching.

## Future plugin boundary

A future separately installed plugin may request these capabilities:

- `score.read`
- `selection.read`
- `analysis.harmony`
- `preview.render`
- `score.command.submit`
- `parts.create`

The plugin receives immutable snapshots and never receives mutable score objects or private Qt widgets. Preview must not mutate the score. Score command submission and part creation require installation-time grants and explicit confirmation for each operation. Generated commands must pass core semantic validation and participate in undo, redo, persistence and deterministic serialization.

The current repository contains only manifest validation and permission gates. It does **not** contain or bundle a harmonization plugin.
