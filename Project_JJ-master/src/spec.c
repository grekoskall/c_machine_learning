#include "../hdr/includes.h"

SpecInfo *initSpecInfo(char *site, char *id, char *title)
{
  SpecInfo *newSpec = (SpecInfo *)safe_malloc(sizeof(SpecInfo));
  char *json_stripped_id = (char *)safe_malloc(strlen(id) - 4);
  strncpy(json_stripped_id, id, strlen(id) - 5);
  json_stripped_id[strlen(id) - 5] = 0;
  newSpec->specId = (char *)safe_malloc(strlen(site) + strlen(json_stripped_id) + 3);
  strcpy(newSpec->specId, site);
  strcat(newSpec->specId, "//");
  strcat(newSpec->specId, json_stripped_id);
  free(json_stripped_id);
  newSpec->pageTitle = (char *)safe_malloc(strlen(title) + 1);
  strcpy(newSpec->pageTitle, title);
  newSpec->infoList = NULL;
  newSpec->tfidf_HTable = NULL;
  newSpec->tfidf_vector = NULL;

  return newSpec;
}

SpecInfo *initSpecInfoEmpt(char *site, char *id)
{
  SpecInfo *newSpec = (SpecInfo *)safe_malloc(sizeof(SpecInfo));
  char *json_stripped_id = (char *)safe_malloc(strlen(id) - 4);
  strncpy(json_stripped_id, id, strlen(id) - 5);
  json_stripped_id[strlen(id) - 5] = 0;
  newSpec->specId = (char *)safe_malloc(strlen(site) + strlen(json_stripped_id) + 3);
  strcpy(newSpec->specId, site);
  strcat(newSpec->specId, "//");
  strcat(newSpec->specId, json_stripped_id);
  free(json_stripped_id);
  newSpec->pageTitle = NULL;
  newSpec->infoList = NULL;
  newSpec->tfidf_HTable = NULL;
  newSpec->tfidf_vector = NULL;

  return newSpec;
}

void add_pageTitle_toSpec(SpecInfo *spec, char *page_title)
{
  if (spec == NULL || spec->specId == NULL || spec->pageTitle != NULL)
  {
    printf("Error: adding pagetitle to inappopriate spec (uninitialized or with pagetitle).\n");
    return;
  }
  if (page_title == NULL)
  {
    printf("Error: adding null page_title to spec\n");
    return;
  }

  spec->pageTitle = (char *)safe_malloc(strlen(page_title) + 1);
  strcpy(spec->pageTitle, page_title);
  return;
}

// void unitSpecInfo(SpecInfo *spec)
// {
//     free(spec->specId);
//     free(spec->pageTitle);
//     free(spec->infoList);
//     free(spec);
// }

void freeInfoNode(InfoNode *infoNode)
{
  if (infoNode == NULL)
    return;
  free(infoNode->description);
  free(infoNode->content);
  freeInfoNode(infoNode->next);
  free(infoNode);
}

void freeInfoList(InfoList *infoList)
{
  if (infoList == NULL)
    return;
  freeInfoNode(infoList->head);
  free(infoList);
}

void add_newInfo_toSpec(SpecInfo *spec, char *desc, char *info)
{
  if (desc == NULL || info == NULL || spec == NULL)
  {
    printf("Error: adding null info\n");
    return;
  }
  if (spec->specId == NULL)
  {
    printf("Error: add to uninitialized spec\n");
    return;
  }
  if (spec->infoList == NULL)
  {
    spec->infoList = (InfoList *)safe_malloc(sizeof(InfoList));
    spec->infoList->head = NULL;
  }
  add_newInfo_toList(spec->infoList, desc, info);
  return;
}

void add_newInfo_toList(InfoList *list, char *desc, char *info)
{
  if (list == NULL)
  {
    return;
  }
  if (desc == NULL || info == NULL)
  {
    return;
  }
  if (list->head == NULL)
  {
    list->head = (InfoNode *)safe_malloc(sizeof(InfoNode));
    list->head->description = NULL;
    list->head->content = NULL;
    list->head->next = NULL;
    add_newInfo_toNode(list->head, desc, info);
  }
  else
  {
    InfoNode *temp;
    InfoNode *last = list->head;
    while (last->next != NULL)
    {
      last = last->next;
    }
    temp = (InfoNode *)safe_malloc(sizeof(InfoNode));
    temp->description = NULL;
    temp->content = NULL;
    temp->next = NULL;
    add_newInfo_toNode(temp, desc, info);
    last->next = temp;
  }
}

void add_newInfo_toNode(InfoNode *node, char *desc, char *info)
{
  if (desc == NULL || info == NULL)
  {
    return;
  }
  if (node == NULL)
  {
    return;
  }
  node->description = (char *)safe_malloc(strlen(desc) + 1);
  strcpy(node->description, desc);
  node->content = (char *)safe_malloc(strlen(info) + 1);
  strcpy(node->content, info);
  node->next = NULL;
}

void print_spec(SpecInfo *spec)
{
  if (spec->specId == NULL || spec->pageTitle == NULL)
  {
    printf("UNINITIALIZED SPEC\n");
    return;
  }
  printf("\tSPEC:\t%s\n\t\tTITLE:\t%s\n", spec->specId, spec->pageTitle);

  print_info_list(spec->infoList);
  return;
}

void print_info_list(InfoList *list)
{
  if (list == NULL || list->head == NULL)
  {
    //printf("UNINITIALIZED LIST\n");
    return;
  }

  InfoNode *temp = list->head;
  while (temp != NULL)
  {
    printf("\t\t%s:\t%s\n", temp->description, temp->content);
    temp = temp->next;
  }
  return;
}

void freeSpecInfo(SpecInfo *specInfo)
{
  if (specInfo == NULL)
    return;
  free(specInfo->specId);
  free(specInfo->pageTitle);
  freeInfoList(specInfo->infoList);
  //freeMLinfo(specInfo->vectorMLinfo);
  //freeVector(specInfo->vectorMLinfo);
  //freeTFinfo(specInfo->tfVector);
  //freeVector(specInfo->tfVector);
  free(specInfo);
}
