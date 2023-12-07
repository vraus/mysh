#include "../include/myls.h"

int compare(const void *a, const void *b)
{
  const char *str1 = *(const char **)a;
  const char *str2 = *(const char **)b;

  // Skip characters while they are dots ('.')
  while (*str1 != '\0' && *str1 == '.')
  {
    str1++;
  }
  while (*str2 != '\0' && *str2 == '.')
  {
    str2++;
  }

  // Convert characters to lowercase before comparing
  char c1, c2;
  while (*str1 != '\0' && *str2 != '\0')
  {
    c1 = tolower(*str1);
    c2 = tolower(*str2);

    if (c1 != c2)
    {
      return c1 - c2;
    }

    str1++;
    str2++;
  }

  return strcmp(str1, str2);
}

const char *get_permissions_string(mode_t mode)
{
  static char perms[10]; // Use a local array instead of a static array

  perms[0] = (S_ISDIR(mode)) ? 'd' : '-';
  perms[1] = (mode & S_IRUSR) ? 'r' : '-';
  perms[2] = (mode & S_IWUSR) ? 'w' : '-';
  perms[3] = (mode & S_IXUSR) ? 'x' : '-';
  perms[4] = (mode & S_IRGRP) ? 'r' : '-';
  perms[5] = (mode & S_IWGRP) ? 'w' : '-';
  perms[6] = (mode & S_IXGRP) ? 'x' : '-';
  perms[7] = (mode & S_IROTH) ? 'r' : '-';
  perms[8] = (mode & S_IWOTH) ? 'w' : '-';
  perms[9] = (mode & S_IXOTH) ? 'x' : '-';

  return perms;
}

void affiche(int totalBlocks, int num_files, char **file_names, struct stat st)
{
  static const char *month_names[] = {"jan", "feb", "mar", "apr", "may", "jun",
                                      "jul", "aug", "sep", "oct", "nov", "dec"};
  printf("total %d\n", totalBlocks);

  for (int i = 0; i < num_files; i++)
  {
    if (lstat(file_names[i], &st) == 0)
    {
      struct passwd *pw = getpwuid(st.st_uid);
      struct group *gr = getgrgid(st.st_gid);

      char month_name[10]; // To store the name of the month
      // Use strftime to extract the name of the month
      strftime(month_name, sizeof(month_name), "%b", localtime(&st.st_mtime));

      printf("%s %lu ", get_permissions_string(st.st_mode), st.st_nlink);
      if (pw != NULL && gr != NULL)
      {
        const char *user_name = pw->pw_name;
        const char *group_name = gr->gr_name;

        printf("%s %s ", user_name, group_name);
      }
      printf("%7ld  %s. %2d %02d:%02d ", st.st_size,
             month_names[localtime(&st.st_mtime)->tm_mon],
             localtime(&st.st_mtime)->tm_mday, localtime(&st.st_mtime)->tm_hour,
             localtime(&st.st_mtime)->tm_min);
    }

    printf("%s\n", file_names[i]);
    free(file_names[i]);
  }
}

void files(char *cwd, int *num_files, int *max_files, char **file_names, int hasA, int hasR)
{
  struct dirent *entry;
  struct stat st;
  int totalBlocks = 0;
  int blockSize = 1024; // Taille d'un bloc de disque (modif 512)

  // BUG: /Documents/SE/projet/mysh2/mysh/option$ ./myls ERROR OPENDIR: File name too long
  DIR *dir = opendir(cwd);
  if (dir == NULL)
  {
    perror("ERROR OPENDIR");
    exit(1);
  }

  if (lstat(cwd, &st) == -1)
  {
    perror("lstat");
    exit(EXIT_FAILURE);
  }

  // Traverse the current directory
  while ((entry = readdir(dir)) != NULL)
  {
    if (lstat(entry->d_name, &st) == 0)
    {
      int blocks = (st.st_size + blockSize - 1) / blockSize;
      totalBlocks += blocks;
    }

    if (entry->d_name[0] == '.' && !hasA)
      continue;

    /*if (!hasR && S_ISDIR(st.st_mode))
    {
      // If the -R option is specified and it's a directory, call the function recursively
      char *full_path = malloc(strlen(cwd) + strlen(entry->d_name) + 2);
      sprintf(full_path, "%s/%s", cwd, entry->d_name);
      files(full_path, num_files, max_files, file_names, hasA, hasR);
      free(full_path);
    }*/

    if (*num_files == *max_files)
    { // The array is full
      *max_files *= 2;
      file_names = (char **)realloc(file_names, *max_files * sizeof(char *));
      if (file_names == NULL)
      {
        perror("Memory allocation error");
        exit(1);
      }
    }
    file_names[(*num_files)++] = strdup(entry->d_name); // Duplicate the string into the array and
                                                        // allocate a new memory zone
  }

  if (closedir(dir) == -1)
  {
    perror("ERROR CLOSEDIR");
    exit(-1);
  }

  qsort(file_names, *num_files, sizeof(char *),
        compare); // Sort alphabetically

  affiche(totalBlocks, *num_files, file_names, st);
  free(cwd);
}

void hasOption(int argc, char **argv, int *hasA, int *hasR)
{
  for (int i = 1; i < argc; i++)
  {
    if (strcmp(argv[i], "-a") == 0)
    {
      *hasA = 1; // Set hasA to 1 if the -a option is present.
    }
    else if (strcmp(argv[i], "-R") == 0)
    {
      *hasR = 1; // Set hasR to 1 if the -R option is present.
    }
  }
}

int main(int argc, char *argv[])
{
  char **file_names = NULL, *cwd = (char *)malloc(1024);
  int num_files = 0, max_files = 10, hasA = 0, hasR = 0;
  file_names = (char **)malloc(max_files * sizeof(char *));

  hasOption(argc, argv, &hasA, &hasR);

  if (getcwd(cwd, 1024) == NULL)
  {
    perror("ERROR GETCWD");
    exit(1);
  }

  if (file_names == NULL)
  {
    perror("Erreur d'allocation mémoire");
    return 1;
  }

  files(cwd, &num_files, &max_files, file_names, hasA, hasR);

  return 0;
}