#include<stdlib.h>
#include<stdio.h>
#include<pulse/simple.h>
#include<pulse/error.h>

struct riff_t {
  char id[4];
  uint32_t size;
  char type[4];
};

struct format_chunk_t {
  char id[4];
  uint32_t size;
  uint16_t format;
  uint16_t channel;
  uint32_t samples_per_sec;
  uint32_t bytes_per_sec;
  uint16_t block_size;
  uint16_t bits_per_sample;
};

struct data_chunk_t {
  char id[4];
  uint32_t size;
  char *data;
};
typedef struct {
  struct riff_t riff;
  struct format_chunk_t format_chunk;
  struct data_chunk_t data_chunk;
} __attribute__ ((packed)) wav_header_t;

int main() {
  // open wav file
  char filename[255] = "/home/wakuto/src/j5prog/network/holst_test.wav";
  FILE *wavefile = fopen(filename, "rb");

  if(wavefile == NULL) {
    fprintf(stderr, "Could not open file: %s\n", filename);
    exit(1);
  }

  // read wav header
  wav_header_t wav_header;
  uint32_t header_size = sizeof(wav_header_t) - sizeof(char*);
  uint32_t n_read = fread((void*)&wav_header, header_size, 1, wavefile);
  if(n_read != 1) {
    fprintf(stderr, "Error occured while reading wave header.\n");
    exit(1);
  }

  pa_simple *s;
  pa_sample_spec ss;
  ss.format = wav_header.format_chunk.format;//PA_SAMPLE_S16NE;
  ss.channels = wav_header.format_chunk.channel;
  ss.rate = wav_header.format_chunk.samples_per_sec;

  s = pa_simple_new(NULL,
    "Fooapp",
    PA_STREAM_PLAYBACK,
    NULL,
    "Music",
    &ss,
    NULL,
    NULL,
    NULL
  );
  if(s == NULL)
    exit(1);
  printf("OK!\n");

  // TODO: pa_simple_write()

  pa_simple_free(s);
  return 0;
}
