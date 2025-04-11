#include "mystring.h"

int countWords(char* str, char* spr);
char* strsub_my(char* subs, const char* s, unsigned len);

int countWords(char* str, char* spr)
{
    if (str == 0 || *str == '\0')
        return 0;
    int counter = 0;
    const char* p = str;
    while (*p && strchr(spr, *p))
        p++;
    while (*p)
    {
        const char* end = p;
        while (*end && !strchr(spr, *end))
            end++;
        if (end != p)
        {
            counter++;
            p = end;
        }
        while (*p && strchr(spr, *p))
            p++;
    }

    return counter;
}

char* strsub_my(char* subs, const char* s, unsigned len)
{
    strncpy(subs, s, len);
    subs[len] = '\0';

    return subs;
}

int parseWords(char*** words, char* s, char* sbr)
{
    int wordCount = countWords(s, sbr);
    char* p = s;
    *words = (char**)malloc(wordCount * sizeof(char*));
    int i = 0;
    for (char* pSpace; (pSpace = strpbrk(p, sbr)); p++)
    {
        if (p != pSpace)
        {
            long len = pSpace - p;
            (*words)[i] = (char*)malloc((len + 1) * sizeof(char));
            strsub_my((*words)[i], p, len);
            i++;
            p = pSpace;
        }
    }
    if (strlen(p))
    {
        (*words)[i] = (char*)malloc(sizeof(char) * (strlen(p) + 1));
        strcpy((*words)[i], p);
    }

    return wordCount;
}
