/* MollenOS
*
* Copyright 2011 - 2016, Philip Meulengracht
*
* This program is free software : you can redistribute it and / or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation ? , either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.If not, see <http://www.gnu.org/licenses/>.
*
*
* MollenOS C Library - Timezone variables lock
*/

#include <os/spinlock.h>
#include "local.h"

/* Declare the lock */
Spinlock_t __GlbTzLock = SPINLOCK_INIT;

/* Grab tz lock */
void __tz_lock(void) {
	SpinlockAcquire(&__GlbTzLock);
}

/* Release tz lock */
void __tz_unlock(void) {
	SpinlockRelease(&__GlbTzLock);
}
