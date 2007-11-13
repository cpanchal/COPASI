// Begin CVS Header
//   $Source: /Volumes/Home/Users/shoops/cvs/copasi_dev/copasi/optimization/CTruncatedNewton.cpp,v $
//   $Revision: 1.1 $
//   $Name:  $
//   $Author: jdada $
//   $Date: 2007/11/06 15:01:39 $
// End CVS Header

// Copyright (C) 2007 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc. and EML Research, gGmbH.
// All rights reserved.

/* tn.f -- translated by f2c (version 19970805).
   You must link the resulting object file with the libraries:
 -lf2c -lm   (in that order)
 */

#include "CTruncatedNewton.h"

#include <float.h>

/* Common Block Declarations */

union {
    struct
      {
        integer lgv, lz1, lzk, lv, lsk, lyk, ldiagb, lsr, lyr, loldg, lhg,
        lhyk, lpk, lemat, lwtest;
      }
    _1;
    struct
      {
        integer lgv, lz1, lzk, lv, lsk, lyk, ldiagb, lsr, lyr, lhyr, lhg,
        lhyk, lpk, lemat, lwtest;
      }
    _2;
    struct
      {
        integer lsub[14], lwtest;
      }
    _3;
  } subscr_;

#define subscr_1 (subscr_._1)
#define subscr_2 (subscr_._2)
#define subscr_3 (subscr_._3)

/* Table of constant values */

static integer c__1 = 1;
static logical c_false = FALSE_;
static logical c_true = TRUE_;
static doublereal c_b246 = .6666;

/* %% TRUNCATED-NEWTON METHOD:  SUBROUTINES */
/*   FOR OTHER MACHINES, MODIFY ROUTINE MCHPR1 (MACHINE EPSILON) */
/*   WRITTEN BY:  STEPHEN G. NASH */
/*                OPERATIONS RESEARCH AND APPLIED STATISTICS DEPT. */
/*                GEORGE MASON UNIVERSITY */
/*                FAIRFAX, VA 22030 */
/* ****************************************************************** */
/* Subroutine */ int tn_(integer *ierror, integer *n, doublereal *x,
                         doublereal *f, doublereal *g, doublereal *w, integer *lw, FTruncatedNewton *sfun)
{
  /* Format strings */
  static char fmt_800[] = "(//,\002 ERROR CODE =\002,i3)";
  static char fmt_810[] = "(//,\002 OPTIMAL FUNCTION VALUE = \002,1pd22.15)"
;
  static char fmt_820[] = "(10x,\002CURRENT SOLUTION IS (AT MOST 10 COMPON"
                          "ENTS)\002,/,14x,\002I\002,11x,\002X(I)\002)";
  static char fmt_830[] = "(10x,i5,2x,1pd22.15)";

  /* System generated locals */
  integer i__1;

  /* Builtin functions */
  double sqrt(doublereal);
  integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(void);

  /* Local variables */
  static integer nmax;
  extern /* Subroutine */ int lmqn_(integer *, integer *, doublereal *,
                                      doublereal *, doublereal *, doublereal *, integer *, FTruncatedNewton *,
                                      integer *, integer *, integer *, doublereal *, doublereal *,
                                      doublereal *, doublereal *);
  static doublereal xtol;
  static integer i__, maxit;
  extern doublereal mchpr1_(void);
  static doublereal accrcy;
  static integer maxfun, msglvl;
  static doublereal stepmx, eta;

  /* Fortran I/O blocks */
  static cilist io___8 = {0, 6, 0, fmt_800, 0 };
  static cilist io___9 = {0, 6, 0, fmt_810, 0 };
  static cilist io___10 = {0, 6, 0, fmt_820, 0 };
  static cilist io___12 = {0, 6, 0, fmt_830, 0 };

  /* THIS ROUTINE SOLVES THE OPTIMIZATION PROBLEM */

  /*            MINIMIZE F(X) */
  /*               X */

  /* WHERE X IS A VECTOR OF N REAL VARIABLES.  THE METHOD USED IS */
  /* A TRUNCATED-NEWTON ALGORITHM (SEE "NEWTON-TYPE MINIMIZATION VIA */
  /* THE LANCZOS METHOD" BY S.G. NASH (SIAM J. NUMER. ANAL. 21 (1984), */
  /* PP. 770-778).  THIS ALGORITHM FINDS A LOCAL MINIMUM OF F(X).  IT DOES
  */
  /* NOT ASSUME THAT THE FUNCTION F IS CONVEX (AND SO CANNOT GUARANTEE A */
  /* GLOBAL SOLUTION), BUT DOES ASSUME THAT THE FUNCTION IS BOUNDED BELOW.
  */
  /* IT CAN SOLVE PROBLEMS HAVING ANY NUMBER OF VARIABLES, BUT IT IS */
  /* ESPECIALLY USEFUL WHEN THE NUMBER OF VARIABLES (N) IS LARGE. */

  /* SUBROUTINE PARAMETERS: */

  /* IERROR - (INTEGER) ERROR CODE */
  /*          (0 => NORMAL RETURN) */
  /*          (2 => MORE THAN MAXFUN EVALUATIONS) */
  /*          (3 => LINE SEARCH FAILED TO FIND */
  /*          (LOWER POINT (MAY NOT BE SERIOUS) */
  /*          (-1 => ERROR IN INPUT PARAMETERS) */
  /* N      - (INTEGER) NUMBER OF VARIABLES */
  /* X      - (REAL*8) VECTOR OF LENGTH AT LEAST N; ON INPUT, AN INITIAL */
  /*          ESTIMATE OF THE SOLUTION; ON OUTPUT, THE COMPUTED SOLUTION. */
  /* G      - (REAL*8) VECTOR OF LENGTH AT LEAST N; ON OUTPUT, THE FINAL */
  /*          VALUE OF THE GRADIENT */
  /* F      - (REAL*8) ON INPUT, A ROUGH ESTIMATE OF THE VALUE OF THE */
  /*          OBJECTIVE FUNCTION AT THE SOLUTION; ON OUTPUT, THE VALUE */
  /*          OF THE OBJECTIVE FUNCTION AT THE SOLUTION */
  /* W      - (REAL*8) WORK VECTOR OF LENGTH AT LEAST 14*N */
  /* LW     - (INTEGER) THE DECLARED DIMENSION OF W */
  /* SFUN   - A USER-SPECIFIED SUBROUTINE THAT COMPUTES THE FUNCTION */
  /*          AND GRADIENT OF THE OBJECTIVE FUNCTION.  IT MUST HAVE */
  /*          THE CALLING SEQUENCE */
  /*             SUBROUTINE SFUN (N, X, F, G) */
  /*             INTEGER           N */
  /*             DOUBLE PRECISION  X(N), G(N), F */

  /* THIS IS AN EASY-TO-USE DRIVER FOR THE MAIN OPTIMIZATION ROUTINE */
  /* LMQN.  MORE EXPERIENCED USERS WHO WISH TO CUSTOMIZE PERFORMANCE */
  /* OF THIS ALGORITHM SHOULD CALL LMQN DIRECTLY. */

  /* ----------------------------------------------------------------------
  */
  /* THIS ROUTINE SETS UP ALL THE PARAMETERS FOR THE TRUNCATED-NEWTON */
  /* ALGORITHM.  THE PARAMETERS ARE: */

  /* ETA    - SEVERITY OF THE LINESEARCH */
  /* MAXFUN - MAXIMUM ALLOWABLE NUMBER OF FUNCTION EVALUATIONS */
  /* XTOL   - DESIRED ACCURACY FOR THE SOLUTION X* */
  /* STEPMX - MAXIMUM ALLOWABLE STEP IN THE LINESEARCH */
  /* ACCRCY - ACCURACY OF COMPUTED FUNCTION VALUES */
  /* MSGLVL - DETERMINES QUANTITY OF PRINTED OUTPUT */
  /*          0 = NONE, 1 = ONE LINE PER MAJOR ITERATION. */
  /* MAXIT  - MAXIMUM NUMBER OF INNER ITERATIONS PER STEP */

  /* SET UP PARAMETERS FOR THE OPTIMIZATION ROUTINE */

  /* Parameter adjustments */
  --g;
  --x;
  --w;

  /* Function Body */
  maxit = *n / 2;
  if (maxit > 50)
    {
      maxit = 50;
    }
  if (maxit <= 0)
    {
      maxit = 1;
    }
  msglvl = 0;
  maxfun = *n * 150;
  eta = .25;
  stepmx = 10.;
  accrcy = mchpr1_() * 100.;
  xtol = sqrt(accrcy);

  /* MINIMIZE THE FUNCTION */

  lmqn_(ierror, n, &x[1], f, &g[1], &w[1], lw, sfun, &msglvl, &maxit,
        &maxfun, &eta, &stepmx, &accrcy, &xtol);

  /* PRINT THE RESULTS */

  /*  if (*ierror != 0) {
  s_wsfe(&io___8);
  do_fio(&c__1, (char *)&(*ierror), (ftnlen)sizeof(integer));
  e_wsfe();
     }
     s_wsfe(&io___9);
     do_fio(&c__1, (char *)&(*f), (ftnlen)sizeof(doublereal));
     e_wsfe();
     if (msglvl < 1) {
  return 0;
     }
     s_wsfe(&io___10);
     e_wsfe();
     nmax = 10;
     if (*n < nmax) {
  nmax = *n;
     }
     s_wsfe(&io___12);
     i__1 = nmax;
     for (i__ = 1; i__ <= i__1; ++i__) {
  do_fio(&c__1, (char *)&i__, (ftnlen)sizeof(integer));
  do_fio(&c__1, (char *)&x[i__], (ftnlen)sizeof(doublereal));
     }
     e_wsfe();*/
  return 0;
}  /* tn_ */

/* Subroutine */ int tnbc_(integer *ierror, integer *n, doublereal *x,
                           doublereal *f, doublereal *g, doublereal *w, integer *lw, FTruncatedNewton * sfun,
                           doublereal *low, doublereal *up, integer *ipivot)
{
  /* Format strings */
  static char fmt_800[] = "(//,\002 ERROR CODE =\002,i3)";
  static char fmt_810[] = "(//,\002 OPTIMAL FUNCTION VALUE = \002,1pd22.15)"
;
  static char fmt_820[] = "(10x,\002CURRENT SOLUTION IS (AT MOST 10 COMPON"
                          "ENTS)\002,/,14x,\002I\002,11x,\002X(I)\002)";
  static char fmt_830[] = "(10x,i5,2x,1pd22.15)";

  /* System generated locals */
  integer i__1;

  /* Builtin functions */
  double sqrt(doublereal);
  integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(void);

  /* Local variables */
  static integer nmax;
  static doublereal xtol;
  static integer i__, maxit;
  extern doublereal mchpr1_(void);
  static doublereal accrcy;
  extern /* Subroutine */ int lmqnbc_(integer *, integer *, doublereal *,
                                        doublereal *, doublereal *, doublereal *, integer *, FTruncatedNewton *,
                                        doublereal *, doublereal *, integer *, integer *, integer *,
                                        integer *, doublereal *, doublereal *, doublereal *, doublereal *)
;
  static integer maxfun, msglvl;
  static doublereal stepmx, eta;

  /* Fortran I/O blocks */
  static cilist io___21 = {0, 6, 0, fmt_800, 0 };
  static cilist io___22 = {0, 6, 0, fmt_810, 0 };
  static cilist io___23 = {0, 6, 0, fmt_820, 0 };
  static cilist io___25 = {0, 6, 0, fmt_830, 0 };

  /* THIS ROUTINE SOLVES THE OPTIMIZATION PROBLEM */

  /*   MINIMIZE     F(X) */
  /*      X */
  /*   SUBJECT TO   LOW <= X <= UP */

  /* WHERE X IS A VECTOR OF N REAL VARIABLES.  THE METHOD USED IS */
  /* A TRUNCATED-NEWTON ALGORITHM (SEE "NEWTON-TYPE MINIMIZATION VIA */
  /* THE LANCZOS ALGORITHM" BY S.G. NASH (TECHNICAL REPORT 378, MATH. */
  /* THE LANCZOS METHOD" BY S.G. NASH (SIAM J. NUMER. ANAL. 21 (1984), */
  /* PP. 770-778).  THIS ALGORITHM FINDS A LOCAL MINIMUM OF F(X).  IT DOES
  */
  /* NOT ASSUME THAT THE FUNCTION F IS CONVEX (AND SO CANNOT GUARANTEE A */
  /* GLOBAL SOLUTION), BUT DOES ASSUME THAT THE FUNCTION IS BOUNDED BELOW.
  */
  /* IT CAN SOLVE PROBLEMS HAVING ANY NUMBER OF VARIABLES, BUT IT IS */
  /* ESPECIALLY USEFUL WHEN THE NUMBER OF VARIABLES (N) IS LARGE. */

  /* SUBROUTINE PARAMETERS: */

  /* IERROR  - (INTEGER) ERROR CODE */
  /*           (0 => NORMAL RETURN */
  /*           (2 => MORE THAN MAXFUN EVALUATIONS */
  /*           (3 => LINE SEARCH FAILED TO FIND LOWER */
  /*           (POINT (MAY NOT BE SERIOUS) */
  /*           (-1 => ERROR IN INPUT PARAMETERS */
  /* N       - (INTEGER) NUMBER OF VARIABLES */
  /* X       - (REAL*8) VECTOR OF LENGTH AT LEAST N; ON INPUT, AN INITIAL */
  /*           ESTIMATE OF THE SOLUTION; ON OUTPUT, THE COMPUTED SOLUTION.
  */
  /* G       - (REAL*8) VECTOR OF LENGTH AT LEAST N; ON OUTPUT, THE FINAL */
  /*           VALUE OF THE GRADIENT */
  /* F       - (REAL*8) ON INPUT, A ROUGH ESTIMATE OF THE VALUE OF THE */
  /*           OBJECTIVE FUNCTION AT THE SOLUTION; ON OUTPUT, THE VALUE */
  /*           OF THE OBJECTIVE FUNCTION AT THE SOLUTION */
  /* W       - (REAL*8) WORK VECTOR OF LENGTH AT LEAST 14*N */
  /* LW      - (INTEGER) THE DECLARED DIMENSION OF W */
  /* SFUN    - A USER-SPECIFIED SUBROUTINE THAT COMPUTES THE FUNCTION */
  /*           AND GRADIENT OF THE OBJECTIVE FUNCTION.  IT MUST HAVE */
  /*           THE CALLING SEQUENCE */
  /*             SUBROUTINE SFUN (N, X, F, G) */
  /*             INTEGER           N */
  /*             DOUBLE PRECISION  X(N), G(N), F */
  /* LOW, UP - (REAL*8) VECTORS OF LENGTH AT LEAST N CONTAINING */
  /*           THE LOWER AND UPPER BOUNDS ON THE VARIABLES.  IF */
  /*           THERE ARE NO BOUNDS ON A PARTICULAR VARIABLE, SET */
  /*           THE BOUNDS TO -1.D38 AND 1.D38, RESPECTIVELY. */
  /* IPIVOT  - (INTEGER) WORK VECTOR OF LENGTH AT LEAST N, USED */
  /*           TO RECORD WHICH VARIABLES ARE AT THEIR BOUNDS. */

  /* THIS IS AN EASY-TO-USE DRIVER FOR THE MAIN OPTIMIZATION ROUTINE */
  /* LMQNBC.  MORE EXPERIENCED USERS WHO WISH TO CUSTOMIZE PERFORMANCE */
  /* OF THIS ALGORITHM SHOULD CALL LMQBC DIRECTLY. */

  /* ----------------------------------------------------------------------
  */
  /* THIS ROUTINE SETS UP ALL THE PARAMETERS FOR THE TRUNCATED-NEWTON */
  /* ALGORITHM.  THE PARAMETERS ARE: */

  /* ETA    - SEVERITY OF THE LINESEARCH */
  /* MAXFUN - MAXIMUM ALLOWABLE NUMBER OF FUNCTION EVALUATIONS */
  /* XTOL   - DESIRED ACCURACY FOR THE SOLUTION X* */
  /* STEPMX - MAXIMUM ALLOWABLE STEP IN THE LINESEARCH */
  /* ACCRCY - ACCURACY OF COMPUTED FUNCTION VALUES */
  /* MSGLVL - CONTROLS QUANTITY OF PRINTED OUTPUT */
  /*          0 = NONE, 1 = ONE LINE PER MAJOR ITERATION. */
  /* MAXIT  - MAXIMUM NUMBER OF INNER ITERATIONS PER STEP */

  /* SET PARAMETERS FOR THE OPTIMIZATION ROUTINE */

  /* Parameter adjustments */
  --ipivot;
  --up;
  --low;
  --g;
  --x;
  --w;

  /* Function Body */
  maxit = *n / 2;
  if (maxit > 50)
    {
      maxit = 50;
    }
  if (maxit <= 0)
    {
      maxit = 1;
    }
  msglvl = 0;
  maxfun = *n * 150;
  eta = .25;
  stepmx = 10.;
  accrcy = mchpr1_() * 100.;
  xtol = sqrt(accrcy);

  /* MINIMIZE FUNCTION */

  lmqnbc_(ierror, n, &x[1], f, &g[1], &w[1], lw, sfun, &low[1], &up[1]
          , &ipivot[1], &msglvl, &maxit, &maxfun, &eta, &stepmx, &accrcy, &
          xtol);

  /* PRINT RESULTS */

  /*   if (*ierror != 0) {
  s_wsfe(&io___21);
  do_fio(&c__1, (char *)&(*ierror), (ftnlen)sizeof(integer));
  e_wsfe();
     }
     s_wsfe(&io___22);
     do_fio(&c__1, (char *)&(*f), (ftnlen)sizeof(doublereal));
     e_wsfe();
     if (msglvl < 1) {
  return 0;
     }
     s_wsfe(&io___23);
     e_wsfe();
     nmax = 10;
     if (*n < nmax) {
  nmax = *n;
     }
     s_wsfe(&io___25);
     i__1 = nmax;
     for (i__ = 1; i__ <= i__1; ++i__) {
  do_fio(&c__1, (char *)&i__, (ftnlen)sizeof(integer));
  do_fio(&c__1, (char *)&x[i__], (ftnlen)sizeof(doublereal));
     }
     e_wsfe();*/
  return 0;
}  /* tnbc_ */

/* Subroutine */ int lmqn_(integer *ifail, integer *n, doublereal *x,
                           doublereal *f, doublereal *g, doublereal *w, integer *lw, FTruncatedNewton * sfun,
                           integer *msglvl, integer *maxit, integer *maxfun, doublereal *eta,
                           doublereal *stepmx, doublereal *accrcy, doublereal *xtol)
{
  /* Format strings */
  static char fmt_800[] = "(//\002 NIT   NF   CG\002,9x,\002F\002,21x,\002"
                          "GTG\002,//)";
  static char fmt_810[] = "(\002 \002,i3,1x,i4,1x,i4,1x,1pd22.15,2x,1pd15."
                          "8)";

  /* System generated locals */
  integer i__1;

  /* Builtin functions */
  integer s_wsfe(cilist *), e_wsfe(void), do_fio(integer *, char *, ftnlen);
  double sqrt(doublereal);

  /* Local variables */
  static doublereal fold, oldf;
  extern doublereal ddot_(integer *, doublereal *, integer *, doublereal *,
                            integer *);
  static doublereal fnew;
  static integer numf;
  static doublereal peps;
  static integer lhyr;
  static doublereal zero, rtol, yksk, tiny;
  extern /* Subroutine */ int dxpy_(integer *, doublereal *, integer *,
                                      doublereal *, integer *);
  static integer nwhy;
  static doublereal yrsr;
  extern doublereal dnrm2_(integer *, doublereal *, integer *), step1_(
      doublereal *, doublereal *, doublereal *, doublereal *);
  static integer i__;
  static doublereal alpha, fkeep;
  static integer ioldg;
  static doublereal small;
  static integer modet;
  extern /* Subroutine */ int dcopy_(integer *, doublereal *, integer *,
                                       doublereal *, integer *);
  static integer niter;
  static doublereal gnorm, ftest, fstop, pnorm, rteps, xnorm;
  static integer idiagb;
  static doublereal fm, pe, difold;
  static integer icycle, nlincg, nfeval;
  static doublereal difnew;
  static integer nmodif;
  extern /* Subroutine */ int chkucp_(integer *, integer *, integer *,
                                        integer *, doublereal *, doublereal *, doublereal *, doublereal *,
                                        doublereal *, doublereal *, doublereal *, doublereal *,
                                        doublereal *, doublereal *, doublereal *, doublereal *, integer *,
                                        doublereal *, doublereal *, doublereal *);
  static doublereal epsmch;
  extern /* Subroutine */ int linder_(integer *, FTruncatedNewton *, doublereal *,
                                        doublereal *, doublereal *, doublereal *, doublereal *,
                                        doublereal *, doublereal *, doublereal *, doublereal *,
                                        doublereal *, doublereal *, doublereal *, doublereal *,
                                        doublereal *, integer *, integer *, doublereal *, integer *);
  static doublereal epsred, abstol, oldgtp;
  extern /* Subroutine */ int modlnp_(integer *, doublereal *, doublereal *,
                                        doublereal *, doublereal *, doublereal *, doublereal *,
                                        doublereal *, doublereal *, doublereal *, integer *, doublereal *,
                                        integer *, integer *, integer *, integer *, integer *, integer *,
                                        logical *, doublereal *, doublereal *, doublereal *, logical *,
                                        FTruncatedNewton *, logical *, integer *, doublereal *, doublereal *,
                                        doublereal *, doublereal *);
  static integer ireset;
  static logical lreset;
  extern /* Subroutine */ int setpar_(integer *);
  static doublereal reltol, gtpnew;
  static integer nftotl;
  static doublereal toleps;
  extern /* Subroutine */ int setucr_(doublereal *, integer *, integer *,
                                        integer *, doublereal *, doublereal *, doublereal *, doublereal *,
                                        doublereal *, FTruncatedNewton *, doublereal *, doublereal *);
  static doublereal rtleps;
  static integer ipivot, nm1;
  static doublereal rtolsq, tnytol, gtg, one;
  static integer ipk;
  static doublereal gsk, spe;
  static integer isk, iyk;
  static logical upd1;

  /* Fortran I/O blocks */
  static cilist io___27 = {0, 6, 0, fmt_800, 0 };
  static cilist io___56 = {0, 6, 0, fmt_810, 0 };
  static cilist io___81 = {0, 6, 0, fmt_810, 0 };

  /* THIS ROUTINE IS A TRUNCATED-NEWTON METHOD. */
  /* THE TRUNCATED-NEWTON METHOD IS PRECONDITIONED BY A LIMITED-MEMORY */
  /* QUASI-NEWTON METHOD (THIS PRECONDITIONING STRATEGY IS DEVELOPED */
  /* IN THIS ROUTINE) WITH A FURTHER DIAGONAL SCALING (SEE ROUTINE NDIA3).
  */
  /* FOR FURTHER DETAILS ON THE PARAMETERS, SEE ROUTINE TN. */

  /* THE FOLLOWING IMSL AND STANDARD FUNCTIONS ARE USED */

  /* INITIALIZE PARAMETERS AND CONSTANTS */

  /* Parameter adjustments */
  --g;
  --x;
  --w;

  /* Function Body */
  /*  if (*msglvl >= -2) {
  s_wsfe(&io___27);
  e_wsfe();
    } */
  setpar_(n);
  upd1 = TRUE_;
  ireset = 0;
  nfeval = 0;
  nmodif = 0;
  nlincg = 0;
  fstop = *f;
  zero = 0.;
  one = 1.;
  nm1 = *n - 1;

  /* WITHIN THIS ROUTINE THE ARRAY W(LOLDG) IS SHARED BY W(LHYR) */

  lhyr = subscr_1.loldg;

  /* CHECK PARAMETERS AND SET CONSTANTS */

  chkucp_(&subscr_1.lwtest, maxfun, &nwhy, n, &alpha, &epsmch, eta, &peps, &
          rteps, &rtol, &rtolsq, stepmx, &ftest, xtol, &xnorm, &x[1], lw, &
          small, &tiny, accrcy);
  if (nwhy < 0)
    {
      goto L120;
    }
  setucr_(&small, &nftotl, &niter, n, f, &fnew, &fm, &gtg, &oldf,
          sfun, &g[1], &x[1]);
  fold = fnew;
  if (*msglvl >= 1)
    {
      s_wsfe(&io___56);
      do_fio(&c__1, (char *)&niter, (ftnlen)sizeof(integer));
      do_fio(&c__1, (char *)&nftotl, (ftnlen)sizeof(integer));
      do_fio(&c__1, (char *)&nlincg, (ftnlen)sizeof(integer));
      do_fio(&c__1, (char *)&fnew, (ftnlen)sizeof(doublereal));
      do_fio(&c__1, (char *)&gtg, (ftnlen)sizeof(doublereal));
      e_wsfe();
    }

  /* CHECK FOR SMALL GRADIENT AT THE STARTING POINT. */

  ftest = one + abs(fnew);
  if (gtg < epsmch * 1e-4 * ftest * ftest)
    {
      goto L90;
    }

  /* SET INITIAL VALUES TO OTHER PARAMETERS */

  icycle = nm1;
  toleps = rtol + rteps;
  rtleps = rtolsq + epsmch;
  gnorm = sqrt(gtg);
  difnew = zero;
  epsred = .05;
  fkeep = fnew;

  /* SET THE DIAGONAL OF THE APPROXIMATE HESSIAN TO UNITY. */

  idiagb = subscr_1.ldiagb;
  i__1 = *n;
  for (i__ = 1; i__ <= i__1; ++i__)
    {
      w[idiagb] = one;
      ++idiagb;
      /* L10: */
    }

  /* ..................START OF MAIN ITERATIVE LOOP.......... */

  /* COMPUTE THE NEW SEARCH DIRECTION */

  modet = *msglvl - 3;
  modlnp_(&modet, &w[subscr_1.lpk], &w[subscr_1.lgv], &w[subscr_1.lz1], &w[
            subscr_1.lv], &w[subscr_1.ldiagb], &w[subscr_1.lemat], &x[1], &g[
            1], &w[subscr_1.lzk], n, &w[1], lw, &niter, maxit, &nfeval, &
          nmodif, &nlincg, &upd1, &yksk, &gsk, &yrsr, &lreset, sfun, &
          c_false, &ipivot, accrcy, &gtpnew, &gnorm, &xnorm);
L20:
  dcopy_(n, &g[1], &c__1, &w[subscr_1.loldg], &c__1);
  pnorm = dnrm2_(n, &w[subscr_1.lpk], &c__1);
  oldf = fnew;
  oldgtp = gtpnew;

  /* PREPARE TO COMPUTE THE STEP LENGTH */

  pe = pnorm + epsmch;

  /* COMPUTE THE ABSOLUTE AND RELATIVE TOLERANCES FOR THE LINEAR SEARCH */

  reltol = rteps * (xnorm + one) / pe;
  abstol = -epsmch * ftest / (oldgtp - epsmch);

  /* COMPUTE THE SMALLEST ALLOWABLE SPACING BETWEEN POINTS IN */
  /* THE LINEAR SEARCH */

  tnytol = epsmch * (xnorm + one) / pe;
  spe = *stepmx / pe;

  /* SET THE INITIAL STEP LENGTH. */

  alpha = step1_(&fnew, &fm, &oldgtp, &spe);

  /* PERFORM THE LINEAR SEARCH */

  linder_(n, sfun, &small, &epsmch, &reltol, &abstol, &tnytol, eta, &
          zero, &spe, &w[subscr_1.lpk], &oldgtp, &x[1], &fnew, &alpha, &g[1]
          , &numf, &nwhy, &w[1], lw);

  fold = fnew;
  ++niter;
  nftotl += numf;
  gtg = ddot_(n, &g[1], &c__1, &g[1], &c__1);
  if (*msglvl >= 1)
    {
      s_wsfe(&io___81);
      do_fio(&c__1, (char *)&niter, (ftnlen)sizeof(integer));
      do_fio(&c__1, (char *)&nftotl, (ftnlen)sizeof(integer));
      do_fio(&c__1, (char *)&nlincg, (ftnlen)sizeof(integer));
      do_fio(&c__1, (char *)&fnew, (ftnlen)sizeof(doublereal));
      do_fio(&c__1, (char *)&gtg, (ftnlen)sizeof(doublereal));
      e_wsfe();
    }
  if (nwhy < 0)
    {
      goto L120;
    }
  if (nwhy == 0 || nwhy == 2)
    {
      goto L30;
    }

  /* THE LINEAR SEARCH HAS FAILED TO FIND A LOWER POINT */

  nwhy = 3;
  goto L100;
L30:
  if (nwhy <= 1)
    {
      goto L40;
    }
  (*sfun)(n, &x[1], &fnew, &g[1]);
  ++nftotl;

  /* TERMINATE IF MORE THAN MAXFUN EVALUTATIONS HAVE BEEN MADE */

L40:
  nwhy = 2;
  if (nftotl > *maxfun)
    {
      goto L110;
    }
  nwhy = 0;

  /* SET UP PARAMETERS USED IN CONVERGENCE AND RESETTING TESTS */

  difold = difnew;
  difnew = oldf - fnew;

  /* IF THIS IS THE FIRST ITERATION OF A NEW CYCLE, COMPUTE THE */
  /* PERCENTAGE REDUCTION FACTOR FOR THE RESETTING TEST. */

  if (icycle != 1)
    {
      goto L50;
    }
  if (difnew > difold * 2.)
    {
      epsred += epsred;
    }
  if (difnew < difold * .5)
    {
      epsred *= .5;
    }
L50:
  gnorm = sqrt(gtg);
  ftest = one + abs(fnew);
  xnorm = dnrm2_(n, &x[1], &c__1);

  /* TEST FOR CONVERGENCE */

  if (alpha * pnorm < toleps * (one + xnorm) && abs(difnew) < rtleps *
      ftest && gtg < peps * ftest * ftest || gtg < *accrcy * 1e-4 *
      ftest * ftest)
    {
      goto L90;
    }

  /* COMPUTE THE CHANGE IN THE ITERATES AND THE CORRESPONDING CHANGE */
  /* IN THE GRADIENTS */

  isk = subscr_1.lsk;
  ipk = subscr_1.lpk;
  iyk = subscr_1.lyk;
  ioldg = subscr_1.loldg;
  i__1 = *n;
  for (i__ = 1; i__ <= i__1; ++i__)
    {
      w[iyk] = g[i__] - w[ioldg];
      w[isk] = alpha * w[ipk];
      ++ipk;
      ++isk;
      ++iyk;
      ++ioldg;
      /* L60: */
    }

  /* SET UP PARAMETERS USED IN UPDATING THE DIRECTION OF SEARCH. */

  yksk = ddot_(n, &w[subscr_1.lyk], &c__1, &w[subscr_1.lsk], &c__1);
  lreset = FALSE_;
  if (icycle == nm1 || difnew < epsred * (fkeep - fnew))
    {
      lreset = TRUE_;
    }
  if (lreset)
    {
      goto L70;
    }
  yrsr = ddot_(n, &w[subscr_1.lyr], &c__1, &w[subscr_1.lsr], &c__1);
  if (yrsr <= zero)
    {
      lreset = TRUE_;
    }
L70:
  upd1 = FALSE_;

  /*      COMPUTE THE NEW SEARCH DIRECTION */

  modet = *msglvl - 3;
  modlnp_(&modet, &w[subscr_1.lpk], &w[subscr_1.lgv], &w[subscr_1.lz1], &w[
            subscr_1.lv], &w[subscr_1.ldiagb], &w[subscr_1.lemat], &x[1], &g[
            1], &w[subscr_1.lzk], n, &w[1], lw, &niter, maxit, &nfeval, &
          nmodif, &nlincg, &upd1, &yksk, &gsk, &yrsr, &lreset, sfun, &
          c_false, &ipivot, accrcy, &gtpnew, &gnorm, &xnorm);
  if (lreset)
    {
      goto L80;
    }

  /*      STORE THE ACCUMULATED CHANGE IN THE POINT AND GRADIENT AS AN */
  /*      "AVERAGE" DIRECTION FOR PRECONDITIONING. */

  dxpy_(n, &w[subscr_1.lsk], &c__1, &w[subscr_1.lsr], &c__1);
  dxpy_(n, &w[subscr_1.lyk], &c__1, &w[subscr_1.lyr], &c__1);
  ++icycle;
  goto L20;

  /* RESET */

L80:
  ++ireset;

  /* INITIALIZE THE SUM OF ALL THE CHANGES IN X. */

  dcopy_(n, &w[subscr_1.lsk], &c__1, &w[subscr_1.lsr], &c__1);
  dcopy_(n, &w[subscr_1.lyk], &c__1, &w[subscr_1.lyr], &c__1);
  fkeep = fnew;
  icycle = 1;
  goto L20;

  /* ...............END OF MAIN ITERATION....................... */

L90:
  *ifail = 0;
  *f = fnew;
  return 0;
L100:
  oldf = fnew;

  /* LOCAL SEARCH HERE COULD BE INSTALLED HERE */

L110:
  *f = oldf;

  /* SET IFAIL */

L120:
  *ifail = nwhy;
  return 0;
} /* lmqn_ */

/* Subroutine */ int lmqnbc_(integer *ifail, integer *n, doublereal *x,
                             doublereal *f, doublereal *g, doublereal *w, integer *lw, FTruncatedNewton *sfun,
                             doublereal *low, doublereal *up, integer *ipivot, integer *msglvl,
                             integer *maxit, integer *maxfun, doublereal *eta, doublereal *stepmx,
                             doublereal *accrcy, doublereal *xtol)
{
  /* Format strings */
  static char fmt_800[] = "(\002 THERE IS NO FEASIBLE POINT; TERMINATING A"
                          "LGORITHM\002)";
  static char fmt_810[] = "(//\002  NIT   NF   CG\002,9x,\002F\002,21x,"
                          "\002GTG\002,//)";
  static char fmt_820[] = "(\002        LINESEARCH RESULTS:  ALPHA,PNOR"
                          "M\002,2(1pd12.4))";
  static char fmt_830[] = "(\002 UPD1 IS TRUE - TRIVIAL PRECONDITIONING"
                          "\002)";
  static char fmt_840[] = "(\002 NEWCON IS TRUE - CONSTRAINT ADDED IN LINE"
                          "SEARCH\002)";

  /* System generated locals */
  integer i__1;
  doublereal d__1;

  /* Builtin functions */
  integer s_wsfe(cilist *), e_wsfe(void);
  double sqrt(doublereal);
  integer do_fio(integer *, char *, ftnlen);

  /* Local variables */
  static doublereal fold, oldf;
  extern doublereal ddot_(integer *, doublereal *, integer *, doublereal *,
                            integer *);
  static doublereal fnew;
  static integer numf;
  static logical conv;
  static doublereal peps;
  extern /* Subroutine */ int modz_(integer *, doublereal *, doublereal *,
                                      integer *, doublereal *, doublereal *, doublereal *, doublereal *,
                                      doublereal *);
  static integer lhyr;
  static doublereal zero, rtol, yksk, tiny;
  extern /* Subroutine */ int dxpy_(integer *, doublereal *, integer *,
                                      doublereal *, integer *);
  static integer nwhy;
  static doublereal yrsr;
  extern doublereal dnrm2_(integer *, doublereal *, integer *), step1_(
      doublereal *, doublereal *, doublereal *, doublereal *);
  static integer i__;
  static doublereal alpha, fkeep;
  static integer ioldg;
  extern /* Subroutine */ int crash_(integer *, doublereal *, integer *,
                                       doublereal *, doublereal *, integer *);
  static doublereal small, flast;
  static integer modet;
  extern /* Subroutine */ int dcopy_(integer *, doublereal *, integer *,
                                       doublereal *, integer *);
  static integer niter;
  static doublereal gnorm, ftest;
  extern int monit_(integer *, doublereal *, doublereal *, doublereal *,
                      integer *, integer *, integer *, integer *, integer *);
  extern /* Subroutine */ /* int monit_(),*/ int ztime_(integer *, doublereal *,
        integer *);
  static doublereal fstop, pnorm, rteps, xnorm;
  static integer idiagb;
  static doublereal fm, pe, difold;
  static integer icycle, nlincg, nfeval;
  static doublereal difnew;
  static integer nmodif;
  static doublereal epsmch;
  extern /* Subroutine */ int chkucp_(integer *, integer *, integer *,
                                        integer *, doublereal *, doublereal *, doublereal *, doublereal *,
                                        doublereal *, doublereal *, doublereal *, doublereal *,
                                        doublereal *, doublereal *, doublereal *, doublereal *, integer *,
                                        doublereal *, doublereal *, doublereal *), linder_(integer *,
                                            FTruncatedNewton *, doublereal *, doublereal *, doublereal *, doublereal *,
                                            doublereal *, doublereal *, doublereal *, doublereal *,
                                            doublereal *, doublereal *, doublereal *, doublereal *,
                                            doublereal *, doublereal *, integer *, integer *, doublereal *,
                                            integer *);
  static doublereal epsred, abstol, oldgtp;
  static logical newcon;
  static integer ireset;
  extern /* Subroutine */ int modlnp_(integer *, doublereal *, doublereal *,
                                        doublereal *, doublereal *, doublereal *, doublereal *,
                                        doublereal *, doublereal *, doublereal *, integer *, doublereal *,
                                        integer *, integer *, integer *, integer *, integer *, integer *,
                                        logical *, doublereal *, doublereal *, doublereal *, logical *,
                                        FTruncatedNewton *, logical *, integer *, doublereal *, doublereal *,
                                        doublereal *, doublereal *);
  static logical lreset;
  extern /* Subroutine */ int setpar_(integer *);
  static doublereal reltol, gtpnew;
  static integer nftotl;
  static doublereal toleps;
  extern /* Subroutine */ int setucr_(doublereal *, integer *, integer *,
                                        integer *, doublereal *, doublereal *, doublereal *, doublereal *,
                                        doublereal *, FTruncatedNewton *, doublereal *, doublereal *);
  static doublereal rtleps;
  static integer nm1;
  extern /* Subroutine */ int stpmax_(doublereal *, doublereal *,
                                        doublereal *, integer *, doublereal *, doublereal *, integer *,
                                        doublereal *, doublereal *), cnvtst_(logical *, doublereal *,
                                                                             doublereal *, doublereal *, doublereal *, doublereal *,
                                                                             doublereal *, doublereal *, doublereal *, doublereal *,
                                                                             doublereal *, doublereal *, doublereal *, doublereal *,
                                                                             doublereal *, integer *, integer *, doublereal *);
  static doublereal rtolsq, tnytol;
  static integer ier;
  static doublereal gtg, one;
  static integer ipk;
  static doublereal gsk, spe;
  static integer isk, iyk;
  static logical upd1;

  /* Fortran I/O blocks */
  static cilist io___88 = {0, 6, 0, fmt_800, 0 };
  static cilist io___89 = {0, 6, 0, fmt_810, 0 };
  static cilist io___144 = {0, 6, 0, fmt_820, 0 };
  static cilist io___150 = {0, 6, 0, fmt_830, 0 };
  static cilist io___151 = {0, 6, 0, fmt_840, 0 };

  /* THIS ROUTINE IS A BOUNDS-CONSTRAINED TRUNCATED-NEWTON METHOD. */
  /* THE TRUNCATED-NEWTON METHOD IS PRECONDITIONED BY A LIMITED-MEMORY */
  /* QUASI-NEWTON METHOD (THIS PRECONDITIONING STRATEGY IS DEVELOPED */
  /* IN THIS ROUTINE) WITH A FURTHER DIAGONAL SCALING (SEE ROUTINE NDIA3).
  */
  /* FOR FURTHER DETAILS ON THE PARAMETERS, SEE ROUTINE TNBC. */

  /* THE FOLLOWING STANDARD FUNCTIONS AND SYSTEM FUNCTIONS ARE USED */

  /* CHECK THAT INITIAL X IS FEASIBLE AND THAT THE BOUNDS ARE CONSISTENT */

  /* Parameter adjustments */
  --ipivot;
  --up;
  --low;
  --g;
  --x;
  --w;

  /* Function Body */
  crash_(n, &x[1], &ipivot[1], &low[1], &up[1], &ier);
  /*if (ier != 0) {
  s_wsfe(&io___88);
  e_wsfe();
  }
  if (ier != 0) {
  return 0;
  }
  if (*msglvl >= 1) {
  s_wsfe(&io___89);
  e_wsfe();
  } */

  /* INITIALIZE VARIABLES */

  setpar_(n);
  upd1 = TRUE_;
  ireset = 0;
  nfeval = 0;
  nmodif = 0;
  nlincg = 0;
  fstop = *f;
  conv = FALSE_;
  zero = 0.;
  one = 1.;
  nm1 = *n - 1;

  /* WITHIN THIS ROUTINE THE ARRAY W(LOLDG) IS SHARED BY W(LHYR) */

  lhyr = subscr_1.loldg;

  /* CHECK PARAMETERS AND SET CONSTANTS */

  chkucp_(&subscr_1.lwtest, maxfun, &nwhy, n, &alpha, &epsmch, eta, &peps, &
          rteps, &rtol, &rtolsq, stepmx, &ftest, xtol, &xnorm, &x[1], lw, &
          small, &tiny, accrcy);
  if (nwhy < 0)
    {
      goto L160;
    }
  setucr_(&small, &nftotl, &niter, n, f, &fnew, &fm, &gtg, &oldf,
          sfun, &g[1], &x[1]);
  fold = fnew;
  flast = fnew;

  /* TEST THE LAGRANGE MULTIPLIERS TO SEE IF THEY ARE NON-NEGATIVE. */
  /* BECAUSE THE CONSTRAINTS ARE ONLY LOWER BOUNDS, THE COMPONENTS */
  /* OF THE GRADIENT CORRESPONDING TO THE ACTIVE CONSTRAINTS ARE THE */
  /* LAGRANGE MULTIPLIERS.  AFTERWORDS, THE PROJECTED GRADIENT IS FORMED. */

  i__1 = *n;
  for (i__ = 1; i__ <= i__1; ++i__)
    {
      if (ipivot[i__] == 2)
        {
          goto L10;
        }
      if (-ipivot[i__] * g[i__] >= 0.)
        {
          goto L10;
        }
      ipivot[i__] = 0;
L10:
;
    }
  ztime_(n, &g[1], &ipivot[1]);
  gtg = ddot_(n, &g[1], &c__1, &g[1], &c__1);
  if (*msglvl >= 1)
    {
      monit_(n, &x[1], &fnew, &g[1], &niter, &nftotl, &nfeval, &lreset, &
             ipivot[1]);
    }

  /* CHECK IF THE INITIAL POINT IS A LOCAL MINIMUM. */

  ftest = one + abs(fnew);
  if (gtg < epsmch * 1e-4 * ftest * ftest)
    {
      goto L130;
    }

  /* SET INITIAL VALUES TO OTHER PARAMETERS */

  icycle = nm1;
  toleps = rtol + rteps;
  rtleps = rtolsq + epsmch;
  gnorm = sqrt(gtg);
  difnew = zero;
  epsred = .05;
  fkeep = fnew;

  /* SET THE DIAGONAL OF THE APPROXIMATE HESSIAN TO UNITY. */

  idiagb = subscr_1.ldiagb;
  i__1 = *n;
  for (i__ = 1; i__ <= i__1; ++i__)
    {
      w[idiagb] = one;
      ++idiagb;
      /* L15: */
    }

  /* ..................START OF MAIN ITERATIVE LOOP.......... */

  /* COMPUTE THE NEW SEARCH DIRECTION */

  modet = *msglvl - 3;
  modlnp_(&modet, &w[subscr_1.lpk], &w[subscr_1.lgv], &w[subscr_1.lz1], &w[
            subscr_1.lv], &w[subscr_1.ldiagb], &w[subscr_1.lemat], &x[1], &g[
            1], &w[subscr_1.lzk], n, &w[1], lw, &niter, maxit, &nfeval, &
          nmodif, &nlincg, &upd1, &yksk, &gsk, &yrsr, &lreset, sfun, &
          c_true, &ipivot[1], accrcy, &gtpnew, &gnorm, &xnorm);
L20:

  /* added manually by Pedro Mendes 12/2/1998 */
  // if(callback != 0/*NULL*/) callback(fnew);

  dcopy_(n, &g[1], &c__1, &w[subscr_1.loldg], &c__1);
  pnorm = dnrm2_(n, &w[subscr_1.lpk], &c__1);
  oldf = fnew;
  oldgtp = gtpnew;

  /* PREPARE TO COMPUTE THE STEP LENGTH */

  pe = pnorm + epsmch;

  /* COMPUTE THE ABSOLUTE AND RELATIVE TOLERANCES FOR THE LINEAR SEARCH */

  reltol = rteps * (xnorm + one) / pe;
  abstol = -epsmch * ftest / (oldgtp - epsmch);

  /* COMPUTE THE SMALLEST ALLOWABLE SPACING BETWEEN POINTS IN */
  /* THE LINEAR SEARCH */

  tnytol = epsmch * (xnorm + one) / pe;
  stpmax_(stepmx, &pe, &spe, n, &x[1], &w[subscr_1.lpk], &ipivot[1], &low[1]
          , &up[1]);

  /* SET THE INITIAL STEP LENGTH. */

  alpha = step1_(&fnew, &fm, &oldgtp, &spe);

  /* PERFORM THE LINEAR SEARCH */

  linder_(n, sfun, &small, &epsmch, &reltol, &abstol, &tnytol, eta, &
          zero, &spe, &w[subscr_1.lpk], &oldgtp, &x[1], &fnew, &alpha, &g[1]
          , &numf, &nwhy, &w[1], lw);
  newcon = FALSE_;
  if ((d__1 = alpha - spe, abs(d__1)) > epsmch * 10.)
    {
      goto L30;
    }
  newcon = TRUE_;
  nwhy = 0;
  modz_(n, &x[1], &w[subscr_1.lpk], &ipivot[1], &epsmch, &low[1], &up[1], &
        flast, &fnew);
  flast = fnew;

L30:
  if (*msglvl >= 3)
    {
      s_wsfe(&io___144);
      do_fio(&c__1, (char *)&alpha, (ftnlen)sizeof(doublereal));
      do_fio(&c__1, (char *)&pnorm, (ftnlen)sizeof(doublereal));
      e_wsfe();
    }
  fold = fnew;
  ++niter;
  nftotl += numf;

  /* IF REQUIRED, PRINT THE DETAILS OF THIS ITERATION */
  /* ############################ */
  if (*msglvl >= 1)
    {
      monit_(n, &x[1], &fnew, &g[1], &niter, &nftotl, &nfeval, &lreset, &
             ipivot[1]);
    }
  if (nwhy < 0)
    {
      goto L160;
    }
  if (nwhy == 0 || nwhy == 2)
    {
      goto L40;
    }

  /* THE LINEAR SEARCH HAS FAILED TO FIND A LOWER POINT */

  nwhy = 3;
  goto L140;
L40:
  if (nwhy <= 1)
    {
      goto L50;
    }
  (*sfun)(n, &x[1], &fnew, &g[1]);
  ++nftotl;

  /* TERMINATE IF MORE THAN MAXFUN EVALUATIONS HAVE BEEN MADE */

L50:
  nwhy = 2;
  if (nftotl > *maxfun)
    {
      goto L150;
    }
  nwhy = 0;

  /* SET UP PARAMETERS USED IN CONVERGENCE AND RESETTING TESTS */

  difold = difnew;
  difnew = oldf - fnew;

  /* IF THIS IS THE FIRST ITERATION OF A NEW CYCLE, COMPUTE THE */
  /* PERCENTAGE REDUCTION FACTOR FOR THE RESETTING TEST. */

  if (icycle != 1)
    {
      goto L60;
    }
  if (difnew > difold * 2.)
    {
      epsred += epsred;
    }
  if (difnew < difold * .5)
    {
      epsred *= .5;
    }
L60:
  dcopy_(n, &g[1], &c__1, &w[subscr_1.lgv], &c__1);
  ztime_(n, &w[subscr_1.lgv], &ipivot[1]);
  gtg = ddot_(n, &w[subscr_1.lgv], &c__1, &w[subscr_1.lgv], &c__1);
  gnorm = sqrt(gtg);
  ftest = one + abs(fnew);
  xnorm = dnrm2_(n, &x[1], &c__1);

  /* TEST FOR CONVERGENCE */

  cnvtst_(&conv, &alpha, &pnorm, &toleps, &xnorm, &difnew, &rtleps, &ftest,
          &gtg, &peps, &epsmch, &gtpnew, &fnew, &flast, &g[1], &ipivot[1],
          n, accrcy);
  if (conv)
    {
      goto L130;
    }
  ztime_(n, &g[1], &ipivot[1]);

  /* COMPUTE THE CHANGE IN THE ITERATES AND THE CORRESPONDING CHANGE */
  /* IN THE GRADIENTS */

  if (newcon)
    {
      goto L90;
    }
  isk = subscr_1.lsk;
  ipk = subscr_1.lpk;
  iyk = subscr_1.lyk;
  ioldg = subscr_1.loldg;
  i__1 = *n;
  for (i__ = 1; i__ <= i__1; ++i__)
    {
      w[iyk] = g[i__] - w[ioldg];
      w[isk] = alpha * w[ipk];
      ++ipk;
      ++isk;
      ++iyk;
      ++ioldg;
      /* L70: */
    }

  /* SET UP PARAMETERS USED IN UPDATING THE PRECONDITIONING STRATEGY. */

  yksk = ddot_(n, &w[subscr_1.lyk], &c__1, &w[subscr_1.lsk], &c__1);
  lreset = FALSE_;
  if (icycle == nm1 || difnew < epsred * (fkeep - fnew))
    {
      lreset = TRUE_;
    }
  if (lreset)
    {
      goto L80;
    }
  yrsr = ddot_(n, &w[subscr_1.lyr], &c__1, &w[subscr_1.lsr], &c__1);
  if (yrsr <= zero)
    {
      lreset = TRUE_;
    }
L80:
  upd1 = FALSE_;

  /*      COMPUTE THE NEW SEARCH DIRECTION */

L90:
  if (upd1 && *msglvl >= 3)
    {
      s_wsfe(&io___150);
      e_wsfe();
    }
  if (newcon && *msglvl >= 3)
    {
      s_wsfe(&io___151);
      e_wsfe();
    }
  modet = *msglvl - 3;
  modlnp_(&modet, &w[subscr_1.lpk], &w[subscr_1.lgv], &w[subscr_1.lz1], &w[
            subscr_1.lv], &w[subscr_1.ldiagb], &w[subscr_1.lemat], &x[1], &g[
            1], &w[subscr_1.lzk], n, &w[1], lw, &niter, maxit, &nfeval, &
          nmodif, &nlincg, &upd1, &yksk, &gsk, &yrsr, &lreset, sfun, &
          c_true, &ipivot[1], accrcy, &gtpnew, &gnorm, &xnorm);
  if (newcon)
    {
      goto L20;
    }
  if (lreset)
    {
      goto L110;
    }

  /* COMPUTE THE ACCUMULATED STEP AND ITS CORRESPONDING */
  /* GRADIENT DIFFERENCE. */

  dxpy_(n, &w[subscr_1.lsk], &c__1, &w[subscr_1.lsr], &c__1);
  dxpy_(n, &w[subscr_1.lyk], &c__1, &w[subscr_1.lyr], &c__1);
  ++icycle;
  goto L20;

  /* RESET */

L110:
  ++ireset;

  /* INITIALIZE THE SUM OF ALL THE CHANGES IN X. */

  dcopy_(n, &w[subscr_1.lsk], &c__1, &w[subscr_1.lsr], &c__1);
  dcopy_(n, &w[subscr_1.lyk], &c__1, &w[subscr_1.lyr], &c__1);
  fkeep = fnew;
  icycle = 1;
  goto L20;

  /* ...............END OF MAIN ITERATION....................... */

L130:
  *ifail = 0;
  *f = fnew;
  return 0;
L140:
  oldf = fnew;

  /* LOCAL SEARCH COULD BE INSTALLED HERE */

L150:
  *f = oldf;
  if (*msglvl >= 1)
    {
      monit_(n, &x[1], f, &g[1], &niter, &nftotl, &nfeval, &ireset, &ipivot[
               1]);
    }
  /* SET IFAIL */

L160:
  *ifail = nwhy;
  return 0;
} /* lmqnbc_ */

/* Subroutine */ int monit_(integer *n, doublereal *x, doublereal *f,
                            doublereal *g, integer *niter, integer *nftotl, integer *nfeval,
                            integer *ireset, integer *ipivot)
{
  /* Format strings */
  static char fmt_800[] = "(\002 \002,i4,1x,i4,1x,i4,1x,1pd22.15,2x,1pd15."
                          "8)";

  /* System generated locals */
  integer i__1;

  /* Builtin functions */
  integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(void);

  /* Local variables */
  static integer i__;
  static doublereal gtg;

  /* Fortran I/O blocks */
  static cilist io___154 = {0, 6, 0, fmt_800, 0 };

  /* PRINT RESULTS OF CURRENT ITERATION */

  /* Parameter adjustments */
  --ipivot;
  --g;
  --x;

  /* Function Body */
  gtg = 0.;
  i__1 = *n;
  for (i__ = 1; i__ <= i__1; ++i__)
    {
      if (ipivot[i__] != 0)
        {
          goto L10;
        }
      gtg += g[i__] * g[i__];
L10:
;
    }
  /* s_wsfe(&io___154);
   do_fio(&c__1, (char *)&(*niter), (ftnlen)sizeof(integer));
   do_fio(&c__1, (char *)&(*nftotl), (ftnlen)sizeof(integer));
   do_fio(&c__1, (char *)&(*nfeval), (ftnlen)sizeof(integer));
   do_fio(&c__1, (char *)&(*f), (ftnlen)sizeof(doublereal));
   do_fio(&c__1, (char *)&gtg, (ftnlen)sizeof(doublereal));
   e_wsfe();*/
  return 0;
} /* monit_ */

/* Subroutine */ int ztime_(integer *n, doublereal *x, integer *ipivot)
{
  /* System generated locals */
  integer i__1;

  /* Local variables */
  static integer i__;

  /* THIS ROUTINE MULTIPLIES THE VECTOR X BY THE CONSTRAINT MATRIX Z */

  /* Parameter adjustments */
  --ipivot;
  --x;

  /* Function Body */
  i__1 = *n;
  for (i__ = 1; i__ <= i__1; ++i__)
    {
      if (ipivot[i__] != 0)
        {
          x[i__] = 0.;
        }
      /* L10: */
    }
  return 0;
} /* ztime_ */

/* Subroutine */ int stpmax_(doublereal *stepmx, doublereal *pe, doublereal *
                             spe, integer *n, doublereal *x, doublereal *p, integer *ipivot,
                             doublereal *low, doublereal *up)
{
  /* System generated locals */
  integer i__1;
  /* Local variables */
  static integer i__;
  static doublereal t;

  /* COMPUTE THE MAXIMUM ALLOWABLE STEP LENGTH */

  /* Parameter adjustments */
  --up; --low; --ipivot; --p; --x;

  /* Function Body */
  *spe = *stepmx / *pe;
  /* SPE IS THE STANDARD (UNCONSTRAINED) MAX STEP */
  i__1 = *n;
  for (i__ = 1; i__ <= i__1; ++i__)
    {
      if (ipivot[i__] != 0) goto L10;
      if (p[i__] == 0.) goto L10;
      if (p[i__] > 0.) goto L5;
      t = low[i__] - x[i__];
      if (t > *spe * p[i__]) *spe = t / p[i__];
      goto L10;
L5:
      t = up[i__] - x[i__];
      if (t < *spe * p[i__]) *spe = t / p[i__];
L10:;
    }
  return 0;
} /* stpmax_ */

/* Subroutine */ int modz_(integer *n, doublereal *x, doublereal *p, integer *
                           ipivot, doublereal *epsmch, doublereal *low, doublereal *up,
                           doublereal *flast, doublereal *fnew)
{
  /* System generated locals */
  integer i__1;
  doublereal d__1;

  /* Local variables */
  static integer i__;
  static doublereal tol;

  /* UPDATE THE CONSTRAINT MATRIX IF A NEW CONSTRAINT IS ENCOUNTERED */

  /* Parameter adjustments */
  --up;
  --low;
  --ipivot;
  --p;
  --x;

  /* Function Body */
  i__1 = *n;
  for (i__ = 1; i__ <= i__1; ++i__)
    {
      if (ipivot[i__] != 0)
        {
          goto L10;
        }
      if (p[i__] == 0.)
        {
          goto L10;
        }
      if (p[i__] > 0.)
        {
          goto L5;
        }
      tol = *epsmch * 10. * ((d__1 = low[i__], abs(d__1)) + 1.);
      if (x[i__] - low[i__] > tol)
        {
          goto L10;
        }
      *flast = *fnew;
      ipivot[i__] = -1;
      x[i__] = low[i__];
      goto L10;
L5:
      tol = *epsmch * 10. * ((d__1 = up[i__], abs(d__1)) + 1.);
      if (up[i__] - x[i__] > tol)
        {
          goto L10;
        }
      *flast = *fnew;
      ipivot[i__] = 1;
      x[i__] = up[i__];
L10:
;
    }
  return 0;
} /* modz_ */

/* Subroutine */ int cnvtst_(logical *conv, doublereal *alpha, doublereal *
                             pnorm, doublereal *toleps, doublereal *xnorm, doublereal *difnew,
                             doublereal *rtleps, doublereal *ftest, doublereal *gtg, doublereal *
                             peps, doublereal *epsmch, doublereal *gtpnew, doublereal *fnew,
                             doublereal *flast, doublereal *g, integer *ipivot, integer *n,
                             doublereal *accrcy)
{
  /* System generated locals */
  integer i__1;

  /* Local variables */
  static doublereal cmax;
  static integer imax, i__;
  static doublereal t;
  static logical ltest;
  static doublereal one;

  /* TEST FOR CONVERGENCE */

  /* Parameter adjustments */
  --ipivot;
  --g;

  /* Function Body */
  imax = 0;
  cmax = 0.;
  ltest = *flast - *fnew <= *gtpnew * -.5;
  i__1 = *n;
  for (i__ = 1; i__ <= i__1; ++i__)
    {
      if (ipivot[i__] == 0 || ipivot[i__] == 2)
        {
          goto L10;
        }
      t = -ipivot[i__] * g[i__];
      if (t >= 0.)
        {
          goto L10;
        }
      *conv = FALSE_;
      if (ltest)
        {
          goto L10;
        }
      if (cmax <= t)
        {
          goto L10;
        }
      cmax = t;
      imax = i__;
L10:
;
    }
  if (imax == 0)
    {
      goto L15;
    }
  ipivot[imax] = 0;
  *flast = *fnew;
  return 0;
L15:
  *conv = FALSE_;
  one = 1.;
  if ((*alpha * *pnorm >= *toleps * (one + *xnorm) || abs(*difnew) >= *
       rtleps * *ftest || *gtg >= *peps * *ftest * *ftest) && *gtg >= *
      accrcy * 1e-4 * *ftest * *ftest)
    {
      return 0;
    }
  *conv = TRUE_;

  /* FOR DETAILS, SEE GILL, MURRAY, AND WRIGHT (1981, P. 308) AND */
  /* FLETCHER (1981, P. 116).  THE MULTIPLIER TESTS (HERE, TESTING */
  /* THE SIGN OF THE COMPONENTS OF THE GRADIENT) MAY STILL NEED TO */
  /* MODIFIED TO INCORPORATE TOLERANCES FOR ZERO. */

  return 0;
} /* cnvtst_ */

/* Subroutine */ int crash_(integer *n, doublereal *x, integer *ipivot,
                            doublereal *low, doublereal *up, integer *ier)
{
  /* System generated locals */
  integer i__1;

  /* Local variables */
  static integer i__;

  /* THIS INITIALIZES THE CONSTRAINT INFORMATION, AND ENSURES THAT THE */
  /* INITIAL POINT SATISFIES  LOW <= X <= UP. */
  /* THE CONSTRAINTS ARE CHECKED FOR CONSISTENCY. */

  /* Parameter adjustments */
  --up;
  --low;
  --ipivot;
  --x;

  /* Function Body */
  *ier = 0;
  i__1 = *n;
  for (i__ = 1; i__ <= i__1; ++i__)
    {
      if (x[i__] < low[i__])
        {
          x[i__] = low[i__];
        }
      if (x[i__] > up[i__])
        {
          x[i__] = up[i__];
        }
      ipivot[i__] = 0;
      if (x[i__] == low[i__])
        {
          ipivot[i__] = -1;
        }
      if (x[i__] == up[i__])
        {
          ipivot[i__] = 1;
        }
      if (up[i__] == low[i__])
        {
          ipivot[i__] = 2;
        }
      if (low[i__] > up[i__])
        {
          *ier = -i__;
        }
      /* L30: */
    }
  return 0;
} /* crash_ */

/* THE VECTORS SK AND YK, ALTHOUGH NOT IN THE CALL, */
/* ARE USED (VIA THEIR POSITION IN W) BY THE ROUTINE MSOLVE. */

/* Subroutine */ int modlnp_(integer *modet, doublereal *zsol, doublereal *gv,
                             doublereal *r__, doublereal *v, doublereal *diagb, doublereal *emat,
                             doublereal *x, doublereal *g, doublereal *zk, integer *n, doublereal *
                             w, integer *lw, integer *niter, integer *maxit, integer *nfeval,
                             integer *nmodif, integer *nlincg, logical *upd1, doublereal *yksk,
                             doublereal *gsk, doublereal *yrsr, logical *lreset, FTruncatedNewton *sfun,
                             logical *bounds, integer *ipivot, doublereal *accrcy, doublereal *gtp,
                             doublereal *gnorm, doublereal *xnorm)
{
  /* Format strings */
  static char fmt_800[] = "(\002 \002,//,\002 ENTERING MODLNP\002)";
  static char fmt_810[] = "(\002 \002,//,\002 ### ITERATION \002,i2,\002 #"
                          "##\002)";
  static char fmt_820[] = "(\002 ALPHA\002,1pd16.8)";
  static char fmt_830[] = "(\002 G(T)Z POSITIVE AT ITERATION \002,i2,\002 "
                          "- TRUNCATING METHOD\002,/)";
  static char fmt_840[] = "(\002 \002,10x,\002HESSIAN NOT POSITIVE-DEFIN"
                          "ITE\002)";
  static char fmt_850[] = "(\002 \002,/,8x,\002MODLAN TRUNCATED AFTER \002"
                          ",i3,\002 ITERATIONS\002,\002  RNORM = \002,1pd14.6)";
  static char fmt_860[] = "(\002 PRECONDITIONING NOT POSITIVE-DEFINITE\002)"
;

  /* System generated locals */
  integer i__1, i__2;
  doublereal d__1;

  /* Builtin functions */
  integer s_wsfe(cilist *), e_wsfe(void), do_fio(integer *, char *, ftnlen);

  /* Local variables */
  static doublereal beta;
  extern doublereal ddot_(integer *, doublereal *, integer *, doublereal *,
                            integer *);
  static doublereal qold, qnew;
  extern /* Subroutine */ int ndia3_(integer *, doublereal *, doublereal *,
                                       doublereal *, doublereal *, doublereal *, integer *);
  static integer i__, k;
  static doublereal alpha, delta;
  extern /* Subroutine */ int dcopy_(integer *, doublereal *, integer *,
                                       doublereal *, integer *), gtims_(doublereal *, doublereal *,
                                                                        integer *, doublereal *, doublereal *, doublereal *, integer *,
                                                                        FTruncatedNewton *, logical *, doublereal *, doublereal *, doublereal *),
    daxpy_(integer *, doublereal *, doublereal *, integer *,
           doublereal *, integer *);
  static logical first;
  extern /* Subroutine */ int ztime_(integer *, doublereal *, integer *);
  static doublereal rzold, rnorm, qtest, pr;
  extern /* Subroutine */ int negvec_(integer *, doublereal *);
  static doublereal rz;
  extern /* Subroutine */ int initpc_(doublereal *, doublereal *, integer *,
                                        doublereal *, integer *, integer *, logical *, doublereal *,
                                        doublereal *, doublereal *, logical *), msolve_(doublereal *,
                                            doublereal *, integer *, doublereal *, integer *, logical *,
                                            doublereal *, doublereal *, doublereal *, logical *, logical *);
  static doublereal rhsnrm, tol, vgv;

  /* Fortran I/O blocks */
  static cilist io___167 = {0, 6, 0, fmt_800, 0 };
  static cilist io___174 = {0, 6, 0, fmt_810, 0 };
  static cilist io___181 = {0, 6, 0, fmt_820, 0 };
  static cilist io___185 = {0, 6, 0, fmt_830, 0 };
  static cilist io___186 = {0, 6, 0, fmt_840, 0 };
  static cilist io___187 = {0, 6, 0, fmt_850, 0 };
  static cilist io___189 = {0, 6, 0, fmt_860, 0 };

  /* THIS ROUTINE PERFORMS A PRECONDITIONED CONJUGATE-GRADIENT */
  /* ITERATION IN ORDER TO SOLVE THE NEWTON EQUATIONS FOR A SEARCH */
  /* DIRECTION FOR A TRUNCATED-NEWTON ALGORITHM.  WHEN THE VALUE OF THE */
  /* QUADRATIC MODEL IS SUFFICIENTLY REDUCED, */
  /* THE ITERATION IS TERMINATED. */

  /* PARAMETERS */

  /* MODET       - INTEGER WHICH CONTROLS AMOUNT OF OUTPUT */
  /* ZSOL        - COMPUTED SEARCH DIRECTION */
  /* G           - CURRENT GRADIENT */
  /* GV,GZ1,V    - SCRATCH VECTORS */
  /* R           - RESIDUAL */
  /* DIAGB,EMAT  - DIAGONAL PRECONDITONING MATRIX */
  /* NITER       - NONLINEAR ITERATION # */
  /* FEVAL       - VALUE OF QUADRATIC FUNCTION */

  /* ************************************************************* */
  /* INITIALIZATION */
  /* ************************************************************* */

  /* GENERAL INITIALIZATION */

  /* Parameter adjustments */
  --zk;
  --g;
  --x;
  --emat;
  --diagb;
  --v;
  --r__;
  --gv;
  --zsol;
  --w;
  --ipivot;

  /* Function Body */
  if (*modet > 0)
    {
      s_wsfe(&io___167);
      e_wsfe();
    }
  if (*maxit == 0)
    {
      return 0;
    }
  first = TRUE_;
  rhsnrm = *gnorm;
  tol = 1e-12;
  qold = 0.;

  /* INITIALIZATION FOR PRECONDITIONED CONJUGATE-GRADIENT ALGORITHM */

  initpc_(&diagb[1], &emat[1], n, &w[1], lw, modet, upd1, yksk, gsk, yrsr,
          lreset);
  i__1 = *n;
  for (i__ = 1; i__ <= i__1; ++i__)
    {
      r__[i__] = -g[i__];
      v[i__] = 0.;
      zsol[i__] = 0.;
      /* L10: */
    }

  /* ************************************************************ */
  /* MAIN ITERATION */
  /* ************************************************************ */

  i__1 = *maxit;
  for (k = 1; k <= i__1; ++k)
    {
      ++(*nlincg);
      if (*modet > 1)
        {
          s_wsfe(&io___174);
          do_fio(&c__1, (char *)&k, (ftnlen)sizeof(integer));
          e_wsfe();
        }

      /* CG ITERATION TO SOLVE SYSTEM OF EQUATIONS */

      if (*bounds)
        {
          ztime_(n, &r__[1], &ipivot[1]);
        }
      msolve_(&r__[1], &zk[1], n, &w[1], lw, upd1, yksk, gsk, yrsr, lreset,
              &first);
      if (*bounds)
        {
          ztime_(n, &zk[1], &ipivot[1]);
        }
      rz = ddot_(n, &r__[1], &c__1, &zk[1], &c__1);
      if (rz / rhsnrm < tol)
        {
          goto L80;
        }
      if (k == 1)
        {
          beta = 0.;
        }
      if (k > 1)
        {
          beta = rz / rzold;
        }
      i__2 = *n;
      for (i__ = 1; i__ <= i__2; ++i__)
        {
          v[i__] = zk[i__] + beta * v[i__];
          /* L20: */
        }
      if (*bounds)
        {
          ztime_(n, &v[1], &ipivot[1]);
        }
      gtims_(&v[1], &gv[1], n, &x[1], &g[1], &w[1], lw, sfun, &first,
             &delta, accrcy, xnorm);
      if (*bounds)
        {
          ztime_(n, &gv[1], &ipivot[1]);
        }
      ++(*nfeval);
      vgv = ddot_(n, &v[1], &c__1, &gv[1], &c__1);
      if (vgv / rhsnrm < tol)
        {
          goto L50;
        }
      ndia3_(n, &emat[1], &v[1], &gv[1], &r__[1], &vgv, modet);

      /* COMPUTE LINEAR STEP LENGTH */

      alpha = rz / vgv;
      if (*modet >= 1)
        {
          s_wsfe(&io___181);
          do_fio(&c__1, (char *)&alpha, (ftnlen)sizeof(doublereal));
          e_wsfe();
        }

      /* COMPUTE CURRENT SOLUTION AND RELATED VECTORS */

      daxpy_(n, &alpha, &v[1], &c__1, &zsol[1], &c__1);
      d__1 = -alpha;
      daxpy_(n, &d__1, &gv[1], &c__1, &r__[1], &c__1);

      /* TEST FOR CONVERGENCE */

      *gtp = ddot_(n, &zsol[1], &c__1, &g[1], &c__1);
      pr = ddot_(n, &r__[1], &c__1, &zsol[1], &c__1);
      qnew = (*gtp + pr) * .5;
      qtest = k * (1. - qold / qnew);
      if (qtest < 0.)
        {
          goto L70;
        }
      qold = qnew;
      if (qtest <= .5)
        {
          goto L70;
        }

      /* PERFORM CAUTIONARY TEST */

      if (*gtp > 0.)
        {
          goto L40;
        }
      rzold = rz;
      /* L30: */
    }

  /* TERMINATE ALGORITHM */

  --k;
  goto L70;

  /* TRUNCATE ALGORITHM IN CASE OF AN EMERGENCY */

L40:
  if (*modet >= -1)
    {
      s_wsfe(&io___185);
      do_fio(&c__1, (char *)&k, (ftnlen)sizeof(integer));
      e_wsfe();
    }
  d__1 = -alpha;
  daxpy_(n, &d__1, &v[1], &c__1, &zsol[1], &c__1);
  *gtp = ddot_(n, &zsol[1], &c__1, &g[1], &c__1);
  goto L90;
L50:
  if (*modet > -2)
    {
      s_wsfe(&io___186);
      e_wsfe();
    }
  /* L60: */
  if (k > 1)
    {
      goto L70;
    }
  msolve_(&g[1], &zsol[1], n, &w[1], lw, upd1, yksk, gsk, yrsr, lreset, &
          first);
  negvec_(n, &zsol[1]);
  if (*bounds)
    {
      ztime_(n, &zsol[1], &ipivot[1]);
    }
  *gtp = ddot_(n, &zsol[1], &c__1, &g[1], &c__1);
L70:
  if (*modet >= -1)
    {
      s_wsfe(&io___187);
      do_fio(&c__1, (char *)&k, (ftnlen)sizeof(integer));
      do_fio(&c__1, (char *)&rnorm, (ftnlen)sizeof(doublereal));
      e_wsfe();
    }
  goto L90;
L80:
  if (*modet >= -1)
    {
      s_wsfe(&io___189);
      e_wsfe();
    }
  if (k > 1)
    {
      goto L70;
    }
  dcopy_(n, &g[1], &c__1, &zsol[1], &c__1);
  negvec_(n, &zsol[1]);
  if (*bounds)
    {
      ztime_(n, &zsol[1], &ipivot[1]);
    }
  *gtp = ddot_(n, &zsol[1], &c__1, &g[1], &c__1);
  goto L70;

  /* STORE (OR RESTORE) DIAGONAL PRECONDITIONING */

L90:
  dcopy_(n, &emat[1], &c__1, &diagb[1], &c__1);
  return 0;
} /* modlnp_ */

/* Subroutine */ int ndia3_(integer *n, doublereal *e, doublereal *v,
                            doublereal *gv, doublereal *r__, doublereal *vgv, integer *modet)
{
  /* Format strings */
  static char fmt_800[] = "(\002 *** EMAT NEGATIVE:  \002,1pd16.8)";

  /* System generated locals */
  integer i__1;

  /* Builtin functions */
  integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(void);

  /* Local variables */
  extern doublereal ddot_(integer *, doublereal *, integer *, doublereal *,
                            integer *);
  static integer i__;
  static doublereal vr;

  /* Fortran I/O blocks */
  static cilist io___192 = {0, 6, 0, fmt_800, 0 };

  /* UPDATE THE PRECONDITIOING MATRIX BASED ON A DIAGONAL VERSION */
  /* OF THE BFGS QUASI-NEWTON UPDATE. */

  /* Parameter adjustments */
  --r__;
  --gv;
  --v;
  --e;

  /* Function Body */
  vr = ddot_(n, &v[1], &c__1, &r__[1], &c__1);
  i__1 = *n;
  for (i__ = 1; i__ <= i__1; ++i__)
    {
      e[i__] = e[i__] - r__[i__] * r__[i__] / vr + gv[i__] * gv[i__] / *vgv;
      if (e[i__] > 1e-6)
        {
          goto L10;
        }
      if (*modet > -2)
        {
          s_wsfe(&io___192);
          do_fio(&c__1, (char *)&e[i__], (ftnlen)sizeof(doublereal));
          e_wsfe();
        }
      e[i__] = 1.;
L10:
;
    }
  return 0;
} /* ndia3_ */

/*      SERVICE ROUTINES FOR OPTIMIZATION */

/* Subroutine */ int negvec_(integer *n, doublereal *v)
{
  /* System generated locals */
  integer i__1;

  /* Local variables */
  static integer i__;

  /* NEGATIVE OF THE VECTOR V */

  /* Parameter adjustments */
  --v;

  /* Function Body */
  i__1 = *n;
  for (i__ = 1; i__ <= i__1; ++i__)
    {
      v[i__] = -v[i__];
      /* L10: */
    }
  return 0;
} /* negvec_ */

/* Subroutine */ int lsout_(integer *iloc, integer *itest, doublereal *xmin,
                            doublereal *fmin, doublereal *gmin, doublereal *xw, doublereal *fw,
                            doublereal *gw, doublereal *u, doublereal *a, doublereal *b,
                            doublereal *tol, doublereal *eps, doublereal *scxbd, doublereal *
                            xlamda)
{
  /* Format strings */
  static char fmt_800[] = "(///\002 OUTPUT FROM LINEAR SEARCH\002)";
  static char fmt_810[] = "(\002  TOL AND EPS\002/2d25.14)";
  static char fmt_820[] = "(\002  CURRENT UPPER AND LOWER BOUNDS\002/2d25."
                          "14)";
  static char fmt_830[] = "(\002  STRICT UPPER BOUND\002/d25.14)";
  static char fmt_840[] = "(\002  XW, FW, GW\002/3d25.14)";
  static char fmt_850[] = "(\002  XMIN, FMIN, GMIN\002/3d25.14)";
  static char fmt_860[] = "(\002  NEW ESTIMATE\002/2d25.14)";
  static char fmt_870[] = "(\002  ILOC AND ITEST\002/2i3)";

  /* Builtin functions */
  integer s_wsfe(cilist *), e_wsfe(void), do_fio(integer *, char *, ftnlen);

  /* Local variables */
  static doublereal ybnd, ya, yb, yu, yw;

  /* Fortran I/O blocks */
  static cilist io___199 = {0, 6, 0, fmt_800, 0 };
  static cilist io___200 = {0, 6, 0, fmt_810, 0 };
  static cilist io___201 = {0, 6, 0, fmt_820, 0 };
  static cilist io___202 = {0, 6, 0, fmt_830, 0 };
  static cilist io___203 = {0, 6, 0, fmt_840, 0 };
  static cilist io___204 = {0, 6, 0, fmt_850, 0 };
  static cilist io___205 = {0, 6, 0, fmt_860, 0 };
  static cilist io___206 = {0, 6, 0, fmt_870, 0 };

  /* ERROR PRINTOUTS FOR GETPTC */

  yu = *xmin + *u;
  ya = *a + *xmin;
  yb = *b + *xmin;
  yw = *xw + *xmin;
  ybnd = *scxbd + *xmin;
  s_wsfe(&io___199);
  e_wsfe();
  s_wsfe(&io___200);
  do_fio(&c__1, (char *)&(*tol), (ftnlen)sizeof(doublereal));
  do_fio(&c__1, (char *)&(*eps), (ftnlen)sizeof(doublereal));
  e_wsfe();
  s_wsfe(&io___201);
  do_fio(&c__1, (char *)&ya, (ftnlen)sizeof(doublereal));
  do_fio(&c__1, (char *)&yb, (ftnlen)sizeof(doublereal));
  e_wsfe();
  s_wsfe(&io___202);
  do_fio(&c__1, (char *)&ybnd, (ftnlen)sizeof(doublereal));
  e_wsfe();
  s_wsfe(&io___203);
  do_fio(&c__1, (char *)&yw, (ftnlen)sizeof(doublereal));
  do_fio(&c__1, (char *)&(*fw), (ftnlen)sizeof(doublereal));
  do_fio(&c__1, (char *)&(*gw), (ftnlen)sizeof(doublereal));
  e_wsfe();
  s_wsfe(&io___204);
  do_fio(&c__1, (char *)&(*xmin), (ftnlen)sizeof(doublereal));
  do_fio(&c__1, (char *)&(*fmin), (ftnlen)sizeof(doublereal));
  do_fio(&c__1, (char *)&(*gmin), (ftnlen)sizeof(doublereal));
  e_wsfe();
  s_wsfe(&io___205);
  do_fio(&c__1, (char *)&yu, (ftnlen)sizeof(doublereal));
  e_wsfe();
  s_wsfe(&io___206);
  do_fio(&c__1, (char *)&(*iloc), (ftnlen)sizeof(integer));
  do_fio(&c__1, (char *)&(*itest), (ftnlen)sizeof(integer));
  e_wsfe();
  return 0;
} /* lsout_ */

doublereal step1_(doublereal *fnew, doublereal *fm, doublereal *gtp,
                  doublereal *smax)
{
  /* System generated locals */
  doublereal ret_val, d__1;

  /* Local variables */
  static doublereal d__, alpha;
  extern doublereal mchpr1_(void);
  static doublereal epsmch;

  /* ******************************************************** */
  /* STEP1 RETURNS THE LENGTH OF THE INITIAL STEP TO BE TAKEN ALONG THE */
  /* VECTOR P IN THE NEXT LINEAR SEARCH. */
  /* ******************************************************** */

  epsmch = mchpr1_();
  d__ = (d__1 = *fnew - *fm, abs(d__1));
  alpha = 1.;
  if (d__ * 2. <= -(*gtp) && d__ >= epsmch)
    {
      alpha = d__ * -2. / *gtp;
    }
  if (alpha >= *smax)
    {
      alpha = *smax;
    }
  ret_val = alpha;
  return ret_val;
} /* step1_ */

doublereal mchpr1_(void)
{
  /* System generated locals */
  doublereal ret_val;

  /* RETURNS THE VALUE OF EPSMCH, WHERE EPSMCH IS THE SMALLEST POSSIBLE */
  /* REAL NUMBER SUCH THAT 1.0 + EPSMCH .GT. 1.0 */

  /* FOR IEEE */

  ret_val = DBL_EPSILON;

  return ret_val;
} /* mchpr1_ */

/* Subroutine */ int chkucp_(integer *lwtest, integer *maxfun, integer *nwhy,
                             integer *n, doublereal *alpha, doublereal *epsmch, doublereal *eta,
                             doublereal *peps, doublereal *rteps, doublereal *rtol, doublereal *
                             rtolsq, doublereal *stepmx, doublereal *test, doublereal *xtol,
                             doublereal *xnorm, doublereal *x, integer *lw, doublereal *small,
                             doublereal *tiny, doublereal *accrcy)
{
  /* Builtin functions */
  double sqrt(doublereal), pow_dd(doublereal *, doublereal *);

  /* Local variables */
  extern doublereal dnrm2_(integer *, doublereal *, integer *), mchpr1_(
      void);

  /* CHECKS PARAMETERS AND SETS CONSTANTS WHICH ARE COMMON TO BOTH */
  /* DERIVATIVE AND NON-DERIVATIVE ALGORITHMS */

  /* Parameter adjustments */
  --x;

  /* Function Body */
  *epsmch = mchpr1_();
  *small = *epsmch * *epsmch;
  *tiny = *small;
  *nwhy = -1;
  *rteps = sqrt(*epsmch);
  *rtol = *xtol;
  if (abs(*rtol) < *accrcy)
    {
      *rtol = *rteps * 10.;
    }

  /* CHECK FOR ERRORS IN THE INPUT PARAMETERS */

  if (*lw < *lwtest || *n < 1 || *rtol < 0. || *eta >= 1. || *eta < 0. || *
      stepmx < *rtol || *maxfun < 1)
    {
      return 0;
    }
  *nwhy = 0;

  /* SET CONSTANTS FOR LATER */

  *rtolsq = *rtol * *rtol;
  *peps = pow_dd(accrcy, &c_b246);
  *xnorm = dnrm2_(n, &x[1], &c__1);
  *alpha = 0.;
  *test = 0.;
  return 0;
} /* chkucp_ */

/* Subroutine */ int setucr_(doublereal *small, integer *nftotl, integer *
                             niter, integer *n, doublereal *f, doublereal *fnew, doublereal *fm,
                             doublereal *gtg, doublereal *oldf, FTruncatedNewton *sfun, doublereal *g,
                             doublereal *x)
{
  extern doublereal ddot_(integer *, doublereal *, integer *, doublereal *,
                            integer *);

  /* CHECK INPUT PARAMETERS, COMPUTE THE INITIAL FUNCTION VALUE, SET */
  /* CONSTANTS FOR THE SUBSEQUENT MINIMIZATION */

  /* Parameter adjustments */
  --x;
  --g;

  /* Function Body */
  *fm = *f;

  /* COMPUTE THE INITIAL FUNCTION VALUE */

  (*sfun)(n, &x[1], fnew, &g[1]);
  *nftotl = 1;

  /* SET CONSTANTS FOR LATER */

  *niter = 0;
  *oldf = *fnew;
  *gtg = ddot_(n, &g[1], &c__1, &g[1], &c__1);
  return 0;
} /* setucr_ */

/* Subroutine */ int gtims_(doublereal *v, doublereal *gv, integer *n,
                            doublereal *x, doublereal *g, doublereal *w, integer *lw, FTruncatedNewton *sfun,
                            logical *first, doublereal *delta, doublereal *accrcy, doublereal *
                            xnorm)
{
  /* System generated locals */
  integer i__1;

  /* Builtin functions */
  double sqrt(doublereal);

  /* Local variables */
  static doublereal dinv, f;
  static integer i__, ihg;

  /* THIS ROUTINE COMPUTES THE PRODUCT OF THE MATRIX G TIMES THE VECTOR */
  /* V AND STORES THE RESULT IN THE VECTOR GV (FINITE-DIFFERENCE VERSION) */

  /* Parameter adjustments */
  --g;
  --x;
  --gv;
  --v;
  --w;

  /* Function Body */
  if (! (*first))
    {
      goto L20;
    }
  *delta = sqrt(*accrcy) * (*xnorm + 1.);
  *first = FALSE_;
L20:
  dinv = 1. / *delta;
  ihg = subscr_2.lhg;
  i__1 = *n;
  for (i__ = 1; i__ <= i__1; ++i__)
    {
      w[ihg] = x[i__] + *delta * v[i__];
      ++ihg;
      /* L30: */
    }
  (*sfun)(n, &w[subscr_2.lhg], &f, &gv[1]);
  i__1 = *n;
  for (i__ = 1; i__ <= i__1; ++i__)
    {
      gv[i__] = (gv[i__] - g[i__]) * dinv;
      /* L40: */
    }
  return 0;
} /* gtims_ */

/* Subroutine */ int msolve_(doublereal *g, doublereal *y, integer *n,
                             doublereal *w, integer *lw, logical *upd1, doublereal *yksk,
                             doublereal *gsk, doublereal *yrsr, logical *lreset, logical *first)
{
  extern /* Subroutine */ int mslv_(doublereal *, doublereal *, integer *,
                                      doublereal *, doublereal *, doublereal *, doublereal *,
                                      doublereal *, doublereal *, doublereal *, doublereal *, logical *,
                                      doublereal *, doublereal *, doublereal *, logical *, logical *);

  /* THIS ROUTINE SETS UPT THE ARRAYS FOR MSLV */

  /* Parameter adjustments */
  --y;
  --g;
  --w;

  /* Function Body */
  mslv_(&g[1], &y[1], n, &w[subscr_2.lsk], &w[subscr_2.lyk], &w[
          subscr_2.ldiagb], &w[subscr_2.lsr], &w[subscr_2.lyr], &w[
          subscr_2.lhyr], &w[subscr_2.lhg], &w[subscr_2.lhyk], upd1, yksk,
        gsk, yrsr, lreset, first);
  return 0;
} /* msolve_ */

/* Subroutine */ int mslv_(doublereal *g, doublereal *y, integer *n,
                           doublereal *sk, doublereal *yk, doublereal *diagb, doublereal *sr,
                           doublereal *yr, doublereal *hyr, doublereal *hg, doublereal *hyk,
                           logical *upd1, doublereal *yksk, doublereal *gsk, doublereal *yrsr,
                           logical *lreset, logical *first)
{
  /* System generated locals */
  integer i__1;

  /* Local variables */
  extern doublereal ddot_(integer *, doublereal *, integer *, doublereal *,
                            integer *);
  static doublereal ghyk, ghyr, yksr;
  static integer i__;
  static doublereal ykhyk, ykhyr, yrhyr, rdiagb;
  extern /* Subroutine */ int ssbfgs_(integer *, doublereal *, doublereal *,
                                        doublereal *, doublereal *, doublereal *, doublereal *,
                                        doublereal *, doublereal *, doublereal *, doublereal *);
  static doublereal one, gsr;

  /* THIS ROUTINE ACTS AS A PRECONDITIONING STEP FOR THE */
  /* LINEAR CONJUGATE-GRADIENT ROUTINE.  IT IS ALSO THE */
  /* METHOD OF COMPUTING THE SEARCH DIRECTION FROM THE */
  /* GRADIENT FOR THE NON-LINEAR CONJUGATE-GRADIENT CODE. */
  /* IT REPRESENTS A TWO-STEP SELF-SCALED BFGS FORMULA. */

  /* Parameter adjustments */
  --hyk;
  --hg;
  --hyr;
  --yr;
  --sr;
  --diagb;
  --yk;
  --sk;
  --y;
  --g;

  /* Function Body */
  if (*upd1)
    {
      goto L100;
    }
  one = 1.;
  *gsk = ddot_(n, &g[1], &c__1, &sk[1], &c__1);
  if (*lreset)
    {
      goto L60;
    }

  /* COMPUTE HG AND HY WHERE H IS THE INVERSE OF THE DIAGONALS */

  i__1 = *n;
  for (i__ = 1; i__ <= i__1; ++i__)
    {
      rdiagb = 1. / diagb[i__];
      hg[i__] = g[i__] * rdiagb;
      if (*first)
        {
          hyk[i__] = yk[i__] * rdiagb;
        }
      if (*first)
        {
          hyr[i__] = yr[i__] * rdiagb;
        }
      /* L57: */
    }
  if (*first)
    {
      yksr = ddot_(n, &yk[1], &c__1, &sr[1], &c__1);
    }
  if (*first)
    {
      ykhyr = ddot_(n, &yk[1], &c__1, &hyr[1], &c__1);
    }
  gsr = ddot_(n, &g[1], &c__1, &sr[1], &c__1);
  ghyr = ddot_(n, &g[1], &c__1, &hyr[1], &c__1);
  if (*first)
    {
      yrhyr = ddot_(n, &yr[1], &c__1, &hyr[1], &c__1);
    }
  ssbfgs_(n, &one, &sr[1], &yr[1], &hg[1], &hyr[1], yrsr, &yrhyr, &gsr, &
          ghyr, &hg[1]);
  if (*first)
    {
      ssbfgs_(n, &one, &sr[1], &yr[1], &hyk[1], &hyr[1], yrsr, &yrhyr, &
              yksr, &ykhyr, &hyk[1]);
    }
  ykhyk = ddot_(n, &hyk[1], &c__1, &yk[1], &c__1);
  ghyk = ddot_(n, &hyk[1], &c__1, &g[1], &c__1);
  ssbfgs_(n, &one, &sk[1], &yk[1], &hg[1], &hyk[1], yksk, &ykhyk, gsk, &
          ghyk, &y[1]);
  return 0;
L60:

  /* COMPUTE GH AND HY WHERE H IS THE INVERSE OF THE DIAGONALS */

  i__1 = *n;
  for (i__ = 1; i__ <= i__1; ++i__)
    {
      rdiagb = 1. / diagb[i__];
      hg[i__] = g[i__] * rdiagb;
      if (*first)
        {
          hyk[i__] = yk[i__] * rdiagb;
        }
      /* L65: */
    }
  if (*first)
    {
      ykhyk = ddot_(n, &yk[1], &c__1, &hyk[1], &c__1);
    }
  ghyk = ddot_(n, &g[1], &c__1, &hyk[1], &c__1);
  ssbfgs_(n, &one, &sk[1], &yk[1], &hg[1], &hyk[1], yksk, &ykhyk, gsk, &
          ghyk, &y[1]);
  return 0;
L100:
  i__1 = *n;
  for (i__ = 1; i__ <= i__1; ++i__)
    {
      /* L110: */
      y[i__] = g[i__] / diagb[i__];
    }
  return 0;
} /* mslv_ */

/* Subroutine */ int ssbfgs_(integer *n, doublereal *gamma, doublereal *sj,
                             doublereal *yj, doublereal *hjv, doublereal *hjyj, doublereal *yjsj,
                             doublereal *yjhyj, doublereal *vsj, doublereal *vhyj, doublereal *
                             hjp1v)
{
  /* System generated locals */
  integer i__1;

  /* Local variables */
  static doublereal beta;
  static integer i__;
  static doublereal delta;

  /* SELF-SCALED BFGS */

  /* Parameter adjustments */
  --hjp1v;
  --hjyj;
  --hjv;
  --yj;
  --sj;

  /* Function Body */
  delta = (*gamma * *yjhyj / *yjsj + 1.) * *vsj / *yjsj - *gamma * *vhyj / *
          yjsj;
  beta = -(*gamma) * *vsj / *yjsj;
  i__1 = *n;
  for (i__ = 1; i__ <= i__1; ++i__)
    {
      hjp1v[i__] = *gamma * hjv[i__] + delta * sj[i__] + beta * hjyj[i__];
      /* L10: */
    }
  return 0;
} /* ssbfgs_ */

/* ROUTINES TO INITIALIZE PRECONDITIONER */

/* Subroutine */ int initpc_(doublereal *diagb, doublereal *emat, integer *n,
                             doublereal *w, integer *lw, integer *modet, logical *upd1, doublereal
                             *yksk, doublereal *gsk, doublereal *yrsr, logical *lreset)
{
  extern /* Subroutine */ int initp3_(doublereal *, doublereal *, integer *,
                                        logical *, doublereal *, doublereal *, doublereal *, doublereal *
                                        , doublereal *, doublereal *, doublereal *, integer *, logical *);

  /* Parameter adjustments */
  --emat;
  --diagb;
  --w;

  /* Function Body */
  initp3_(&diagb[1], &emat[1], n, lreset, yksk, yrsr, &w[subscr_2.lhyk], &w[
            subscr_2.lsk], &w[subscr_2.lyk], &w[subscr_2.lsr], &w[
            subscr_2.lyr], modet, upd1);
  return 0;
} /* initpc_ */

/* Subroutine */ int initp3_(doublereal *diagb, doublereal *emat, integer *n,
                             logical *lreset, doublereal *yksk, doublereal *yrsr, doublereal *bsk,
                             doublereal *sk, doublereal *yk, doublereal *sr, doublereal *yr,
                             integer *modet, logical *upd1)
{
  /* Format strings */
  static char fmt_800[] = "(\002 \002,//8x,\002DMIN =\002,1pd12.4,\002  DM"
                          "AX =\002,1pd12.4,\002 COND =\002,1pd12.4,/)";

  /* System generated locals */
  integer i__1;

  /* Builtin functions */
  integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(void);

  /* Local variables */
  static doublereal cond;
  extern doublereal ddot_(integer *, doublereal *, integer *, doublereal *,
                            integer *);
  static doublereal srds, yrsk;
  static integer i__;
  extern /* Subroutine */ int dcopy_(integer *, doublereal *, integer *,
                                       doublereal *, integer *);
  static doublereal d1, dn, td, sds;

  /* Fortran I/O blocks */
  static cilist io___235 = {0, 6, 0, fmt_800, 0 };

  /* Parameter adjustments */
  --yr;
  --sr;
  --yk;
  --sk;
  --bsk;
  --emat;
  --diagb;

  /* Function Body */
  if (*upd1)
    {
      goto L90;
    }
  if (*lreset)
    {
      goto L60;
    }
  i__1 = *n;
  for (i__ = 1; i__ <= i__1; ++i__)
    {
      bsk[i__] = diagb[i__] * sr[i__];
      /* L10: */
    }
  sds = ddot_(n, &sr[1], &c__1, &bsk[1], &c__1);
  srds = ddot_(n, &sk[1], &c__1, &bsk[1], &c__1);
  yrsk = ddot_(n, &yr[1], &c__1, &sk[1], &c__1);
  i__1 = *n;
  for (i__ = 1; i__ <= i__1; ++i__)
    {
      td = diagb[i__];
      bsk[i__] = td * sk[i__] - bsk[i__] * srds / sds + yr[i__] * yrsk / *
                 yrsr;
      emat[i__] = td - td * td * sr[i__] * sr[i__] / sds + yr[i__] * yr[i__]
                  / *yrsr;
      /* L20: */
    }
  sds = ddot_(n, &sk[1], &c__1, &bsk[1], &c__1);
  i__1 = *n;
  for (i__ = 1; i__ <= i__1; ++i__)
    {
      emat[i__] = emat[i__] - bsk[i__] * bsk[i__] / sds + yk[i__] * yk[i__]
                  / *yksk;
      /* L30: */
    }
  goto L110;
L60:
  i__1 = *n;
  for (i__ = 1; i__ <= i__1; ++i__)
    {
      bsk[i__] = diagb[i__] * sk[i__];
      /* L70: */
    }
  sds = ddot_(n, &sk[1], &c__1, &bsk[1], &c__1);
  i__1 = *n;
  for (i__ = 1; i__ <= i__1; ++i__)
    {
      td = diagb[i__];
      emat[i__] = td - td * td * sk[i__] * sk[i__] / sds + yk[i__] * yk[i__]
                  / *yksk;
      /* L80: */
    }
  goto L110;
L90:
  dcopy_(n, &diagb[1], &c__1, &emat[1], &c__1);
L110:
  if (*modet < 1)
    {
      return 0;
    }
  d1 = emat[1];
  dn = emat[1];
  i__1 = *n;
  for (i__ = 1; i__ <= i__1; ++i__)
    {
      if (emat[i__] < d1)
        {
          d1 = emat[i__];
        }
      if (emat[i__] > dn)
        {
          dn = emat[i__];
        }
      /* L120: */
    }
  cond = dn / d1;
  s_wsfe(&io___235);
  do_fio(&c__1, (char *)&d1, (ftnlen)sizeof(doublereal));
  do_fio(&c__1, (char *)&dn, (ftnlen)sizeof(doublereal));
  do_fio(&c__1, (char *)&cond, (ftnlen)sizeof(doublereal));
  e_wsfe();
  return 0;
} /* initp3_ */

/* Subroutine */ int setpar_(integer *n)
{
  static integer i__;

  /* SET UP PARAMETERS FOR THE OPTIMIZATION ROUTINE */

  for (i__ = 1; i__ <= 14; ++i__)
    {
      subscr_3.lsub[i__ - 1] = (i__ - 1) * *n + 1;
      /* L10: */
    }
  subscr_3.lwtest = subscr_3.lsub[13] + *n - 1;
  return 0;
} /* setpar_ */

/*      LINE SEARCH ALGORITHMS OF GILL AND MURRAY */

/* Subroutine */ int linder_(integer *n, FTruncatedNewton *sfun, doublereal *small,
                             doublereal *epsmch, doublereal *reltol, doublereal *abstol,
                             doublereal *tnytol, doublereal *eta, doublereal *sftbnd, doublereal *
                             xbnd, doublereal *p, doublereal *gtp, doublereal *x, doublereal *f,
                             doublereal *alpha, doublereal *g, integer *nftotl, integer *iflag,
                             doublereal *w, integer *lw)
{
  /* System generated locals */
  integer i__1;

  /* Builtin functions */
  double sqrt(doublereal);

  /* Local variables */
  static doublereal oldf, fmin, gmin;
  extern doublereal ddot_(integer *, doublereal *, integer *, doublereal *,
                            integer *);
  static integer numf;
  static doublereal step, xmin, a, b, e;
  static integer i__, l;
  static doublereal u;
  extern /* Subroutine */ int dcopy_(integer *, doublereal *, integer *,
                                       doublereal *, integer *);
  static integer itcnt;
  static doublereal b1;
  static integer itest, nprnt;
  extern /* Subroutine */ int lsout_(integer *, integer *, doublereal *,
                                       doublereal *, doublereal *, doublereal *, doublereal *,
                                       doublereal *, doublereal *, doublereal *, doublereal *,
                                       doublereal *, doublereal *, doublereal *, doublereal *);
  static doublereal gtest1, gtest2;
  static integer lg;
  static doublereal fu, gu, fw, gw;
  static integer lx;
  static logical braktd;
  static doublereal ualpha, factor, scxbnd, xw;
  extern /* Subroutine */ int getptc_(doublereal *, doublereal *,
                                        doublereal *, doublereal *, doublereal *, doublereal *,
                                        doublereal *, doublereal *, doublereal *, doublereal *,
                                        doublereal *, doublereal *, doublereal *, doublereal *,
                                        doublereal *, doublereal *, doublereal *, doublereal *,
                                        doublereal *, doublereal *, doublereal *, doublereal *,
                                        doublereal *, doublereal *, doublereal *, doublereal *,
                                        doublereal *, logical *, doublereal *, doublereal *, doublereal *,
                                        integer *, integer *);
  static doublereal fpresn;
  static integer ientry;
  static doublereal rtsmll;
  static integer lsprnt;
  static doublereal big, tol, rmu;

  /*      THE FOLLOWING STANDARD FUNCTIONS AND SYSTEM FUNCTIONS ARE */
  /*      CALLED WITHIN LINDER */

  /*      ALLOCATE THE ADDRESSES FOR LOCAL WORKSPACE */

  /* Parameter adjustments */
  --g;
  --x;
  --p;
  --w;

  /* Function Body */
  lx = 1;
  lg = lx + *n;
  lsprnt = 0;
  nprnt = 10000;
  rtsmll = sqrt(*small);
  big = 1. / *small;
  itcnt = 0;

  /*      SET THE ESTIMATED RELATIVE PRECISION IN F(X). */

  fpresn = *epsmch * 10.;
  numf = 0;
  u = *alpha;
  fu = *f;
  fmin = *f;
  gu = *gtp;
  rmu = 1e-4;

  /*      FIRST ENTRY SETS UP THE INITIAL INTERVAL OF UNCERTAINTY. */

  ientry = 1;
L10:

  /* TEST FOR TOO MANY ITERATIONS */

  ++itcnt;
  *iflag = 1;
  if (itcnt > 20)
    {
      goto L50;
    }
  *iflag = 0;
  getptc_(&big, small, &rtsmll, reltol, abstol, tnytol, &fpresn, eta, &rmu,
          xbnd, &u, &fu, &gu, &xmin, &fmin, &gmin, &xw, &fw, &gw, &a, &b, &
          oldf, &b1, &scxbnd, &e, &step, &factor, &braktd, &gtest1, &gtest2,
          &tol, &ientry, &itest);
  /* LSOUT */
  if (lsprnt >= nprnt)
    {
      lsout_(&ientry, &itest, &xmin, &fmin, &gmin, &xw, &fw, &gw, &u, &a, &
             b, &tol, reltol, &scxbnd, xbnd);
    }

  /*      IF ITEST=1, THE ALGORITHM REQUIRES THE FUNCTION VALUE TO BE */
  /*      CALCULATED. */

  if (itest != 1)
    {
      goto L30;
    }
  ualpha = xmin + u;
  l = lx;
  i__1 = *n;
  for (i__ = 1; i__ <= i__1; ++i__)
    {
      w[l] = x[i__] + ualpha * p[i__];
      ++l;
      /* L20: */
    }
  (*sfun)(n, &w[lx], &fu, &w[lg]);
  ++numf;
  gu = ddot_(n, &w[lg], &c__1, &p[1], &c__1);

  /*      THE GRADIENT VECTOR CORRESPONDING TO THE BEST POINT IS */
  /*      OVERWRITTEN IF FU IS LESS THAN FMIN AND FU IS SUFFICIENTLY */
  /*      LOWER THAN F AT THE ORIGIN. */

  if (fu <= fmin && fu <= oldf - ualpha * gtest1)
    {
      dcopy_(n, &w[lg], &c__1, &g[1], &c__1);
    }
  goto L10;

  /*      IF ITEST=2 OR 3 A LOWER POINT COULD NOT BE FOUND */

L30:
  *nftotl = numf;
  *iflag = 1;
  if (itest != 0)
    {
      goto L50;
    }

  /*      IF ITEST=0 A SUCCESSFUL SEARCH HAS BEEN MADE */

  *iflag = 0;
  *f = fmin;
  *alpha = xmin;
  i__1 = *n;
  for (i__ = 1; i__ <= i__1; ++i__)
    {
      x[i__] += *alpha * p[i__];
      /* L40: */
    }
L50:
  return 0;
} /* linder_ */

/* Subroutine */ int getptc_(doublereal *big, doublereal *small, doublereal *
                             rtsmll, doublereal *reltol, doublereal *abstol, doublereal *tnytol,
                             doublereal *fpresn, doublereal *eta, doublereal *rmu, doublereal *
                             xbnd, doublereal *u, doublereal *fu, doublereal *gu, doublereal *xmin,
                             doublereal *fmin, doublereal *gmin, doublereal *xw, doublereal *fw,
                             doublereal *gw, doublereal *a, doublereal *b, doublereal *oldf,
                             doublereal *b1, doublereal *scxbnd, doublereal *e, doublereal *step,
                             doublereal *factor, logical *braktd, doublereal *gtest1, doublereal *
                             gtest2, doublereal *tol, integer *ientry, integer *itest)
{
  /* System generated locals */
  doublereal d__1, d__2;

  /* Builtin functions */
  double sqrt(doublereal);

  /* Local variables */
  static doublereal half, abgw, absr, five, zero, p, q, r__, s, scale,
  denom, three, a1, d1, d2, sumsq, point1, abgmin, chordm, eleven,
  chordu;
  static logical convrg;
  static doublereal xmidpt, twotol, one;

  /* ************************************************************ */
  /* GETPTC, AN ALGORITHM FOR FINDING A STEPLENGTH, CALLED REPEATEDLY BY */
  /* ROUTINES WHICH REQUIRE A STEP LENGTH TO BE COMPUTED USING CUBIC */
  /* INTERPOLATION. THE PARAMETERS CONTAIN INFORMATION ABOUT THE INTERVAL */
  /* IN WHICH A LOWER POINT IS TO BE FOUND AND FROM THIS GETPTC COMPUTES A
  */
  /* POINT AT WHICH THE FUNCTION CAN BE EVALUATED BY THE CALLING PROGRAM. */
  /* THE VALUE OF THE INTEGER PARAMETERS IENTRY DETERMINES THE PATH TAKEN */
  /* THROUGH THE CODE. */
  /* ************************************************************ */

  /* THE FOLLOWING STANDARD FUNCTIONS AND SYSTEM FUNCTIONS ARE CALLED */
  /* WITHIN GETPTC */

  zero = 0.;
  point1 = .1;
  half = .5;
  one = 1.;
  three = 3.;
  five = 5.;
  eleven = 11.;

  /*      BRANCH TO APPROPRIATE SECTION OF CODE DEPENDING ON THE */
  /*      VALUE OF IENTRY. */

  switch (*ientry)
    {
    case 1: goto L10;
    case 2: goto L20;
    }

  /*      IENTRY=1 */
  /*      CHECK INPUT PARAMETERS */

L10:
  *itest = 2;
  if (*u <= zero || *xbnd <= *tnytol || *gu > zero)
    {
      return 0;
    }
  *itest = 1;
  if (*xbnd < *abstol)
    {
      *abstol = *xbnd;
    }
  *tol = *abstol;
  twotol = *tol + *tol;

  /* A AND B DEFINE THE INTERVAL OF UNCERTAINTY, X AND XW ARE POINTS */
  /* WITH LOWEST AND SECOND LOWEST FUNCTION VALUES SO FAR OBTAINED. */
  /* INITIALIZE A,SMIN,XW AT ORIGIN AND CORRESPONDING VALUES OF */
  /* FUNCTION AND PROJECTION OF THE GRADIENT ALONG DIRECTION OF SEARCH */
  /* AT VALUES FOR LATEST ESTIMATE AT MINIMUM. */

  *a = zero;
  *xw = zero;
  *xmin = zero;
  *oldf = *fu;
  *fmin = *fu;
  *fw = *fu;
  *gw = *gu;
  *gmin = *gu;
  *step = *u;
  *factor = five;

  /*      THE MINIMUM HAS NOT YET BEEN BRACKETED. */

  *braktd = FALSE_;

  /* SET UP XBND AS A BOUND ON THE STEP TO BE TAKEN. (XBND IS NOT COMPUTED
  */
  /* EXPLICITLY BUT SCXBND IS ITS SCALED VALUE.)  SET THE UPPER BOUND */
  /* ON THE INTERVAL OF UNCERTAINTY INITIALLY TO XBND + TOL(XBND). */

  *scxbnd = *xbnd;
  *b = *scxbnd + *reltol * abs(*scxbnd) + *abstol;
  *e = *b + *b;
  *b1 = *b;

  /* COMPUTE THE CONSTANTS REQUIRED FOR THE TWO CONVERGENCE CRITERIA. */

  *gtest1 = -(*rmu) * *gu;
  *gtest2 = -(*eta) * *gu;

  /* SET IENTRY TO INDICATE THAT THIS IS THE FIRST ITERATION */

  *ientry = 2;
  goto L210;

  /* IENTRY = 2 */

  /* UPDATE A,B,XW, AND XMIN */

L20:
  if (*fu > *fmin)
    {
      goto L60;
    }

  /* IF FUNCTION VALUE NOT INCREASED, NEW POINT BECOMES NEXT */
  /* ORIGIN AND OTHER POINTS ARE SCALED ACCORDINGLY. */

  chordu = *oldf - (*xmin + *u) * *gtest1;
  if (*fu <= chordu)
    {
      goto L30;
    }

  /* THE NEW FUNCTION VALUE DOES NOT SATISFY THE SUFFICIENT DECREASE */
  /* CRITERION. PREPARE TO MOVE THE UPPER BOUND TO THIS POINT AND */
  /* FORCE THE INTERPOLATION SCHEME TO EITHER BISECT THE INTERVAL OF */
  /* UNCERTAINTY OR TAKE THE LINEAR INTERPOLATION STEP WHICH ESTIMATES */
  /* THE ROOT OF F(ALPHA)=CHORD(ALPHA). */

  chordm = *oldf - *xmin * *gtest1;
  *gu = -(*gmin);
  denom = chordm - *fmin;
  if (abs(denom) >= 1e-15)
    {
      goto L25;
    }
  denom = 1e-15;
  if (chordm - *fmin < 0.)
    {
      denom = -denom;
    }
L25:
  if (*xmin != zero)
    {
      *gu = *gmin * (chordu - *fu) / denom;
    }
  *fu = half * *u * (*gmin + *gu) + *fmin;
  if (*fu < *fmin)
    {
      *fu = *fmin;
    }
  goto L60;
L30:
  *fw = *fmin;
  *fmin = *fu;
  *gw = *gmin;
  *gmin = *gu;
  *xmin += *u;
  *a -= *u;
  *b -= *u;
  *xw = -(*u);
  *scxbnd -= *u;
  if (*gu <= zero)
    {
      goto L40;
    }
  *b = zero;
  *braktd = TRUE_;
  goto L50;
L40:
  *a = zero;
L50:
  *tol = abs(*xmin) * *reltol + *abstol;
  goto L90;

  /* IF FUNCTION VALUE INCREASED, ORIGIN REMAINS UNCHANGED */
  /* BUT NEW POINT MAY NOW QUALIFY AS W. */

L60:
  if (*u < zero)
    {
      goto L70;
    }
  *b = *u;
  *braktd = TRUE_;
  goto L80;
L70:
  *a = *u;
L80:
  *xw = *u;
  *fw = *fu;
  *gw = *gu;
L90:
  twotol = *tol + *tol;
  xmidpt = half * (*a + *b);

  /* CHECK TERMINATION CRITERIA */

  convrg = abs(xmidpt) <= twotol - half * (*b - *a) || abs(*gmin) <= *
           gtest2 && *fmin < *oldf && ((d__1 = *xmin - *xbnd, abs(d__1)) > *
                                       tol || ! (*braktd));
  if (! convrg)
    {
      goto L100;
    }
  *itest = 0;
  if (*xmin != zero)
    {
      return 0;
    }

  /* IF THE FUNCTION HAS NOT BEEN REDUCED, CHECK TO SEE THAT THE RELATIVE */
  /* CHANGE IN F(X) IS CONSISTENT WITH THE ESTIMATE OF THE DELTA- */
  /* UNIMODALITY CONSTANT, TOL.  IF THE CHANGE IN F(X) IS LARGER THAN */
  /* EXPECTED, REDUCE THE VALUE OF TOL. */

  *itest = 3;
  if ((d__1 = *oldf - *fw, abs(d__1)) <= *fpresn * (one + abs(*oldf)))
    {
      return 0;
    }
  *tol = point1 * *tol;
  if (*tol < *tnytol)
    {
      return 0;
    }
  *reltol = point1 * *reltol;
  *abstol = point1 * *abstol;
  twotol = point1 * twotol;

  /* CONTINUE WITH THE COMPUTATION OF A TRIAL STEP LENGTH */

L100:
  r__ = zero;
  q = zero;
  s = zero;
  if (abs(*e) <= *tol)
    {
      goto L150;
    }

  /* FIT CUBIC THROUGH XMIN AND XW */

  r__ = three * (*fmin - *fw) / *xw + *gmin + *gw;
  absr = abs(r__);
  q = absr;
  if (*gw == zero || *gmin == zero)
    {
      goto L140;
    }

  /* COMPUTE THE SQUARE ROOT OF (R*R - GMIN*GW) IN A WAY */
  /* WHICH AVOIDS UNDERFLOW AND OVERFLOW. */

  abgw = abs(*gw);
  abgmin = abs(*gmin);
  s = sqrt(abgmin) * sqrt(abgw);
  if (*gw / abgw * *gmin > zero)
    {
      goto L130;
    }

  /* COMPUTE THE SQUARE ROOT OF R*R + S*S. */

  sumsq = one;
  p = zero;
  if (absr >= s)
    {
      goto L110;
    }

  /* THERE IS A POSSIBILITY OF OVERFLOW. */

  if (s > *rtsmll)
    {
      p = s * *rtsmll;
    }
  if (absr >= p)
    {
      /* Computing 2nd power */
      d__1 = absr / s;
      sumsq = one + d__1 * d__1;
    }
  scale = s;
  goto L120;

  /* THERE IS A POSSIBILITY OF UNDERFLOW. */

L110:
  if (absr > *rtsmll)
    {
      p = absr * *rtsmll;
    }
  if (s >= p)
    {
      /* Computing 2nd power */
      d__1 = s / absr;
      sumsq = one + d__1 * d__1;
    }
  scale = absr;
L120:
  sumsq = sqrt(sumsq);
  q = *big;
  if (scale < *big / sumsq)
    {
      q = scale * sumsq;
    }
  goto L140;

  /* COMPUTE THE SQUARE ROOT OF R*R - S*S */

L130:
  q = sqrt((d__1 = r__ + s, abs(d__1))) * sqrt((d__2 = r__ - s, abs(d__2)));
  if (r__ >= s || r__ <= -s)
    {
      goto L140;
    }
  r__ = zero;
  q = zero;
  goto L150;

  /* COMPUTE THE MINIMUM OF FITTED CUBIC */

L140:
  if (*xw < zero)
    {
      q = -q;
    }
  s = *xw * (*gmin - r__ - q);
  q = *gw - *gmin + q + q;
  if (q > zero)
    {
      s = -s;
    }
  if (q <= zero)
    {
      q = -q;
    }
  r__ = *e;
  if (*b1 != *step || *braktd)
    {
      *e = *step;
    }

  /* CONSTRUCT AN ARTIFICIAL BOUND ON THE ESTIMATED STEPLENGTH */

L150:
  a1 = *a;
  *b1 = *b;
  *step = xmidpt;
  if (*braktd)
    {
      goto L160;
    }
  *step = -(*factor) * *xw;
  if (*step > *scxbnd)
    {
      *step = *scxbnd;
    }
  if (*step != *scxbnd)
    {
      *factor = five * *factor;
    }
  goto L170;

  /* IF THE MINIMUM IS BRACKETED BY 0 AND XW THE STEP MUST LIE */
  /* WITHIN (A,B). */

L160:
  if ((*a != zero || *xw >= zero) && (*b != zero || *xw <= zero))
    {
      goto L180;
    }

  /* IF THE MINIMUM IS NOT BRACKETED BY 0 AND XW THE STEP MUST LIE */
  /* WITHIN (A1,B1). */

  d1 = *xw;
  d2 = *a;
  if (*a == zero)
    {
      d2 = *b;
    }
  /* THIS LINE MIGHT BE */
  /*     IF (A .EQ. ZERO) D2 = E */
  *u = -d1 / d2;
  *step = five * d2 * (point1 + one / *u) / eleven;
  if (*u < one)
    {
      *step = half * d2 * sqrt(*u);
    }
L170:
  if (*step <= zero)
    {
      a1 = *step;
    }
  if (*step > zero)
    {
      *b1 = *step;
    }

  /* REJECT THE STEP OBTAINED BY INTERPOLATION IF IT LIES OUTSIDE THE */
  /* REQUIRED INTERVAL OR IT IS GREATER THAN HALF THE STEP OBTAINED */
  /* DURING THE LAST-BUT-ONE ITERATION. */

L180:
  if (abs(s) <= (d__1 = half * q * r__, abs(d__1)) || s <= q * a1 || s >= q
      * *b1)
    {
      goto L200;
    }

  /* A CUBIC INTERPOLATION STEP */

  *step = s / q;

  /* THE FUNCTION MUST NOT BE EVALUTATED TOO CLOSE TO A OR B. */

  if (*step - *a >= twotol && *b - *step >= twotol)
    {
      goto L210;
    }
  if (xmidpt > zero)
    {
      goto L190;
    }
  *step = -(*tol);
  goto L210;
L190:
  *step = *tol;
  goto L210;
L200:
  *e = *b - *a;

  /* IF THE STEP IS TOO LARGE, REPLACE BY THE SCALED BOUND (SO AS TO */
  /* COMPUTE THE NEW POINT ON THE BOUNDARY). */

L210:
  if (*step < *scxbnd)
    {
      goto L220;
    }
  *step = *scxbnd;

  /* MOVE SXBD TO THE LEFT SO THAT SBND + TOL(XBND) = XBND. */

  *scxbnd -= (*reltol * abs(*xbnd) + *abstol) / (one + *reltol);
L220:
  *u = *step;
  if (abs(*step) < *tol && *step < zero)
    {
      *u = -(*tol);
    }
  if (abs(*step) < *tol && *step >= zero)
    {
      *u = *tol;
    }
  *itest = 1;
  return 0;
} /* getptc_ */

// this special BLAS is copied here without modification by Joseph

/*   DXPY (A VERSION OF DAXPY WITH A=1.0) */
/*   WRITTEN BY:  STEPHEN G. NASH */
/*                OPERATIONS RESEARCH AND APPLIED STATISTICS DEPT. */
/*                GEORGE MASON UNIVERSITY */
/*                FAIRFAX, VA 22030 */
/* ****************************************************************** */
/* SPECIAL BLAS FOR Y = X+Y */
/* ****************************************************************** */
/* Subroutine */ int dxpy_(integer *n, doublereal *dx, integer *incx,
                           doublereal *dy, integer *incy)
{
  /* System generated locals */
  integer i__1;

  /* Local variables */
  static integer i__, m, ix, iy, mp1;

  /*     VECTOR PLUS A VECTOR. */
  /*     USES UNROLLED LOOPS FOR INCREMENTS EQUAL TO ONE. */
  /*     STEPHEN G. NASH 5/30/89. */

  /* Parameter adjustments */
  --dy;
  --dx;

  /* Function Body */
  if (*n <= 0)
    {
      return 0;
    }
  if (*incx == 1 && *incy == 1)
    {
      goto L20;
    }

  /*        CODE FOR UNEQUAL INCREMENTS OR EQUAL INCREMENTS */
  /*          NOT EQUAL TO 1 */

  ix = 1;
  iy = 1;
  if (*incx < 0)
    {
      ix = (-(*n) + 1) * *incx + 1;
    }
  if (*incy < 0)
    {
      iy = (-(*n) + 1) * *incy + 1;
    }
  i__1 = *n;
  for (i__ = 1; i__ <= i__1; ++i__)
    {
      dy[iy] += dx[ix];
      ix += *incx;
      iy += *incy;
      /* L10: */
    }
  return 0;

  /*        CODE FOR BOTH INCREMENTS EQUAL TO 1 */

  /*        CLEAN-UP LOOP */

L20:
  m = *n % 4;
  if (m == 0)
    {
      goto L40;
    }
  i__1 = m;
  for (i__ = 1; i__ <= i__1; ++i__)
    {
      dy[i__] += dx[i__];
      /* L30: */
    }
  if (*n < 4)
    {
      return 0;
    }
L40:
  mp1 = m + 1;
  i__1 = *n;
  for (i__ = mp1; i__ <= i__1; i__ += 4)
    {
      dy[i__] += dx[i__];
      dy[i__ + 1] += dx[i__ + 1];
      dy[i__ + 2] += dx[i__ + 2];
      dy[i__ + 3] += dx[i__ + 3];
      /* L50: */
    }
  return 0;
} /* dxpy_ */