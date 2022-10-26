#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
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
};
typedef struct {
  struct riff_t riff;
  struct format_chunk_t format_chunk;
  struct data_chunk_t data_chunk;
} __attribute__ ((packed)) wav_header_t;

int main() {
  // open wav file
  char filename[255] = "/home/wakuto/src/j5prog/network/holst_test.wav";
  uint32_t wavefile = open(filename, O_RDONLY);

  if(wavefile == -1) {
    fprintf(stderr, "Could not open file: %s\n", filename);
    exit(1);
  }

  // read wav header
  wav_header_t wav_header;
  uint32_t header_size = sizeof(wav_header_t);
  uint32_t n_read = read(wavefile, (void*)&wav_header, header_size);
  if(n_read != header_size) {
    fprintf(stderr, "Error occured while reading wave header.\n");
    exit(1);
  }

  pa_simple *s;
  pa_sample_spec ss;
  ss.format = PA_SAMPLE_S16NE;
  ss.channels = wav_header.format_chunk.channel;
  ss.rate = wav_header.format_chunk.samples_per_sec;

  s = pa_simple_new(NULL,
    "Fooapp",
    PA_STREAM_PLAYBACK,
    //NULL,
    "alsa_output.pci-0000_00_1f.3.analog-stereo",
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

  char *buffer[BUFSIZ];
  uint32_t n_write;

  uint32_t break_flag = 0;

  while(1) {
    n_read = read(wavefile, buffer, BUFSIZ);
    if(n_read < 0) {
      fprintf(stderr, "Could not read wave file.\n");
      exit(1);
    } else if(n_read == 0) // EOF
      break_flag = 1;


    n_write = pa_simple_write(s, buffer, n_read, NULL);
    if(n_write < 0) {
      fprintf(stderr, "Could not write to pulseaudio.\n");
      exit(1);
    }

    if(break_flag) // EOF
      break;
  }


  pa_simple_free(s);
  return 0;
}
