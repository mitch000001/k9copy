/* Cpu detection code, extracted from mmx.h ((c)1997-99 by H. Dietz
   and R. Fisher). Converted to C and improved by Fabrice Bellard */

#include <stdio.h>
#include <stdlib.h>
#include "ac.h"

#ifdef HAVE_STRING_H
# include <string.h>
#endif

#ifdef ARCH_X86_64
#  define REG_b "rbx"
#  define REG_S "rsi"
#else
#  define REG_b "ebx"
#  define REG_S "esi"
#endif

//exported
 int mm_flag;

/* ebx saving is necessary for PIC. gcc seems unable to see it alone */
#define cpuid(index,eax,ebx,ecx,edx)\
    __asm __volatile\
        ("mov %%"REG_b", %%"REG_S"\n\t"\
         "cpuid\n\t"\
         "xchg %%"REG_b", %%"REG_S\
         : "=a" (eax), "=S" (ebx),\
           "=c" (ecx), "=d" (edx)\
         : "0" (index));

#define CPUID_STD_MMX          0x00800000
#define CPUID_STD_SSE          0x02000000
#define CPUID_STD_SSE2         0x04000000
#define CPUID_STD_SSE3         0x00000001  // ECX!
#define CPUID_EXT_AMD_3DNOW    0x80000000
#define CPUID_EXT_AMD_3DNOWEXT 0x40000000
#define CPUID_EXT_AMD_MMXEXT   0x00400000
#define CPUID_EXT_CYR_MMX      0x00800000
#define CPUID_EXT_CYR_MMXEXT   0x01000000

/* Function to test if multimedia instructions are supported...  */
static int mm_support(void)
{
    int rval = 0;
#if defined(ARCH_X86) || defined(ARCH_X86_64)
    int eax, ebx, ecx, edx;
    int max_std_level, max_ext_level, std_caps=0, std_caps2=0, ext_caps=0;
    long a, c;

    char vendor[13] = "UnknownVndr";

    __asm__ __volatile__ (
                          /* See if CPUID instruction is supported ... */
                          /* ... Get copies of EFLAGS into eax and ecx */
                          "pushf\n\t"
                          "pop %0\n\t"
                          "mov %0, %1\n\t"

                          /* ... Toggle the ID bit in one copy and store */
                          /*     to the EFLAGS reg */
                          "xor $0x200000, %0\n\t"
                          "push %0\n\t"
                          "popf\n\t"

                          /* ... Get the (hopefully modified) EFLAGS */
                          "pushf\n\t"
                          "pop %0\n\t"
                          : "=a" (a), "=c" (c)
                          :
                          : "cc"
                          );

    if (a == c)
        return 0; /* CPUID not supported */

    cpuid(0, max_std_level, ebx, ecx, edx);

    /* highest cpuid is 0, no standard features */
    if (max_std_level == 0)
        return rval;

    /* save the vendor string */
    *(int *)vendor = ebx;
    *(int *)&vendor[4] = edx;
    *(int *)&vendor[8] = ecx;

    if (max_std_level >= 1){
        cpuid(1, eax, ebx, std_caps2, std_caps);
        if (std_caps & (1<<15))
            rval |= MM_CMOVE;
        if (std_caps & (1<<23))
            rval |= MM_MMX;
        if (std_caps & (1<<25))
            rval |= MM_MMXEXT | MM_SSE;
        if (std_caps & (1<<26))
            rval |= MM_SSE2;
    }

    cpuid(0x80000000, max_ext_level, ebx, ecx, edx);

    if (max_ext_level >= 0x80000001) {
        cpuid(0x80000001, eax, ebx, ecx, ext_caps);
    }

    cpuid(0, eax, ebx, ecx, edx);
    if (strcmp(vendor, "AuthenticAMD") == 0) {
        /* AMD */
        if (ext_caps & (1<<22))
            rval |= MM_MMXEXT;
        if (ext_caps & (1<<31))
            rval |= MM_3DNOW;
        if (ext_caps & (1<<30))
            rval |= MM_3DNOWEXT;
        if (std_caps2 & (1<<0))
            rval |= MM_SSE3;
    } else if (strcmp(vendor, "CentaurHauls") == 0) {
        /* VIA C3 */
        if (ext_caps & (1<<24))
            rval |= MM_MMXEXT;
    } else if (strcmp(vendor, "CyrixInstead") == 0) {
        /* Cyrix */
        if (ext_caps & (1<<24))
            rval |= MM_MMXEXT;
    }
#if 0
    av_log(NULL, AV_LOG_DEBUG, "%s%s%s%s%s%s\n",
        (rval&MM_MMX) ? "MMX ":"",
        (rval&MM_MMXEXT) ? "MMX2 ":"",
        (rval&MM_SSE) ? "SSE ":"",
        (rval&MM_SSE2) ? "SSE2 ":"",
        (rval&MM_3DNOW) ? "3DNow ":"",
        (rval&MM_3DNOWEXT) ? "3DNowExt ":"");
#endif
#endif  /* ARCH_X86(_64) */
    return rval;
}


int ac_mmflag(void)
{
  //if (mm_flag==-1) {
    mm_flag = mm_support();
#ifdef ARCH_X86
    mm_flag |= MM_IA32ASM;
#endif
#ifdef ARCH_X86_64
    mm_flag |= MM_AMD64ASM;
#endif
  //}
  return(mm_flag);
}

void ac_mmtest()
{
 mm_flag=-1;
  int cc=ac_mmflag();
  return;
  printf("(%s) available multimedia extensions:", __FILE__);

  if(cc & MM_SSE3) {
    printf(" sse3\n");
    return;
  } else if(cc & MM_SSE2) {
    printf(" sse2\n");
    return;
  } else if(cc & MM_SSE) {
    printf(" sse\n");
    return;
  } else if(cc & MM_3DNOWEXT) {
    printf(" 3dnowext\n");
    return;
  } else if(cc & MM_3DNOW) {
    printf(" 3dnow\n");
    return;
  } else if(cc & MM_MMXEXT) {
    printf(" mmxext\n");
    return;
  } else if(cc & MM_MMX) {
    printf(" mmx\n");
    return;
  } else if(cc & MM_AMD64ASM) {
    printf(" 64asm\n");
    return;
  } else if(cc & MM_IA32ASM) {
    printf(" 32asm\n");
    return;
  } else printf(" C\n");
}

/*
 * Appends src to string dst of size siz (unlike strncat, siz is the
 * full size of dst, not space left).  At most siz-1 characters
 * will be copied.  Always NUL terminates (unless siz <= strlen(dst)).
 * Returns strlen(src) + MIN(siz, strlen(initial dst)).
 * If retval >= siz, truncation occurred.
 */
size_t
strlcat(char *dst, const char *src, size_t siz)
{
	char *d = dst;
	const char *s = src;
	size_t n = siz;
	size_t dlen;

	/* Find the end of dst and adjust bytes left but don't go past end */
	while (n-- != 0 && *d != '\0')
		d++;
	dlen = d - dst;
	n = siz - dlen;

	if (n == 0)
		return(dlen + strlen(s));
	while (*s != '\0') {
		if (n != 1) {
			*d++ = *s;
			n--;
		}
		s++;
	}
	*d = '\0';

	return(dlen + (s - src));	/* count does not include NUL */
}

char *ac_mmstr(int flag, int mode)
{
  static char mmstr[64]="";
  int cc;

  if(flag==-1)
    //get full mm caps
    cc=ac_mmflag();
  else
    cc=flag;

  //return max supported mm extensions, or str for user provided flag
  if(mode==0) {
    if(cc & MM_SSE3) {
      return("sse3");
    } else if(cc & MM_SSE2) {
      return("sse2");
    } else if(cc & MM_SSE) {
      return("sse");
    } else if(cc & MM_3DNOWEXT) {
      return("3dnowext");
    } else if(cc & MM_3DNOW) {
      return("3dnow");
    } else if(cc & MM_MMXEXT) {
      return("mmxext");
    } else if(cc & MM_MMX) {
      return("mmx");
    } else if(cc & (MM_AMD64ASM|MM_IA32ASM)) {
      return("asm");
    } else return("C");
  }

  //return full capability list
  if(mode==1) {
    if(cc & MM_SSE3) strlcat(mmstr, "sse3 ", sizeof(mmstr));
    if(cc & MM_SSE2) strlcat(mmstr, "sse2 ", sizeof(mmstr));
    if(cc & MM_SSE) strlcat(mmstr, "sse ", sizeof(mmstr));
    if(cc & MM_3DNOWEXT) strlcat(mmstr, "3dnowext ", sizeof(mmstr));
    if(cc & MM_3DNOW) strlcat(mmstr, "3dnow ", sizeof(mmstr));
    if(cc & MM_MMXEXT) strlcat(mmstr, "mmxext ", sizeof(mmstr));
    if(cc & MM_MMX) strlcat(mmstr, "mmx ", sizeof(mmstr));
    if(cc & (MM_AMD64ASM|MM_IA32ASM)) strlcat(mmstr, "asm ", sizeof(mmstr));
    strlcat(mmstr, "C", sizeof(mmstr));
    return(mmstr);
  }

  return("");
}
