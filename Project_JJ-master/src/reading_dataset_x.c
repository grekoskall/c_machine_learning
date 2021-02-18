#include "../hdr/includes.h"

void read_from_file(char *file_name)
{
  FILE *dataset_x;
  char line[1000];
  dataset_x = fopen(file_name, "r");
  if (dataset_x == NULL)
  {
    perror("Unable to open dataset_x file!");
    exit(1);
  }
  else
  {
    while (fgets(line, 1000, dataset_x) != NULL)
    {
      puts(line);
    }
    fclose(dataset_x);
  }
}

void read_from_dir(char *dir_name, HashTable *hash_table)
{
  if (hash_table == NULL)
  {
    return;
  }
  DIR *sites_dir;
  struct dirent *sites_folder;

  // Open dir for site folders
  sites_dir = opendir(dir_name);

  if (sites_dir == NULL)
  {
    perror("Unable to read Dataset X base directory!");
    exit(1);
  }

  // For each site folder, open it and read the data.
  while ((sites_folder = readdir(sites_dir)))
  {
    if (strcmp(sites_folder->d_name, ".") != 0 && strcmp(sites_folder->d_name, "..") != 0)
    {
      char *dir_path;
      DIR *sub_f;
      struct dirent *specs_folder;

      // Open the sub directory.
      dir_path = (char *)safe_malloc(strlen(dir_name) + strlen(sites_folder->d_name) + 1);
      sprintf(dir_path, "%s%s", dir_name, sites_folder->d_name);

      //printf("%s\n", dir_path);

      sub_f = opendir(dir_path);
      if (sub_f == NULL)
      {
        perror("Unable to read Dataset X sub directory!");
        exit(1);
      }
      else
      {
        while ((specs_folder = readdir(sub_f)))
        {
          if (strcmp(specs_folder->d_name, ".") != 0 && strcmp(specs_folder->d_name, "..") != 0)
          {
            //printf("%s\n", specs_folder->d_name); // json files
            FILE *dataset_x;
            char line[10000];
            char *spec_path;

            // Open the spec file
            spec_path = (char *)safe_malloc(strlen(dir_path) + strlen(specs_folder->d_name) + 2);
            sprintf(spec_path, "%s/%s", dir_path, specs_folder->d_name);

            //printf("%s\n", spec_path);

            dataset_x = fopen(spec_path, "r");
            if (dataset_x == NULL)
            {
              perror("Unable to open Dataset X's file!");
              exit(1);
            }
            else
            {
              // Time to read the spec
              SpecInfo *new_spec = NULL;
              new_spec = initSpecInfoEmpt(sites_folder->d_name, specs_folder->d_name);
              const char s[2] = "\"";
              char *description = NULL;

              if (fgets(line, 10000, dataset_x) == NULL)
              {
                perror("Unable to read data.");
                exit(1);
              }
              if (strcmp(line, "{\n") != 0)
              {
                //printf("%s", line);
                // Incorrect spec definition
                fclose(dataset_x);
                free(spec_path);
                continue;
              }
              while (fgets(line, 10000, dataset_x) != NULL)
              {
                if (strcmp(line, "}") == 0)
                {
                  continue;
                }
                //printf("%s", line);

                strtok(line, s);
                description = strtok(NULL, s);

                if (description != NULL)
                {
                  //printf("%s\n", description);
                  if (strcmp(description, "<page title>") == 0)
                  {
                    char *pageTitle = NULL;
                    char *pageTitle_rest = NULL;
                    char *whole_title;
                    int clfl = 0;
                    strtok(NULL, s);
                    pageTitle = strtok(NULL, s);
                    pageTitle_rest = strtok(NULL, s);
                    //printf("%s\n", s);
                    if ( strcmp(pageTitle_rest, ",\n") != 0 ) {
                      whole_title = (char*)safe_malloc(sizeof(char)*(strlen(pageTitle_rest)+strlen(pageTitle)+1));
                      strcpy(whole_title, pageTitle);
                      strcat(whole_title, pageTitle_rest);
                      clfl = 1;
                    } else {
                      whole_title = pageTitle;
                    }
                    //whole_title = pageTitle;
                    //pageTitle = strtok(pageTitle, "|-,"); // If we wanted to strip the title and keep only the "usefull" info
                    if (whole_title != NULL)
                    {
                      //printf("%s\n", whole_title);
                      add_pageTitle_toSpec(new_spec, whole_title);
                      //new_spec = initSpecInfo(sites_folder->d_name, specs_folder->d_name, whole_title);
                      if ( clfl == 1) {
                        free(whole_title);

                      }
                      //free(whole_title);
                      continue;
                    }
                  }
                  else
                  {
                    // The attribute is not page title, but a new one
                    // description -> name, content -> info
                    char *option;
                    char *content;
                    option = strtok(NULL, "\":");
                    if (option != NULL)
                    {
                      if (strcmp(option, " [\n") == 0)
                      {
                        //printf("%s\n", option);
                        // Parse option List
                        char option_line[10000];
                        while (fgets(option_line, 10000, dataset_x) != NULL)
                        {
                          //puts(option_line);
                          strtok(option_line, s);
                          content = strtok(NULL, s);
                          if (content != NULL)
                          {
                            //printf("%s: %s\n", description, content);
                            add_newInfo_toSpec(new_spec, description, content);
                          }
                          content = strtok(NULL, s);
                          if (content != NULL)
                          {
                            //printf("%s", content);
                            if (strcmp(content, ",\n") != 0)
                            {
                              break;
                            }
                          }
                        }
                      }
                    }
                    // Parse simple info
                    content = strtok(NULL, s);
                    if (content != NULL)
                    {
                      //printf("%s: %s\n", description, content);
                      add_newInfo_toSpec(new_spec, description, content);
                    }
                  }
                }
              }
              //print_spec(new_spec);

              addToHashTable(hash_table, new_spec);
              fclose(dataset_x);
            }
            free(spec_path);
          }
        }
        closedir(sub_f);
      }
      free(dir_path);
    }
  }
  closedir(sites_dir);
}



int count_datafiles(char *path)
{
  DIR *dir_ptr = NULL;
  struct dirent *direntp;
  char *npath;
  if (!path)
  {
    return 0;
  }
  if ((dir_ptr = opendir(path)) == NULL)
  {
    return 0;
  }

  int count = 0;
  while ((direntp = readdir(dir_ptr)))
  {
    if (strcmp(direntp->d_name, ".") == 0 || strcmp(direntp->d_name, "..") == 0)
    {
      continue;
    }
    switch (direntp->d_type)
    {
    case DT_REG:
    {
      count++;
      break;
    }
    case DT_DIR:
    {
      npath = (char *)safe_malloc(strlen(path) + strlen(direntp->d_name) + 2);
      sprintf(npath, "%s%s", path, direntp->d_name);
      count += count_datafiles(npath);
      free(npath);
      break;
    }
    }
  }
  closedir(dir_ptr);
  return count;
}
