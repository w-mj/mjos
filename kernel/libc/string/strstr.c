
#include <string.h>

// Function to implement strstr() function using KMP algorithm
char* strstr(const char* X, const char* Y)
{
    int m = strlen(X);
    int n = strlen(Y);
    // Base Case 1: Y is NULL or empty
    if (*Y == '\0' || n == 0)
        return (char *)X;

    // Base Case 2: X is NULL or X's length is less than that of Y's
    if (*X == '\0' || n > m)
        return NULL;

    // next[i] stores the index of next best partial match
    int next[n + 1];

    for (int i = 0; i < n + 1; i++)
        next[i] = 0;

    for (int i = 1; i < n; i++)
    {
        int j = next[i + 1];

        while (j > 0 && Y[j] != Y[i])
            j = next[j];

        if (j > 0 || Y[j] == Y[i])
            next[i + 1] = j + 1;
    }

    for (int i = 0, j = 0; i < m; i++)
    {
        if (*(X + i) == *(Y + j))
        {
            if (++j == n)
                return (char *)(X + i - j + 1);
        }
        else if (j > 0) {
            j = next[j];
            i--;	// since i will be incremented in next iteration
        }
    }

    return NULL;
}

