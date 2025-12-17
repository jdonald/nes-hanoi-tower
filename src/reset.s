; NES Reset and initialization code
; Sets up the NES hardware and calls main()

.export _reset, _nmi, _irq
.import _main
.importzp sp

.segment "STARTUP"

; Reset handler - called when NES starts or is reset
_reset:
    sei             ; Disable interrupts
    cld             ; Clear decimal mode (NES doesn't have decimal mode)
    ldx #$40
    stx $4017       ; Disable APU frame IRQ
    ldx #$ff
    txs             ; Set up stack
    inx             ; X = 0
    stx $2000       ; Disable NMI
    stx $2001       ; Disable rendering
    stx $4010       ; Disable DMC IRQs

    ; Wait for first vblank
vblank1:
    bit $2002
    bpl vblank1

    ; Clear RAM
    txa
clear_ram:
    sta $0000, x
    sta $0100, x
    sta $0200, x
    sta $0300, x
    sta $0400, x
    sta $0500, x
    sta $0600, x
    sta $0700, x
    inx
    bne clear_ram

    ; Wait for second vblank
vblank2:
    bit $2002
    bpl vblank2

    ; Initialize stack pointer for C
    lda #$00
    sta sp
    lda #$07
    sta sp+1

    ; Jump to C main function
    jmp _main

; NMI handler - called during vertical blank
_nmi:
    rti

; IRQ handler - not used but required
_irq:
    rti

.segment "VECTORS"
    .word _nmi      ; NMI vector
    .word _reset    ; Reset vector
    .word _irq      ; IRQ/BRK vector
