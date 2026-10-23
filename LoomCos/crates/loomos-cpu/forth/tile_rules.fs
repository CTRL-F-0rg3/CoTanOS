\ ==============================================================================
\ LOOMCOS FORTH TILE RULE ENGINE & STOCHASTIC REFRESH DICTIONARY
\ ==============================================================================

DECIMAL

\ --- Memory Layout Constants ---
32 CONSTANT TILE-WIDTH
32 CONSTANT TILE-HEIGHT
4096 CONSTANT TILE-BYTE-SIZE
16 CONSTANT MAX-PRIORITY-LEVELS

\ --- Tile Record Field Offsets (in bytes) ---
\ Offset 0: Tile X coordinate (32-bit int)
\ Offset 4: Tile Y coordinate (32-bit int)
\ Offset 8: Active state flag (0 = dormant, 1 = active)
\ Offset 12: Activity age counter (frames remaining before sleep)
\ Offset 16: Priority level (0 to 15)
\ Offset 20: Source buffer pointer
\ Offset 28: Destination buffer pointer
28 CONSTANT TILE-RECORD-SIZE

\ --- Global Variables ---
VARIABLE CURRENT-FRAME-ID
VARIABLE GLOBAL-LFSR-SEED
VARIABLE TOTAL-TILES-COUNT
VARIABLE ACTIVE-TILES-COUNT
VARIABLE NOISE-SAMPLE-INDEX

\ --- Initialization Words ---
: INIT-RULE-ENGINE ( total-tiles -- )
    TOTAL-TILES-COUNT !
    0 CURRENT-FRAME-ID !
    123456789 GLOBAL-LFSR-SEED !
    0 ACTIVE-TILES-COUNT !
    CR ." [FORTH] Tile Rule Engine Initialized successfully." CR ;

\ --- LFSR Noise Generation in Forth (Matches ASM logic) ---
: NEXT-NOISE-STEP ( -- next-random )
    GLOBAL-LFSR-SEED @
    DUP 0= IF DROP 439813 THEN
    DUP 1 AND 
    IF 
        2 / 16#80000057 XOR 
    ELSE 
        2 / 
    THEN
    DUP GLOBAL-LFSR-SEED ! ;

\ --- Tile Accessor Words ---
: TILE-X@ ( tile-ptr -- x ) @ ;
: TILE-Y@ ( tile-ptr -- y ) 4 + @ ;
: TILE-ACTIVE? ( tile-ptr -- flag ) 8 + @ 0<> ;
: TILE-AGE@ ( tile-ptr -- age ) 12 + @ ;
: TILE-PRIORITY@ ( tile-ptr -- prio ) 16 + @ ;

: TILE-ACTIVE! ( flag tile-ptr -- ) 8 + ! ;
: TILE-AGE! ( age tile-ptr -- ) 12 + ! ;
: TILE-PRIORITY! ( prio tile-ptr -- ) 16 + ! ;

\ --- Stochastic Noise Sampling Rule ---
: EVALUATE-STOCHASTIC-SAMPLE ( tile-index -- should-refresh? )
    NEXT-NOISE-SEED
    TOTAL-TILES-COUNT @ MOD
    NOISE-SAMPLE-INDEX !
    
    \ Check if current tile index matches the non-linear noise sample
    DUP NOISE-SAMPLE-INDEX @ = 
    OVER CURRENT-FRAME-ID @ + 17 MOD 0 = OR
    SWAP TILE-ACTIVE? OR ;

\ --- Priority & Activity Aging Rules ---
: DECREMENT-TILE-AGE ( tile-ptr -- )
    DUP TILE-AGE@ 
    DUP 0 > IF
        1 - SWAP TILE-AGE!
    ELSE
        DROP 0 SWAP TILE-AGE!
    THEN ;

: UPDATE-TILE-STATE ( tile-ptr diff-detected? -- )
    IF
        \ If difference detected by ASM diff engine, set active and reset age to 15 frames
        1 OVER TILE-ACTIVE!
        15 SWAP TILE-AGE!
        1 ACTIVE-TILES-COUNT +!
    ELSE
        \ If no difference, decrement age. If age hits zero, mark as dormant
        DUP DECREMENT-TILE-AGE
        DUP TILE-AGE@ 0= IF
            0 SWAP TILE-ACTIVE!
        else
            DROP
        THEN
    THEN ;

\ --- Frame Cycle Optimization Rule ---
: OPTIMIZE-REFRESH-CYCLE ( -- )
    1 CURRENT-FRAME-ID +!
    0 ACTIVE-TILES-COUNT !
    CR ." [FORTH] Frame " CURRENT-FRAME-ID @ . ." optimized via non-linear rules." CR ;

\ --- Rule Evaluation Dictionary Extension ---
: CHECK-REFRESH-DECISION ( tile-ptr tile-index -- refresh-flag )
    SWAP \ index tile-ptr
    OVER TILE-ACTIVE? 
    IF
        \ Active tiles are always refreshed (Priority 100%)
        2DROP -1 \ true (-1)
    ELSE
        \ Dormant tiles are evaluated via noise sampling rule
        SWAP EVALUATE-STOCHASTIC-SAMPLE
    THEN ;

\ --- Batch Sorter & Filter Words ---
: FILTER-ACTIVE-TILES ( tile-array-ptr count -- active-count )
    0 -ROT ( count 0 array-ptr )
    0 DO
        DUP TILE-RECORD-SIZE * OVER + ( array-ptr record-ptr )
        DUP TILE-ACTIVE? IF
            1+ 
        THEN
        DROP
    LOOP
    DROP ;

\ --- Complex Rule Macro: Adaptive Frame Rate Throttling ---
: SHOULD-THROTTLE-CPU? ( active-threshold -- throttle? )
    ACTIVE-TILES-COUNT @ > ;

\ --- End of Forth Dictionary ---