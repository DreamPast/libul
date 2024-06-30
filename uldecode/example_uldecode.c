#include "uldecode.h"
#include <stdio.h>
int main(int argc, char** argv) {
    FILE* input;
    FILE* output;
    const uldecode_t* dec;
    ulencode_func_t encoder;
    uldecode_func_t decoder;
    uldecode_state_t _decoder_state = ULDECODE_STATE_INIT, _encoder_state = ULDECODE_STATE_INIT;
    uldecode_u32_t _db[ULDECODE_RETURN_MAX]; uldecode_u8_t _eb[ULENCODE_RETURN_MAX];
    int _di = 0;
    int _dr = 0, _er = 0;
    int c;
    unsigned long read = 0;

    if(argc < 5) {
        printf("Usage: %s <input_file> <output_file> <input_encoding> <output_encoding>\n", argv[0]);
        return 1;
    }
    input = fopen(argv[1], "rb"); output = fopen(argv[2], "wb");
    if(input == NULL || output == NULL) {
        fprintf(stderr, "error: could not open files\n");
        goto fallback;
    }

    dec = uldecode_get(argv[3]);
    if(dec == NULL) { fprintf(stderr, "error: invalid encoding: %s\n", argv[3]); goto fallback; }
    decoder = dec->decode;

    dec = uldecode_get(argv[4]);
    if(dec == NULL) { fprintf(stderr, "error: invalid encoding: %s\n", argv[4]); goto fallback; }
    encoder = dec->encode;


    while((c = fgetc(input)) != EOF) {
        _dr = decoder(_db, c, &_decoder_state);
        if(_dr < 0) { fprintf(stderr, "error: invalid character 0x%X at %lu\n", c, read); goto fallback; }
        for(_di = 0; _di < _dr; ++_di) {
            _er = encoder(_eb, _db[_di], &_encoder_state);
            if(_er < 0) {
                fprintf(stderr, "error: cannot encode U+%X after decoding at %lu\n", _db[_di], read); goto fallback;
            }
            fwrite(_eb, 1, _er, output);
        }
    }

    _dr = decoder(_db, ULDECODE_EOF, &_decoder_state);
    if(_dr < 0) { fprintf(stderr, "error: invalid EOF\n"); goto fallback; }
    for(_di = 0; _di < _dr; ++_di) {
        _er = encoder(_eb, _db[_di], &_encoder_state);
        if(_er < 0) {
            fprintf(stderr, "error: cannot encode U+%X after decoding at %lu\n", _db[_di], read); goto fallback;
        }
        fwrite(_eb, 1, _er, output);
    }

    _er = encoder(_eb, ULENCODE_EOF, &_encoder_state);
    if(_er < 0) {
        fprintf(stderr, "error: cannot encode EOF after decoding at %lu\n", read); goto fallback;
    }
    fwrite(_eb, 1, _er, output);

    fclose(input); fclose(output); return 0;

fallback:
    fclose(input); fclose(output); return 1;
}