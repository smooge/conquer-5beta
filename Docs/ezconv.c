/*
 * Written by Adam Bryant (adb@bu.edu)
 *
 * This was written as a quicky conversion program... do with
 * it what you will.
 *
 */
#include <stdio.h>
#ifndef TRUE
#define TRUE (1)
#define FALSE (0)
#endif /* TRUE */

/* syntax */
char *usage = "Usage: %s [infile [outfile]]\n";

/*
 * main - Text conversion utility for document formatting
 *
 * This is a quick conversion program that processes text files to handle
 * special formatting sequences. It primarily converts page break indicators
 * (^L sequences) into actual form feed characters and manages newline
 * consolidation to clean up document formatting.
 *
 * The program reads from stdin or a specified input file and writes to
 * stdout or a specified output file. It processes characters sequentially,
 * looking for special formatting sequences and converting them appropriately.
 *
 * Parameters:
 *   argc - Number of command line arguments (0-2 additional args expected)
 *   argv - Array of argument strings:
 *          argv[0] - Program name
 *          argv[1] - Input filename (optional, defaults to stdin)
 *          argv[2] - Output filename (optional, defaults to stdout)
 *
 * Returns:
 *   Always exits with status 0 on success, 1 on error
 *   Does not return to caller (calls exit() directly)
 *
 * Side Effects:
 *   - Opens and reads from input file or stdin
 *   - Opens and writes to output file or stdout
 *   - Closes files before exit
 *   - Prints error messages to stderr on file open failures
 *   - Exits program with appropriate status code
 *
 * Processing Logic:
 *   - Converts "^L" at line start to form feed character (\014)
 *   - Consolidates multiple consecutive newlines
 *   - Preserves other text characters unchanged
 *   - Handles EOF conditions properly
 *
 * Input Format:
 *   - Plain text with optional formatting sequences
 *   - "^L" at beginning of line indicates page break
 *   - Regular text and newlines processed normally
 *
 * Output Format:
 *   - Clean text with form feed characters for page breaks
 *   - Normalized newline sequences
 *   - Suitable for printing or further document processing
 *
 * Notes:
 *   - Written as a "quicky conversion program" by Adam Bryant
 *   - Uses K&R style function definition (legacy code)
 *   - Limited error handling (exits on file open failure)
 *   - No validation of file permissions or disk space
 */
main(argc,argv)
  int argc;
  char *argv[];
{
  FILE *fpi = stdin, *fpo = stdout;
  int ch, count = 1, posit;

  /* check for the input file */
  if (argc>count && (fpi = fopen(argv[count],"r"))==(FILE *)NULL) {
    fprintf(stderr,"%s: error opening input file <%s>\n",argv[0],argv[count]);
    exit(1);
  }
  count++;

  /* check for the output file */
  if (argc>count && (fpo = fopen(argv[count],"w"))==(FILE *)NULL) {
    fprintf(stderr,"%s: error opening output file <%s>\n",argv[0],argv[count]);
    exit(1);
  }
  count++;

  /* check for proper number of arguments */
  if (argc>count) {
    fprintf(stderr,usage,argv[0]);
    exit(1);
  }

  /* processing loop */
  count = 1;
  posit = 0;
  while (!feof(fpi)) {

    /* switch appropriately */
    switch (ch = getc(fpi)) {
    case '\n':
      if (count == 0) putc('\n', fpo);
      posit = 0;
      count++;
      break;
    case '^':
      /* now check for page break combination */
      if (posit == 0) {
	/* page break indicator? */
	if ((ch = getc(fpi)) == 'L') {
	  putc('\014', fpo);
	  count = 0;
	  posit = 1;
	  break;
	} else {
	  /* put it back and continue */
	  ungetc(ch, fpo);
	  ch = '^';
	}
      }
      /* just let it go by */
    default:
    Finish_Off:
      /* check for finish */
      if (ch == EOF) break;

      /* take care of pending new lines */
      while (count > 1) {
	putc('\n', fpo);
	count--;
	posit = 0;
      }
      putc((char)ch,fpo);
      count = 0;
      posit++;
      break;
    }

  }

  /* close up shop */
  if (fpi != stdin) fclose(fpi);
  if (fpo != stdout) fclose(fpo);
  exit(0);
}
