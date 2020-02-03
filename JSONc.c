#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

enum JSONctype {single, object, array};

typedef enum JSONctype JSONctype;

struct JSONc
{
    struct JSONc *pre, *next, *child, *mom;
    char name[100], stringVal[100];
    JSONctype type;
};

typedef struct JSONc JSONc;

void init_JSONc(JSONc *source)
{
    source -> pre = NULL;
    source -> next = NULL;
    source -> child = NULL;
    source -> mom = NULL;
    strcpy(source -> name, "");
    strcpy(source -> stringVal, "");
}

JSONc *JSONc_createObject()
{
    JSONc *ptr = (JSONc *)malloc(sizeof(JSONc));
    init_JSONc(ptr);
    ptr -> type = object;
    return ptr;
}

JSONc *JSONc_createString(char *val)
{
    JSONc *ptr = (JSONc *)malloc(sizeof(JSONc));
    init_JSONc(ptr);
    strcpy(ptr -> stringVal, val);
    ptr -> type = single;
    return ptr;
}

JSONc *JSONc_createArray()
{
    JSONc *ptr = (JSONc *)malloc(sizeof(JSONc));
    init_JSONc(ptr);
    ptr -> type = array;
    return ptr;
}

void JSONc_addItem2Object(JSONc *sourceObject, char *name, JSONc *item)
{
    JSONc *tmp;
    strcpy(item -> name, name);
    if (sourceObject -> child == NULL)
    {
        sourceObject -> child = item;
        item -> mom = sourceObject;
    }
    else if ((tmp = sourceObject -> child) -> next == NULL)
    {
        tmp -> next = item;
        item -> pre = tmp;
    }
    else
    {
        while (tmp -> next != NULL)
            tmp = tmp -> next;
        tmp -> next = item;
        item -> pre = tmp;        
    }    
}

void JSONc_addItem2Array(JSONc *sourceArray, JSONc *item)
{
    JSONc *tmp;
    if (sourceArray -> child == NULL)
    {
        sourceArray -> child = item;
        item -> mom = sourceArray;
    }
    else if ((tmp = sourceArray -> child) -> next == NULL)
    {
        tmp -> next = item;
    }
    else
    {
        while (tmp -> next != NULL)
            tmp = tmp -> next;
        tmp -> next = item;
        item -> pre = tmp;        
    }
}

void JSONc_printUnformatted2Console(JSONc *source)
{
    if (source -> type == single)
    {
        if (strcmp(source -> name, ""))
            printf("\"%s\":", source -> name);
        printf("\"%s\"", source -> stringVal);
        if (source -> next != NULL)
        {
            printf(",");
            JSONc_printUnformatted2Console(source -> next);
        }
    }
    else if (source -> type == array)
    {
        if (strcmp(source -> name, ""))
            printf("\"%s\":", source -> name);
        printf("[");
        if (source -> child != NULL)
            JSONc_printUnformatted2Console(source -> child);
        printf("]");
        if (source -> next != NULL)
        {
            printf(",");
            JSONc_printUnformatted2Console(source -> next);
        }
    }
    else if (source -> type == object)
    {
        if (strcmp(source -> name, ""))
            printf("\"%s\":", source -> name);
        printf("{");
        if (source -> child != NULL)
            JSONc_printUnformatted2Console(source -> child);
        printf("}");    
        if (source -> next != NULL)
        {
            printf(",");
            JSONc_printUnformatted2Console(source -> next);
        }   
    }
}

char *JSONc_printUnformatted (JSONc *source)
{
    char *result = calloc(1, 1000);
    if (source -> type == single)
    {
        if (strcmp(source -> name, ""))
        {
            strcat(result, "\"");
            strcat(result, source -> name);
            strcat(result, "\":");
        }
        strcat(result, "\"");
        strcat(result, source -> stringVal);
        strcat(result, "\"");
        if (source -> next != NULL)
        {
            strcat(result, ",");
            strcat(result, JSONc_printUnformatted(source -> next));
        }
    }
    else if (source -> type == array)
    {
        if (strcmp(source -> name, ""))
        {
            strcat(result, "\"");
            strcat(result, source -> name);
            strcat(result, "\":");
        }
        strcat(result, "[");
        if (source -> child != NULL)
            strcat(result, JSONc_printUnformatted(source -> child));
        strcat(result, "]");
        if (source -> next != NULL)
        {
            strcat(result, ",");
            strcat(result, JSONc_printUnformatted(source -> next));
        }
    }
    else if (source -> type == object)
    {
        if (strcmp(source -> name, ""))
        {
            strcat(result, "\"");
            strcat(result, source -> name);
            strcat(result, "\":");
        }
        strcat(result, "{");
        if (source -> child != NULL)
            strcat(result, JSONc_printUnformatted(source -> child));
        strcat(result, "}"); 
        if (source -> next != NULL)
        {
            strcat(result, ",");
            strcat(result, JSONc_printUnformatted(source -> next));
        }   
    }
    //free(result);
    return result;
}

JSONc *JSONc_parseNotFirst(char *source, int *i)
{
    JSONc *result;
    if (source[(*i)] == '"')
    {
        (*i)++;
        int k = 0;
        char tmpChar[100];
        while ((tmpChar[k++] = source[(*i)++]) != '"');
        tmpChar[k-1] = 0;
        if (source[(*i)] == ':')
        {
            (*i)++;
            result = JSONc_parseNotFirst(source, i);
            strcpy(result -> name, tmpChar);
        }
        else
            result = JSONc_createString(tmpChar);
    }
    else if (source[(*i)] == '[')
    {
        result = JSONc_createArray();
        JSONc *tmp;
        if (source[++(*i)] != ']')
        {
            tmp = JSONc_parseNotFirst(source, i);
            result -> child = tmp;
        }
        while (source[(*i)] != ']')
        {
            (*i)++;
            tmp -> next = JSONc_parseNotFirst(source, i);
            tmp = tmp -> next;
        }
        (*i)++;
    }
    else if (source[(*i)] == '{')
    {
        result = JSONc_createObject();
        JSONc *tmp;
        if (source[++(*i)] != '}')
        {
            tmp = JSONc_parseNotFirst(source, i);
            result -> child = tmp;
        }
        while (source[(*i)] != '}')
        {
            (*i)++;
            tmp -> next = JSONc_parseNotFirst(source, i);
            tmp = tmp -> next;
        }
        (*i)++;
    }
    return result;
}

JSONc *JSONc_parse(char *source)
{
    int *i = calloc(1, sizeof(int));
    *i = 0;
    JSONc *result = JSONc_parseNotFirst(source, i);
    free (i);
    return result;
}

int JSONc_getArraySize(JSONc *array)
{
    JSONc *tmp = array -> child;
    int i = 0;
    while (tmp != NULL)
    {
        tmp = tmp -> next;
        i++;
    }        
    return i;
}

JSONc *JSONc_getObjectItem(JSONc *source, char *name)
{
    JSONc *tmp = source -> child;
    while (tmp != NULL)
    {
        if (!strcmp(name, tmp -> name))
            return tmp;
        tmp = tmp -> next;
    }
    return NULL;
}

JSONc *JSONc_getArrayItem(JSONc *array, int index)
{
    if (index >= JSONc_getArraySize(array))
        return NULL;
    JSONc *tmp = array -> child;
    for (int i = 0; i < index; i++)
        tmp = tmp -> next;
    return tmp;
}

void JSONc_delete(JSONc *ptr)
{
    if (ptr -> child != NULL)
        JSONc_delete(ptr -> child);
    if (ptr -> next != NULL)
        JSONc_delete(ptr -> next);
    free(ptr);
}
