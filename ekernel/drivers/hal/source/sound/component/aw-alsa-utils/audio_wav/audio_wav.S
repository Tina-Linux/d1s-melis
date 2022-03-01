#ifdef CONFIG_BUILTIN_WAV_FILE

.global music_16K_16bit_1ch_start
music_16K_16bit_1ch_start:
.incbin "drivers/hal/source/sound/component/aw-alsa-utils/audio_wav/16000-16bit-1ch.wav"
.global music_16K_16bit_1ch_end
music_16K_16bit_1ch_end:


.global music_8K_16bit_2ch_start
music_8K_16bit_2ch_start:
.incbin "drivers/hal/source/sound/component/aw-alsa-utils/audio_wav/8000-16bit-2ch.wav"
.global music_8K_16bit_2ch_end
music_8K_16bit_2ch_end:

#ifdef ALL_WAV_FILE

.global music_16K_16bit_2ch_start
music_16K_16bit_2ch_start:
.incbin "drivers/hal/source/sound/component/aw-alsa-utils/audio_wav/16000-16bit-2ch.wav"
.global music_16K_16bit_2ch_end
music_16K_16bit_2ch_end:



.global music_16K_24bit_2ch_start
music_16K_24bit_2ch_start:
.incbin "drivers/hal/source/sound/component/aw-alsa-utils/audio_wav/16000-24bit-2ch.wav"
.global music_16K_24bit_2ch_end
music_16K_24bit_2ch_end:

.global music_44K_16bit_2ch_start
music_44K_16bit_2ch_start:
.incbin "drivers/hal/source/sound/component/aw-alsa-utils/audio_wav/44100-16bit-2ch.wav"
.global music_44K_16bit_2ch_end
music_44K_16bit_2ch_end:

.global music_48K_16bit_2ch_start
music_48K_16bit_2ch_start:
.incbin "drivers/hal/source/sound/component/aw-alsa-utils/audio_wav/48000-16bit-2ch.wav"
.global music_48K_16bit_2ch_end
music_48K_16bit_2ch_end:

#endif /* ALL_WAV_FILE */


#endif /* CONFIG_BUILTIN_WAV_FILE */
