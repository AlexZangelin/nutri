#include <stdio.h>                      // printf
#include <string.h>                     // strchr
#include <stdlib.h>                     // exit



// -----------------------------------------------------------------------------
//
// LM -
//
#define LM(s)            \
do                       \
{                        \
  if (verbose == 1)      \
  {                      \
    printf("%s[%d]: ", __FILE__, __LINE__);		\
    printf s;            \
    printf("\n");        \
  }                      \
} while (0)



// -----------------------------------------------------------------------------
//
// CLI parameters
//
int verbose    = 0;

int protMax    = -1;
int protMin    = -1;

int fatMax    = -1;
int fatMin    = -1;

int sFatMax    = -1;
int sFatMin    = -1;

int carbsMax    = -1;
int carbsMin    = -1;

int sugarMax    = -1;
int sugarMin    = -1;

int kcalMax    = -1;
int kcalMin    = -1;

 

// -----------------------------------------------------------------------------
//
// Prima - ...
//
typedef struct Prima
{
  char*          name;
  int            carbs;
  int            prot;
  int            fat;
  int            sFat;
  int            sugar;
  int            kcal;
  struct Prima*  next;
} Prima;


Prima* primaList = NULL;

void pInsert(Prima* primaP)
{
  if (primaList == NULL)
  {
    primaList = primaP;
    primaP->next = NULL;
  }
  else
  {
    primaP->next = primaList;
    primaList = primaP;
  }
}


char* nextField(char* str, char** delPP)
{
  char* endOfWord = strchr(str, '|');

  if (endOfWord == NULL)
    return str;

  *endOfWord = 0;
  ++endOfWord;

  *delPP = endOfWord;
  return str;
}


void pPopulate(char* fName)
{
  FILE* fP = fopen(fName, "r");
  char  buf[256];
  
  if (fP == NULL)
  {
    fprintf(stderr, "error opening %s\n", fName);
    exit(1);
  }

  
  while (fgets(buf, sizeof(buf), fP) != NULL)
  {
    char* nameP;
    char* carbsP;
    char* protP;
    char* fatP;
    char* sugarP;
    char* delP;

    nameP    = nextField(buf, &delP);
    carbsP   = nextField(delP, &delP);
    sugarP   = nextField(delP, &delP);
    protP    = nextField(delP, &delP);
    fatP     = nextField(delP, &delP);

    // preparing Prima instance
    Prima* p = (Prima*) malloc(sizeof(Prima));

    p->name   = strdup(nameP);
    p->carbs  = atoi(carbsP);
    p->sugar  = atoi(sugarP);
    p->prot   = atoi(protP);
    p->fat    = atoi(fatP);
    pInsert(p);
  }
}

Prima* pLookup(char* name)
{
  Prima* pP = primaList;

  while (pP != NULL)
  {
    if (strcmp( pP->name , name) == 0)
      return pP;
    pP = pP->next;
  }
  
  return NULL;
}


int main(int argC, char* argV[])
{
  char* inNameP = NULL;
  int   ix      = 1;
  int   all     = 0;
  verbose = 1;
  while (ix < argC)
  {
    LM(("argV[%d] = %s", ix, argV[ix]));
    if (strcmp(argV[ix], "-s") == 0)
      inNameP = argV[ix + 1];
    else if (strcmp(argV[ix], "-v") == 0)
      verbose = 1;
    else if (strcmp(argV[ix], "-a") == 0)
      all = 1;
    else if (strcmp(argV[ix], "-cP") == 0)
    {
      if (argC <= ix + 1)
      {
	fprintf(stderr, "Missing value for -cP\n");
	exit(1);
      }

      char* cP = argV[ix + 1];
      if (*cP == '-')
      {
	protMin = 0;
	protMax = atoi(&cP[1]);
      }
      else if (*cP == '+')
      {
	protMax = 1000;
	protMin = atoi(&cP[1]);
      }
      else
      {
	char* hyphenP = strchr(cP, '-');

	if (hyphenP == NULL)
	{
	  fprintf(stderr, "Invalid range for -cP\n");
	  exit(1);
	}
	
	protMin = atoi(cP);
	protMax = atoi(&hyphenP[1]);
      }
      LM(("protMin = %d", protMin));
      LM(("protMax = %d", protMax));
      
    }
    else if (strcmp(argV[ix], "-cC") == 0)
      ; // carbsLimit = argV[ix + 1]
    else if (strcmp(argV[ix], "-cS") == 0)
    {
    }
    else if (strcmp(argV[ix], "-cF") == 0)
    {
    }
    else if (strcmp(argV[ix], "-cSf") == 0)
    {
    }

    ix++;
  }

  pPopulate("./DB");

  Prima* pP;
  
  if (all == 1)
  {
    pP = primaList;

    printf("%-12s%-9s%-9s%-9s%-9s\n", "Name", "Carbs", "Sugar", "Prot", "Fat");
    printf("---------------------------------------------------------------------------\n");
    while (pP != NULL)
    {
      printf("%-12s%-9d%-9d%-9d%-9d\n", pP->name, pP->carbs, pP->sugar, pP->prot, pP->fat);
      pP = pP->next;
    }    
  }
  else if (protMax != -1)
  {
    pP = primaList;

    printf("%-12s%-9s%-9s%-9s%-9s\n", "Name", "Carbs", "Sugar", "Prot", "Fat");
    printf("---------------------------------------------------------------------------\n");
    for (Prima* pP = primaList; pP != NULL; pP = pP->next)
    {
      if ((pP->prot <= protMax) && (pP->prot >= protMin))
	printf("%-12s%-9d%-9d%-9d%-9d\n", pP->name, pP->carbs, pP->sugar, pP->prot, pP->fat);
    }    
  }
  else if (inNameP != NULL)
  {    
    pP = pLookup(inNameP);
    
    if (pP != NULL)	
      printf("%s: c: %d, s: %d, p: %d, f: %d\n", pP->name, pP->carbs, pP->sugar, pP->prot, pP->fat);
    else
      printf("can't find '%s'\n", inNameP);
  }
  else
    printf("nothing to do\n");

    
  return 0;
}
