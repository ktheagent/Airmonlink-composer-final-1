# Write Workspace — Compact SMUFL Notation Input

Status: *@AUTHORITATIVE PRODUCT SPECIFICATION — IMPLEMENTATION PENDING**

## Scope

The native core application shall provide one compact `Write` workspace for notation entry. Note-entry controls must not be scattered across the ribbon, window, sidebars or floating panels.

This document defines organization and behaviour only. Controls may remain disabled or hidden until their semantic command, undo/redo, persistence, hit-testing and rendering tests pass.

## Primary presentation

- Visible notation controls use professional SMuFL glyphs from the bundled, licence-verified music font.
- No permanent note-value, rest, accidental, tie, beam, tuplet, articulation, ornament, tremolo, dynamic or line control shall show its written name beside or beneath its individual button.
 - Voice controls may use compact `V1`, `V2`, `V3` and `V4` labels with their editing colours.
- Each group has one visually secondary label at its edge or bottom.
- Controls must remain readable. When space is insufficient, use category-specific overflow palettes instead of unreadable compression or horizontal scrolling.

## Groups in musical order

### 1. Note values

Always-visible when supported: whole, half, quarter, eighth, sixteenth, thirty-second and sixty-fourth. Longa/breve, smaller values, cue, grace, dot and double-dot belong in a compact expandable palette when not essential to the current slice.

### 2. Notes and rests

- Note-entry mode
- Rest-entry mode
- Chord/add-note mode
- Replace-duration mode
- Repitch mode
- Keyboard, mouse and MIDI input selection
- Voices 1 – 4

### 3. Accidentals

Double flat, flat, natural, sharp, double sharp, courtesy accidental and remove accidental. Microtonal accidentals remain hidden until genuinely supported by the model, SMuFL metadata, serializer, renderer and tests.

### 4. Ties, beams and tuplets

Tie, remove tie, start/end beam, break beam, automatic beam, presets 3:2, 5:4, 6:4, 7:4 and a custom actual-to-normal ratio.

### 5. Articulations

Staccato, staccatissimo, tenuto, accent, marcato and fermata. Additional articulations belong in a categorised expandable palette.

### 6. Ornaments and tremolos

Trill, mordent, turn, grace ornaments, single-note tremolo, two-note tremolo and arpeggio.

### 7. Lines and connections

Slur, phrase slur, glissando, crescendo, diminuendo, pedal, octave line, volta/ending and other supported spanners.

## Tooltips and accessibility

- Tooltip format is `<Command name> · <Shortcut>`, for example `Quarter note · 4`.
- Tooltips appear after a brief hover delay, use restrained semi-transparent styling, remain inside the application window and must not cover the active staff, caret or ghost symbol.
- Tooltips hide immediately on pointer leave, selection or keyboard focus change.
 - Every control has a plain-language accessible name and state.
- Every group supports keyboard navigation.

## Shared command authority

The ribbon, notation keypad, keyboard shortcuts, pointer input and MIDI input must invoke the same registered semantic commands. Secondary surfaces may present the same command, but they must not maintain independent state or mixtures of visual only handlers.

A control may be enabled only when its command passes:

- semantic before/after assertions;
- atomic failure behaviour;
- undo and redo;
- save and reopen;
- hit-testing;
- graphical rendering;
- packaged-Windows interaction;
- accessibility.

## Ghost preview

Selecting a supported symbol arms a real notation tool. Hovering a valid staff target shows the same SMuFL glyph that will be committed, with:

- active voice colour;
- stem direction;
- flags or predictable beams;
- dots;
- accidental;
- ledger lines;
- exact snapped pitch and rhythmic position;
- valid/invalid feedback.

The ghost and committed event must use the same `PointerTarget`.

## Voice editing colours

- Voice 1: blue
- Voice 2: teal/green
- Voice 3: orange
- Voice 4: magenta/purple

Active voice notation uses full editing colour. Inactive voices are slightly faded during entry. Selected notes use a faint transparent voice-colour halo. Print, PDF and image export remain professional black unless the user explicitly enables colour printing.

## Keypad and layout rules

- The notation keypad may dock on the right or bottom.
- It may collapse to an icon strip.
- It must not cover the active staff, caret, selection or ghost note.
- The application remembers the last open Write group and keypad position.
- No essential group requires horizontal scrolling at standard Windows desktop sizes.

## Visual organization acceptance test

At standard Windows desktop sizes, automated tests must prove:

1. No notation controls are scattered outside their intended groups.
2. No permanent note-input panel covers the score.
3. No essential group requires horizontal scrolling.
1. Every visible symbol invokes its registered semantic command.
5. Ribbon, keypad and keyboard shortcuts produce identical `ScoreModel` results.
6. Visible controls are symbol-first and do not show permanent per-button text names.
7. Hover names, shortcuts, focus and accessibility state are present and non-obstructive.
