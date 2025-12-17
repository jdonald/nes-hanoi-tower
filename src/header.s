; iNES header for NES ROM
; This defines the cartridge configuration

.segment "HEADER"
    .byte "NES", $1A    ; iNES header identifier
    .byte $02           ; 2 * 16KB PRG-ROM
    .byte $01           ; 1 * 8KB CHR-ROM
    .byte $01           ; Mapper 0, vertical mirroring
    .byte $00           ; Mapper 0
    .byte $00           ; No PRG-RAM
    .byte $00           ; NTSC
    .byte $00           ; No special features
    .byte $00, $00, $00, $00, $00  ; Padding
