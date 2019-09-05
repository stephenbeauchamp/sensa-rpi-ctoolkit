//
// TOOLKIT FOR c
//   HELPER FUNCTIONS AND FRAMEWORKS FOR
//   - LOGGING
//   - READING CONFIG
//   - FORMATING DATES
//

#ifndef _tk_h_
  #define _tk_h_

  #define TK_VERSION "0.1.0"

  #include <time.h>

  //
  // TIMESTAMP - DATE FORMATING
  //

  typedef enum TK_TIME_STAMP_FORMAT {
    TK_DD_MMM_YYYY_HH_MM_SS, // 31-JAN-2008 17:45:01 - DD-MMM-YYYY HH:MM:SS
    TK_YYYYMMDD_HHMMSSZ,     // 20080131.174501+1000 - YYYYMMDD.HHMMSSZZZZZ
  } TK_TIME_STAMP_FORMAT;

  void tk_format_date( char *stamp, const size_t stamp_char_max, const time_t date, const TK_TIME_STAMP_FORMAT format );

  //
  // LOGGING
  //

  typedef enum TK_LOG_LEVEL {
    TK_DEBUG, TK_INFO, TK_WARN, TK_ERROR, TK_FAIL
  } TK_LOG_LEVEL;

  void tk_log_set_level( TK_LOG_LEVEL log_level );

  int tk_log_is_level( TK_LOG_LEVEL log_level );

  void tk_log( TK_LOG_LEVEL log_level, const char *format, ... );

  void tk_debug( const char *format, ...  );

  void tk_info( const char *format, ...  );

  void tk_warn( const char *format, ... );

  void tk_error( const char *format, ...  );

  void tk_fail( const char *format, ...  );

  //
  // FILE STUFF AND TEXT FILE READ WRITE
  //

  void tk_file_get_app_path(char *app_path, int app_path_char_max );

  void tk_file_write_txt( char *path, char *txt );

  //
  // CONFIG
  //

  void tk_config_read();

  void tk_config_string( char *val, char *key, char *default_value );

#endif /* _tk_h_ */
