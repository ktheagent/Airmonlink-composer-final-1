# Notation Standards Register

Status: ACTIVE FOUNDATION REGISTER

| Resource | Pinned/current contract | Purpose | Licence/notice handling | Incorporated files | Update policy |
|---|---|---|---|---|---|
| MusicXML | MusicXML 4.0, W3C release dated 2021-06 | Import/export and schema validation contract | Preserve W3C notices with vendored schemas | Not yet vendored | Pin exact schema release before enabling export |
| SMuFL | Stable SMuFL release; exact tag must be recorded before vendoring | Canonical glyph names, metadata, ranges, anchors and engraving defaults | Preserve specification and metadata notices | Not yet vendored | Never consume moving draft URLs in production |
| Bravura | Steinberg Bravura stable release; package currently downloads release assets in CI | Redistributable SMuFL font and matching metadata | Bundle exact licence and notices with font | Font packaging exists; metadata pin pending | Pin font and metadata to one reviewed release |
| MEI | Current stable Common Music Notation or Basic schema | Secondary semantic coverage reference | Record schema licence before vendoring | None | Reference only until an explicit MEI milestone |
| LilyPond documentation | Stable engraving essay and notation reference | Behavioural research for spacing and engraving conventions | Do not copy GPL implementation code | None | Documentation/reference use only |
| Verovio | Not approved as editing engine | Optional independent fixture oracle | Licence and architectural role require approval | None | Diagnostic-only after approval |
| Standard MIDI File | Official MIDI Association SMF specification | MIDI import/export and timing interoperability | Preserve applicable notices | None | Pin referenced specification revision before implementation |

## Mandatory controls

- MusicXML is not the internal score model.
- SMuFL metadata, not hard-coded Unicode assumptions, must drive glyph metrics and attachment anchors.
- Bravura font and metadata must use matching reviewed releases.
- No third-party executable plugin is bundled without reviewed licence, checksum and trust policy.
- `THIRD_PARTY_NOTICES.md` must be created before a production release that vendors external files.
