#include "uldecode.h"
#include <stdio.h>
int main(int argc, char** argv) {
  FILE *input, *output;
  const uldecode_t* dec;
  ulencode_func_t encoder;
  uldecode_func_t decoder;
  uldecode_state_t _decoder_state = ULDECODE_STATE_INIT, _encoder_state = ULDECODE_STATE_INIT;
  uldecode_u32_t _db[ULDECODE_RETURN_MAX];
  uldecode_u8_t _eb[ULENCODE_RETURN_MAX];
  int _di = 0;
  int _dr = 0, _er = 0;
  int c;
  unsigned long read = 0;
  int err = 1;

  if(argc < 4) {
    const uldecode_t* const* iter;
    const char* const* p;
    unsigned n = 0;

    printf(
      "Usage: %s <input_file> <output_file> <input_encoding> [output_encoding=\"utf-8\"]\n"
      "\nAvailable encodings:\n",
      argv[0]
    );

    for(iter = uldecode_get_lists(); (dec = *iter) != NULL; ++iter) {
      printf("%s", dec->name);
      p = dec->labels;
      n = 0;
      while(*p != NULL) {
        if(n++ % 4 == 0)
          printf("\n\t");
        printf("%s\t\t", *(p++));
      }
      printf("\n");
    }
    return 1;
  }

  input = fopen(argv[1], "rb");
  output = fopen(argv[2], "wb");
  if(input == NULL || output == NULL) {
    fprintf(stderr, "error: could not open files\n");
    goto done;
  }

  dec = uldecode_get(argv[3]);
  if(dec == NULL) {
    fprintf(stderr, "error: invalid encoding: %s\n", argv[3]);
    goto done;
  }
  decoder = dec->decode;

  dec = uldecode_get(argc < 5 ? "UTF-8" : argv[4]);
  if(dec == NULL) {
    fprintf(stderr, "error: invalid encoding: %s\n", argv[4]);
    goto done;
  }
  encoder = dec->encode;


  while((c = fgetc(input)) != EOF) {
    ++read;
    _dr = decoder(_db, c, &_decoder_state);
    if(_dr < 0) {
      fprintf(stderr, "error: invalid character 0x%X at %lu\n", c, read);
      goto done;
    }
    for(_di = 0; _di < _dr; ++_di) {
      _er = encoder(_eb, _db[_di], &_encoder_state);
      if(_er < 0) {
        fprintf(stderr, "error: cannot encode U+%X after decoding at %lu\n", _db[_di], read);
        goto done;
      }
      fwrite(_eb, 1, _er, output);
    }
  }

  _dr = decoder(_db, ULDECODE_EOF, &_decoder_state);
  if(_dr < 0) {
    fprintf(stderr, "error: invalid EOF\n");
    goto done;
  }
  for(_di = 0; _di < _dr; ++_di) {
    _er = encoder(_eb, _db[_di], &_encoder_state);
    if(_er < 0) {
      fprintf(stderr, "error: cannot encode U+%X after decoding at %lu\n", _db[_di], read);
      goto done;
    }
    fwrite(_eb, 1, _er, output);
  }

  _er = encoder(_eb, ULENCODE_EOF, &_encoder_state);
  if(_er < 0) {
    fprintf(stderr, "error: cannot encode EOF after decoding at %lu\n", read);
    goto done;
  }
  fwrite(_eb, 1, _er, output);

  err = 0;
done:
  fclose(input);
  fclose(output);
  return err;
}
