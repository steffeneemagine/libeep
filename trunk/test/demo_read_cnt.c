// system
#include <stdlib.h>
// libeep
#include <cnt/cnt.h>
#include <cnt/trg.h>
///////////////////////////////////////////////////////////////////////////////
void
handle_file(const char *filename) {
  fprintf(stderr, "handling %s...\n", filename);
  /*********************************
   * variables we need for reading *
   *********************************/
  int i,status;
  FILE   * _libeep_file;            // file access.
  eeg_t  * _libeep_cnt;             // pointer to eeprobe data structure
  sraw_t * _libeep_muxbuf;          // buffer for sample data
  slen_t   trg_offset;
  char   * trg_code;
  /**************
   * initialize *
   **************/
  _libeep_file=fopen(filename, "r");
  if(_libeep_file == NULL) {
    fprintf(stderr, "could not open %s\n", filename);
    return;
  }
  _libeep_cnt=eep_init_from_file(filename, _libeep_file, &status);
  if(status != CNTERR_NONE) {
    fprintf(stderr, "could not initialize %s\n", filename);
  }
  /*******************
   * print some info *
   *******************/
  printf("sampling rate........ %f\n", 1.0 / eep_get_period(_libeep_cnt));
  printf("number of channels... %i\n", eep_get_chanc(_libeep_cnt));
  printf("history.............. %s\n", eep_get_history(_libeep_cnt));
  /******************
   * print channels *
   ******************/
  for(i=0;i<eep_get_chanc(_libeep_cnt);i++) {
    printf("  channel(%i): %s, %s, scaling: %f\n",
      i,
      eep_get_chan_label(_libeep_cnt, i),
      eep_get_chan_unit(_libeep_cnt, i),
      eep_get_chan_scale(_libeep_cnt, i));
  }
  /****************
   * print events *
   ****************/
  for(i=0;i<trg_get_c(eep_get_trg(_libeep_cnt));i++) {
    trg_code=trg_get(eep_get_trg(_libeep_cnt), i, &trg_offset);
    printf("  trg(%i): %li %s\n", i, trg_offset, trg_code);
  }
  /****************
   * print sample *
   ****************/
  // as a demo, read one sample at our favourite position in the file: offset 13
  // first, allocate memory(to hold 1 sample)
  _libeep_muxbuf = (sraw_t*)(malloc(CNTBUF_SIZE(_libeep_cnt, 1)));
  // seek to offset
  eep_seek(_libeep_cnt, DATATYPE_EEG, 13, 0);
  // read sample
  eep_read_sraw(_libeep_cnt, DATATYPE_EEG, _libeep_muxbuf, 1);
  printf("sample 13: ");
  for(i=0;i<eep_get_chanc(_libeep_cnt);i++) {
    printf("%f ", _libeep_muxbuf[i] * eep_get_chan_scale(_libeep_cnt, i));
  }
  printf("\n");
  /***********
   * cleanup *
   ***********/
  eep_free(_libeep_cnt);
  free(_libeep_muxbuf);
  fclose(_libeep_file);
}
///////////////////////////////////////////////////////////////////////////////
int
main(int argc, char **argv) {
  int i;
  for(i=1;i<argc;i++) {
    handle_file(argv[i]);
  }
  return 0;
}
