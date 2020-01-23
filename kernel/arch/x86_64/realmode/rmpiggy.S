/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Wrapper script for the realmode binary as a transport object
 * before copying to low memory.
 */
#include <linkage.h>
	.section ".init.data","aw"

	.balign 4096   /* PAGE SIZE */

GLOBAL(real_mode_blob)
	.incbin	"arch/x86/realmode/realmode.bin"
END(real_mode_blob)
GLOBAL(real_mode_blob_end);

/*
GLOBAL(real_mode_relocs)
	.incbin	"arch/x86/realmode/rm/realmode.relocs"
END(real_mode_relocs)
*/
