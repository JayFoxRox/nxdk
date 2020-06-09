#include <stdint.h>
#include <stdio.h>
#if defined(NXDK)
#include <xboxkrnl/xboxkrnl.h>
#else
#define KeQueryPerformanceCounter() 0
#define KeQueryPerformanceFrequency() 1000
#endif

static uint8_t* buffer = NULL;
static uint8_t* cursor = NULL;
#define BUFFER_LEN (16*1024*1024) // 16 MiB of buffer

#define LOG_LEN 100000
typedef struct {
  char* text;
  unsigned long long buffer;
  unsigned long long start;
  unsigned long long end;
} LogEntry;
static LogEntry* log_entries;
static unsigned int log_cursor = 0;
static unsigned long long log_start = 0;

unsigned int add_log(const char* text) {
  unsigned long long samples = (cursor - buffer) / 4;
  unsigned long long time = KeQueryPerformanceCounter() - log_start;
  log_entries[log_cursor].text = strdup(text);
  log_entries[log_cursor].buffer = (samples * KeQueryPerformanceFrequency()) / 48000ULL;
  log_entries[log_cursor].start = time;
  log_entries[log_cursor].end = time;
  return log_cursor++;
}

void set_log_end(unsigned int index) {
  log_entries[index].end = KeQueryPerformanceCounter() - log_start;
}

void set_log_duration(unsigned int index, unsigned int samples) {
  log_entries[index].end = log_entries[index].start + (samples * KeQueryPerformanceFrequency()) / 48000ULL;
}

void add_buffer(const void* data, size_t size) {
  memcpy(cursor, data, size);
  cursor += size;
}

static void init() {
  buffer = malloc(BUFFER_LEN);
  cursor = buffer;

  log_entries = malloc(LOG_LEN * sizeof(LogEntry));

  log_start = KeQueryPerformanceCounter();
}

static void _dump_time(FILE* f, unsigned long long time) {
  unsigned long long t = (time * 1000000ULL) / KeQueryPerformanceFrequency();
  fprintf(f, "%u.%06u\t", (unsigned int)(t / 1000000ULL), (unsigned int)(t % 1000000ULL));
}

static void dump() {
  FILE* f;

  f = fopen("D:\\buffer.bin", "wb");
  assert(f != NULL);
  fwrite(buffer, 1, cursor - buffer, f);
  fclose(f);
  
  f = fopen("D:\\labels.txt", "wb");
  assert(f != NULL); 
  for(int i = 0; i < log_cursor; i++) {
    if (log_entries[i].text) {
      _dump_time(f, log_entries[i].start);
      _dump_time(f, log_entries[i].end);
      fprintf(f, "%s\n", log_entries[i].text);
    }
  }
  fclose(f);

  f = fopen("D:\\delta.txt", "wb");
  assert(f != NULL); 
  for(int i = 0; i < log_cursor; i++) {
    if (log_entries[i].text) {
      _dump_time(f, log_entries[i].buffer);
      _dump_time(f, log_entries[i].start);
      fprintf(f, "delta: %s\n", log_entries[i].text);
    }
  }
  fclose(f);
}
