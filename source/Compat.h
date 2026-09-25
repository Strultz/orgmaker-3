#pragma once

#include <cstdint>

enum CompatFlags {
	COMPAT_OLD_VOLUME       = 1 << 0,
	COMPAT_RXO_FREQ_TABLE   = 1 << 1,
	COMPAT_1_0_NOTE_CHANGE  = 1 << 2,
	COMPAT_1_0_VOLUME_BUG   = 1 << 3,
	COMPAT_CS_PIZZICATO     = 1 << 4,
	COMPAT_CS_VOLUME_BUG    = 1 << 5,
	COMPAT_CS_PERCUSSION    = 1 << 6,
	COMPAT_CS_CHANNEL_U_I   = 1 << 7,
	COMPAT_NO_VOLUME_RAMP   = 1 << 8,
	COMPAT_NO_INTERPOLATION = 1 << 9,
};

extern uint32_t gCompatFlags;
