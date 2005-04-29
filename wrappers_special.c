/* -*- C -*- 

   @PROLOGUE@

   ----- 

   Jeffrey Vetter vetter3@llnl.gov
   Center for Applied Scientific Computing, LLNL
   12 Aug 2001

   wrappers_special.c -- wrappers that demand special attention

 */

#ifndef lint
static char *rcsid =
  "$Header$";
#endif

#include "mpiPconfig.h"
#include "mpiPi.h"
#include "symbols.h"
#include <string.h>


/* ----- INIT -------------------------------------------------- */

static int
_MPI_Init (int *argc, char ***argv)
{
  int rc = 0;
  int enabledStatus;

  enabledStatus = mpiPi.enabled;
  mpiPi.enabled = 0;

  rc = PMPI_Init (argc, argv);

  mpiPi.enabled = enabledStatus;

#if defined(Linux) && ! defined(ppc64)
  mpiPi.appFullName = getProcExeLink();
  mpiPi_msg_debug ("appFullName is %s\n", mpiPi.appFullName);
  mpiPi_init (GetBaseAppName (mpiPi.appFullName));
#else
  if ( argv != NULL )
  {
    mpiPi_init (GetBaseAppName (**argv));
    mpiPi.appFullName = strdup(**argv);
  }
  else
  {
    mpiPi_init ("Unknown");
    mpiPi_msg_debug( "argv is NULL\n" );
  }
#endif

  return rc;
}

extern int
MPI_Init (int *argc, char ***argv)
{
  int rc = 0;

  mpiPi.toolname = "mpiP";

  rc = _MPI_Init (argc, argv);

  if ( argc != NULL && argv != NULL )
    mpiPi_copy_given_args (&(mpiPi.ac), mpiPi.av, 32, *argc, *argv);
  else
  {
#ifdef Linux
    getProcCmdLine (&(mpiPi.ac), mpiPi.av, 32);
#else
    mpiPi.ac = 0;
#endif
  }

  return rc;
}

extern void
F77_MPI_INIT (int *ierr)
{
  int rc = 0;
  char **tmp_argv;

  mpiPi.toolname = "mpiP";
#ifdef Linux
  getProcCmdLine (&(mpiPi.ac), mpiPi.av, 32);
#else
  mpiPi_copy_args (&(mpiPi.ac), mpiPi.av, 32);
#endif

  tmp_argv = mpiPi.av;
  rc = _MPI_Init (&(mpiPi.ac), (char ***) &tmp_argv);
  *ierr = rc;

  return;
}


/* ----- FINALIZE -------------------------------------------------- */

static int
_MPI_Finalize ()
{
  int rc = 0;

  mpiPi_finalize ();
  mpiPi.enabled = 0;
  mpiPi_msg_debug ("calling PMPI_Finalize\n");
  rc = PMPI_Finalize ();
  mpiPi_msg_debug ("returning from PMPI_Finalize\n");

  return rc;
}

extern int
MPI_Finalize (void)
{
  int rc = 0;

  rc = _MPI_Finalize ();

  return rc;
}

extern void
F77_MPI_FINALIZE (int *ierr)
{
  int rc = 0;

  rc = _MPI_Finalize ();
  *ierr = rc;

  return;
}

/* eof */
