#include "chibicc.h"
#include <unistd.h>
#include <fcntl.h>

typedef enum {
  FILE_NONE, FILE_C, FILE_ASM, FILE_OBJ, FILE_AR, FILE_DSO,
} FileType;

typedef enum {
  T_EMU6800,T_MIKBUG,T_BM,T_JR100,T_JR200
} MachineType;

StringArray include_paths;
bool opt_fcommon = false;

static FileType opt_x;
static StringArray opt_include;
static bool opt_E;
static bool opt_M;
static bool opt_MD;
static bool opt_MMD;
static bool opt_MP;
static bool opt_S;
static bool opt_c;
static bool opt_cc1;
static bool opt_hash_hash_hash;
static bool opt_static;
static bool opt_shared;
static bool opt_nostartfiles = 0;
static char *opt_MF;
static char *opt_MT;
static char *opt_o;
static uint16_t opt_C = 0;
static uint16_t opt_Z = 0;
static int  opt_v = 0;
static int  opt_lm = 0;
char opt_O = '1';
char opt_g = '0';
MachineType opt_t = T_EMU6800;
bool opt_fbuiltin_memcpy = true;
bool opt_fbuiltin_memset = true;
bool opt_fbuiltin_strcpy = true;

bool opt(char op, char lv)
{
  switch(op) {
  case 'O':
    switch(lv) {
    case 's':
      return opt_O == 's';
    case '0':
      return opt_O == '0';
    case '1':
      return opt_O != 's' && opt_O != '0';
    case '2':
      return opt_O == '2' || opt_O == '3';
    case '3':
      return opt_O == '3';
    default:
      return false;
    }
  case 'g':
    switch(lv) {
    case '0':
      return opt_g == '0';
    case '1':
      return opt_g == '1' || opt_g == '2' || opt_g == '3';
    case '2':
      return opt_g == '2' || opt_g == '3';
    case '3':
      return opt_g == '3';
    default:
      return false;
    }
  }
  assert(op=='O' || op=='g');
  return false;
}

#define copt_path  "/opt/fcc/lib/copt"
#define chibicc_lib_path "/opt/chibicc/lib/"
#define copt_rules "copt.rules"
#define copt_O2_rules "copt_O2.rules"
#define copt_O3_rules "copt_O3.rules"

static StringArray ld_extra_args;
static StringArray std_include_paths;

char *base_file;
static char *output_file;

static StringArray input_paths;
static StringArray tmpfiles;

static void usage(int status) {
  fprintf(stderr, "chibicc [-v][-g][-O][-Os][-D name][-o <path>][-I dir][-tarch][-Zaddr][-Caddr][-nostartfiles]  <file> [-lxx]\n");
  exit(status);
}

static bool take_arg(char *arg) {
  char *x[] = {
    "-o", "-I", "-idirafter", "-include", "-x", "-MF", "-MT", "-Xlinker",
    "-v", "-D", "-O",
  };

  for (int i = 0; i < sizeof(x) / sizeof(*x); i++)
    if (!strcmp(arg, x[i]))
      return true;
  return false;
}

static void add_default_include_paths(char *argv0) {
  // We expect that chibicc-specific include files are installed
  // to ./include relative to argv[0].
  strarray_push(&include_paths, format("%s/include", dirname(strdup(argv0))));
  strarray_push(&include_paths, "/opt/chibicc/include");

  // Add standard include paths.
  strarray_push(&include_paths, "/usr/local/include");
  strarray_push(&include_paths, "/usr/include/x86_64-linux-gnu");
  strarray_push(&include_paths, "/usr/include");

  // Keep a copy of the standard include paths for -MMD option.
  for (int i = 0; i < include_paths.len; i++)
    strarray_push(&std_include_paths, include_paths.data[i]);
}

#if 0
static void define(char *str) {
  char *eq = strchr(str, '=');
  if (eq)
    define_macro(strndup(str, eq - str), eq + 1);
  else
    define_macro(str, "1");
}
#else
static void define(char *str) {
  char *eq = strchr(str, '=');
  if (eq) {
    define_macro_cli(strndup(str, eq - str), eq + 1);
  }else {
    define_macro(str, "1");
  }
}
#endif

static FileType parse_opt_x(char *s) {
  if (!strcmp(s, "c"))
    return FILE_C;
  if (!strcmp(s, "assembler"))
    return FILE_ASM;
  if (!strcmp(s, "none"))
    return FILE_NONE;
  error("<command line>: unknown argument for -x: %s", s);
}

static char *quote_makefile(char *s) {
  char *buf = calloc(1, strlen(s) * 2 + 1);

  for (int i = 0, j = 0; s[i]; i++) {
    switch (s[i]) {
    case '$':
      buf[j++] = '$';
      buf[j++] = '$';
      break;
    case '#':
      buf[j++] = '\\';
      buf[j++] = '#';
      break;
    case ' ':
    case '\t':
      for (int k = i - 1; k >= 0 && s[k] == '\\'; k--)
        buf[j++] = '\\';
      buf[j++] = '\\';
      buf[j++] = s[i];
      break;
    default:
      buf[j++] = s[i];
      break;
    }
  }
  return buf;
}

static void parse_args(int argc, char **argv) {
  // Make sure that all command line options that take an argument
  // have an argument.
  for (int i = 1; i < argc; i++)
    if (take_arg(argv[i]))
      if (!argv[++i])
        usage(1);

  StringArray idirafter = {};

  for (int i = 1; i < argc; i++) {
    if (!strcmp(argv[i], "-###")) {
      opt_hash_hash_hash = true;
      continue;
    }

    if (!strcmp(argv[i], "-cc1")) {
      opt_cc1 = true;
      continue;
    }

    if (!strcmp(argv[i], "--help"))
      usage(0);

    if (!strcmp(argv[i], "-o")) {
      opt_o = argv[++i];
      continue;
    }

    if (!strncmp(argv[i], "-o", 2)) {
      opt_o = argv[i] + 2;
      continue;
    }

    if (!strcmp(argv[i], "-S")) {
      opt_S = true;
      continue;
    }

    if (!strcmp(argv[i], "-fcommon")) {
      opt_fcommon = true;
      continue;
    }

    if (!strcmp(argv[i], "-fno-common")) {
      opt_fcommon = false;
      continue;
    }

    if (!strcmp(argv[i], "-fno-builtin-memcpy")) {
      opt_fbuiltin_memcpy = false;
      continue;
    }
    if (!strcmp(argv[i], "-fno-builtin-memset")) {
      opt_fbuiltin_memset = false;
      continue;
    }
    if (!strcmp(argv[i], "-fno-builtin-strcpy")) {
      opt_fbuiltin_strcpy = false;
      continue;
    }

    if (!strcmp(argv[i], "-c")) {
      opt_c = true;
      continue;
    }

    if (!strcmp(argv[i], "-E")) {
      opt_E = true;
      continue;
    }

    if (!strncmp(argv[i], "-I", 2)) {
      strarray_push(&include_paths, argv[i] + 2);
      continue;
    }

    if (!strcmp(argv[i], "-D")) {
      define(argv[++i]);
      continue;
    }

    if (!strncmp(argv[i], "-D", 2)) {
      define(argv[i] + 2);
      continue;
    }

    if (!strcmp(argv[i], "-U")) {
      undef_macro(argv[++i]);
      continue;
    }

    if (!strncmp(argv[i], "-U", 2)) {
      undef_macro(argv[i] + 2);
      continue;
    }

    if (!strcmp(argv[i], "-include")) {
      strarray_push(&opt_include, argv[++i]);
      continue;
    }

    if (!strcmp(argv[i], "-x")) {
      opt_x = parse_opt_x(argv[++i]);
      continue;
    }

    if (!strncmp(argv[i], "-x", 2)) {
      opt_x = parse_opt_x(argv[i] + 2);
      continue;
    }

    if (!strncmp(argv[i], "-l", 2) || !strncmp(argv[i], "-Wl,", 4)) {
      strarray_push(&input_paths, argv[i]);
      if (!strcmp(argv[i],"-lm")) {
        opt_lm = 1;
      }
      continue;
    }

    if (!strcmp(argv[i], "-Xlinker")) {
      strarray_push(&ld_extra_args, argv[++i]);
      continue;
    }

    if (!strcmp(argv[i], "-s")) {
      strarray_push(&ld_extra_args, "-s");
      continue;
    }

    if (!strcmp(argv[i], "-M")) {
      opt_M = true;
      continue;
    }

    if (!strcmp(argv[i], "-MF")) {
      opt_MF = argv[++i];
      continue;
    }

    if (!strcmp(argv[i], "-MP")) {
      opt_MP = true;
      continue;
    }

    if (!strcmp(argv[i], "-MT")) {
      if (opt_MT == NULL)
        opt_MT = argv[++i];
      else
        opt_MT = format("%s %s", opt_MT, argv[++i]);
      continue;
    }

    if (!strcmp(argv[i], "-MD")) {
      opt_MD = true;
      continue;
    }

    if (!strcmp(argv[i], "-MQ")) {
      if (opt_MT == NULL)
        opt_MT = quote_makefile(argv[++i]);
      else
        opt_MT = format("%s %s", opt_MT, quote_makefile(argv[++i]));
      continue;
    }

    if (!strcmp(argv[i], "-MMD")) {
      opt_MD = opt_MMD = true;
      continue;
    }

    if (!strcmp(argv[i], "-cc1-input")) {
      base_file = argv[++i];
      continue;
    }

    if (!strcmp(argv[i], "-cc1-output")) {
      output_file = argv[++i];
      continue;
    }

    if (!strcmp(argv[i], "-idirafter")) {
      strarray_push(&idirafter, argv[i++]);
      continue;
    }

    if (!strcmp(argv[i], "-static")) {
      opt_static = true;
      strarray_push(&ld_extra_args, "-static");
      continue;
    }

    if (!strcmp(argv[i], "-shared")) {
      opt_shared = true;
      strarray_push(&ld_extra_args, "-shared");
      continue;
    }

    if (!strcmp(argv[i], "-L")) {
      strarray_push(&ld_extra_args, "-L");
      strarray_push(&ld_extra_args, argv[++i]);
      continue;
    }

    if (!strncmp(argv[i], "-L", 2)) {
      strarray_push(&ld_extra_args, "-L");
      strarray_push(&ld_extra_args, argv[i] + 2);
      continue;
    }

    if (!strcmp(argv[i], "-hashmap-test")) {
      hashmap_test();
      exit(0);
    }

    if (!strncmp(argv[i], "-v", 2)) {
      if (argv[i][2]=='\0') {
        opt_v++;
      } else if (isdigit(argv[i][2])) {
        opt_v = atoi(&argv[i][2]);
      } else {
        for (int j=1; argv[i][j]; j++) {
          if (argv[i][j]=='v') {
            opt_v++;
          }else
            error("unknown argument: %s", argv[i]);
        }
      }
      continue;
    }
    if (!strncmp(argv[i], "-O", 2)) {
      if (argv[i][2]=='\0') {
        opt_O = '1';
      }else{
        opt_O = argv[i][2];
      }
      switch (opt_O) {
      case 's':
      case '0':
      case '1':
      case '2':
      case '3':
        break;
      default:
        error("unknown optimize option: %s", argv[i]);
      }
      continue;
    }
    if (!strncmp(argv[i], "-g", 2)) {
      if (argv[i][2]=='\0') {
        opt_g = '1';
      }else if (argv[i][2]=='g') {
        opt_g = '2';
        if (argv[i][3]=='g') {
          opt_g = '3';
        }
      }else if(isdigit(argv[i][2])){
        opt_g = argv[i][2];
        switch (opt_g) {
        case '0':
        case '1':
        case '2':
        case '3':
          break;
        default:
          error("unknown debug option: %s", argv[i]);
        }
      }else{
        error("unknown debug option: %s", argv[i]);
      }
      continue;
    }
    if (!strncmp(argv[i], "-t", 2)) {
      if (argv[i][2]=='\0') {
        error("unknown macine name: %s", argv[i]);
      }
      if (!strcmp(argv[i]+2,"emu6800")) {
        opt_t = T_EMU6800;
      }else if (!strcmp(argv[i]+2,"mikbug")) {
        opt_t = T_MIKBUG;
      }else if (!strcmp(argv[i]+2,"bm")) {
        opt_t = T_BM;
      }else if (!strcmp(argv[i]+2,"jr100")) {
        opt_t = T_JR100;
      }else if (!strcmp(argv[i]+2,"jr200")) {
        opt_t = T_JR200;
      }else{
        error("unknown macine name: %s", argv[i]);
      }
      continue;
    }
    if (!strncmp(argv[i], "-C", 2)) {
      if (argv[i][2]=='\0' || !isdigit(argv[i][2])) {
        error("invalid code segment base: %s", argv[i]);
      }
      opt_C = (uint16_t)strtol(&argv[i][2],NULL,0);
      continue;
    }
    if (!strncmp(argv[i], "-Z", 2)) {
      if (argv[i][2]=='\0' || !isdigit(argv[i][2])) {
        error("invalid zero page work area: %s", argv[i]);
      }
      opt_Z = (uint16_t)strtol(&argv[i][2],NULL,0);
      continue;
    }

    if (!strcmp(argv[i], "-nostartfiles")) {
      opt_nostartfiles = 1;
      continue;
    }

    // These options are ignored for now.
    if (!strncmp(argv[i], "-W", 2) ||
        !strncmp(argv[i], "-std=", 5) ||
        !strcmp(argv[i], "-ffreestanding") ||
        !strcmp(argv[i], "-fno-builtin") ||
        !strcmp(argv[i], "-fno-omit-frame-pointer") ||
        !strcmp(argv[i], "-fno-stack-protector") ||
        !strcmp(argv[i], "-fno-strict-aliasing") ||
        !strcmp(argv[i], "-m64") ||
        !strcmp(argv[i], "-mno-red-zone") ||
        !strcmp(argv[i], "-w"))
      continue;

    if (argv[i][0] == '-' && argv[i][1] != '\0')
      error("unknown argument: %s", argv[i]);

    strarray_push(&input_paths, argv[i]);
  }

  for (int i = 0; i < idirafter.len; i++)
    strarray_push(&include_paths, idirafter.data[i]);

  if (input_paths.len == 0)
    error("no input files");

  // -E implies that the input is the C macro language.
  if (opt_E)
    opt_x = FILE_C;
}

static FILE *open_file(char *path) {
  if (!path || strcmp(path, "-") == 0)
    return stdout;

  FILE *out = fopen(path, "w");
  if (!out)
    error("cannot open output file: %s: %s", path, strerror(errno));
  return out;
}

static bool endswith(char *p, char *q) {
  int len1 = strlen(p);
  int len2 = strlen(q);
  return (len1 >= len2) && !strcmp(p + len1 - len2, q);
}

// Replace file extension
static char *replace_extn(char *tmpl, char *extn) {
  char *filename = basename(strdup(tmpl));
  char *dot = strrchr(filename, '.');
  if (dot)
    *dot = '\0';
  return format("%s%s", filename, extn);
}

static void cleanup(void) {
  for (int i = 0; i < tmpfiles.len; i++)
    unlink(tmpfiles.data[i]);
}

static char *create_tmpfile(void) {
  char *path = strdup("/tmp/chibicc-XXXXXX");
  int fd = mkstemp(path);
  if (fd == -1)
    error("mkstemp failed: %s", strerror(errno));
  close(fd);

  strarray_push(&tmpfiles, path);
  return path;
}

static void run_subprocess(char **argv, char *redirect_in, char *redirect_out) {
  // If -### is given, dump the subprocess's command line.
  if (opt_hash_hash_hash) {
    fprintf(stderr, "\"%s\"", argv[0]);
    for (int i = 1; argv[i]; i++)
      fprintf(stderr, " \"%s\"", argv[i]);
    fprintf(stderr, "\n");
  }

  if (fork() == 0) {
    // Child process. Run a new command.
    if (opt_v>1) {
      for (int i=0; argv[i]; i++) {
        fprintf(stderr,"%s ",argv[i]);
      }
      fprintf(stderr,"\n");fflush(stdout);
    }
    if (redirect_in) {
      int fd = open(redirect_in, O_RDONLY);
      if (fd<0) {
        error("can't open input file %s for %s",argv[0],redirect_in);
      }
      dup2(fd, STDIN_FILENO);
      close(fd);
    }
    if (redirect_out) {
      int fd = open(redirect_out, O_WRONLY | O_CREAT | O_TRUNC, 0644);
      if (fd<0) {
        error("can't open input file %s for %s",argv[0],redirect_in);
      }
      dup2(fd, STDOUT_FILENO);
      close(fd);
    }
    execvp(argv[0], argv);
    fprintf(stderr, "exec failed: %s: %s\n", argv[0], strerror(errno));
    _exit(1);
  }

  // Wait for the child process to finish.
  int status;
  if (wait(&status) <=0 ||status != 0){
    fprintf(stderr,"Wait for the child process to finish. status=%d\n",status);
    exit(1);
  }
}

static void run_cc1(int argc, char **argv, char *input, char *output) {
  char **args = calloc(argc + 10, sizeof(char *));
  memcpy(args, argv, argc * sizeof(char *));
  args[argc++] = "-cc1";

  if (input) {
    args[argc++] = "-cc1-input";
    args[argc++] = input;
  }

  if (output) {
    args[argc++] = "-cc1-output";
    args[argc++] = output;
  }

  run_subprocess(args,NULL,NULL);
}

// Print tokens to stdout. Used for -E.
static void print_tokens(Token *tok) {
  FILE *out = open_file(opt_o ? opt_o : "-");

  int line = 1;
  for (; tok->kind != TK_EOF; tok = tok->next) {
    if (line > 1 && tok->at_bol)
      fprintf(out, "\n");
    if (tok->has_space && !tok->at_bol)
      fprintf(out, " ");
    fprintf(out, "%.*s", tok->len, tok->loc);
    line++;
  }
  fprintf(out, "\n");
}

static bool in_std_include_path(char *path) {
  for (int i = 0; i < std_include_paths.len; i++) {
    char *dir = std_include_paths.data[i];
    int len = strlen(dir);
    if (strncmp(dir, path, len) == 0 && path[len] == '/')
      return true;
  }
  return false;
}

// If -M options is given, the compiler write a list of input files to
// stdout in a format that "make" command can read. This feature is
// used to automate file dependency management.
static void print_dependencies(void) {
  char *path;
  if (opt_MF)
    path = opt_MF;
  else if (opt_MD)
    path = replace_extn(opt_o ? opt_o : base_file, ".d");
  else if (opt_o)
    path = opt_o;
  else
    path = "-";

  FILE *out = open_file(path);
  if (opt_MT)
    fprintf(out, "%s:", opt_MT);
  else
    fprintf(out, "%s:", quote_makefile(replace_extn(base_file, ".o")));

  File **files = get_input_files();

  for (int i = 0; files[i]; i++) {
    if (opt_MMD && in_std_include_path(files[i]->name))
      continue;
    fprintf(out, " \\\n  %s", files[i]->name);
  }

  fprintf(out, "\n\n");

  if (opt_MP) {
    for (int i = 1; files[i]; i++) {
      if (opt_MMD && in_std_include_path(files[i]->name))
        continue;
      fprintf(out, "%s:\n\n", quote_makefile(files[i]->name));
    }
  }
}

static Token *must_tokenize_file(char *path) {
  Token *tok = tokenize_file(path);
  if (!tok)
    error("%s: %s", path, strerror(errno));
  return tok;
}

static Token *append_tokens(Token *tok1, Token *tok2) {
  if (!tok1 || tok1->kind == TK_EOF)
    return tok2;

  Token *t = tok1;
  while (t->next->kind != TK_EOF)
    t = t->next;
  t->next = tok2;
  return tok1;
}

static void cc1(void) {
  Token *tok = NULL;

  // Process -include option
  for (int i = 0; i < opt_include.len; i++) {
    char *incl = opt_include.data[i];

    char *path;
    if (file_exists(incl)) {
      path = incl;
    } else {
      path = search_include_paths(incl);
      if (!path)
        error("-include: %s: %s", incl, strerror(errno));
    }

    Token *tok2 = must_tokenize_file(path);
    tok = append_tokens(tok, tok2);
  }

  // Tokenize and parse.
  Token *tok2 = must_tokenize_file(base_file);
  tok = append_tokens(tok, tok2);
  tok = preprocess(tok);

  // If -M or -MD are given, print file dependencies.
  if (opt_M || opt_MD) {
    print_dependencies();
    if (opt_M)
      return;
  }

  // If -E is given, print out preprocessed C code as a result.
  if (opt_E) {
    print_tokens(tok);
    return;
  }

  Obj *prog = parse(tok);

  // Open a temporary output buffer.
  char *buf;
  size_t buflen;
  FILE *output_buf = open_memstream(&buf, &buflen);

  // Traverse the AST to emit assembly.
  codegen(prog, output_buf);
  fclose(output_buf);

  // Write the asembly text to a file.
  FILE *out = open_file(output_file);
  fwrite(buf, buflen, 1, out);
  fclose(out);
}

int can_copt(void)
{
  return access(copt_path,F_OK)==0 && access(copt_path,X_OK) == 0;
}

char *has_copt_rules(char *path, char *file)
{
  size_t bufsize = strlen(chibicc_lib_path) + strlen(file) + 1;
  char *buf = malloc(bufsize);

  if (snprintf(buf,bufsize,"%s%s",path,file)>=bufsize) {
     error("copt path error %s%s",path,file);
  }
  if (access(buf,F_OK)==0 && access(buf,X_OK) == 0) {
    return buf;
  }

  return NULL;
}

static void run_copt(char *input, char *output, char *copt_file) {
  char **args = calloc(10, sizeof(char *));
  int argc=0;
  char *path;

  if (!can_copt()) {
    error("can't access copt %s\n",copt_path);
  }

  args[argc++] = copt_path;

  if ((path=has_copt_rules("./",copt_file))) {
    args[argc++] = path;
  }else if ((path=has_copt_rules(chibicc_lib_path,copt_file))) {
    args[argc++] = path;
  }else{
    error("copt file not found %s",copt_file);
  }
  run_subprocess(args,input,output);
}

static void assemble(char *input, char *output) {
//  char *cmd[] = {"as6800", "-c", input, "-o", output, NULL};
  char *cmd[] = {"as6800", "-o", output, input,  NULL};
  run_subprocess(cmd,NULL,NULL);
}

static char *find_file(char *pattern) {
  char *path = NULL;
  glob_t buf = {};
  glob(pattern, 0, NULL, &buf);
  if (buf.gl_pathc > 0)
    path = strdup(buf.gl_pathv[buf.gl_pathc - 1]);
  globfree(&buf);
  return path;
}

// Returns true if a given file exists.
bool file_exists(char *path) {
  struct stat st;
  return !stat(path, &st);
}

static char *find_libpath(void) {
  if (file_exists("/opt/chibicc/lib/crt0.o"))
    return "/opt/chibicc/lib";
  error("library path is not found");
}

static void run_linker(StringArray *inputs, char *output) {
  StringArray arr = {};
  uint16_t C,Z;

//ld6800 -v -b -C256 -Z0 -m $s.map -o $s.bin ../crt0.o $s.o ../libc/libc.a
  strarray_push(&arr, "ld6800");
  strarray_push(&arr, "-b");
  static char buf_C[256];
  static char buf_Z[256];

  switch(opt_t) {
  case T_EMU6800:
    C = opt_C? opt_C:0x0100;
    Z = opt_Z;
    break;
  case T_MIKBUG:    // MIKBUG 1.0
    C = opt_C? opt_C:0x100;
    Z = opt_Z? opt_Z:0x40;   // ???
    break;
  case T_BM:    // hitach BASIC MASTER
    C = opt_C? opt_C:0x1000;
    Z = opt_Z? opt_Z:0xd4;
    break;
  case T_JR100: // national JR-100
    C = opt_C? opt_C:0x1000;
    Z = opt_Z? opt_Z:0xe3;
    break;
  case T_JR200: // national JR-100
    C = opt_C? opt_C:0x1000;
    Z = opt_Z? opt_Z:0xc8;
    break;
  default:
    error("unknown machine type %d",opt_t);
  }
  sprintf(buf_C,"-C%d",C); strarray_push(&arr, buf_C);
  sprintf(buf_Z,"-Z%d",Z); strarray_push(&arr, buf_Z);
  strarray_push(&arr, "-m");
  char *mapfile = replace_extn(output, ".map");
  strarray_push(&arr, mapfile);
  strarray_push(&arr, "-o");
  char *binfile = replace_extn(output, ".bin");
  strarray_push(&arr, binfile);

  char *libpath = find_libpath();

  if (!opt_nostartfiles) {
    switch(opt_t) {
    case T_EMU6800:
      strarray_push(&arr, format("%s/crt0.o", libpath));
      break;
    case T_MIKBUG:
      strarray_push(&arr, format("%s/crt0_mikbug.o", libpath));
      break;
    case T_BM:
      strarray_push(&arr, format("%s/crt0_bm.o", libpath));
      break;
    case T_JR100:
      strarray_push(&arr, format("%s/crt0_jr100.o", libpath));
      break;
    case T_JR200:
      strarray_push(&arr, format("%s/crt0_jr200.o", libpath));
      break;
    }
  }

  for (int i = 0; i < ld_extra_args.len; i++)
    strarray_push(&arr, ld_extra_args.data[i]);

  for (int i = 0; i < inputs->len; i++) {
    if (strlen(inputs->data[i])>=3 && strncmp(inputs->data[i],"-l",2)==0) {
      // skip -lx
    }else{
      strarray_push(&arr, inputs->data[i]);
    }
  }

  if (!opt_lm) {
    strarray_push(&arr, format("%s/dummyfloat.o", libpath));
  }

  for (int i = 0; i < inputs->len; i++) {
    if (strlen(inputs->data[i])>=3 && strncmp(inputs->data[i],"-l",2)==0) {
      if (strchr(inputs->data[i],'/'))
        continue;
      strarray_push(&arr, format("%s/lib%s.a",libpath,inputs->data[i]+2));
    }
  }

  strarray_push(&arr, "/opt/chibicc/lib/libc.a");
  strarray_push(&arr, NULL);

  run_subprocess(arr.data,NULL,NULL);
}

static FileType get_file_type(char *filename) {
  if (opt_x != FILE_NONE)
    return opt_x;

  if (endswith(filename, ".a"))
    return FILE_AR;
  if (endswith(filename, ".so"))
    return FILE_DSO;
  if (endswith(filename, ".o"))
    return FILE_OBJ;
  if (endswith(filename, ".c"))
    return FILE_C;
  if (endswith(filename, ".s"))
    return FILE_ASM;

  error("<command line>: unknown file extension: %s", filename);
}

int main(int argc, char **argv) {
  atexit(cleanup);
  init_macros();
  parse_args(argc, argv);

  if (opt_cc1) {
    add_default_include_paths(argv[0]);
    cc1();
    return 0;
  }

  if (input_paths.len > 1 && opt_o && (opt_c || opt_S | opt_E))
    error("cannot specify '-o' with '-c,' '-S' or '-E' with multiple files");

  StringArray ld_args = {};

  for (int i = 0; i < input_paths.len; i++) {
    char *input = input_paths.data[i];

    if (!strncmp(input, "-l", 2)) {
      strarray_push(&ld_args, input);
      continue;
    }

    if (!strncmp(input, "-Wl,", 4)) {
      char *s = strdup(input + 4);
      char *arg = strtok(s, ",");
      while (arg) {
        strarray_push(&ld_args, arg);
        arg = strtok(NULL, ",");
      }
      continue;
    }

    char *output;
    if (opt_o)
      output = opt_o;
    else if (opt_S)
      output = replace_extn(input, ".s");
    else
      output = replace_extn(input, ".o");

    FileType type = get_file_type(input);

    // Handle .o or .a
    if (type == FILE_OBJ || type == FILE_AR || type == FILE_DSO) {
      strarray_push(&ld_args, input);
      continue;
    }

    // Handle .s
    if (type == FILE_ASM) {
      if (!opt_S)
        assemble(input, output);
      continue;
    }

    assert(type == FILE_C);

    // Just preprocess
    if (opt_E || opt_M) {
      run_cc1(argc, argv, input, NULL);
      continue;
    }

    // Compile
    if (opt_S) {
      if (!opt('O','0') && can_copt()) {
        char *tmp = create_tmpfile();
        char *tmp3 = create_tmpfile();
        run_cc1(argc, argv, input, tmp);
        if (opt('O','3') && can_copt()) {
          run_copt(tmp, tmp3,  copt_rules);
          run_copt(tmp3,tmp  , copt_O2_rules);
          run_copt(tmp, output,copt_O3_rules);
        }else if (opt('O','2') && can_copt()) {
          run_copt(tmp, tmp3,  copt_rules);
          run_copt(tmp3,output,copt_O2_rules);
        }else{
          run_copt(tmp, output,copt_rules);
        }
      }else{
        run_cc1(argc, argv, input, output);
      }
      continue;
    }

    // Compile and assemble
    if (opt_c) {
      char *tmp = create_tmpfile();
      run_cc1(argc, argv, input, tmp);
      if (!opt('O','0') && can_copt()) {
        char *tmp3 = create_tmpfile();
        run_copt(tmp,tmp3,copt_rules);
        tmp = tmp3;
        if (opt('O','3') && can_copt()) {
          char *tmp2 = create_tmpfile();
          run_copt(tmp ,tmp2,copt_O2_rules);
          run_copt(tmp2,tmp, copt_O3_rules);
        }else if (opt('O','2') && can_copt()) {
          char *tmp2 = create_tmpfile();
          run_copt(tmp,tmp2,copt_O2_rules);
          tmp = tmp2;
        }
      }
      assemble(tmp, output);
      continue;
    }

    // Compile, assemble and link
    char *tmp1 = create_tmpfile();
    run_cc1(argc, argv, input, tmp1);
    if (!opt('O','0') && can_copt()) {
      char *tmp3 = create_tmpfile();
      run_copt(tmp1,tmp3,copt_rules);
      tmp1 = tmp3;
    }
    if (opt('O','3') && can_copt()) {
      char *tmp3 = create_tmpfile();
      run_copt(tmp1,tmp3,copt_O2_rules);
      run_copt(tmp3,tmp1,copt_O3_rules);
    }else if (opt('O','2') && can_copt()) {
      char *tmp3 = create_tmpfile();
      run_copt(tmp1,tmp3,copt_O2_rules);
      tmp1 = tmp3;
    }
    char *tmp2 = create_tmpfile();
    assemble(tmp1, tmp2);
    strarray_push(&ld_args, tmp2);
    continue;
  }

  if (ld_args.len > 0)
    run_linker(&ld_args, opt_o ? opt_o : "a.out");
  return 0;
}
