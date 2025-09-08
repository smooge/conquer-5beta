/*
**  This is a public domain version of getopt(3).
**  Bugs, fixes to:
**		Keith Bostic
**			ARPA: keith@seismo
**			UUCP: seismo!keith
**  Added NO_STDIO, opterr handling, Rich $alz (mirror!rs).
*/

#include <stdio.h>

/*
**  Error macro.  Maybe we want stdio, maybe we don't.
**  The (undocumented?) variable opterr tells us whether or not
**  to print errors.
*/
#ifdef VAXC
#define index(s,c)	strchr(s,c)
extern char	*strchr();	/* This may be index	*/
#else
extern char	*index();	/* This may be strchr	*/
#endif

#ifdef	NO_STDIO

#define tell(s)	\
	if (opterr) \
	{ \
		char	ebuf[2]; \
		(void)write(2, nargv, (unsigned int)strlen(nargv)); \
		(void)write(2, s, (unsigned int)strlen(s)); \
		ebuf[0] = optopt; \
		ebuf[1] = '\n';	\
		(void)write(2, ebuf, 2); \
	}

#else

#define tell(s) \
	if (opterr) \
		(void)fputs(*nargv, stderr), \
		(void)fputs(s,stderr), \
		(void)fputc(optopt, stderr), \
		(void)fputc('\n', stderr)

#endif


/* Global variables. */
static char	 EMSG[] = "";
int	 opterr = 1;	/* undocumented error-suppressor*/
int	 optind = 1;	/* index into argv vector	*/
int	 optopt;	/* char checked for validity	*/
char	*optarg;	/* arg associated with option	*/


/*
 * getopt - Parse command line options following POSIX conventions
 *
 * This is a public domain implementation of the standard getopt() function
 * for parsing command line arguments. It supports short options (single character
 * preceded by '-') with optional arguments. The function processes options
 * sequentially and maintains state between calls.
 *
 * Parameters:
 *   nargc - Number of arguments in argv array (typically argc from main)
 *   nargv - Array of argument strings (typically argv from main)
 *   ostr  - Option string specifying valid option characters (must not be NULL)
 *           Format: "abc:d::" where:
 *           - 'a', 'b' are options without arguments
 *           - 'c:' is option requiring an argument
 *           - 'd::' is option with optional argument (non-standard)
 *
 * Returns:
 *   Option character (0-255) when valid option found
 *   EOF (-1) when no more options to process
 *   '?' when invalid option character encountered
 *
 * Side Effects:
 *   - Sets optarg to argument string for options requiring arguments
 *   - Sets optind to index of next argv element to process
 *   - Sets optopt to the option character that caused error
 *   - May write error messages to stderr if opterr is non-zero
 *   - Maintains internal static state between calls
 *
 * Global Variables Used:
 *   - optarg: Set to option argument or NULL
 *   - optind: Index of next argument to process (starts at 1)
 *   - optopt: Option character that caused error
 *   - opterr: If non-zero, error messages printed to stderr
 *
 * Notes:
 *   - Stops processing at first non-option argument or "--"
 *   - Options can be combined: "-abc" same as "-a -b -c"
 *   - Option arguments can be adjacent ("-oarg") or separate ("-o arg")
 *   - Thread safety: Not thread-safe due to static variables
 *   - Standard behavior: Follows POSIX getopt() conventions
 *   - Error handling: Invalid options return '?' and set optopt
 */
getopt(nargc, nargv, ostr)
int	  nargc;
char	**nargv;
char	 *ostr;
{
	static char	 *place = EMSG;	/* option letter processing	*/
	register char	 *oli;		/* option letter list index	*/

	if (!*place)			/* update scanning pointer	*/
	{
	if (optind >= nargc || *(place = nargv[optind]) != '-' || !*++place)
		return(EOF);
	if (*place == '-')	/* found "--"	*/
	{
		optind++;
		return(EOF);
	}
	}
	/* option letter okay? */
	if ((optopt = *place++) == ':'
	|| (oli = index(ostr, optopt)) == NULL)
	{
	if (!*place)
		optind++;
	tell(": illegal option -- ");
	goto Bad;
	}
	if(*++oli != ':')	/* don't need argument	*/
	{
	optarg = NULL;
	if (!*place)
		optind++;
	}
	else	/* need an argument*/
	{
	if (*place)
		optarg = place;	/* no white space */
	else
		if (nargc <= ++optind)
		{
		place = EMSG;
		tell(": option requires an argument -- ");
		goto Bad;
		}
		else
		optarg = nargv[optind];	/* white space	*/
	place = EMSG;
	optind++;
	}
	return(optopt);	/* dump back option letter	*/
Bad:
	return('?');
}
