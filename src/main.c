#include <assert.h>
#include <stdio.h>
#include "tk.h"

int main() {
  tk_info("main starts...");

  printf("\n========== TEST DATESTAMPS =========\n");
  char stamp1[128];
  tk_format_date( *&stamp1, sizeof(stamp1), NULL, TK_YYYYMMDD_HHMMSSZ );
  printf( "stamp 1 %s\n", stamp1 );
  tk_format_date( *&stamp1, sizeof(stamp1), NULL, TK_DD_MMM_YYYY_HH_MM_SS);
  printf( "stamp 1 %s\n", stamp1 );

  //
  printf("\n========== TEST LOGGIN WITH DEFAULTS =========\n");
  printf( "Logging DEBUG ... %d\n", tk_log_is_level(TK_DEBUG) );
  printf( "Logging INFO .... %d\n", tk_log_is_level(TK_INFO ) );
  printf( "Logging WARN .... %d\n", tk_log_is_level(TK_WARN ) );
  printf( "Logging ERROR ... %d\n", tk_log_is_level(TK_ERROR) );
  printf( "Logging FAIL .... %d\n", tk_log_is_level(TK_FAIL ) );
  tk_debug( "debug message..." );
  tk_log( TK_DEBUG, "another debug message..." );
  tk_info( "info message..." );
  tk_log( TK_INFO, "another info message..." );
  tk_warn( "warn message..." );
  tk_log( TK_WARN, "another warn message..." );
  tk_error( "error message..." );
  tk_log( TK_ERROR, "another error message..." );
  tk_fail( "fail message..." );
  tk_log( TK_FAIL, "another fail message..." );

  //
  printf("\n========== TEST LOGGIN SET TO DEBUG =========\n");
  tk_log_set_level( TK_DEBUG );
  printf( "Logging DEBUG ... %d\n", tk_log_is_level(TK_DEBUG) );
  printf( "Logging INFO .... %d\n", tk_log_is_level(TK_INFO ) );
  printf( "Logging WARN .... %d\n", tk_log_is_level(TK_WARN ) );
  printf( "Logging ERROR ... %d\n", tk_log_is_level(TK_ERROR) );
  printf( "Logging FAIL .... %d\n", tk_log_is_level(TK_FAIL ) );
  tk_debug( "debug message..." );
  tk_log( TK_DEBUG, "another debug message..." );
  tk_info( "info message..." );
  tk_log( TK_INFO, "another info message..." );
  tk_warn( "warn message..." );
  tk_log( TK_WARN, "another warn message..." );
  tk_error( "error message..." );
  tk_log( TK_ERROR, "another error message..." );
  tk_fail( "fail message..." );
  tk_log( TK_FAIL, "another fail message..." );

  printf("\n========== TEST LOGGIN SET TO INFO =========\n");
  tk_log_set_level( TK_INFO );
  printf( "Logging DEBUG ... %d\n", tk_log_is_level(TK_DEBUG) );
  printf( "Logging INFO .... %d\n", tk_log_is_level(TK_INFO ) );
  printf( "Logging WARN .... %d\n", tk_log_is_level(TK_WARN ) );
  printf( "Logging ERROR ... %d\n", tk_log_is_level(TK_ERROR) );
  printf( "Logging FAIL .... %d\n", tk_log_is_level(TK_FAIL ) );
  tk_debug( "debug message..." );
  tk_log( TK_DEBUG, "another debug message..." );
  tk_info( "info message..." );
  tk_log( TK_INFO, "another info message..." );
  tk_warn( "warn message..." );
  tk_log( TK_WARN, "another warn message..." );
  tk_error( "error message..." );
  tk_log( TK_ERROR, "another error message..." );
  tk_fail( "fail message..." );
  tk_log( TK_FAIL, "another fail message..." );

  printf("\n========== TEST LOGGIN SET TO ERROR =========\n");
  tk_log_set_level( TK_ERROR );
  printf( "Logging DEBUG ... %d\n", tk_log_is_level(TK_DEBUG) );
  printf( "Logging INFO .... %d\n", tk_log_is_level(TK_INFO ) );
  printf( "Logging WARN .... %d\n", tk_log_is_level(TK_WARN ) );
  printf( "Logging ERROR ... %d\n", tk_log_is_level(TK_ERROR) );
  printf( "Logging FAIL .... %d\n", tk_log_is_level(TK_FAIL ) );
  tk_debug( "debug message..." );
  tk_log( TK_DEBUG, "another debug message..." );
  tk_info( "info message..." );
  tk_log( TK_INFO, "another info message..." );
  tk_warn( "warn message..." );
  tk_log( TK_WARN, "another warn message..." );
  tk_error( "error message..." );
  tk_log( TK_ERROR, "another error message..." );
  tk_fail( "fail message..." );
  tk_log( TK_FAIL, "another fail message..." );

  tk_log_set_level( TK_DEBUG );
  printf("\n========== TESTING VARIOUS OPTIONAL ARG INPUTS TO LOG ==========\n");
  tk_log( TK_INFO, "here is a static string ..." );
  tk_log( TK_INFO, "here is a %s...", "dynamic string" );
  tk_log( TK_INFO, "here is a %d", 100 );
  tk_log( TK_INFO, "here is a %s %d", "dynamic string", 100 );

  printf("\n========== TEST FILE GET APP PATH =========\n");
  char *app_path[512];
  tk_file_get_app_path( app_path, 512 );
  tk_info( "app file path is '%s'.", app_path );

  printf("\n========== TESTING CONFIG ==========\n");
  tk_config_read();

  printf("\n========== TESTING CONFIG FETCHING ==========\n");
  //
  double test_double_1 = tk_config_float( "FLOAT_NON_EXISTANT", 123.456789 );
  tk_info( "FLOAT_NON_EXISTANT = %f", test_double_1 );
  assert( test_double_1==123.456789 );
  //
  double test_double_2 = tk_config_float( "FLOAT_987654321", 123.456789 );
  tk_info( "FLOAT_987654321 = %f", test_double_2 );
  assert( test_double_2==987.654321 );
  //
  long test_long_1 = tk_config_int( "INT_NON_EXISTANT", 12345 );
  tk_debug("yo A");
  tk_info( "INT_NON_EXISTANT = %d", test_long_1 );
  assert( test_long_1==12345 );
  //
  long test_long_2 = tk_config_int( "INT_9876", 12345 );
  tk_info( "INT_9876 = %d", test_long_2 );
  assert( test_long_2==9876 );
  //

  return 0;
}
