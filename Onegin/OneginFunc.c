/*
 * OneginFunc.c
 *
 *  Created on: Oct 14, 2019
 *      Author: artem
 */

#include "OneginFunc.h"
#include "OneginConsts.h"

//Read from file "filename" and create char* with file contents
char* CreateBuffer(char* filename){
    assert(filename != NULL);
    FILE *rf = fopen(filename, "rb");
    assert(rf != NULL);
    fseek(rf, 0, SEEK_END);
    long int flen = ftell(rf); //get file length in bytes

    //create and fill the buffer
    fseek(rf, 0, SEEK_SET);
    char* buff = (char*)malloc((flen+1)*sizeof(char));
    assert(buff != NULL);
    long int freadcounter;
    char* supbuff = (char*)malloc((flen+1)*sizeof(char));
    assert(supbuff != NULL);
    for(freadcounter = 0; freadcounter < flen; ) {
        long int i = 0;
        i = (long int)fread(supbuff, sizeof(char), flen - freadcounter, rf);
        memcpy((buff + freadcounter), supbuff, i);
        freadcounter += i;
    }
    //while(fread(buff, sizeof(char), flen, rf) != (size_t)flen){}
    free(supbuff);
    fclose(rf);
    buff[flen] = '\0';
    return buff;
}

//Fill structs line* from the buffer, also get number of structs
line* ParseBuffer(char* buffer, ll* numline){
    assert(buffer != NULL);
    line* lines = (line*)malloc(step * sizeof(line));
    assert(lines != NULL);
    *numline = 0;

    char* previous = buffer;
    char* current  = buffer;
    while(current != NULL && (*previous) != '\0'){
        current = strchr(previous, '\n');                   //get pointer to the ending of previous line
        lines[*numline].length = current - previous + 1;    //set length for line
        lines[*numline].str = previous; //set line
        previous = current + 1;
        (*numline)++;
        if((*numline) % step == 0) lines = (line*)realloc(lines, (step + *numline) * sizeof(line)); //increase number of lines
        assert(lines != NULL);
    }
    lines = (line*)realloc(lines, (*numline + 1) * sizeof(line)); //set memory in scope
    assert(lines != NULL);
    lines[*numline].str    = NULL; //set null last line
    lines[*numline].length = -1;
    return lines;
}

//Get number of lines by search for last (NULL) line
ll CountLines(line* lines){
    assert(lines != NULL);
    ll c = 0;
    while(lines[c].str != NULL) c++;
    return c;
}

//Support function, delete symbols (sym) from string (from)
int DelSymbols(char* from, char* sym){
    assert(from != NULL);
    assert(sym  != NULL);

    int i = 0,
        j = 0,
        l = 0;
    int flag = 1;
    for(;i < (int)strlen(from); i++){
        flag = 1;
        for(j = 0; j < (int)strlen(sym); j++){
            if(from[i] == sym[j]) flag = 0;
            break;
        }

        if(flag){
            from[l] = from[i];
            l++;
        }
    }
    from[l]     = '\n';
    from[l + 1] = '\0';
    return 0;
}

//Check function. Don't accept lines without any LETTERS.
int IsLine(line l){
    assert(l.str != NULL);
    char* a = (char*)calloc(l.length + 1, sizeof(char));
    assert(a != NULL);
    strncpy(a, l.str, l.length);
    a[l.length] = '\0';

    //Check free lines
    if(strcmp(a, "\n\0") == 0){
        free(a);
        return 0;
    }

    //Check lines without letters
    int li = 0;
    int flag = 1;
    for(int i = 0; i < (int)strlen(a); i++){
        flag = isalpha(a[i]);
        if(flag){
            a[li] = a[i];
            li++;
        }
    }
    a[li]     = '\n';
    a[li + 1] = '\0';

    if (a[0] == '\n'){
        free(a);
        return 0;
    }
    free(a);
    return 1;
}

//Compare two structs (< = >) == (-1 0 1)
int Compare(line s1, line s2){
    assert(s1.str != NULL);
    assert(s2.str != NULL);
    if(s1.str == s2.str) return 0;
    int len = 0;
    int len1 = s1.length;
    int len2 = s2.length;

    if(len1 <= len2) len = len1; //save lens first time and then use
    else len = len2;
    int i = 0;
    while((s1.str)[i] == (s2.str)[i]) {
        i++;
        if(i == len) break;
    }

    if(len1 == i && len1 == i) return 0; //equal
    if(len1 == i) return -1; //if one string end
    if(len2 == i) return 1;
    if(((s2.str)[i] == ' ' || (s2.str)[i] == '\n')
    && ((s1.str)[i] == ' ' || (s1.str)[i] == '\n'))  return 0;

    if((s1.str)[i] == ' ' || (s1.str)[i] == '\n')  return -1;
    if((s2.str)[i] == ' ' || (s2.str)[i] == '\n')  return 1;

    if((s1.str)[i] > (s2.str)[i]) return 1;
    if((s1.str)[i] < (s2.str)[i]) return -1;
    return 1;
}

//Compare two structs by their endings (< = >) == (-1 0 1)
int CompareEnd(line s1, line s2){
    assert(s1.str != NULL);
    assert(s2.str != NULL);
    if(s1.str == s2.str) return 0;

    int len1 = s1.length;
    int len2 = s2.length;
    if(len1 < 2 && len2 < 2) return 0;
    if(len1 < 2) return -1;
    if(len2 < 2) return 1;

    while(isalpha((s1.str)[len1-1]) == 0 && len1-2 > 0)len1--; //delete other symbols
    while(isalpha((s2.str)[len2-1]) == 0 && len2-2 > 0)len2--;
    if(len1 < 2) return -1;
    if(len2 < 2) return 1;

    if((s1.str)[len1 - 2] > (s2.str)[len2 - 2]) return 1; //check last 2 letters
    if((s1.str)[len1 - 2] < (s2.str)[len2 - 2]) return -1;
    if((s1.str)[len1 - 1] > (s2.str)[len2 - 1]) return 1;
    if((s1.str)[len1 - 1] < (s2.str)[len2 - 1]) return -1;
    return 0;
}

//Quick sort array of structs
int quicksort(line* l, ll first, ll last, int order){
    assert(l != NULL);
    int (*cmp)(line s1, line s2);
    if(order == CMP_END) cmp = CompareEnd;
    else cmp = Compare;
    if(first >= last) return 1;
    ll left  =  first;
    ll right =  last;
    line middle = l[(first + last)/2];
    int bool1 = 0;

    while(left <= right){
        while((bool1 = cmp(middle, l[left])) == 1) left++;
        while((bool1 = cmp(l[right], middle)) == 1) right--;
        if(left > right) break;

        line a   = l[left];
        l[left]  = l[right];
        l[right] = a;
        left++;
        right--;
    }

    quicksort(l, first, right, order);
    quicksort(l, left, last, order);
    return 0;
}

// Parse the Onegin's rhyme, set lines with same endings nearby
int ParseRhyme(line* l){
    assert(l != NULL);
    int j = 0;
    ll i  = 0;

    while(l[i].str != NULL){
        if(! IsLine(l[i])){
            i++;
            continue;
        }
        if(j == 14) j = 0;
        if(j == 0){ // Set desired order for 0-3 numbers of strings
            line sup = l[i + 1];
            l[i + 1] = l[i + 2];
            l[i + 2] = sup;
        }
        if(j == 8){ //Set desired order for 9-12 numbers of strings
            line sup = l[i + 1];
            l[i + 1] = l[i + 3];
            l[i + 3] = l[i + 2];
            l[i + 2] = sup;
        }
        j++;
        i++;
    }
    return 1;
}

//Parse by function ParseRhyme() and print to file in random order
//WARNING: it needs Onegins rhyme!
int PrintRaveToFile(line *l, ll length, FILE* wf){
    assert(l  != NULL);
    assert(wf != NULL);
    ParseRhyme(l);
    ll NumOfLines = GetOnlyText(l, length) + 1; // Set in first part - text, into second - other lines and get number of last line
    ll *id = (ll*)calloc(NumOfLines, sizeof(ll)); // support array
    for(ll i = 0; i < NumOfLines; i++) id[i] = i;

    //Generate Rave
    for(ll i = 0; i < NumOfLines / 2; i++){
        ll nl = rand() % NumOfLines;
        while(id[nl] == -1) nl = rand() % NumOfLines; //get random index of string
        if(nl % 2 == 1) nl--;

        if(i  % enter2 == 0 && i != 0) fprintf(wf, "\n"); // print enters for better vision
        fwrite(l[id[nl]].str, sizeof(char), l[id[nl]].length, wf);
        fwrite(l[id[nl+1]].str, sizeof(char), l[id[nl+1]].length, wf);
        id[nl]     = -1;
        id[nl + 1] = -1;
    }
    free(id);
    return 0;
}

// Set in first part - text, into second - other lines and get end of text
ll GetOnlyText(line *l, ll length){
    assert(l != NULL);
    ll i  = 0;
    ll nl = length;
    while(i < nl){
        if(!IsLine(l[i])){ //finds line without letters
            line sup = l[i];
            for(int k = i; k < nl-1; k++){
                l[k] = l[k+1];
            }
            nl--;
            l[nl] = sup;
            i--;
        }
        i++;
    }
    return nl - 1;
}

