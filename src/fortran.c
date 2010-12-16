#include "xcfun.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/*
  Functions used by the Fortran interface. Use single underscore 
  convention. Do _not_ put underscores in the names except for
  the final one.

  note about pgi compilation:
  pgcc 10.2-0 does not like FSYM(foo)FCSYM(FOO)
  instead please use FSYM(foo) FCSYM(FOO)
 */

#ifdef FTN_UPPERCASE
#define FSYM(name)
#define FCSYM(name) name
#elif defined FTN_UPPERCASE_UNDERSCORE
#define FSYM(name)
#define FCSYM(name) name##_
#else
#define FSYM(name) name##_
#define FCSYM(name)
#endif

#define MAX_FORTRAN_FUNCTIONALS 5

static xc_functional fortran_functionals[MAX_FORTRAN_FUNCTIONALS] = {0};

double FSYM(xcfuve) FCSYM(XCFUVE)(void)
{
  return xcfun_version();
}

int FSYM(xcnewf) FCSYM(XCNEWF)(void)
{
  int i;
  for (i=0;i<MAX_FORTRAN_FUNCTIONALS;i++)
    {
      if (fortran_functionals[i] == 0)
	{
	  fortran_functionals[i] = xc_new_functional();
	  return i;
	}
    }
  return -1;
}

void FSYM(xcfree) FCSYM(XCFREE)(int *fun)
{
  assert(*fun >= 0 && *fun < MAX_FORTRAN_FUNCTIONALS);
  xc_free_functional(fortran_functionals[*fun]);
  fortran_functionals[*fun] = 0;
}

/*
  Figure out the memory distance between points
  from the first and second pointers.
 */
void FSYM(xceval) FCSYM(XCEVAL)(int *fun,  int *order, 
		  int *nr_points, 
		  double *first_density,
		  double *second_density,
		  double *first_result,
		  double *second_result)
{
  int dpitch, rpitch;
  assert(*fun >= 0 && *fun < MAX_FORTRAN_FUNCTIONALS);
  dpitch = second_density - first_density;
  rpitch = second_result - first_result;
  xc_eval_vec(fortran_functionals[*fun], *order, *nr_points, 
	      first_density, dpitch,
	      first_result, rpitch);
}


void FSYM(xcpotential) FCSYM(XCPOTENTIAL)(int *fun, double *density, double *energy, double *potential)
{
  assert(*fun >= 0 && *fun < MAX_FORTRAN_FUNCTIONALS);
  xc_potential(fortran_functionals[*fun], density, energy, potential);
}

void FSYM(xcsmod) FCSYM(XCSMOD)(int *fun, int *mode)
{
  assert(*fun >= 0 && *fun < MAX_FORTRAN_FUNCTIONALS);
  xc_set_mode(fortran_functionals[*fun], *mode);
}

int FSYM(xcgett) FCSYM(XCGETT)(int *fun)
{
  assert(*fun >= 0 && *fun < MAX_FORTRAN_FUNCTIONALS);
  return xc_get_type(fortran_functionals[*fun]);
}

int FSYM(xcmord) FCSYM(XCMORD)(int *fun)
{
  assert(*fun >= 0 && *fun < MAX_FORTRAN_FUNCTIONALS);
  return xc_max_order(fortran_functionals[*fun]);
}

int FSYM(xcinle) FCSYM(XCINLE)(int *fun)
{
  assert(*fun >= 0 && *fun < MAX_FORTRAN_FUNCTIONALS);
  return xc_input_length(fortran_functionals[*fun]);
}

int FSYM(xcoule) FCSYM(XCOULE)(int *fun, int *order)
{
  assert(*fun >= 0 && *fun < MAX_FORTRAN_FUNCTIONALS);
  return xc_output_length(fortran_functionals[*fun],*order);
}

int FSYM(xcdind) FCSYM(XCDIND)(int *fun, const int *derivative)
{
  assert(*fun >= 0 && *fun < MAX_FORTRAN_FUNCTIONALS);
  return xc_derivative_index(fortran_functionals[*fun],derivative);
}

static void str2ints(int ints[], int len, const char *s)
{
  int i = 0;
  while (*s && i < len)
    ints[i++] = *s++;
  if (*s)
    ints[len-1] = 0;
  else
    ints[i] = 0;
}

#if 0
static char *ints2str(int ints[])
{
  int len = 0;
  while (ints[len])
    len++;
  char *s = new char[len+1];
  for (int i=0;i<=len;i++)
    s[i] = ints[i];
  return s;
}
#endif

void FSYM(xcspla) FCSYM(XCSPLA)(int *text, int *len)
{
  str2ints(text,*len,xcfun_splash());
}

void FSYM(xcsnam) FCSYM(XCSNAM)(int *dst, int *dstlen, int *n)
{
  const char *s;
  s= xc_name(*n-1);
  if (s)
    str2ints(dst,*dstlen,s);
  else
    dst[0] = 0;
}

void FSYM(xcssho) FCSYM(XCSSHO)(int *dst, int *dstlen, int *n)
{
  const char *s;
  s = xc_short_description(*n-1);
  str2ints(dst,*dstlen,s);
}

void FSYM(xcslon) FCSYM(XCSLON)(int *dst, int *dstlen, int *n)
{
  const char *s;   
  s = xc_long_description(*n-1);
  str2ints(dst,*dstlen,s);
}

int FSYM(xcisfu) FCSYM(XCISFU)(int *n)
{
  return xc_is_functional(*n-1);
}

void FSYM(xcsets) FCSYM(XCSETS)(int *fun, int *n, double *value)
{
  assert(*fun >= 0 && *fun < MAX_FORTRAN_FUNCTIONALS);
  xc_set_param(fortran_functionals[*fun],*n-1,*value);
}

double FSYM(xcgets) FCSYM(XCGETS)(int *fun, int *n)
{
  assert(*fun >= 0 && *fun < MAX_FORTRAN_FUNCTIONALS);
  return xc_get_param(fortran_functionals[*fun],*n-1);
}
