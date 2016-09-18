
/**
  * This program indexes  
  */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <err.h>
#include <assert.h>

/***************************************************************************
 * externs
 */


/***************************************************************************
 * Globals & statics
 */

#define global
static time_t START;
       int  opt_verbosity = 0;
global bool opt_defer_postproc = false;
global bool opt_defer_removal  = false;
global char env_sample_prefix  = 0;
#undef  global

static void *_module = NULL;
struct seqref_parser *_parser = NULL;

/***************************************************************************
 * Functions
 */

static void _rstrip( char *buf, int c ) {
	int i = strlen(buf);
	while( i-- > 0 && buf[i] == c ) buf[i] = 0;
}

/***************************************************************************
 */

static const char *USAGE = 
"%s -p <shared object> [ options ] <root path>\n"
"Options:\n"
"  --parser | -p  shared object containing a parser\n"
"  --query  | -q  find references overlapping the given range\n"
"  --update | -u  add the sample data specified in the named file\n"
"                 to the index\n"
"  --nopp   | -P  skip post-processing\n"
"Output options:\n"
"  --verbosity | -v verbosity [%d]\n"
"Notes:\n"
#ifdef _DEBUG
"This is a _DEBUG build.\n"
#endif
#ifdef HAVE_LOG2_TRANSFORM
"This version log2 transforms input"
#endif
"Written by rkramer@systemsbiology.org.\n";


static void _print_usage( const char *exename, FILE *fp ) {
	fprintf( fp, USAGE, 
		exename,
		opt_verbosity );
#ifdef _DEBUG
	// Just for reference...
	fprintf( fp, "sizeof(__time_t) = %ld, sizeof(long) = %ld\n",
		sizeof(__time_t),
		sizeof(long) );
#endif
}


int main( int argc, char *argv[] ) {

	const bool unbiased = false;
	double m[4];
	double n = 0;
	int i;
	char *line = NULL;
	size_t llen = 0;

	/**
	  * Mandatory initializations.
	  */

	/**
	 * Argument checks
	 */

	if( argc < 2 ) {
		//_print_usage( argv[0], stderr );
		//exit(-1);
	}

	do {
		static const char *CHAR_OPTIONS 
			= "";
		static struct option LONG_OPTIONS[] = {

			{ NULL,        0,0, 0 }
		};

		int opt_offset = 0;
		const int c = getopt_long( argc, argv, CHAR_OPTIONS, LONG_OPTIONS, &opt_offset );
		switch (c) {


		case -1: // ...signals no more options.
			break;
		default:
			printf ("error: unknown option: %c\n", c );
			exit(-1);
		}
		if( -1 == c ) break;

	} while( true );

	memset( m, sizeof(m), 0 );

	if( getline( &line, &llen, stdin ) > 0 ) {
		m[0] = (double)strtol( line, NULL, 0 );
#ifdef HAVE_LOG2_TRANSFORM
		m[0] = log2( 1.0 + m[0] );
#endif
		n    = 1.0;
		while( getline( &line, &llen, stdin ) > 0 ) {
			const double x
#ifdef HAVE_LOG2_TRANSFORM
				= log2( 1.0 + (double)strtol( line, NULL, 0 ) );
#else
				= (double)strtol( line, NULL, 0 );
#endif
			const double d   = x - m[0];
			const double dd  = d*d;
			const double ddd = dd*d;
			const double np1 = n+1.0;
			// 4th central moment
			m[3] += (
				( n*(n*n-n+1)*dd*dd / ( np1 * np1 * np1 ) )
				+ ( 6.0*m[1]*dd/ ( np1 *np1) )
				- ( 4.0*m[2]*d / ( np1 ) )
					);

			// 3rd central moment
			m[2] += ( ( n*(n-1)*ddd/( np1 * np1 ) )
					- ( 3.0*m[1]*d/ np1           )
					);
			// 2nd central moment
			m[1] += ( (n*(dd)) /  np1  );
			// 1st moment (mean)
			m[0] += (    ( d ) /  np1  );
			n += 1.0;
		}
	}

	if( unbiased ) {

		const double BV
			= m[1] / n;
		m[2]  = ( ( m[2] / n ) / pow(BV,1.5) ); // BIASED skewness
		m[2] *= ( sqrt(n*(n-1)) / (n-2) );  // UNbiased skewness

		m[1] /= (n-1.0);                    // UNbiased variance

	} else {

		m[1] /= n;
		m[2] = ( ( m[2] / n ) / pow(m[1],1.5) );
		m[3] = ( ( m[3] / n ) / pow(m[1],2.0) );
	}

	printf( "%.6e\t%.6e\t%.6e\t%.6e\t%.0f\n", 
		m[0],
		m[1],
		m[2],
		m[3],
		n );

	return EXIT_SUCCESS;
}

