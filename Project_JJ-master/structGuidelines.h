
HashBucket *hashArray[n_buckets];

typedef struct HashBucket
{
    SpecNode *specList;
};

typedef struct HashTable
{
    long size;
    HashBucket **hashArray;
};

typedef struct SpecNode
{
    CliqueNode *cliquePtr;
    SpecNode *nextSpec; // Θα δείχνει στο επόμενο spec, στην περίπτωση που το hashFunction μας δείξει στο ίδιο bucket.
};

typedef struct CliqueNode // Θα υλοποιηθεί κυκλική λίστα. Αρχικά, το next θα δείχνει στον εαυτό του λόγω κυκλικότητας.
{                         // Όταν δημιουργούνται ζευγάρια, θα ενωποιούνται αυτά, κάνοντας update το *next.
    SpecInfo *specInfo;
    CliqueNode *next;
    CliqueNode *prev;
    int isPrinted;
};

typedef struct MissMatchNode
{
    CliqueNode *cliqueNode;
    MissMatchNode *next;
};

typedef struct SpecInfo
{
    char *specId;
    char *pageTitle;
    InfoList *infoList;
};

typedef struct InfoList
{
    InfoNode *head;
};

typedef struct InfoNode // Περιέχει την περιγραφή (descirption) του spec.
{                       // Σε μορφή λίστας θα αποθηκεύονται.
    char *descirption;
    char *content; // description:content αντιστοίχιση των γραμμών του .json
    InfoNode *next;
};
