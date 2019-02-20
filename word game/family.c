#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "family.h"

/* Number of word pointers allocated for a new family.
   This is also the number of word pointers added to a family
   using realloc, when the family is full.
*/
static int family_increment = 0;


/* Set family_increment to size, and initialize random number generator.
   The random number generator is used to select a random word from a family.
   This function should be called exactly once, on startup.
*/
void init_family(int size) {
    family_increment = size;
    srand(time(0));
}


/* Given a pointer to the head of a linked list of Family nodes,
   print each family's signature and words.

   Do not modify this function. It will be used for marking.
*/
void print_families(Family* fam_list) {
    int i;
    Family *fam = fam_list;
    
    while (fam) {
        printf("***Family signature: %s Num words: %d\n",
               fam->signature, fam->num_words);
        for(i = 0; i < fam->num_words; i++) {
            printf("     %s\n", fam->word_ptrs[i]);
        }
        printf("\n");
        fam = fam->next;
    }
}


/* Return a pointer to a new family whose signature is 
   a copy of str. Initialize word_ptrs to point to 
   family_increment+1 pointers, numwords to 0, 
   maxwords to family_increment, and next to NULL.
*/
Family *new_family(char *str) {
    Family *newFamily = malloc(sizeof(Family));
    if (newFamily == NULL) {
        perror("malloc");
        exit(1);
    }
    newFamily -> signature = malloc((strlen(str) + 1) * sizeof(char));
    if ((newFamily -> signature) == NULL) {
        perror("malloc");
        exit(1);
    }
    strcpy(newFamily -> signature, str);
    newFamily -> num_words = 0;
    newFamily -> word_ptrs = malloc((family_increment + 1) * sizeof(char*));
    if ((newFamily -> word_ptrs) == NULL) {
        perror("malloc");
        exit(1);
    }
    newFamily -> word_ptrs[family_increment] = NULL;
    newFamily -> max_words = family_increment;
    newFamily -> next = NULL;
    return newFamily;
}


/* Add word to the next free slot fam->word_ptrs.
   If fam->word_ptrs is full, first use realloc to allocate family_increment
   more pointers and then add the new pointer.  
*/
void add_word_to_family(Family *fam, char *word) {
    if((fam -> num_words) < (fam -> max_words)){
        fam -> word_ptrs[fam -> num_words] = word;
        fam -> num_words++;
    }else{
        (fam -> max_words) += family_increment;
        (fam -> word_ptrs) = realloc(fam -> word_ptrs, ((fam -> max_words) + 1) * sizeof(char*));
        fam -> word_ptrs[fam -> max_words] = NULL;
        fam -> word_ptrs[fam -> num_words] = word;
        fam -> num_words++;
    }
    return;
}


/* Return a pointer to the family whose signature is sig;
   if there is no such family, return NULL.
   fam_list is a pointer to the head of a list of Family nodes.
*/
Family *find_family(Family *fam_list, char *sig) {
    while(fam_list != NULL){
        if((fam_list -> signature) == sig){
            return fam_list;
        }else{
            fam_list++;
        }
    }
    return NULL;
}


/* Return a pointer to the family in the list with the most words;
   if the list is empty, return NULL. If multiple families have the most words,
   return a pointer to any of them.
   fam_list is a pointer to the head of a list of Family nodes.
*/
Family *find_biggest_family(Family *fam_list) {
    int tempMax = (fam_list -> num_words);
    Family *tempFam = fam_list;
    while(fam_list != NULL){
        int currLen = (fam_list -> num_words);
        if(currLen > tempMax){
            tempMax = currLen;
            tempFam = fam_list;
        }
        fam_list = fam_list -> next;
    }
    return tempFam;
}


/* Deallocate all memory rooted in the List pointed to by fam_list. */
void deallocate_families(Family *fam_list) {
    Family *curr = fam_list;
    while(curr != NULL){
        free(curr -> word_ptrs);
        free(curr -> signature);
        Family *tmp = curr->next;
        free(curr);
        curr = tmp;
    }
    return;
}


char *generate_signature(char *word, char letter){
    int i = 0;
    int length = strlen(word);
    char *signature = malloc((length + 1) * sizeof(char*));
    if (signature == NULL) {
        perror("malloc");
        exit(1);
    }
    while(word[i] != '\0'){
        if(word[i] == letter){
            signature[i] = letter;
        }else{
            signature[i] = '-';
        }
        i++;
    }
    signature[i] = '\0';
    return signature;
}

/* Generate and return a linked list of all families using words pointed to
   by word_list, using letter to partition the words.

   Implementation tips: To decide the family in which each word belongs, you
   will need to generate the signature of each word. Create only the families
   that have at least one word from the current word_list.
*/
Family *generate_families(char **word_list, char letter) {
    int word_index = 0;
    char* word_signature = generate_signature(word_list[word_index], letter);
    Family *newFamily = new_family(word_signature);
    Family *famhead = newFamily;
    add_word_to_family(newFamily, word_list[word_index]);
    word_index++;
    free(word_signature);
    while(word_list[word_index] != NULL){
        word_signature = generate_signature(word_list[word_index], letter);
        Family *tempFamily = famhead;
        int find = 0;
        while(tempFamily != NULL){
            if(strcmp(tempFamily -> signature, word_signature)==0){
                add_word_to_family(tempFamily, word_list[word_index]);
                find = 1;
                break;
            }
            tempFamily = tempFamily -> next;
        }
        if(find == 0){
            Family *nextFamily = new_family(word_signature);
            add_word_to_family(nextFamily, word_list[word_index]);
            nextFamily -> next = famhead;
            famhead = nextFamily;
        }
        word_index++;
        free(word_signature);
    }
    
    return famhead;
}

/* Return the signature of the family pointed to by fam. */
char *get_family_signature(Family *fam) {
    return fam -> signature;
}


/* Return a pointer to word pointers, each of which
   points to a word in fam. These pointers should not be the same
   as those used by fam->word_ptrs (i.e. they should be independently malloc'd),
   because fam->word_ptrs can move during a realloc.
   As with fam->word_ptrs, the final pointer should be NULL.
*/
char **get_new_word_list(Family *fam) {
    int len = fam -> num_words;
    char **new_word_list = malloc((len+1) * sizeof(char*));
    if (new_word_list == NULL) {
        perror("malloc");
        exit(1);
    }
    for(int i = 0; i < len; i++){
        new_word_list[i] = (fam -> word_ptrs)[i];
    }
    new_word_list[len] = NULL;
    return new_word_list;
}


/* Return a pointer to a random word from fam. 
   Use rand (man 3 rand) to generate random integers.
*/
char *get_random_word_from_family(Family *fam) {
    int n = rand() % (fam -> num_words);
    return (*fam).word_ptrs[n];
}
