
/*
  mosiscrc -- calculate and print POSIX.2 checksums and sizes of text 
              or binary files.

  This file is a modified version of the file "cksum.c" from GNU
  textutils version 1.22.  The changes were made on October 27, 1999.
  It packages most of what that program did into a single file, for
  ease of compilation, with some changes.  The changes cause the 
  program to work differently for text files than for binary files.
  Use the -b option to compute a checksum on a binary file and the
  -t option to compute a checksum on a text file.

  MOSIS customers who need to compute the CRC checksum of a text file
  (e.g., CIF file) must use this program.  The standard GNU cksum
  program will NOT give the checksum MOSIS expects for a text file.
  
  MOSIS customers who need to compute the CRC checksum of a binary
  file (e.g., GDS file) can use either the standard GNU cksum program
  or this program since they both compute identical checksums.
  
  The MOSIS CRC checksum for a text file uses the same CRC computation
  as the standard GNU cksum program, but applies it to a different set
  of characters.  The differences are:

     1. Every contiguous sequence of characters with value <= ASCII
        space (32) is treated as a single space character.
        
     2. There is an implied leading and trailing space character
        in every file.
        
  These changes make the checksum for a text file insensitive to
  differences in end of line conventions (CR LF versus LF) and
  insensitive to control characters and white space that may be added
  by mailers.
   
  To compile and run this checksum program:
   
	For Unix (using GNU C compiler):

	% gcc -O2 -o mosiscrc mosiscrc.c 
	% mosiscrc -h	[To get help]
	% mosiscrc -t your-layout-file		[For a CIF file]
	% mosiscrc -b your-layout-file		[For a GDS file]

	For VMS:
	
	$ cc mosiscrc.c
	$ define lnk$library sys$library:vaxcrtl.olb
	$ link mosiscrc.obj
	$ mosiscrc :== $your-disk:[your-full-directory-path]mosiscrc.exe
	$ mosiscrc -h	[To get help]
	$ mosiscrc -t your-layout-file		[For a CIF file]
	$ mosiscrc -b your-layout-file		[For a GDS file]

*/

/* cksum -- calculate and print POSIX.2 checksums and sizes of files
   Copyright (C) 92, 95, 1996 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

/* Written by Q. Frank Xia, qx@math.columbia.edu.
   Cosmetic changes and reorganization by David MacKenzie, djm@gnu.ai.mit.edu.

  Usage: cksum [file...]

  The code segment between "#ifdef CRCTAB" and "#else" is the code
  which calculates the "crctab". It is included for those who want
  verify the correctness of the "crctab". To recreate the "crctab",
  do following:

      cc -DCRCTAB -o crctab cksum.c
      crctab > crctab.h

  As Bruce Evans pointed out to me, the crctab in the sample C code
  in 4.9.10 Rationale of P1003.2/D11.2 is represented in reversed order.
  Namely, 0x01 is represented as 0x80, 0x02 is represented as 0x40, etc.
  The generating polynomial is crctab[0x80]=0xedb88320 instead of
  crctab[1]=0x04C11DB7.  But the code works only for a non-reverse order
  crctab.  Therefore, the sample implementation is wrong.

  This software is compatible with neither the System V nor the BSD
  `sum' program.  It is supposed to conform to P1003.2/D11.2,
  except foreign language interface (4.9.5.3 of P1003.2/D11.2) support.
  Any inconsistency with the standard except 4.9.5.3 is a bug.  */

#ifdef CRCTAB

#include <stdio.h>

#define BIT(x)	( (unsigned long)1 << (x) )
#define SBIT	BIT(31)

/* The generating polynomial is

          32   26   23   22   16   12   11   10   8   7   5   4   2   1
    G(X)=X  + X  + X  + X  + X  + X  + X  + X  + X + X + X + X + X + X + 1

  The i bit in GEN is set if X^i is a summand of G(X) except X^32.  */

#define GEN     (BIT(26)|BIT(23)|BIT(22)|BIT(16)|BIT(12)|BIT(11)|BIT(10)\
                |BIT(8) |BIT(7) |BIT(5) |BIT(4) |BIT(2) |BIT(1) |BIT(0));

static unsigned long r[8];

static void
fill_r ()
{
  int i;

  r[0] = GEN;
  for (i = 1; i < 8; i++)
    r[i] = (r[i - 1] & SBIT) ? (r[i - 1] << 1) ^ r[0] : r[i - 1] << 1;
}

static unsigned long
remainder (m)
     int m;
{
  unsigned long rem = 0;
  int i;

  for (i = 0; i < 8; i++)
    if (BIT (i) & m)
      rem = rem ^ r[i];

  return rem & 0xFFFFFFFF;	/* Make it run on 64-bit machine.  */
}

int
main ()
{
  int i;

  fill_r ();
  printf ("unsigned long crctab[256] = {\n  0x0");
  for (i = 0; i < 51; i++)
    {
      printf (",\n  0x%08X, 0x%08X, 0x%08X, 0x%08X, 0x%08X",
	remainder (i * 5 + 1), remainder (i * 5 + 2), remainder (i * 5 + 3),
	      remainder (i * 5 + 4), remainder (i * 5 + 5));
    }
  printf ("\n};\n");
  exit (0);
}

#else /* !CRCTAB */

#include <stdio.h>
#include <sys/types.h>
#include <string.h>

/* Number of bytes to read at once.  */
#define BUFLEN (1 << 16)

/* Mode for opening binary file for read.  */
#ifndef WIN32
#define READ_MODE "r"
#else
#define READ_MODE "rb"
#endif

/* The name this program was run with.  */
char *program_name;

static unsigned long const crctab[256] =
{
  0x0,
  0x04C11DB7, 0x09823B6E, 0x0D4326D9, 0x130476DC, 0x17C56B6B,
  0x1A864DB2, 0x1E475005, 0x2608EDB8, 0x22C9F00F, 0x2F8AD6D6,
  0x2B4BCB61, 0x350C9B64, 0x31CD86D3, 0x3C8EA00A, 0x384FBDBD,
  0x4C11DB70, 0x48D0C6C7, 0x4593E01E, 0x4152FDA9, 0x5F15ADAC,
  0x5BD4B01B, 0x569796C2, 0x52568B75, 0x6A1936C8, 0x6ED82B7F,
  0x639B0DA6, 0x675A1011, 0x791D4014, 0x7DDC5DA3, 0x709F7B7A,
  0x745E66CD, 0x9823B6E0, 0x9CE2AB57, 0x91A18D8E, 0x95609039,
  0x8B27C03C, 0x8FE6DD8B, 0x82A5FB52, 0x8664E6E5, 0xBE2B5B58,
  0xBAEA46EF, 0xB7A96036, 0xB3687D81, 0xAD2F2D84, 0xA9EE3033,
  0xA4AD16EA, 0xA06C0B5D, 0xD4326D90, 0xD0F37027, 0xDDB056FE,
  0xD9714B49, 0xC7361B4C, 0xC3F706FB, 0xCEB42022, 0xCA753D95,
  0xF23A8028, 0xF6FB9D9F, 0xFBB8BB46, 0xFF79A6F1, 0xE13EF6F4,
  0xE5FFEB43, 0xE8BCCD9A, 0xEC7DD02D, 0x34867077, 0x30476DC0,
  0x3D044B19, 0x39C556AE, 0x278206AB, 0x23431B1C, 0x2E003DC5,
  0x2AC12072, 0x128E9DCF, 0x164F8078, 0x1B0CA6A1, 0x1FCDBB16,
  0x018AEB13, 0x054BF6A4, 0x0808D07D, 0x0CC9CDCA, 0x7897AB07,
  0x7C56B6B0, 0x71159069, 0x75D48DDE, 0x6B93DDDB, 0x6F52C06C,
  0x6211E6B5, 0x66D0FB02, 0x5E9F46BF, 0x5A5E5B08, 0x571D7DD1,
  0x53DC6066, 0x4D9B3063, 0x495A2DD4, 0x44190B0D, 0x40D816BA,
  0xACA5C697, 0xA864DB20, 0xA527FDF9, 0xA1E6E04E, 0xBFA1B04B,
  0xBB60ADFC, 0xB6238B25, 0xB2E29692, 0x8AAD2B2F, 0x8E6C3698,
  0x832F1041, 0x87EE0DF6, 0x99A95DF3, 0x9D684044, 0x902B669D,
  0x94EA7B2A, 0xE0B41DE7, 0xE4750050, 0xE9362689, 0xEDF73B3E,
  0xF3B06B3B, 0xF771768C, 0xFA325055, 0xFEF34DE2, 0xC6BCF05F,
  0xC27DEDE8, 0xCF3ECB31, 0xCBFFD686, 0xD5B88683, 0xD1799B34,
  0xDC3ABDED, 0xD8FBA05A, 0x690CE0EE, 0x6DCDFD59, 0x608EDB80,
  0x644FC637, 0x7A089632, 0x7EC98B85, 0x738AAD5C, 0x774BB0EB,
  0x4F040D56, 0x4BC510E1, 0x46863638, 0x42472B8F, 0x5C007B8A,
  0x58C1663D, 0x558240E4, 0x51435D53, 0x251D3B9E, 0x21DC2629,
  0x2C9F00F0, 0x285E1D47, 0x36194D42, 0x32D850F5, 0x3F9B762C,
  0x3B5A6B9B, 0x0315D626, 0x07D4CB91, 0x0A97ED48, 0x0E56F0FF,
  0x1011A0FA, 0x14D0BD4D, 0x19939B94, 0x1D528623, 0xF12F560E,
  0xF5EE4BB9, 0xF8AD6D60, 0xFC6C70D7, 0xE22B20D2, 0xE6EA3D65,
  0xEBA91BBC, 0xEF68060B, 0xD727BBB6, 0xD3E6A601, 0xDEA580D8,
  0xDA649D6F, 0xC423CD6A, 0xC0E2D0DD, 0xCDA1F604, 0xC960EBB3,
  0xBD3E8D7E, 0xB9FF90C9, 0xB4BCB610, 0xB07DABA7, 0xAE3AFBA2,
  0xAAFBE615, 0xA7B8C0CC, 0xA379DD7B, 0x9B3660C6, 0x9FF77D71,
  0x92B45BA8, 0x9675461F, 0x8832161A, 0x8CF30BAD, 0x81B02D74,
  0x857130C3, 0x5D8A9099, 0x594B8D2E, 0x5408ABF7, 0x50C9B640,
  0x4E8EE645, 0x4A4FFBF2, 0x470CDD2B, 0x43CDC09C, 0x7B827D21,
  0x7F436096, 0x7200464F, 0x76C15BF8, 0x68860BFD, 0x6C47164A,
  0x61043093, 0x65C52D24, 0x119B4BE9, 0x155A565E, 0x18197087,
  0x1CD86D30, 0x029F3D35, 0x065E2082, 0x0B1D065B, 0x0FDC1BEC,
  0x3793A651, 0x3352BBE6, 0x3E119D3F, 0x3AD08088, 0x2497D08D,
  0x2056CD3A, 0x2D15EBE3, 0x29D4F654, 0xC5A92679, 0xC1683BCE,
  0xCC2B1D17, 0xC8EA00A0, 0xD6AD50A5, 0xD26C4D12, 0xDF2F6BCB,
  0xDBEE767C, 0xE3A1CBC1, 0xE760D676, 0xEA23F0AF, 0xEEE2ED18,
  0xF0A5BD1D, 0xF464A0AA, 0xF9278673, 0xFDE69BC4, 0x89B8FD09,
  0x8D79E0BE, 0x803AC667, 0x84FBDBD0, 0x9ABC8BD5, 0x9E7D9662,
  0x933EB0BB, 0x97FFAD0C, 0xAFB010B1, 0xAB710D06, 0xA6322BDF,
  0xA2F33668, 0xBCB4666D, 0xB8757BDA, 0xB5365D03, 0xB1F740B4
};

/* Nonzero if any of the files read were the standard input. */
static int have_read_stdin;

/* Calculate and print the checksum and length in bytes
   of file FILE, or of the standard input if FILE is "-".
   If PRINT_NAME is nonzero, print FILE next to the checksum and size.
   TEXT indicates whether file is text or binary.
   Return 0 if successful, -1 if an error occurs. */

static int
cksum (char *file, int print_name, int text)
{
  unsigned char buf[BUFLEN];
  unsigned long crc = 0;
  long length = 0;
  long high_bits = 0;
  long controls = 0;
  long bytes_read;
  register FILE *fp;
  int insep;		/* Are we in a separator sequence? */
  unsigned char c;

  if (!strcmp (file, "-"))
    {
      fp = stdin;
      have_read_stdin = 1;
    }
  else
    {
      fp = fopen (file, READ_MODE);
      if (fp == NULL)
	{
	  fprintf (stderr, "%s: cannot open input file %s\n", program_name, file);
	  fflush(stderr);
	  return -1;
	}
    }

  
  if (text) {
    /* Process implied leading space character */

    length = 1;
    insep = 1;
    crc = (crc << 8) ^ crctab[((crc >> 24) ^ (unsigned char) 32) & 0xFF];
  
    while ((bytes_read = fread (buf, 1, BUFLEN, fp)) > 0)
      {
	unsigned char *cp = buf;

	while (bytes_read--) {
	  c = *(cp++);
	  if (c >= 128) {
	    high_bits++;
	  } else if (c < ' ') {
	    switch (c) {
	    case '\t':
	    case '\n':
	    case '\r':
	    case '\f':
	      break;
	    default:
	      controls++;
	    }
	  }
	  if (c > ' ') {
	    crc = (crc << 8) ^ crctab[((crc >> 24) ^ c) & 0xFF];
	    length++;
	    insep = 0;
	  } else if (!insep) {
	    crc = (crc << 8) ^ crctab[((crc >> 24) ^ (unsigned char) 32) & 0xFF];
	    length++;
	    insep = 1;
	  }
	}
      }
  
    /* Add implied trailing space character, if needed */

    if (!insep) {
      crc = (crc << 8) ^ crctab[((crc >> 24) ^ (unsigned char) 32) & 0xFF];
      length++;
    }
      
  } else {	/* Binary file */
    while ((bytes_read = fread (buf, 1, BUFLEN, fp)) > 0)
      {
	unsigned char *cp = buf;

	length += bytes_read;
	while (bytes_read--) {
	  c = *(cp++);
	  if (c >= 128) {
	    high_bits++;
	  } else if (c < ' ') {
	    switch (c) {
	    case '\t':
	    case '\n':
	    case '\r':
	    case '\f':
	      break;
	    default:
	      controls++;
	    }
	  }
	  crc = (crc << 8) ^ crctab[((crc >> 24) ^ c) & 0xFF];
	}
      }
  }

  if (ferror (fp))
    {
      fprintf (stderr, "%s: error reading file %s\n", program_name, file);
      fflush(stderr);
      if (strcmp (file, "-"))
	fclose (fp);
      return -1;
    }

  if (strcmp (file, "-") && fclose (fp) == EOF)
    {
      fprintf (stderr, "%s: error closing file %s\n", program_name, file);
      fflush(stderr);
      return -1;
    }

  bytes_read = length;
  while (bytes_read > 0)
    {
      crc = (crc << 8) ^ crctab[((crc >> 24) ^ bytes_read) & 0xFF];
      bytes_read >>= 8;
    }

  crc = ~crc & 0xFFFFFFFF;

  printf ("CRC-Checksum: %lu %ld", crc, length);
  if (print_name)
    printf (" %s", file);
  putchar ('\n');
  fflush(stdout);
    
  if (text && (controls != 0 || high_bits != 0)) {
    fprintf (stderr, "\n%s: file has %ld control characters and %ld high bit characters\n", program_name, controls, high_bits);
    fprintf (stderr, "%s: file %s may not be a text file\n", program_name, file);
    fprintf (stderr, "%s: you may need to use -b option instead of -t\n\n", program_name);
    fflush(stderr);
  } else if (!text && controls == 0 && high_bits == 0) {
    fprintf (stderr, "\n%s: file has no control characters or high bit characters\n", program_name);
    fprintf (stderr, "%s: file %s may not be a binary file\n", program_name, file);
    fprintf (stderr, "%s: you may need to use -t option instead of -b\n\n", program_name);
    fflush(stderr);
  }

  return 0;
}

static void usage (void) {
  printf ("\nUsage: %s OPTIONS [FILE]...\n", program_name);
  printf ("Print CRC checksum and byte count of each FILE.\nRead from standard input if no files named.\nOptions:\n  -b   input files are binary files (GDS)\n  -t   input files are text files (CIF)\n  -h   display this help and exit\n\n");
  fflush(stdout);
}

/* Pause until get input from user on Windows, so program output
   remains visible. */

void pause_for_input (void) {
#ifdef WIN32
  char buffer[100];

  printf ("\nPress Enter to exit: ");
  fflush(stdout);
  fgets(buffer, 100, stdin);
#endif
}

int
main (int argc, char **argv)
{
  int i;
  int errors = 0;
  int optind;
  int text, binary;

  program_name = "mosiscrc";

  have_read_stdin = 0;

  text = 0;
  binary = 0;
  optind = 1;
  while (optind < argc) {
    if (argv[optind][0] != '-') break;
    switch (argv[optind][1]) {
    case 'b':
      binary = 1;
      break;
    case 't':
      text = 1;
      break;
    case 'h':
    case 'H':
      usage();
      pause_for_input();
      return(1);
      break;
    default:
      usage();
      pause_for_input();
      return(1);
    }
    optind++;
  }
  
  if (!binary && !text) {
    fprintf (stderr, "%s: must specify either -b or -t option (binary or text)\n  Try -h option for more detailed help\n", program_name);
    fflush(stderr);
    pause_for_input();
    return(1);
  } else if (binary && text) {
    fprintf (stderr, "%s: cannot specify both -b and -t options\n  Try -h option for more detailed help\n", program_name);
    fflush(stderr);
    pause_for_input();
    return(1);
  }

  if (optind >= argc)
    {
      if (cksum ("-", 0, text) < 0)
	errors = 1;
    }
  else
    {
      for (i = optind; i < argc; i++)
	if (cksum (argv[i], 1, text) < 0)
	  errors = 1;
    }

  printf("\n");
  printf("NOTE: The CRC-Checksum consists of two numbers.  If you submit your project\n");
  printf("via the web, both numbers must be entered in the Fabricate web form.\n");
  printf("The first  number must be entered in the Checksum field.\n");
  printf("The second number must be entered in the Count field.\n");
  printf("\n");
  
  if (have_read_stdin && fclose (stdin) == EOF) {
    fprintf (stderr, "%s: error closing standard input\n", program_name);
    fflush(stderr);
    errors = 1;
  }
  pause_for_input();
  return (errors == 0 ? 0 : 1);
}

#endif /* !CRCTAB */
