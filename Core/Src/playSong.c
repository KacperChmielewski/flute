#include <midi.h>
#include <stdbool.h>
#include <stm32l4xx.h>

extern struct midi_handle myMidi;

uint8_t song[] = { MIDI_NOTE_C4, MIDI_NOTE_D4, MIDI_NOTE_E4, MIDI_NOTE_F4,
		MIDI_NOTE_G4, MIDI_NOTE_A4, MIDI_NOTE_H4, MIDI_NOTE_C5, MIDI_NOTE_C5,
		MIDI_NOTE_D4, };

static void playSong() {
	static uint32_t note = 0;
	static bool isOn = false;

	if (isOn) {
		midi_doNote(&myMidi, song[note], 0);
		isOn = false;
		note++;
	} else {
		midi_doNote(&myMidi, song[note], 0x7f);
		isOn = true;
	}

	if (note == sizeof(song)) {
		note = 0;
	}
}

void playSong_isBlow(bool isOn) {
	static bool lastOn = false;

	if (isOn != lastOn) {
		lastOn = isOn;
		playSong();
	}
}
