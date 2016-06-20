/*
* POSIX tsearch(3) example
* Copyright 2016 Jonathan Bailey
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
*/

#include <stdio.h>   /* printf */
#include <string.h>  /* strcmp, strcpy, strlen */
#include <stdlib.h>  /* malloc */
#include <search.h>  /* tsearch, twalk */


/* tsearch(3) tree root pointer, and metadata as desired */
typedef struct {
    void *root;
    int size;
} Tree;
typedef struct {
    char *name;
    char *color;
} Fruit;
void add_fruit(Tree *, char *, char *);
int f_cmp(const void *, const void *);
void fruitwalk (const void *, VISIT, int);
void sfruitwalk (const void *, VISIT, int);
void ft_destroy(Tree *);


int
main(int argc, char **argv)
{
    Tree fruit_tree;
    fruit_tree.size = 0;
    fruit_tree.root = NULL;
    (void)argc;
    (void)argv;

    add_fruit(&fruit_tree, "Honeydew", "Blue");
    add_fruit(&fruit_tree, "Fig", "Blue");
    add_fruit(&fruit_tree, "Pineapple", "Blue");
    add_fruit(&fruit_tree, "Salak", "Blue");
    add_fruit(&fruit_tree, "Avocado", "Blue");
    add_fruit(&fruit_tree, "Cantaloupe", "Blue");
    add_fruit(&fruit_tree, "Apple", "Blue");
    add_fruit(&fruit_tree, "Clementine", "Blue");
    add_fruit(&fruit_tree, "Jujube", "Blue");
    add_fruit(&fruit_tree, "Jackfruit", "Blue");
    add_fruit(&fruit_tree, "Rambutan", "Yellow");
    add_fruit(&fruit_tree, "Plum", "Yellow");
    add_fruit(&fruit_tree, "Tomato", "Yellow");
    add_fruit(&fruit_tree, "Jabuticaba", "Yellow");
    add_fruit(&fruit_tree, "Avocado", "Blue");
    add_fruit(&fruit_tree, "Cantaloupe", "Blue");
    add_fruit(&fruit_tree, "Apple", "Blue");
    add_fruit(&fruit_tree, "Avocado", "Yellow");
    add_fruit(&fruit_tree, "Cantaloupe", "Yellow");
    add_fruit(&fruit_tree, "Apple", "Blue");
    add_fruit(&fruit_tree, "Peach", "Yellow");
    add_fruit(&fruit_tree, "Nance", "Yellow");
    add_fruit(&fruit_tree, "Olive", "Yellow");
    add_fruit(&fruit_tree, "Mulberry", "Yellow");
    add_fruit(&fruit_tree, "Salmonberry", "Yellow");
    add_fruit(&fruit_tree, "Cranberry", "Yellow");

    twalk(fruit_tree.root,fruitwalk);
    twalk(fruit_tree.root,sfruitwalk);

    /* Free up all resources used in the tree */
    ft_destroy(&fruit_tree);

    return (1);
}


void
add_fruit(Tree *tree, char *name, char *color) {
    Fruit *fruitp, **testpp;

    /* Allocate & populate a Fruit & members from parameters. */
    fruitp        = (Fruit *)malloc(sizeof(Fruit));
    fruitp->name  =  (char *)malloc(strlen(name) + 1);  /* remember the '\0' */
    fruitp->color =  (char *)malloc(strlen(color) + 1);
    strcpy(fruitp->name, name);
    strcpy(fruitp->color, color);

    /* Try adding Fruit's pointer to the tree.  We get a pointer to "something" back. */
    testpp = tsearch(fruitp, &(tree->root), f_cmp);

    /* If "something" is the pointer we put in, our reference is in the tree, hooray! */
    if( *testpp == fruitp ) {
        tree->size += 1;
        printf("%s %s added to tree. (Total: %d)\n",
                fruitp->color, fruitp->name, tree->size);
    } else {
        /* Otherwise, "something" points to what the tree "already had" */
        printf("%s %s rejected, tree already has %s %s\n",
                fruitp->color, fruitp->name,
                (*testpp)->color, (*testpp)->name);

        /* Must free up the resources of the rejected Fruit, to not leak memory! ;) */
        free(fruitp->name);
        free(fruitp->color);
        free(fruitp);
    }
}


/* tsearch(3) function to compare two Fruit */
int
f_cmp(const void *a, const void *b)
{
    /*
     * Since our function only compares Fruit->name, a different color
     * Fruit will be considered "already in the tree", because the name
     * matches.  (By design for this example.)
     */
    return strcmp(((Fruit *)a)->name, ((Fruit *)b)->name);
}
/* Here's a more verbose version without casts:
 *
 * const Fruit *fa = a;
 * const Fruit *fb = b;
 *
 * const int result = (strcmp(fa->name,fb->name));
 *
 * printf("comparing %s with %s - result %d\n", fa->name, fb->name, result);
 *
 * return (result);
 */


/* twalk() function - print Fruit tree nodes on postorder & leaf visits (that is, sorted) */
void
sfruitwalk(const void *node, VISIT visit, int level)
{
    const Fruit *fp = *(Fruit **)node;
    (void)level;

    switch(visit) {
        case postorder:
        case leaf:
            printf("%s (%s)\n", fp->name, fp->color);
            break;
        case preorder:
        case endorder:
            ;
    }
}
/* ...without casts:
 * const Fruit * const *fpp = node;
 * const Fruit *fp = *fpp;
 */


/* twalk() function - print each Fruit tree node details, to visualize the data structure */
void
fruitwalk(const void *node, VISIT visit, int level)
{
    const Fruit *fp = *(Fruit**)node;
    char *vtype = NULL;

    /*
     * node points to a structure in the tree.  Each node starts with a
     * pointer to Fruit, the rest of its contents are private data.
     *
     * So to the user, node is a pointer-to-pointer-to-Fruit.
     */

    switch(visit) {
        case preorder:  vtype = "preorder";  break;
        case postorder: vtype = "postorder"; break;
        case endorder:  vtype = "endorder";  break;
        case leaf:      vtype = "leaf";      break;
    }

    printf("Fruit Tree - %s visit at level %d - %s (%s)\n",
            vtype, level, fp->name, fp->color);
}


/*
 * Delete root node until it's NULL.  This might be sub-optimal speed wise.
 * GLibC users can use tdestroy(), and pass it a Fruit-freeing function.
 */
void
ft_destroy(Tree *tree)
{
    Fruit *fp;

    while(tree->root != NULL) {
        /* Grab the root node's fruit. */
        fp = *((Fruit**)(tree->root));

        /* Remove the tree entry */
        tdelete(fp, &(tree->root), f_cmp);

        /* Free its resources */
        free(fp->name);
        free(fp->color);
        free(fp);
    }
}
