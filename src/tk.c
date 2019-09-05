#include "tk.h"

#include <libgen.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <hiredis.h>

// KERNEL SYTLE GUIDE - REF: https://www.kernel.org/doc/html/v4.17/process/coding-style.html
// STYLE GUIDE - REF: https://www.doc.ic.ac.uk/lab/cplus/cstyle.html

//
// STRING
//

void tk_string_strip_nonascii( char *str, int str_max_chars ) {
  char tmp[str_max_chars];
  strcpy( tmp, str );
  int j = 0;
  for( size_t i=0; i<str_max_chars; i++ ) {
    char c = tmp[i];
    if ( c>=32 && c<=126 ) { // WE'VE FOUND AN PRINTABLE ASCII CHAR
      str[j++] = c;
    }
    if ( c == '\0' ) { // WE'RE FOUND THE END OF THE STRING
      str[j++] = c;
      i = str_max_chars; // MOVE THE INDEX PAST THE END OF THE MEMORY SPACE
    }
  }
}

//
// DATE AND TIME
//

// FILL A STRING stamp WITH A TIMESTAMP OF FORMAT format (OR DEFAULT DD-MMM-YYYY HH:MM:SS).
// A SPECIFIC DATE MAY BE SUPPLIED date OR THE CURRENT TIME IS USED.
void tk_format_date( char *stamp, const size_t stamp_char_max, const time_t date, const TK_TIME_STAMP_FORMAT format ) {
  time_t tmp = date;
  if ( tmp == NULL ) {
    tmp = time(NULL); // NOW
  }
  char timeinfo_format[32];
  switch ( format ) { //REF: http://man7.org/linux/man-pages/man3/strftime.3.html
    case TK_YYYYMMDD_HHMMSSZ:
      strcpy(timeinfo_format, "%Y%m%d.%H%M%S%z");
      break;
    default:
      strcpy(timeinfo_format, "%d-%b-%Y %H:%M:%S"); // TK_DD_MMM_YYYY_HH_MM_SS
      break;
  }
  strftime( stamp, stamp_char_max, timeinfo_format, localtime( &tmp ) );
}

//
// LOGGING
//

static const char *tk_log_level_label[] = {
  "DEBUG", "INFO", "WARN", "ERROR", "FAULT"
};

static enum TK_LOG_LEVEL tk_log_level = TK_INFO;

void tk_log_set_level( TK_LOG_LEVEL log_level ) {
  tk_log_level = log_level;
}

int tk_log_is_level( TK_LOG_LEVEL log_level ) {
  if ( tk_log_level <= log_level ) {
    return 1; // TRUE, LOG MESSAGES OF THIS LEVEL
  }
  return 0; // FALSE, DON'T LOG MESSAGES OF THIS LEVEL
}

void tk_logger( TK_LOG_LEVEL log_level, const char *format, va_list args ) {
  // TIMESTAMP THE MESSAGE WITH THE CURREN TIME
  time_t time_now = time(NULL);
  struct tm *timeinfo_now = localtime( &time_now );
  char timestamp[21];
  strftime( timestamp, sizeof(timestamp), "%Y%m%d.%H%M%S%z", timeinfo_now );
  // CREATE THE LOG MESSAGE
  char msg[1024];
  vsnprintf(msg, sizeof(msg), format, args);
  // NOW DO THE ACTUAL LOGGING
  printf( "%s %s: %s\n", timestamp, tk_log_level_label[log_level], msg );
}

void tk_log( TK_LOG_LEVEL log_level, const char *format, ... ) {
  if ( tk_log_is_level(log_level) == 0 ) {
    return; // LOGGING SET TO A HIGHER LEVEL, DON'T LOG THIS
  }
  va_list args;
  va_start( args, format );
  tk_logger( log_level, format, args );
  va_end(args);
}

void tk_debug( const char *format, ...  ) {
  if ( tk_log_is_level(TK_DEBUG) == 0 ) {
    return; // LOGGING SET TO A HIGHER LEVEL, DON'T LOG THIS
  }
  va_list args;
  va_start( args, format );
  tk_logger( TK_DEBUG, format, args );
  va_end(args);
}

void tk_info( const char *format, ...  ) {
  if ( tk_log_is_level(TK_INFO) == 0 ) {
    return; // LOGGING SET TO A HIGHER LEVEL, DON'T LOG THIS
  }
  va_list args;
  va_start( args, format );
  tk_logger( TK_INFO, format, args );
  va_end(args);
}

void tk_warn( const char *format, ...  ) {
  if ( tk_log_is_level(TK_WARN) == 0 ) {
    return; // LOGGING SET TO A HIGHER LEVEL, DON'T LOG THIS
  }
  va_list args;
  va_start( args, format );
  tk_logger( TK_WARN, format, args );
  va_end(args);
}

void tk_error( const char *format, ... ) {
  if ( tk_log_is_level(TK_ERROR) == 0 ) {
    return; // LOGGING SET TO A HIGHER LEVEL, DON'T LOG THIS
  }
  va_list args;
  va_start( args, format );
  tk_logger( TK_ERROR, format, args );
  va_end(args);
}

void tk_fail( const char *format, ... ) {
  if ( tk_log_is_level(TK_FAIL) == 0 ) {
    return; // LOGGING SET TO A HIGHER LEVEL, DON'T LOG THIS
  }
  va_list args;
  va_start( args, format );
  tk_logger( TK_FAIL, format, args );
  va_end(args);
}

//
// FILE STUFF AND TEXT FILE READER / WRITER
//

void tk_file_get_app_path(char *app_path, int app_path_char_max ) {
  size_t app_path_l = readlink( "/proc/self/exe", app_path, app_path_char_max ); // THIS GETS THE FULL APP PATH
  app_path[ app_path_l ] = '\0';
}

void tk_file_write_txt( char *path, char *txt ) {
  FILE *wtr;
  wtr = fopen( path, "w" );
  if ( wtr == NULL ) {
    tk_fail( "cannot open file for writing '%s'.", path );
    exit(1);
  }
  fputs( txt, wtr );
  fclose( wtr );
}

//
// APPLICATION CONFIG
//

struct tk_config_s {
  char key[128];
  char value_string[128];
  int is_int;
  long value_int;
  int is_float;
  double value_float;
  int is_date;
  time_t value_date;
};

struct tk_config_s tk_config_a[128];

int tk_config_count = 0;

void tk_config_read() {
  int ary_l = sizeof( tk_config_a )/sizeof( tk_config_a[0] );
  for ( int i=0; i<ary_l; i++ ) { // INIT ALL CONFIG ARRAYS
    strcpy( tk_config_a[i].key, "");
    strcpy( tk_config_a[i].value_string, "");
    tk_config_a[i].is_int = 0;
    tk_config_a[i].value_int = 0;
    tk_config_a[i].is_float = 0;
    tk_config_a[i].value_float = 0.0;
    tk_config_a[i].is_date = 0;
    tk_config_a[i].value_date = time(0);
  }
  //
  char config_path[ 512 ];
  tk_file_get_app_path( config_path, 512 );
  strcat( config_path, ".config" );
  tk_info( "loading config '%s'", config_path);
  FILE *rdr;
  if ( ( rdr = fopen( config_path, "r" ) ) == NULL ) {
    tk_warn( "couldn't open or no read rights on config file '%s'.", config_path );
    return;
  }
  int line_count = 0;
  char *line = NULL;
  size_t line_size = 0;
  ssize_t read;
  while( ( read = getline( &line, &line_size, rdr ) ) != -1 ) {
    line_count++;
    int line_l = strlen( line );
    if ( line_l>0 && line[ line_l-1 ] == '\n' ) { // TRIM TRAILING NEW LINE IF PRESENT
      line[ line_l-1 ] = '\0';
    }
    //tk_debug( "%d : %s", line_count, line );
    int pos_hash = -1;  // MARKS THE POSITION OF A COMMENT STARTING (#)
    int pos_key_1 = -1; // MARKS THE START POITION OF A KEY ( AS IN KEY = VALUE )
    int pos_key_2 = -1; // MARKS THE END POSITION OF A KEY
    int pos_eql = -1;   // MARKS THE POSITION OF AN EQUALS ( AS IN KEY = VALUE )
    int pos_val_1 = -1; // MARKS THE START POSITION OF A VALUE ( AS IN KEY = VALUE )
    int pos_val_2 = -1; // MARKS THE END POSITION OF A VALUE
    int str_l = strlen( line );
    for ( size_t i=0; i<str_l; i++ ) {
      char c = line[i];
      if ( isspace( c ) ) {
        continue;
      }
      if ( c == '#' ) {
        if ( pos_key_1!=-1 ) {
          tk_fail("Error reading config line %d. Comment markers (#) can only appear as the first character in a line. ", line_count);
          exit(1);
        }
        pos_hash = i;
        i = str_l; // NO NEED TO KEEP PROCESSING, THIS IS A COMMENT
      }
      if ( pos_eql==-1 && c=='=' ) {
        pos_eql = i;
        continue; // WE'VE FOUND THE EQUALS
      }
      if ( c>=32 && c<=126 ) {
        if ( pos_eql!=-1 ) {
          if ( pos_val_1==-1 ) {
            pos_val_1 = i;
          }
          pos_val_2 = i+1;
        } else {
          if ( pos_key_1==-1) {
            pos_key_1 = i;
          }
          if ( pos_key_2!=-1 && pos_key_2<i ) {
            tk_fail("Error reading config line %d. The key must be a single word.", line_count );
            exit(1);
          }
          pos_key_2 = i+1;
        }
      }
    }
    if ( pos_hash==-1 && pos_key_1==-1 ) {
      continue; // SKIP THIS LINE IT IS EMPTY OR JUST WHITESPACE
    }
    if ( pos_hash!=-1 ) {
      continue; // SKIP THIS LINE, IT'S A COMMENT
    }
    if ( pos_key_1!=-1 && pos_eql==-1 ) {
      tk_fail("Error reading config line %d. There is no equals sign on this line.", line_count );
      exit(1);
    }
    //
    int key_l = pos_key_2 - pos_key_1;
    char key[ key_l + 1 ];
    size_t j = 0;
    for ( j=0 ; j<key_l; j++ ) {
      key[j] = line[ ( pos_key_1 + j ) ];
    }
    key[j] = '\0';
    //
    int val_l = pos_val_2 - pos_val_1;
    char val[ val_l + 1 ];
    for ( j=0 ; j<val_l; j++ ) {
      val[j] = line[ ( pos_val_1 + j ) ];
    }
    val[j] = '\0';
    if ( tk_config_count>=128 ) {
      tk_fail( "Error reading config line %d. You can only load 128 config settings.", line_count );
      exit(1);
    }
    //
    time_t value_date = time(NULL);
    long value_int = 0;
    double value_float = 0.0;
    // CREATE CONFIG STRUCT
    strcpy( tk_config_a[tk_config_count].key, key );
    strcpy( tk_config_a[tk_config_count].value_string, val );
    tk_config_a[tk_config_count].is_int = tk_is_string_a_int( val, &value_int );
    tk_config_a[tk_config_count].value_int = value_int;
    tk_config_a[tk_config_count].is_float = tk_is_string_a_float( val, &value_float );
    tk_config_a[tk_config_count].value_float = value_float;
    tk_config_a[tk_config_count].is_date = tk_is_string_a_date( val, &value_date );
    tk_config_a[tk_config_count].value_date = value_date;
    tk_config_count++;
  }
  fclose( rdr );
  if ( line ) {
    free( line );
  }
  // CHECK FOR LOG_LEVEL IN THE JUST LOADED SETTINGS.
  tk_log_set_level( TK_DEBUG );
  // DUMP
  if ( tk_log_is_level( TK_DEBUG ) ) {
    for ( int i=0; i<tk_config_count; i++ ) {
      time_t stamp_t = tk_config_a[i].value_date;
      int stamp_max_char = 64;
      char stamp[stamp_max_char];
      tk_format_date( stamp, stamp_max_char, stamp_t, TK_DD_MMM_YYYY_HH_MM_SS );
      tk_debug(
        "  %s = %s\t[%ld, %f, %s]",
        tk_config_a[i].key,
        tk_config_a[i].value_string,
        tk_config_a[i].value_int,
        tk_config_a[i].value_float,
        stamp
      );
    }
  }
}

void tk_config_string( char *val, char *key, char *default_value ) {
  for ( int i=0; i<tk_config_count; i++ ) {
    if ( tk_config_a[i].key == key ) {
      strcpy( val, tk_config_a[i].value_string ); // KEY FOUND IN CONFIG
      return;
    }
  }
  strcpy( val, default_value ); // KEY NOT IN CONFIG, RETURN DEFAULT VALUE
}

long tk_config_int( const char key, const long default_value ) {
  for ( int i=0; i<tk_config_count; i++ ) {
    if ( tk_config_a[i].is_int!=0 && tk_config_a[i].key == key ) {
      return tk_config_a[i].value_int;
    }
  }
  return default_value;
}

double tk_config_float( const char *key, const double default_value ) {
  for ( int i=0; i<tk_config_count; i++ ) {
    if ( tk_config_a[i].is_float!=0 ) {
      char *a[128];
      char *b[128];
      strcpy( a, tk_config_a[i].key );
      strcpy( b, key );
      if ( *a == *b ) { // (WEIRD WORK AROUND) WHY DOES (*tk_config_a[i].key == *key) ALWASY = TRUE, BUT THIS DOESN'T
        return tk_config_a[i].value_float;
      }
    }
  }
  return default_value;
}

time_t tk_config_date( const char key, const time_t default_value ) {
  for ( int i=0; i<tk_config_count; i++ ) {
    if ( tk_config_a[i].is_date!=0 && tk_config_a[i].key == key ) {
      return tk_config_a[i].value_date;
    }
  }
  return default_value;
}

//
// TYPE CONVERSIONS
//

int tk_is_string_a_int( const char *s, long *l ) {
  int s_l = strlen( s );
  if ( s_l==0 ) {
    return 0;
  }
  for( size_t i=0; i<s_l; i++ ) {
    char c = s[i];
    if ( i==0 && ( c=='+' || c=='-' ) ) {
      continue;
    }
    if ( c>='0' && c<='9' ) {
      continue;
    }
    return 0;
  }
  *l = strtol( s, NULL, 10 );
  return 1;
}

int tk_is_string_a_float( const char *s, double *d ) {
  int s_l = strlen(s);
  if ( s_l==0 ) {
    return 0;
  }
  int point_count = 0;
  for( size_t i=0; i<s_l; i++ ) {
    char c = s[i];
    if ( i==0 && ( c=='+' || c=='-' ) ) {
      continue;
    }
    if ( c>='0' && c<='9' ) {
      continue;
    }
    if ( c=='.' && point_count++ == 0 ) {
      continue;
    }
    return 0;
  }
  *d = strtod( s, NULL );
  return 1;
}

int tk_is_string_a_date( const char *s, time_t *t ) {
  int s_l = strlen(s);
  if ( s_l!=10 && s_l!=16 && s_l!=19 ) {
    return 0;
  }
  char *tmp[19];
  strcpy( tmp, s );
  if ( strlen( tmp )==10 ) {
    strcat( tmp, " 00:00" );
  }
  if ( strlen( tmp )==16 ) {
    strcat( tmp, ":00" );
  }

  int year=0, month=0, day=0, hour=0, min=0, sec=0;
  if ( sscanf( tmp, "%4d-%2d-%2d %2d:%2d:%2d", &year, &month, &day, &hour, &min, &sec ) == 6 ) {
    //
    if ( month<1 || month>12 || day<0 || day>31 || hour<0 || hour>23 || min<0 || min>59 || sec<0 || sec>59 ) {
      return 0;
    }
    if ( day==31 && ( month==4 || month==6 || month==9 || month==11  ) ) {
      return 0;
    }
    if ( month==2 && day>28 ) {
      if ( day==29 && year%4==0 && ( year%100!=0 || year%400==0) ) {
        // LEAP DAY!
      } else {
        return 0;
      }
    }
    //
    struct tm u = {0};
    u.tm_year = year - 1900;
    u.tm_mon = month -1;
    u.tm_mday = day;
    u.tm_hour = hour;
    u.tm_min = min;
    u.tm_sec = sec;
    time_t result = mktime( &u );
    if ( result == (time_t)-1 ) {
      return 0; // FAILED TO CONVERT STRUCT TM REPRESENTATION OF STRING TO TIME_T
    }
    *t = mktime( &u );
    return 1;
  } else {
    return 0;
  }
}

int tk_is_string_a_hex( const char *s ) {
  int s_l = strlen(s);
  if ( s_l==0 ) {
    return 0;
  }
  if ( s_l<3 ) {
    return 0;
  }
  if ( s[0]!='0' || s[1]!='x' ) {
    return 0;
  }
  for( size_t i=2; i<s_l; i++ ) {
    char c = s[i];
    if ( ( c>='0' && c<='9') || ( c>='a' && c<='f' ) || ( c>='A' && c<='F' ) ) {
      continue;
    }
    return 0;
  }
  return 1;
}

//
// REDIS
//

redisContext redis_conn;
