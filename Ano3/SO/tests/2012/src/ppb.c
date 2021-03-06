/* ppb.c
 *  The Hungry Birds Problem - pass arguments to threads and receive values from them when they finish!

 Program invocation:
 ./ppb <n. babybirds> <n. food portions> <n. refills>

 Invocation example:
 ./ppb 3 5 10000
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#define MAXBABIES 100

int finish = 0;	// termination of simulation (flagged by parent bird)
int foodbits = 0;	// is the current number of bits of food in the "food-teat"

struct parentarg {	// conveys info to parent bird
	int f;	// number of bits of food of each refill
	long r;	// number of refills - parent bird can then retire!
	int *working;	// ptr to binary state of parent bird
};
struct babyarg {	// conveys info to baby birds
	int id;	// baby identification
	int *eating;	// ptr to number of babies that are eating at a time
};

void *parent(void *);	// parent thread
void *baby(void *);	// baby thread

int main(int argc, char *argv[]) {
    setbuf(stdout, NULL);
	int BabyCount, FoodCap;
	long RefillCount;
    int Working = 0;
    int Eating = 0;
    struct parentarg ParentArg;
	struct babyarg BabyArgs[MAXBABIES];
	pthread_t ParentThread, BabyThreads[MAXBABIES];

    if(argc != 4) {
        printf("Program invoked with wrong number of arguments.\n");
        printf("Program usage: %s <n. babybirds> <n. food portions> <n. refills>\n", argv[0]);
        exit(-1);
    }
    else {
        BabyCount = atoi(argv[1]);
        FoodCap = atoi(argv[2]);
        RefillCount = atoi(argv[3]);
	}

    printf("\nSimulation started\n");

    ParentArg.f = FoodCap;
    ParentArg.r = RefillCount;
    ParentArg.working = &Working;
    pthread_create(&ParentThread, 0, parent, &ParentArg);

    for(int I = 0; I < BabyCount; ++I) {
        struct babyarg *BabyArg = BabyArgs + I;
        BabyArg->id = I;
        BabyArg->eating = &Eating;
        pthread_create(BabyThreads + I, 0, baby, BabyArg);
    }

    pthread_join(ParentThread, 0);
    for(int I = 0; I < BabyCount; ++I) {
        long *BitsEaten;
        pthread_join(BabyThreads[I], (void **)&BitsEaten);
        printf("\nNumber of bits of food eaten by baby %d / total of bits: %ld / %ld ",
               I, *BitsEaten, FoodCap*RefillCount);
        free(BitsEaten);
    }

    printf("\nSimulation finished\n");
    exit (0);
}

void *parent(void *arg) {
	struct parentarg Arg = *((struct parentarg *)arg);
    printf("\n\tParent starting");
    printf("\n\tParent received args f (%d), r (%ld), working (%d)", Arg.f, Arg.r, *Arg.working);
    printf("\n\tParent finishing");
    return 0;
}

void *baby(void *arg) {
	struct babyarg Arg = *((struct babyarg *)arg);
    printf("\n   Baby bird %d beginning", Arg.id);
    printf("\n   Baby received args id (%d), eating (%d)", Arg.id, *Arg.eating);

    long *Eaten = malloc(sizeof(*Eaten));
    *Eaten = 0;

    printf("\n   Baby bird %d finishing", Arg.id);
    return Eaten;
}
