/***************************************************************************
 *   Project TUPI: Magia 2D                                                *
 *   Project Contact: info@maefloresta.com                                 *
 *   Project Website: http://www.maefloresta.com                           *
 *   Project Leader: Gustav Gonzalez <info@maefloresta.com>                *
 *                                                                         *
 *   Developers:                                                           *
 *   2010:                                                                 *
 *    Gustavo Gonzalez / xtingray                                          *
 *                                                                         *
 *   KTooN's versions:                                                     * 
 *                                                                         *
 *   2006:                                                                 *
 *    David Cuadrado                                                       *
 *    Jorge Cuadrado                                                       *
 *   2003:                                                                 *
 *    Fernado Roldan                                                       *
 *    Simena Dinas                                                         *
 *                                                                         *
 *   Copyright (C) 2010 Gustav Gonzalez - http://www.maefloresta.com       *
 *   License:                                                              *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 ***************************************************************************/

#include "kcpuinfo.h"

#if defined(__GNUC__) || defined(__INTEL_COMPILER)
#  define HAVE_GNU_INLINE_ASM
#endif

typedef void (*kde_sighandler_t) (int);

/*
#ifdef __i386__
static jmp_buf env;
//Sighandler for the SSE OS support check
static void sighandler( int )
{
    std::longjmp( env, 1 );
}
#endif
*/

#ifdef __PPC__
static sigjmp_buf  jmpbuf;
static sig_atomic_t  canjump = 0;

static void sigill_handler( int sig )
{
    if ( !canjump ) {
        signal( sig, SIG_DFL );
        raise( sig );
    }
    canjump = 0;
    siglongjmp( jmpbuf, 1 );
}
#endif

static int getCpuFeatures()
{
    int features = 0;

#if defined( HAVE_GNU_INLINE_ASM )
#if defined( __i386__ )
    bool haveCPUID = false;
    bool have3DNOW = false;
    int result = 0;

    // First check if the CPU supports the CPUID instruction
    __asm__ __volatile__(
    // Try to toggle the CPUID bit in the EFLAGS register
    "pushf                      \n\t"   // Push the EFLAGS register onto the stack
    "popl   %%ecx               \n\t"   // Pop the value into ECX
    "movl   %%ecx, %%edx        \n\t"   // Copy ECX to EDX
    "xorl   $0x00200000, %%ecx  \n\t"   // Toggle bit 21 (CPUID) in ECX
    "pushl  %%ecx               \n\t"   // Push the modified value onto the stack
    "popf                       \n\t"   // Pop it back into EFLAGS

    // Check if the CPUID bit was successfully toggled
    "pushf                      \n\t"   // Push EFLAGS back onto the stack
    "popl   %%ecx               \n\t"   // Pop the value into ECX
    "xorl   %%eax, %%eax        \n\t"   // Zero out the EAX register
    "cmpl   %%ecx, %%edx        \n\t"   // Compare ECX with EDX
    "je    .Lno_cpuid_support%= \n\t"   // Jump if they're identical
    "movl      $1, %%eax        \n\t"   // Set EAX to true
    ".Lno_cpuid_support%=:      \n\t"
    : "=a"(haveCPUID) : : "%ecx", "%edx" );

    // If we don't have CPUID we won't have the other extensions either
    if ( ! haveCPUID )
        return 0L;

    // Execute CPUID with the feature request bit set
    __asm__ __volatile__(
    "pushl  %%ebx               \n\t"   // Save EBX
    "movl      $1, %%eax        \n\t"   // Set EAX to 1 (features request)
    "cpuid                      \n\t"   // Call CPUID
    "popl   %%ebx               \n\t"   // Restore EBX
    : "=d"(result) : : "%eax", "%ecx" );

    // Test bit 23 (MMX support)
    if ( result & 0x00800000 )
        features |= KCPUInfo::IntelMMX;

    __asm__ __volatile__(
      "pushl %%ebx             \n\t"
      "movl $0x80000000, %%eax \n\t"
      "cpuid                   \n\t"
      "cmpl $0x80000000, %%eax \n\t"
      "jbe .Lno_extended%=     \n\t"
      "movl $0x80000001, %%eax \n\t"
      "cpuid                   \n\t"
      "test $0x80000000, %%edx \n\t"
      "jz .Lno_extended%=      \n\t"
      "movl      $1, %%eax     \n\t"   // // Set EAX to true
      ".Lno_extended%=:        \n\t"
      "popl   %%ebx            \n\t"   // Restore EBX
      : "=a"(have3DNOW) : );

    if ( have3DNOW )
        features |= KCPUInfo::AMD3DNOW;

#ifdef HAVE_X86_SSE
    // Test bit 25 (SSE support)
    if ( result & 0x00200000 ) {
        features |= KCPUInfo::IntelSSE;

        // OS support test for SSE.
        // Install our own sighandler for SIGILL.
        kde_sighandler_t oldhandler = std::signal( SIGILL, sighandler );

        // Try executing an SSE insn to see if we get a SIGILL
        if ( setjmp( env ) )
            features ^= KCPUInfo::IntelSSE; // The OS support test failed
        else
            __asm__ __volatile__("xorps %xmm0, %xmm0");

        // Restore the default sighandler
        std::signal( SIGILL, oldhandler );

        // Test bit 26 (SSE2 support)
        if ( (result & 0x00400000) && (features & DCPUInfo::IntelSSE) )
            features |= KCPUInfo::IntelSSE2;

        // Note: The OS requirements for SSE2 are the same as for SSE
        //       so we don't have to do any additional tests for that.
    }
#endif // HAVE_X86_SSE
#elif defined __PPC__ && defined HAVE_PPC_ALTIVEC
    signal( SIGILL, sigill_handler );
    if ( sigsetjmp( jmpbuf, 1 ) ) {
        signal( SIGILL, SIG_DFL );
    } else {
        canjump = 1;
        __asm__ __volatile__( "mtspr 256, %0\n\t"
                              "vand %%v0, %%v0, %%v0"
                              : /* none */
                              : "r" (-1) );
        signal( SIGILL, SIG_DFL );
        features |= KCPUInfo::AltiVec;
    }
#endif // __i386__
#endif //HAVE_GNU_INLINE_ASM

    return features;
}

unsigned int KCPUInfo::s_features = getCpuFeatures();
