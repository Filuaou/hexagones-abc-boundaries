/* Increment 20: original dual-coordinate transformations, moved verbatim.
   Included by NSWorldView2.m at their original position; not a separate target.
   Keep the C arithmetic, normalization, and coordinate conventions unchanged. */

void convertXY2ABCn(int x, int y, int *A, int *B, int *C) // the true new converting function
{
    x++; y++;
    *B = y;
    *A = x + floor(y/2);
    *C = 0;
}

    void
convertABC2XY(int A1, int B1, int C1, int *x, int *y)
{
    int A,B,C;
    A=A1; B=B1; C=C1;
    normalizeABC(&A,&B,&C);
    *x = A - (B + C) / 2 -1; //  - ((B-C+1000)%2);
    *y = B - C -1;
}

void normalizeABC(int *A, int *B, int *C)
{
    if ((*A < *B) && (*A < *C)) {*B -= *A; *C -= *A; *A -= *A;}
    else
        if (*B < *C) {*A -= *B; *C -= *B; *B -= *B;}
        else {*A -= *C; *B -= *C; *C -= *C;}
}

