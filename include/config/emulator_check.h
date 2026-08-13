#ifndef GUARD_CONFIG_EMULATOR_CHECK_H
#define GUARD_CONFIG_EMULATOR_CHECK_H

// ### UI settings for the error screen ### //

#define ERROR_MUSIC    MUS_RG_MYSTERY_GIFT // the music playing while the error screen is showing
#define ANIM_SPEED     2   // used for the scrolling background effect; the lower the faster

// SPRITE_MODE determines which sprites to show
#define MODE_VANILLA   0 // shows Pikachu and Porygon
#define MODE_EXPANSION 1 // shows Dizzy Egg and Porygon
#ifdef ROGUE_EXPANSION
  #define SPRITE_MODE  MODE_EXPANSION
#else
  #define SPRITE_MODE  MODE_VANILLA
#endif

#define FADING_STRIPES  TRUE // the border stripes are fading out and back in over time
#define PULSE_FRAMES    120 // frames until change of direction
#define PULSE_MAX_BLEND 10 // maximum darkness level (0 = original, 16 = full black)

// x/y coordinates for all sprites
#define WARNING_X    40
#define WARNING_Y    7
#define DIZZY_EGG_X  174
#define DIZZY_EGG_Y  108
#define PIKACHU_X    185
#define PIKACHU_Y    113
#define PORYGON_X    217
#define PORYGON_Y    97


// ### functional settings for the error screen ### //

#define ALLOW_BOOT_CONTINUATION   FALSE // show the continue message box and allow booting with START
#define BOOT_CONTINUATION_KEY     (START_BUTTON) // The input key to continue booting.


// ================================================================ //
//  Emulator accuracy detection                                     //
//                                                                  //
//  Set ACTIVE_EMU_CHECKS to any combination of the flags below.    //
//  IsInaccurateEmulator() returns TRUE if ANY enabled check fires. //
//  Set ACTIVE_EMU_CHECKS to FALSE to disable all checks.           //
// ================================================================ //

#define EMU_CHECK_REG_IMC       (1 << 0)
#define EMU_CHECK_TIMER_CASCADE (1 << 1)
#define EMU_CHECK_SOUNDCNT_H    (1 << 2)
#define EMU_CHECK_KEYCNT        (1 << 3)
#define EMU_CHECK_SOUNDCNT_L    (1 << 4)

#define ACTIVE_EMU_CHECKS       (EMU_CHECK_SOUNDCNT_L)


// Below are explanations for each check and their known results on popular emulators as of April 2026.

// === EMU_CHECK_REG_IMC ============================================== //
//                                                                      //
// Undocumented I/O mirror register 0x04000800                          //
// expected return value:           0x0D000020                          //
//                                                                      //
// works on:                                                            //
//   - real hardware                                                    //
//   - mGBA (desktop and RetroArch iOS)                                 //
//   - Lemuroid                                                         //
//                                                                      //
// fails on:                                                            //
//   - mGBA (RetroArch Android)                                         //
//   - VBA M 2.2.3                                                      //
//   - Pizzaboy                                                         //
//   - Delta                                                            //
//   - JohnGBA                                                          //
//   - MyBoy                                                            //
//                                                                      //
// Source: GBATEK – "GBA Undocumented Hardware"                         //
//             https://problemkaputt.de/gbatek.htm#gbasystemcontrol     //
//             under 4000800h                                           //
// ==================================================================== //

// === EMU_CHECK_TIMER_CASCADE ======================================== //
//                                                                      //
// Load TM2 with 0xFFFF (one tick to overflow at F/1 prescaler) and TM3 //
// in cascade mode. On accurate emulators TM2 overflows within the      //
// first few cycles after being started, incrementing TM3 to >= 1.      //
// Inaccurate emulators with lazy batch timer updates leave TM3 at 0.   //
// TM2/TM3 are used intentionally: the m4a sound engine uses TM0/TM1    //
// for mixing, so touching TM0/TM1 here corrupts audio init on JohnGBA  //
// or MyBoy.                                                            //
//                                                                      //
// works on:                                                            //
//   - real hardware                                                    //
//   - mGBA (desktop, RetroArch iOS and Android)                        //
//   - Lemuroid                                                         //
//                                                                      //
// fails on:                                                            //
//   - VBA M 2.2.3                                                      //
//   - Pizzaboy                                                         //
//   - Delta                                                            //
//   - JohnGBA                                                          //
//   - MyBoy                                                            //
//                                                                      //
// Source: GBATEK – "GBA Timers"                                        //
//             https://problemkaputt.de/gbatek.htm#gbatimers            //
// ==================================================================== //

// === EMU_CHECK_SOUNDCNT_H =========================================== //
//                                                                      //
// Write 0xFFFF to SOUNDCNT_H (DMA sound control register), then read   //
// back.                                                                //
// On real HW / mGBA: bits 4-7 are unused (read 0) and bits 11 and      // 
// 15 (FIFO A/B reset) are write-only (read 0), so result is 0x770F.    //
// Inaccurate emulators may return 0xFFFF (no masking) or 0x0000.       //
//                                                                      //
// works on:                                                            //
//   - real hardware                                                    //
//   - mGBA (desktop, RetroArch iOS and Android)                        //
//   - Lemuroid                                                         //
//   - VBA M 2.2.3                                                      //
//   - Pizzaboy                                                         //
//   - Delta                                                            //
//   - JohnGBA                                                          //
//                                                                      //
// fails on:                                                            //
//   - MyBoy                                                            //
//                                                                      //
// Source: GBATEK – "GBA Sound Control Registers"                       //
//         https://problemkaputt.de/gbatek.htm#gbasoundcontrolregisters //
// ==================================================================== //

// === EMU_CHECK_KEYCNT =============================================== //
//                                                                      //
// Write 0xFFFF to KEYCNT (key interrupt control, 0x04000132), then     //
// read back. Bits 10-13 are unused and must read 0, giving an expected //
// result of 0xC3FF.                                                    //
//                                                                      //
// works on:                                                            //
//   - real hardware                                                    //
//   - mGBA (desktop, RetroArch iOS and Android)                        //
//   - Lemuroid                                                         //
//   - VBA M 2.2.3                                                      //
//   - Delta                                                            //
//   - JohnGBA                                                          //
//                                                                      //
// fails on:                                                            //
//   - Pizzaboy                                                         //
//   - MyBoy                                                            //
//                                                                      //
// Source: GBATEK – "GBA Keypad Input"                                  //
//         https://problemkaputt.de/gbatek.htm#gbakeypadinput           //
//         (see 4000132h – KEYCNT, bits 10-13 "Not Used")               //
// ==================================================================== //

// === EMU_CHECK_SOUNDCNT_L =========================================== //
//                                                                      //
// Write 0xFFFF to SOUNDCNT_L (PSG channel volume/enable, 0x04000080),  //
// then read back. Bits 3 and 7 are unused and must read 0, giving an   //
// expected result of 0xFF77.                                           //
//                                                                      //
// works on:                                                            //
//   - real hardware                                                    //
//   - mGBA (desktop, RetroArch iOS and Android)                        //
//   - Lemuroid                                                         //
//   - Pizzaboy                                                         //
//   - JohnGBA                                                          //
//                                                                      //
// fails on:                                                            //
//   - VBA M 2.2.3                                                      //
//   - Delta                                                            //
//   - MyBoy                                                            //
//                                                                      //
// Source: GBATEK – "GBA Sound Control Registers"                       //
//         https://problemkaputt.de/gbatek.htm#gbasoundcontrolregisters //
//         (see 4000080h – SOUNDCNT_L, bits 3 and 7 "Not Used")         //
// ==================================================================== //


// ### macro handling for the dynamic string creation of recommended emulators ### //

// define which of the recommended platforms passes which check
// clang-format off
#define EMU_PASSES_MGBA_RA_IOS      (EMU_CHECK_REG_IMC | EMU_CHECK_TIMER_CASCADE | EMU_CHECK_SOUNDCNT_H | EMU_CHECK_KEYCNT | EMU_CHECK_SOUNDCNT_L)
#define EMU_PASSES_MGBA_RA_ANDROID  (                    EMU_CHECK_TIMER_CASCADE | EMU_CHECK_SOUNDCNT_H | EMU_CHECK_KEYCNT | EMU_CHECK_SOUNDCNT_L)
#define EMU_PASSES_LEMUROID         (EMU_CHECK_REG_IMC | EMU_CHECK_TIMER_CASCADE | EMU_CHECK_SOUNDCNT_H | EMU_CHECK_KEYCNT | EMU_CHECK_SOUNDCNT_L)
#define EMU_PASSES_PIZZABOY         (                                              EMU_CHECK_SOUNDCNT_H |                    EMU_CHECK_SOUNDCNT_L)
// clang-format on

// helper macro to check if the passed platform is viable for the current ACTIVE_EMU_CHECKS setting
#define EMU_VIABLE(passes)  (((passes) & ACTIVE_EMU_CHECKS) == ACTIVE_EMU_CHECKS)

// define the text strings to show in the error message for recommended emulators
#if EMU_VIABLE(EMU_PASSES_MGBA_RA_IOS)
 #define VIABLE_MGBA_RA_IOS     "\n- mGBA (RetroArch iOS)"
#else
 #define VIABLE_MGBA_RA_IOS     ""
#endif

#if EMU_VIABLE(EMU_PASSES_MGBA_RA_ANDROID)
 #define VIABLE_MGBA_RA_ANDROID "\n- mGBA (RetroArch Android)"
#else
 #define VIABLE_MGBA_RA_ANDROID ""
#endif

#if EMU_VIABLE(EMU_PASSES_LEMUROID)
 #define VIABLE_LEMUROID        "\n- Lemuroid"
#else
 #define VIABLE_LEMUROID        ""
#endif

#if EMU_VIABLE(EMU_PASSES_PIZZABOY)
 #define VIABLE_PIZZABOY        "\n- Pizzaboy"
#else
 #define VIABLE_PIZZABOY        ""
#endif

#endif // GUARD_CONFIG_EMULATOR_CHECK_H