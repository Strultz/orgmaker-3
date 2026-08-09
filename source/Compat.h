#pragma once

#include <cstdint>

enum CompatFlags {
	COMPAT_OLD_VOLUME = 1 << 0,
	COMPAT_RXO_FREQ_TABLE = 1 << 1,
	COMPAT_1_0_NOTE_CHANGE = 1 << 2,
	COMPAT_NO_INTERPOLATE = 1 << 3, // TODO implement
	COMPAT_NO_VOLUME_RAMP = 1 << 4, // TODO implement
	COMPAT_CS_PIZZICATO = 1 << 5,
	COMPAT_CS_VOLUME_BUG = 1 << 6,
	COMPAT_MUTE_U_I = 1 << 7, // TODO implement
	COMPAT_DEFAULT_PERC = 1 << 8, // TODO implement
};

extern uint32_t gCompatFlags;
