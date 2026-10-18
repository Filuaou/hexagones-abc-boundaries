/* Increment 19: original C dual-boundary extraction, relocated verbatim.
   This file is included by NSWorldView2.m and is not an independent build target.
   No algorithmic changes; preserve the original fixed arrays and conventions. */

typedef struct {
    int             x1, y1, x2, y2;
}               dualPair;

    int
compareDualPairs(const void * left, const void * right)
{
    const dualPair *a = (const dualPair *)left;
    const dualPair *b = (const dualPair *)right;

    /* Compare the original (y, x) endpoint ordering, without collapsing
       coordinates into y * 100 + x.  Equal pairs must compare equal. */
    if (a->y1 != b->y1) return (a->y1 > b->y1) ? 1 : -1;
    if (a->x1 != b->x1) return (a->x1 > b->x1) ? 1 : -1;
    if (a->y2 != b->y2) return (a->y2 > b->y2) ? 1 : -1;
    if (a->x2 != b->x2) return (a->x2 > b->x2) ? 1 : -1;
    return 0;
}

void extractFromPrimalMultipleDualEnclosingPaths(superpath * setOfPrimalElements, superpath dualEnclosingPaths[10], int * numberOfPathsFound)
{
    /* No boundary exists for an empty primal set. Avoid reading an
       uninitialized segment from the local arrays below. */
    if (setOfPrimalElements->length == 0) {
        *numberOfPathsFound = 0;
        return;
    }

    /* Check the next routine for a general description of how a dual path is extracted from
     * a primal path. The difference here is that there is a priori no reason why there should
     * be a unique dual path. This means that we must keep track of the number of dual paths extracted.
     * Since extracting a dual path is performed after suppressing all duplicates, we first copy what
     * remains to be extracted in a local path variable
     */

    /*
     * extract from setOfPrimalElements all boundaries (6 for each) and
     * store them in setOfPairsInDual; recall that x1 y1 stands for
     * point-of-hex and x2 y2 stand for tail-of-hex.
     */

    dualPair    setOfPairsInDual[6 * maxSquaresForSuperpath*20], setOfPairsCopy[6 * maxSquaresForSuperpath*20];

    int i, j, k, le, pA, pB, pC, pA1, pB1, pC1, posXdual, posYdual,
        posX2, posY2, temporaryPointX, temporaryPointY, stopCond, start;
    
    superpath *dualEnclosingPath;

    for (i = 0, j = 0; j < (*setOfPrimalElements).length; j++) {
        convertXY2ABCn((*setOfPrimalElements).x[j], (*setOfPrimalElements).y[j], &pA, &pB, &pC);
        normalizeABC(&pA, &pB, &pC);

        /* right neighbour link */
        convertABC2XY(pA + 1, pB, pC+1, &(setOfPairsInDual[i].x1), &(setOfPairsInDual[i].y1));
        convertABC2XY(pA + 1, pB+1, pC, &(setOfPairsInDual[i].x2), &(setOfPairsInDual[i].y2));
        i++;

        /* second neighbour link */
        convertABC2XY(pA, pB, pC, &(setOfPairsInDual[i].x1), &(setOfPairsInDual[i].y1));
        convertABC2XY(pA+1, pB+1, pC, &(setOfPairsInDual[i].x2), &(setOfPairsInDual[i].y2));
        i++;

        /* third neighbour link */
        convertABC2XY(pA, pB, pC, &(setOfPairsInDual[i].x1), &(setOfPairsInDual[i].y1));
        convertABC2XY(pA, pB+1, pC, &(setOfPairsInDual[i].x2), &(setOfPairsInDual[i].y2));
        i++;

        /* fourth neighbour link */
        convertABC2XY(pA, pB, pC+1, &(setOfPairsInDual[i].x1), &(setOfPairsInDual[i].y1));
        convertABC2XY(pA, pB+1, pC, &(setOfPairsInDual[i].x2), &(setOfPairsInDual[i].y2));
        i++;

        /* fifth neighbour link */
        convertABC2XY(pA, pB, pC + 1, &(setOfPairsInDual[i].x1), &(setOfPairsInDual[i].y1));
        convertABC2XY(pA, pB, pC, &(setOfPairsInDual[i].x2), &(setOfPairsInDual[i].y2));
        i++;

        /* sixth neighbour link */
        convertABC2XY(pA+1, pB, pC+1, &(setOfPairsInDual[i].x1), &(setOfPairsInDual[i].y1));
        convertABC2XY(pA, pB, pC, &(setOfPairsInDual[i].x2), &(setOfPairsInDual[i].y2));
        i++;
    }

    /* sort setOfPrimalElements */

    qsort(setOfPairsInDual, (size_t) 6 * (*setOfPrimalElements).length, sizeof(dualPair),
            compareDualPairs);

    /* remove duplicates */

    i = 0;
    j = 0;

    while (i < 6 * (*setOfPrimalElements).length - 1) {
        /* test if the next element is identical */

        while (
                ((setOfPairsInDual[i].x1 == setOfPairsInDual[i + 1].x1) &&
                 (setOfPairsInDual[i].y1 == setOfPairsInDual[i + 1].y1) &&
                 (setOfPairsInDual[i].x2 == setOfPairsInDual[i + 1].x2) &&
                 (setOfPairsInDual[i].y2 == setOfPairsInDual[i + 1].y2)) &&
                i < 6 * (*setOfPrimalElements).length - 2) {

            /* if yes, skip both */
            i += 2;
        }
        /* else copy the element */
        setOfPairsCopy[j] = setOfPairsInDual[i];
        i++;
        j++;
    }
    le = j; /* store the max number of setOfPairsCopy */
    setOfPairsCopy[j] = setOfPairsInDual[i];
    /* j is the index in setOfPairs */

    /* reconstruct the dual paths (i.e. the enclosing paths) 
     *
     * now copy setOfPairsCopy back to *dualEnclosingPath, adjusting
     * length accordingly
     */

    stopCond = 1;
    dualEnclosingPath = &(dualEnclosingPaths[0]);
    *numberOfPathsFound = 1;

    stopCond = 0;

    start = 0; // start indicates the beginning of the next loop to be extracted
    while (!stopCond) {
        (*dualEnclosingPath).x[0] = setOfPairsCopy[start].x1;
        (*dualEnclosingPath).y[0] = setOfPairsCopy[start].y1;
        
        i = 0; 
        k = start; /* k is used to store the value of the current matching element, i.e. equals j temporarily */
        do {
            j = start; i++;
            if (i >= maxSquaresForSuperpath) { *numberOfPathsFound = 0; return; }
            if ((i+1)%2) { // 2000 is used to indicatethat the current value has been treated (hence suppressed)
                while (j <= le && ((setOfPairsCopy[j].x1 != (setOfPairsCopy[k]).x1) ||
                        setOfPairsCopy[j].x1 == 2000 || setOfPairsCopy[j].y1 == 2000 ||
                        (setOfPairsCopy[j].y1 !=  (setOfPairsCopy[k]).y1) || (j == k))) j++;
                if (j > le) { *numberOfPathsFound = 0; return; }
                (*dualEnclosingPath).x[i] = setOfPairsCopy[j].x1;
                (*dualEnclosingPath).y[i] = setOfPairsCopy[j].y1;	
                setOfPairsCopy[j].x1 = 2000; setOfPairsCopy[j].y1 = 2000;
                setOfPairsCopy[k].x1 = 2000; setOfPairsCopy[k].y1 = 2000;
            }		
            else {
                while (j <= le && ((setOfPairsCopy[j].x2 != (setOfPairsCopy[k]).x2) ||
                        setOfPairsCopy[j].x2 == 2000 || setOfPairsCopy[j].y2 == 2000 ||
                        (setOfPairsCopy[j].y2 != (setOfPairsCopy[k]).y2) || (j == k))) j++;
                if (j > le) { *numberOfPathsFound = 0; return; } 
                (*dualEnclosingPath).x[i] = setOfPairsCopy[j].x2;
                (*dualEnclosingPath).y[i] = setOfPairsCopy[j].y2;
                setOfPairsCopy[j].x2 = 2000; setOfPairsCopy[j].y2 = 2000; 
                setOfPairsCopy[k].x2 = 2000; setOfPairsCopy[k].y2 = 2000; 
            } // until loops back to first element
            k = j;
        } while (((*dualEnclosingPath).x[i] != (*dualEnclosingPath).x[0]) || 
                ((*dualEnclosingPath).y[i] != (*dualEnclosingPath).y[0]));

        (*dualEnclosingPath).length = i+1;

        j = -1; //look for next unprocessed element
        do {
            j++;
            stopCond = setOfPairsCopy[j].x1 == 2000 && setOfPairsCopy[j].y1 == 2000 &&
                setOfPairsCopy[j].x2 == 2000 && setOfPairsCopy[j].y2 == 2000;	
        } while (j<le && stopCond);
        if (!stopCond) {
            if (*numberOfPathsFound >= 10) { *numberOfPathsFound = 0; return; }
            dualEnclosingPath=&(dualEnclosingPaths[*numberOfPathsFound]);
            (*numberOfPathsFound)++;
            start = j; //the next loop has been detected and therefore update 'start'
        } 
        if (j == le) 
            stopCond=1;
    } 
}

void extractFromPrimalSetDualEnclosingPath(superpath * setOfPrimalElements, path * dualEnclosingPath)
{
    /* An empty set has no enclosing path. */
    if (setOfPrimalElements->length == 0) {
        dualEnclosingPath->length = 0;
        return;
    }

    /*
     * First a set of dual elements is constructed, each element of which
     * consists
     * of a pair of points in primal space. Elements of this set are the
     * six boundaries
     * of all corresponding primal elements passed initially in
     * setOfPrimalElements.
     * Once the dual pairs are obtained, they are sorted and then
     * duplicates are
     * eliminated. Finally, the dual path is obtained by following each
     * remaining
     * pair (after deletion of the duplicates). This yields the enclosing
     * path.
     */

    dualPair        setOfPairsInDual[6 * maxSquaresForSuperpath*30], setOfPairsCopy[6 * maxSquaresForSuperpath*30];

    int             i, j, k, pA, pB, pC, pA1, pB1, pC1, posXdual, posYdual,
                    posX2, posY2, temporaryPointX, temporaryPointY;

    /*
     * extract from setOfPrimalElements all boundaries (6 for each) and
     * store them in setOfPairsInDual; recall that x1 y1 stands for
     * point-of-hex and x2 y2 stand for tail-of-hex.
     */

    for (i = 0, j = 0; j < (*setOfPrimalElements).length; j++) {
        convertXY2ABCn((*setOfPrimalElements).x[j], (*setOfPrimalElements).y[j], &pA, &pB, &pC);
        normalizeABC(&pA, &pB, &pC);

        /* right neighbour link */
        convertABC2XY(pA + 1, pB, pC+1, &(setOfPairsInDual[i].x1), &(setOfPairsInDual[i].y1));
        convertABC2XY(pA + 1, pB+1, pC, &(setOfPairsInDual[i].x2), &(setOfPairsInDual[i].y2));
        i++;

        /* second neighbour link */
        convertABC2XY(pA, pB, pC, &(setOfPairsInDual[i].x1), &(setOfPairsInDual[i].y1));
        convertABC2XY(pA+1, pB+1, pC, &(setOfPairsInDual[i].x2), &(setOfPairsInDual[i].y2));
        i++;

        /* third neighbour link */
        convertABC2XY(pA, pB, pC, &(setOfPairsInDual[i].x1), &(setOfPairsInDual[i].y1));
        convertABC2XY(pA, pB+1, pC, &(setOfPairsInDual[i].x2), &(setOfPairsInDual[i].y2));
        i++;

        /* fourth neighbour link */
        convertABC2XY(pA, pB, pC+1, &(setOfPairsInDual[i].x1), &(setOfPairsInDual[i].y1));
        convertABC2XY(pA, pB+1, pC, &(setOfPairsInDual[i].x2), &(setOfPairsInDual[i].y2));
        i++;

        /* fifth neighbour link */
        convertABC2XY(pA, pB, pC + 1, &(setOfPairsInDual[i].x1), &(setOfPairsInDual[i].y1));
        convertABC2XY(pA, pB, pC, &(setOfPairsInDual[i].x2), &(setOfPairsInDual[i].y2));
        i++;

        /* sixth neighbour link */
        convertABC2XY(pA+1, pB, pC+1, &(setOfPairsInDual[i].x1), &(setOfPairsInDual[i].y1));
        convertABC2XY(pA, pB, pC, &(setOfPairsInDual[i].x2), &(setOfPairsInDual[i].y2));
        i++;
    }

    /* sort setOfPrimalElements */

    qsort(setOfPairsInDual, (size_t) 6 * (*setOfPrimalElements).length, sizeof(dualPair),
            compareDualPairs);

    /* remove duplicates */

    i = 0;
    j = 0;

    while (i < 6 * (*setOfPrimalElements).length - 1) {
        /* test if the next element is identical */

        while (
                ((setOfPairsInDual[i].x1 == setOfPairsInDual[i + 1].x1) &&
                 (setOfPairsInDual[i].y1 == setOfPairsInDual[i + 1].y1) &&
                 (setOfPairsInDual[i].x2 == setOfPairsInDual[i + 1].x2) &&
                 (setOfPairsInDual[i].y2 == setOfPairsInDual[i + 1].y2)) &&
                i < 6 * (*setOfPrimalElements).length - 2) {

            /* if yes, skip both */
            i += 2;
        }
        /* else copy the element */
        setOfPairsCopy[j] = setOfPairsInDual[i];
        i++;
        j++;
    }
    setOfPairsCopy[j] = setOfPairsInDual[i];

    /* reconstruct the dual path (i.e. the enclosing path) */
    /*
     * now copy setOfPairsCopy back to *dualEnclosingPath, adjusting
     * length accordingly
     */

    /* j is the index of the last surviving segment. */
    const int lastSegment = j;

    (*dualEnclosingPath).x[0] = setOfPairsCopy[0].x1;
    (*dualEnclosingPath).y[0] = setOfPairsCopy[0].y1;

    i = 0; k=0; /* k is used to store the value of the current matching element, i.e. equals j temporarily */
    do {
        j = 0; i++;
        if (i >= maxSquaresForSuperpath) { dualEnclosingPath->length = 0; return; }
        if ((i+1)%2) {
            while (j <= lastSegment && ((setOfPairsCopy[j].x1 != (setOfPairsCopy[k]).x1) || 
                    (setOfPairsCopy[j].y1 !=  (setOfPairsCopy[k]).y1) || (j == k))) j++;
            if (j > lastSegment) { dualEnclosingPath->length = 0; return; }
            (*dualEnclosingPath).x[i] = setOfPairsCopy[j].x1;
            (*dualEnclosingPath).y[i] = setOfPairsCopy[j].y1;	
        }		
        else {
            while (j <= lastSegment && ((setOfPairsCopy[j].x2 != (setOfPairsCopy[k]).x2) ||
                    (setOfPairsCopy[j].y2 != (setOfPairsCopy[k]).y2) || (j == k))) j++;
            if (j > lastSegment) { dualEnclosingPath->length = 0; return; }
            (*dualEnclosingPath).x[i] = setOfPairsCopy[j].x2;
            (*dualEnclosingPath).y[i] = setOfPairsCopy[j].y2;
        }
        k = j;
    } while (((*dualEnclosingPath).x[i] != (*dualEnclosingPath).x[0]) || 
            ((*dualEnclosingPath).y[i] != (*dualEnclosingPath).y[0]));
    (*dualEnclosingPath).length = i+1;
}

