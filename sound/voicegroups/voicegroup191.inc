	.align 2
voicegroup191:: @ DPPt main
	voice_directsound 60, 0, DirectSoundWaveData_dp_reverscyn_16, 255, 0, 255, 16	@ 127/127/127/126/ 64
	voice_keysplit_all voicegroup192				@ drums 1
	voice_keysplit voicegroup195, KeySplitTable6	@ piano
	voice_keysplit voicegroup196, KeySplitTable7	@ piano low
	voice_keysplit voicegroup197, KeySplitTable8	@ ep 1
	voice_keysplit voicegroup198, KeySplitTable9	@ ep 2
	voice_keysplit_all voicegroup193				@ drums 2
	voice_keysplit voicegroup199, KeySplitTable10	@ clavi
	voice_directsound 60, 0, DirectSoundWaveData_purenoise, 81, 0, 57, 178	@ 110/127/ 60/120/ 64
	voice_directsound 60, 0, DirectSoundWaveData_dp_glocken_c6_16, 255, 246, 0, 208	@ 127/ 90/  0/116/ 64
	voice_directsound 60, 0, DirectSoundWaveData_puresquare_50, 255, 242, 40, 197	@ 126/100/ 50/118/ 64
	voice_keysplit voicegroup200, KeySplitTable11	@ vibraphone
	voice_keysplit voicegroup201, KeySplitTable12	@ marimba
	voice_keysplit voicegroup202, KeySplitTable13	@ xylophone
	voice_keysplit voicegroup203, KeySplitTable14	@ tubular bell
	voice_keysplit voicegroup204, KeySplitTable15	@ trinity piano
	voice_keysplit_all voicegroup194				@ synth kick + snare
	voice_keysplit voicegroup205, KeySplitTable16	@ rotary organ
	voice_keysplit voicegroup206, KeySplitTable17	@ straight organ
	voice_directsound 60, 0, DirectSoundWaveData_dp_org5_c3_16, 255, 172, 235, 178	@ 127/122/122/120/ 64
	voice_directsound 60, 0, DirectSoundWaveData_dp_sawtoothlead60, 255, 197, 255, 178	@ 127/120/127/120/ 64
	voice_keysplit voicegroup207, KeySplitTable18	@ accordion
	voice_directsound 60, 0, DirectSoundWaveData_dp_harmonica_c3_16, 114, 246, 191, 178	@ 116/ 90/110/120/ 64
	voice_directsound 60, 0, DirectSoundWaveData_dp_fretnoise60, 255, 0, 255, 16	@ 127/127/127/126/ 64
	voice_keysplit voicegroup208, KeySplitTable19	@ acoustic guitar
	voice_directsound 60, 0, DirectSoundWaveData_dp_bluesgt_a2_16, 255, 255, 227, 208	@ 127/ 10/120/116/ 64
	voice_directsound 60, 0, DirectSoundWaveData_dp_eternaguitar36, 255, 254, 159, 208	@ 127/ 30/100/116/ 64
	voice_keysplit voicegroup209, KeySplitTable20	@ clean guitar
	voice_directsound 60, 0, DirectSoundWaveData_dp_spearpillarwind60, 255, 0, 255, 48	@ 127/127/127/125/ 64
	voice_keysplit voicegroup210, KeySplitTable21	@ distortion guitar 1
	voice_keysplit voicegroup211, KeySplitTable22	@ power guitar
	voice_directsound 60, 0, DirectSoundWaveData_puresquare_50, 255, 242, 25, 0	@ 126/100/ 40/127/ 64
	voice_keysplit voicegroup212, KeySplitTable23	@ upright bass 1
	voice_directsound 60, 0, DirectSoundWaveData_dp_sting_bass_e1_16, 255, 251, 0, 208	@ 127/ 60/  1/116/ 64
	voice_directsound 60, 0, DirectSoundWaveData_dp_pickbass_e1_16, 255, 251, 0, 208	@ 127/ 60/  1/116/ 64
	voice_directsound 60, 0, DirectSoundWaveData_dp_fretlessbass_d1_16, 255, 0, 255, 197	@ 127/127/127/118/ 64
	voice_directsound 60, 0, DirectSoundWaveData_dp_slapbass_g1_16, 255, 0, 255, 221	@ 127/127/127/112/ 64
	voice_directsound 60, 0, DirectSoundWaveData_dp_slapbass_c1_16, 255, 0, 255, 216	@ 127/127/127/114/ 64
	voice_directsound 60, 0, DirectSoundWaveData_dp_synbass1_c2_16, 255, 64, 255, 197	@ 127/125/127/118/ 64
	voice_directsound 60, 0, DirectSoundWaveData_dp_synbass2_c1_16, 255, 0, 255, 221	@ 127/127/127/112/ 64
	voice_keysplit voicegroup213, KeySplitTable24	@ violin
	voice_directsound 60, 0, DirectSoundWaveData_dp_oct_g3_16, 255, 233, 255, 212	@ 127/110/127/115/ 64
	voice_directsound 60, 0, DirectSoundWaveData_puresquare_50, 255, 252, 0, 48	@ 127/ 50/  0/125/ 64
	voice_directsound 60, 0, DirectSoundWaveData_sc88pro_wind, 67, 243, 155, 225	@  99/ 99/ 99/110/ 64
	voice_directsound 60, 0, DirectSoundWaveData_dp_explosion, 67, 255, 0, 246	@  99/ 10/  0/ 80/ 64
	voice_keysplit voicegroup214, KeySplitTable25	@ pizzicato
	voice_keysplit voicegroup215, KeySplitTable26	@ harp
	voice_keysplit voicegroup216, KeySplitTable27	@ timpani 1
	voice_keysplit voicegroup217, KeySplitTable28	@ strings
	voice_directsound 60, 0, DirectSoundWaveData_dp_octlow_cs2_16, 255, 197, 255, 197	@ 127/120/127/118/ 64
	voice_directsound 60, 0, DirectSoundWaveData_dp_octlowbra_b2_16, 255, 197, 255, 225	@ 127/120/127/110/ 64
	voice_keysplit voicegroup218, KeySplitTable27	@ timpani 2
	voice_directsound 60, 0, DirectSoundWaveData_puresquare_50, 255, 250, 0, 225	@ 127/ 70/  0/110/ 64
	voice_directsound 60, 0, DirectSoundWaveData_sc88pro_square_wave, 255, 250, 0, 178	@ 127/ 70/  0/120/ 64
	voice_directsound 60, 0, DirectSoundWaveData_sc88pro_bubbles, 255, 0, 255, 0	@ 125/127/127/127/ 64
	voice_directsound 60, 0, DirectSoundWaveData_dp_octhit_c2_16, 255, 197, 255, 221	@ 127/120/127/112/ 64
	voice_keysplit voicegroup219, KeySplitTable29	@ trumpets 1
	voice_keysplit voicegroup220, KeySplitTable30	@ trombone
	voice_keysplit voicegroup221, KeySplitTable31	@ tuba
	voice_keysplit voicegroup222, KeySplitTable29	@ trumpets 2
	voice_keysplit voicegroup223, KeySplitTable9	@ french horns
	voice_keysplit voicegroup224, KeySplitTable32	@ distortion guitar 2
	voice_directsound 60, 0, DirectSoundWaveData_puresquare_25, 255, 242, 101, 225	@ 124/100/ 80/110/ 64
	voice_directsound 60, 0, DirectSoundWaveData_puresquare_12, 255, 242, 101, 225	@ 124/100/ 80/110/ 64
	voice_directsound 60, 0, DirectSoundWaveData_dp_synbass_c1_16, 255, 64, 255, 216	@ 127/125/127/114/ 64
	voice_directsound 60, 0, DirectSoundWaveData_dp_altosax_c3_16, 255, 216, 252, 197	@ 127/117/126/118/ 64
	voice_keysplit voicegroup225, KeySplitTable23	@ upright bass 2
	voice_directsound 60, 0, DirectSoundWaveData_puresquare_50, 255, 242, 101, 225	@ 124/100/ 80/110/ 64
	voice_directsound 60, 0, DirectSoundWaveData_dp_oboe_c3_16, 255, 242, 127, 216	@ 127/100/ 90/114/ 64
	voice_directsound 60, 0, DirectSoundWaveData_purenoise, 255, 242, 0, 178	@ 125/100/  0/120/ 64
	voice_directsound 60, 0, DirectSoundWaveData_puresquare_25, 58, 0, 101, 225	@  90/127/ 80/110/ 64
	voice_directsound 60, 0, DirectSoundWaveData_dp_sy_clarinet_c3_16, 255, 242, 159, 216	@ 127/100/100/114/ 64
	voice_directsound 60, 0, DirectSoundWaveData_puresquare_50, 255, 242, 101, 178	@ 124/100/ 80/120/ 64
	voice_keysplit voicegroup226, KeySplitTable33	@ flute
	voice_directsound 60, 0, DirectSoundWaveData_dp_woodpat_c4_16, 240, 246, 78, 149	@ 120/ 90/ 70/122/ 64
	voice_directsound 60, 0, DirectSoundWaveData_dp_basschoir36, 255, 197, 255, 178	@ 127/120/127/120/ 64
	voice_directsound 60, 0, DirectSoundWaveData_dp_tenorchoir48, 255, 197, 255, 178	@ 127/120/127/120/ 64
	voice_directsound 60, 0, DirectSoundWaveData_dp_sopranochoir60, 255, 197, 255, 178	@ 127/120/127/120/ 64
@	voice_directsound 60, 0, DirectSoundWaveData_dp_whistle_c5_16, 255, 246, 78, 197			@ remaining voices wrap around
@	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0	@ dummy												@ to the next voice group, so
@	voice_directsound 60, 0, DirectSoundWaveData_dp_orchhitmajor60, 255, 197, 255, 178			@ have some dittos
@	voice_directsound 60, 0, DirectSoundWaveData_dp_orchhitminor60, 255, 197, 255, 178
@	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0	@ dummy
@	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0	@ dummy
@	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0	@ dummy
@	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0	@ dummy
@	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0	@ dummy
@	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0	@ dummy
@	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0	@ dummy
@	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0	@ dummy
@	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0	@ dummy
@	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0	@ dummy
@	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0	@ dummy
@	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0	@ dummy
@	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0	@ dummy
@	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0	@ dummy
@	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0	@ dummy
@	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0	@ dummy
@	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0	@ dummy
@	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0	@ dummy
@	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0	@ dummy
@	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0	@ dummy
@	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0	@ dummy
@	voice_square_1 60, 0, 0, 2, 0, 0, 15, 0	@ dummy
@	voice_keysplit voicegroup227, KeySplitTable25	@ music box (platinum only)
@	voice_keysplit voicegroup228, KeySplitTable34	@ banjo
