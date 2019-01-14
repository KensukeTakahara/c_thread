#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_PRIME_NUMBERS 100000

int primeNumber[MAX_PRIME_NUMBERS];
int nPrimeNumber;
int primeNumberChecked;
pthread_mutex_t usingPrimeNumber;

int isPrimeNumber(int m) {
  int i;
  for (i = 0; i < nPrimeNumber; ++i) {
    if (primeNumber[i] > m / 2) {
      return 1;
    }
    if (m % primeNumber[i] == 0) {
      return 0;
    }
  }
  return 1;
}

void generatePrimeNumbers(int n) {
  int i;

  pthread_mutex_lock(&usingPrimeNumber);

  if (n <= primeNumberChecked) {
    pthread_mutex_unlock(&usingPrimeNumber);
    return;
  }

  for (i = primeNumberChecked + 1; i <= n; ++i) {
    if (isPrimeNumber(i)) {
      if (nPrimeNumber >= MAX_PRIME_NUMBERS) {
        printf("Oops, too many prime numbers.\n");
        exit(1);
      }
      primeNumber[nPrimeNumber] = i;
      nPrimeNumber++;
    }
  }

  primeNumberChecked = n;

  pthread_mutex_unlock(&usingPrimeNumber);

  return;
}

int countPrimeNumbers(int n) {
  int count, i;

  generatePrimeNumbers(n);
  count = 0;
  for (i = 0; i < nPrimeNumber; ++i) {
    if (primeNumber[i] > n) {
      break;
    }
    count++;
  }
  return count;
}

void* threadFunc(void* arg) {
  int n = (int)arg;
  int x;

  x = countPrimeNumbers(n);
  printf("Number of prime numbers under %d is %d.\n", n, x);

  return NULL;
}

int main() {
  int numberList[6] = {1, 10, 100, 1000, 100000, 1000000};
  pthread_t threads[6];
  int i;

  nPrimeNumber = 0;
  primeNumberChecked = 1;

  pthread_mutex_init(&usingPrimeNumber, NULL);

  for (i = 0; i < 6; ++i) {
    if (pthread_create(&threads[i], NULL, threadFunc, (void*)numberList[i]) !=
        0) {
      printf("Can't create thread (%d).\n", i);
      exit(1);
    }
  }

  for (i = 0; i < 6; ++i) {
    pthread_join(threads[i], NULL);
  }

  pthread_mutex_destroy(&usingPrimeNumber);

  printf("Done.\n");

  return 0;
}