/*-------------------------------------------------------------------------
This software is provided 'as-is', without any express or implied warranty.
In no event will the authors be held liable for any damages arising from
the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.

2. Altered source versions must be plainly marked as such, and must not
   be misrepresented as being the original software.

3. This notice may not be removed or altered from any source distribution.
-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------
 * HDF5 interface to ADF
 *-------------------------------------------------------------------*/

#include "ADFH.h"

#include "hdf5.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <stdlib.h>

/*#define ADFH_DEBUG_ON*/
#define ADFH_NO_ORDER
#define ADFH_USE_STRINGS

#define TO_UPPER( c ) ((islower(c))?(toupper(c)):(c))

/*
 * ADF names are not allowed to start with a space.
 * Since HDF5 allows this, use the space to hide data
 */

/* dataset and group names */

#define D_PREFIX  ' '
#define D_VERSION " version"
#define D_FORMAT  " format"
#define D_MOUNT   " mount"
#define D_DATA    " data"
#define D_FILE    " file"
#define D_PATH    " path"
#define D_LINK    " link"

/* attribute names */

#define A_NAME    "name"
#define A_LABEL   "label"
#define A_TYPE    "type"
#define A_ORDER   "order"
#define A_MOUNT   "mount"
#define A_FILE    "file"
#define A_REFCNT  "refcnt"

/* debugging */

#define ADFH_PREFIX       "#### DBG         "
#ifdef ADFH_DEBUG_ON
#define ADFH_DEBUG(a) \
printf("#### DBG [%5d] " #a "\n",__LINE__);fflush(stdout);
#else
#define ADFH_DEBUG(a)
#endif

/* ADF data types */

#define ADFH_MT "MT"
#define ADFH_LK "LK"
#define ADFH_B1 "B1"
#define ADFH_C1 "C1"
#define ADFH_I4 "I4"
#define ADFH_I8 "I8"
#define ADFH_U4 "U4"
#define ADFH_U8 "U8"
#define ADFH_R4 "R4"
#define ADFH_R8 "R8"
/* these are not supported */
#define ADFH_X4 "X4"
#define ADFH_X8 "X8"

/* file open modes */

#define ADFH_MODE_NEW 1
#define ADFH_MODE_OLD 2
#define ADFH_MODE_RDO 3

/* the following keeps track of open and mounted files */

#define ADFH_MAXIMUM_FILES 128

/* mounted files are kept in a linked-list */

typedef struct _ADFH_MOUNT {
    int mntnum;
    hid_t fid;
    struct _ADFH_MOUNT *next;
} ADFH_MOUNT;

typedef struct _ADFH_FILE {
    hid_t fid;
    int mode;
    ADFH_MOUNT *mounted;
} ADFH_FILE;

static ADFH_FILE g_files[ADFH_MAXIMUM_FILES];

static int g_init = 0; /* set when initialization done */

/* error codes and messages */

static int g_error_state = 0;

static struct _ErrorList {
  int errcode;
  char *errmsg;
} ErrorList[] = {
  {NO_ERROR,                "No Error"},
  {STRING_LENGTH_ZERO,      "String length of zero or blank string detected"},
  {STRING_LENGTH_TOO_BIG,   "String length longer than maximum allowable length"},
  {TOO_MANY_ADF_FILES_OPENED,"Too many files opened"},
  {ADF_FILE_STATUS_NOT_RECOGNIZED,"File status was not recognized"},
  {FILE_OPEN_ERROR,         "File-open error"},
  {NULL_STRING_POINTER,     "A string pointer is NULL"},
  {REQUESTED_NEW_FILE_EXISTS,"File Open Error: NEW - File already exists"},
  {ADF_FILE_FORMAT_NOT_RECOGNIZED,"File format was not recognized"},
  {REQUESTED_OLD_FILE_NOT_FOUND,"File Open Error: OLD - File does not exist"},
  {MEMORY_ALLOCATION_FAILED,"Memory allocation failed"},
  {DUPLICATE_CHILD_NAME,    "Duplicate child name under a parent node"},
  {ZERO_DIMENSIONS,         "Node has no dimensions"},
  {BAD_NUMBER_OF_DIMENSIONS,"Node's number-of-dimensions is not in legal range"},
  {CHILD_NOT_OF_GIVEN_PARENT,"Specified child is NOT a child of the specified parent"},
  {INVALID_DATA_TYPE,       "Invalid Data-Type"},
  {NULL_POINTER,            "A pointer is NULL"},
  {NO_DATA,                 "Node has no data associated with it"},
  {END_OUT_OF_DEFINED_RANGE,"Bad end value"},
  {BAD_STRIDE_VALUE,        "Bad stride value"},
  {MINIMUM_GT_MAXIMUM,      "Minimum value is greater than the maximum value"},
  {DATA_TYPE_NOT_SUPPORTED, "The data format is not support on a particular machine"},
  {FILE_CLOSE_ERROR,        "File Close error"},
  {START_OUT_OF_DEFINED_RANGE,"Bad start value"},
  {ZERO_LENGTH_VALUE,       "A value of zero is not allowable"},
  {BAD_DIMENSION_VALUE,     "Bad dimension value"},
  {BAD_ERROR_STATE,         "Error state must be either a 0 (zero) or a 1 (one)"},
  {UNEQUAL_MEMORY_AND_DISK_DIMS,"Unequal dimensional specifications for disk and memory"},
  {NODE_IS_NOT_A_LINK,      "The node is not a link.  It was expected to be a link"},
  {LINK_TARGET_NOT_THERE,   "The linked-to node does not exist"},
  {LINKED_TO_FILE_NOT_THERE,"The file of a linked-node is not accessable"},
  {INVALID_NODE_NAME,       "Node name contains invalid characters"},
  {FFLUSH_ERROR,            "H5Fflush:flush error"},
  {NULL_NODEID_POINTER,     "The node ID pointer is NULL"},
  {MAX_FILE_SIZE_EXCEEDED,  "The maximum size for a file exceeded"},

  {ADFH_ERR_GLINK,          "H5Glink:soft link creation failed"},
  {ADFH_ERR_NO_ATT,         "Node attribute doesn't exist"},
  {ADFH_ERR_AOPEN,          "H5Aopen:open of node attribute failed"},
  {ADFH_ERR_IGET_NAME,      "H5Iget_name:failed to get node path from ID"},
  {ADFH_ERR_GMOVE,          "H5Gmove:moving a node group failed"},
  {ADFH_ERR_GUNLINK,        "H5Gunlink:node group deletion failed"},
  {ADFH_ERR_GOPEN,          "H5Gopen:open of a node group failed"},
  {ADFH_ERR_DGET_SPACE,     "H5Dget_space:couldn't get node dataspace"},
  {ADFH_ERR_DOPEN,          "H5Dopen:open of the node data failed"},
  {ADFH_ERR_DEXTEND,        "H5Dextend:couldn't extend the node dataspace"},
  {ADFH_ERR_DCREATE,        "H5Dcreate:node data creation failed"},
  {ADFH_ERR_SCREATE_SIMPLE, "H5Screate_simple:dataspace creation failed"},
  {ADFH_ERR_ACREATE,        "H5Acreate:node attribute creation failed"},
  {ADFH_ERR_GCREATE,        "H5Gcreate:node group creation failed"},
  {ADFH_ERR_DWRITE,         "H5Dwrite:write to node data failed"},
  {ADFH_ERR_DREAD,          "H5Dread:read of node data failed"},
  {ADFH_ERR_AWRITE,         "H5Awrite:write to node attribute failed"},
  {ADFH_ERR_AREAD,          "H5Aread:read of node attribute failed"},
  {ADFH_ERR_FMOUNT,         "H5Fmount:file mount failed"},
  {ADFH_ERR_LINK_MOVE,      "Can't move a linked-to node"},
  {ADFH_ERR_LINK_DATA,      "Can't change the data for a linked-to node"},
  {ADFH_ERR_LINK_NODE,      "Parent of node is a link"},
  {ADFH_ERR_LINK_DELETE,    "Can't delete a linked-to node"},
  {ADFH_ERR_NOT_HDF5_FILE,  "File does not exist or is not a HDF5 file"},
  {ADFH_ERR_FILE_DELETE,    "unlink (delete) of file failed"},
  {ADFH_ERR_FILE_INDEX,     "couldn't get file index from node ID"},
  {ADFH_ERR_TCOPY,          "H5Tcopy:copy of existing datatype failed"},
  {ADFH_ERR_AGET_TYPE,      "H5Aget_type:couldn't get attribute datatype"},
  {ADFH_ERR_TSET_SIZE,      "H5Tset_size:couldn't set datatype size"},
  {ADFH_ERR_NOT_IMPLEMENTED,"routine not implemented"}
};

#define NUM_ERRORS (sizeof(ErrorList)/sizeof(struct _ErrorList))

/* usefull macros */

#define CMP_OSTAT(r,n) ((r)->objno[0]==(n)->objno[0] && \
                        (r)->objno[1]==(n)->objno[1] && \
                        (r)->fileno[0]==(n)->fileno[0] && \
                        (r)->fileno[1]==(n)->fileno[1])

static herr_t find_by_name(hid_t, const char *, void *);

#define has_child(ID,NAME) H5Giterate(ID,".",NULL,find_by_name,(void *)NAME)
#define has_att(ID,NAME) H5Aiterate(ID,NULL,find_by_name,(void *)NAME)
#define has_data(ID) H5Giterate(ID,".",NULL,find_by_name,(void *)D_DATA)

/* ----------------------------------------------------------------
 * set error and terminate if error state set
 * ---------------------------------------------------------------- */

static void set_error(int errcode, int *err)
{
  if (errcode != NO_ERROR && g_error_state) {
    char errmsg[ADF_MAX_ERROR_STR_LENGTH+1];
    ADFH_Error_Message(errcode, errmsg);
    fprintf(stderr, "ERROR:%s\n", errmsg);
    exit(1);
  }
  *err = errcode;
}

/* ----- handle HDF5 errors --------------------------------------- */

static herr_t print_H5_error(int n, H5E_error_t *desc, void *data)
{
  const char *p;

  if ((p = strrchr(desc->file_name, '/')) == NULL &&
      (p = strrchr(desc->file_name, '\\')) == NULL)
    p = desc->file_name;
  else
    p++;
  fprintf(stderr, "%s line %u in %s(): %s\n", p,
    desc->line, desc->func_name, desc->desc);
  return 0;
}

static herr_t walk_H5_error(void *data)
{
  if (g_error_state) {
    fflush(stdout);
    fprintf(stderr, "\nHDF5 Error Trace Back\n");
    return H5Ewalk(H5E_WALK_DOWNWARD, print_H5_error, data);
  }
  return 0;
}

/*-----------------------------------------------------------------
 * get the native format - returns pointer to static storage
 *----------------------------------------------------------------- */

static char *native_format(void)
{
  static char format[ADF_FORMAT_LENGTH+1];
  hid_t type = H5Tcopy(H5T_NATIVE_FLOAT);

  if (H5Tequal(type, H5T_IEEE_F32BE))
    strcpy(format, "IEEE_BIG_32");
  else if (H5Tequal(type, H5T_IEEE_F32LE))
    strcpy(format, "IEEE_LITTLE_32");
  else if (H5Tequal(type, H5T_IEEE_F64BE))
    strcpy(format, "IEEE_BIG_64");
  else if (H5Tequal(type, H5T_IEEE_F64LE))
    strcpy(format, "IEEE_LITTLE_64");
  else
    sprintf(format, "NATIVE_%d", H5Tget_precision(type));
  H5Tclose(type);
  return format;
}

/* ----------------------------------------------------------------
 * convert between HDF5 and ADF ids
 * ---------------------------------------------------------------- */

#if 0
# define to_HDF_ID(ID) (hid_t)(ID)
# define to_ADF_ID(ID) (double)(ID)
#else
static union {
  hid_t hdf_id;
  double adf_id;
} ID_CONVERT;

static hid_t to_HDF_ID (double adf_id) {
  ID_CONVERT.hdf_id = 0;
  ID_CONVERT.adf_id = adf_id;
  return ID_CONVERT.hdf_id;
}

static double to_ADF_ID (hid_t hdf_id) {
  ID_CONVERT.adf_id = 0;
  ID_CONVERT.hdf_id = hdf_id;
  return ID_CONVERT.adf_id;
}
#endif

/* -----------------------------------------------------------------
 * set/get attribute values
/* ----------------------------------------------------------------- */

static hid_t get_att_id(hid_t id, const char *name, int *err)
{
  hid_t aid = H5Aopen_name(id, name);

  if (aid < 0) {
    if (!has_att(id, name))
      set_error(ADFH_ERR_NO_ATT, err);
    else
      set_error(ADFH_ERR_AOPEN, err);
  }
  else
    set_error(NO_ERROR, err);
  return aid;
}

/* ----------------------------------------------------------------- */

static int new_str_att(hid_t id, const char *name, const char *value,
                       int max_size, int *err)
{
#ifdef ADFH_USE_STRINGS
  hid_t sid, tid, aid;
  herr_t status;

  sid = H5Screate(H5S_SCALAR);
  if (sid < 0) {
    set_error(ADFH_ERR_SCREATE_SIMPLE, err);
    return 1;
  }

  tid = H5Tcopy(H5T_C_S1);
  if (tid < 0) {
    H5Sclose(sid);
    set_error(ADFH_ERR_TCOPY, err);
    return 1;
  }
  if (H5Tset_size(tid, max_size + 1) < 0) {
    H5Tclose(tid);
    H5Sclose(sid);
    set_error(ADFH_ERR_TSET_SIZE, err);
    return 1;
  }

  aid = H5Acreate(id, name, tid, sid, H5P_DEFAULT);
  if (aid < 0) {
    H5Tclose(tid);
    H5Sclose(sid);
    set_error(ADFH_ERR_ACREATE, err);
    return 1;
  }

  status = H5Awrite(aid, tid, value);

  H5Aclose(aid);
  H5Tclose(tid);
  H5Sclose(sid);

  if (status < 0) {
    set_error(ADFH_ERR_AWRITE, err);
    return 1;
  }
  set_error(NO_ERROR, err);
  return 0;
#else
  /* CAUTION: unly use this for strings <= ADF_FILENAME_LENGTH */
  hid_t sid, aid;
  hsize_t dim;
  herr_t status;
  char buff[ADF_FILENAME_LENGTH+1];

  dim = max_size + 1;
  sid = H5Screate_simple(1, &dim, NULL);
  if (sid < 0) {
    set_error(ADFH_ERR_SCREATE_SIMPLE, err);
    return 1;
  }

  aid = H5Acreate(id, name, H5T_NATIVE_CHAR, sid, H5P_DEFAULT);
  if (aid < 0) {
    H5Sclose(sid);
    set_error(ADFH_ERR_ACREATE, err);
    return 1;
  }

  memset(buff, 0, ADF_FILENAME_LENGTH+1);
  strcpy(buff, value);
  status = H5Awrite(aid, H5T_NATIVE_CHAR, buff);

  H5Aclose(aid);
  H5Sclose(sid);

  if (status < 0) {
    set_error(ADFH_ERR_AWRITE, err);
    return 1;
  }
  set_error(NO_ERROR, err);
  return 0;
#endif
}

/* ----------------------------------------------------------------- */

static int get_str_att(hid_t id, const char *name, char *value, int *err)
{
#ifdef ADFH_USE_STRINGS
  hid_t tid, att_id;
  herr_t status;

  if ((att_id = get_att_id(id, name, err)) < 0) return 1;
  tid = H5Aget_type(att_id);
  if (tid < 0) {
    H5Aclose(att_id);
    set_error(ADFH_ERR_AGET_TYPE, err);
    return 1;
  }
  status = H5Aread(att_id, tid, value);
  H5Aclose(att_id);
  H5Tclose(tid);
  if (status < 0) {
    set_error(ADFH_ERR_AREAD, err);
    return 1;
  }
  return 0;
#else
  /* CAUTION: unly use this for strings <= ADF_FILENAME_LENGTH */
  hid_t att_id;
  herr_t status;
  char buff[ADF_FILENAME_LENGTH+1];

  if ((att_id = get_att_id(id, name, err)) < 0) return 1;
  status = H5Aread(att_id, H5T_NATIVE_CHAR, buff);
  H5Aclose(att_id);
  strcpy(value, buff);
  if (status < 0) {
    set_error(ADFH_ERR_AREAD, err);
    return 1;
  }
  return 0;
#endif
}

/* ----------------------------------------------------------------- */

static int set_str_att(hid_t id, const char *name, const char *value, int *err)
{
#ifdef ADFH_USE_STRINGS
  hid_t tid, att_id;
  herr_t status;

  if ((att_id = get_att_id(id, name, err)) < 0) return 1;
  tid = H5Aget_type(att_id);
  if (tid < 0) {
    H5Aclose(att_id);
    set_error(ADFH_ERR_AGET_TYPE, err);
    return 1;
  }
  status = H5Awrite(att_id, tid, value);
  H5Tclose(tid);
  H5Aclose(att_id);
  if (status < 0) {
    set_error(ADFH_ERR_AWRITE, err);
    return 1;
  }
  return 0;
#else
  /* CAUTION: unly use this for strings <= ADF_FILENAME_LENGTH */
  hid_t att_id;
  herr_t status;
  char buff[ADF_FILENAME_LENGTH+1];

  if ((att_id = get_att_id(id, name, err)) < 0) return 1;
  memset(buff, 0, ADF_FILENAME_LENGTH+1);
  strcpy(buff, value);
  status = H5Awrite(att_id, H5T_NATIVE_CHAR, buff);
  H5Aclose(att_id);
  if (status < 0) {
    set_error(ADFH_ERR_AWRITE, err);
    return 1;
  }
  return 0;
#endif
}

/* ----------------------------------------------------------------- */

static int new_int_att(hid_t id, const char *name, int value, int *err)
{
  hid_t sid, aid;
  hsize_t dim;
  herr_t status;
  int buff = value;

  dim = 1;
  sid = H5Screate_simple(1, &dim, NULL);
  if (sid < 0) {
    set_error(ADFH_ERR_SCREATE_SIMPLE, err);
    return 1;
  }

  aid = H5Acreate(id, name, H5T_NATIVE_INT, sid, H5P_DEFAULT);
  if (aid < 0) {
    H5Sclose(sid);
    set_error(ADFH_ERR_ACREATE, err);
    return 1;
  }

  status = H5Awrite(aid, H5T_NATIVE_INT, &buff);

  H5Aclose(aid);
  H5Sclose(sid);

  if (status < 0) {
    set_error(ADFH_ERR_AWRITE, err);
    return 1;
  }
  set_error(NO_ERROR, err);
  return 0;
}

/* ----------------------------------------------------------------- */

static int get_int_att(hid_t id, char *name, int *value, int *err)
{
  hid_t att_id;
  herr_t status;

  if ((att_id = get_att_id(id, name, err)) < 0) return 1;
  status = H5Aread(att_id, H5T_NATIVE_INT, value);
  H5Aclose(att_id);
  if (status < 0) {
    set_error(ADFH_ERR_AREAD, err);
    return 1;
  }
  return 0;
}

/* ----------------------------------------------------------------- */

static int set_int_att(hid_t id, char *name, int value, int *err)
{
  hid_t att_id;
  herr_t status;
  int buff = value;

  if ((att_id = get_att_id(id, name, err)) < 0) return 1;
  status = H5Awrite(att_id, H5T_NATIVE_INT, &buff);
  H5Aclose(att_id);
  if (status < 0) {
    set_error(ADFH_ERR_AWRITE, err);
    return 1;
  }
  return 0;
}

/* ----------------------------------------------------------------- */

static int new_str_data(hid_t id, const char *name, const char *value,
                       int size, int *err)
{
  hid_t sid, did;
  hsize_t dim;
  herr_t status;

  dim = size+1;
  sid = H5Screate_simple(1, &dim, NULL);
  if (sid < 0) {
    set_error(ADFH_ERR_SCREATE_SIMPLE, err);
    return 1;
  }

  did = H5Dcreate(id, name, H5T_NATIVE_CHAR, sid, H5P_DEFAULT);
  if (did < 0) {
    H5Sclose(sid);
    set_error(ADFH_ERR_DCREATE, err);
    return 1;
  }

  status = H5Dwrite(did, H5T_NATIVE_CHAR, H5S_ALL, H5S_ALL, H5P_DEFAULT, value);
  H5Dclose(did);
  H5Sclose(sid);

  if (status < 0) {
    set_error(ADFH_ERR_DWRITE, err);
    return 1;
  }
  set_error(NO_ERROR, err);
  return 0;
}

/* ----------------------------------------------------------------- */
/*  translate an ADF data type into an HDF one */

static hid_t to_HDF_data_type(const char *tp)
{
  if (0 == strcmp(tp, ADFH_B1))
    return H5Tcopy(H5T_NATIVE_UCHAR);
  if (0 == strcmp(tp, ADFH_C1))
    return H5Tcopy(H5T_NATIVE_CHAR);
  if (0 == strcmp(tp, ADFH_I4))
    return H5Tcopy(H5T_NATIVE_INT32);
  if (0 == strcmp(tp, ADFH_I8))
    return H5Tcopy(H5T_NATIVE_INT64);
  if (0 == strcmp(tp, ADFH_U4))
    return H5Tcopy(H5T_NATIVE_UINT32);
  if (0 == strcmp(tp, ADFH_U8))
    return H5Tcopy(H5T_NATIVE_UINT64);
  if (0 == strcmp(tp, ADFH_R4)) {
    hid_t tid = H5Tcopy(H5T_NATIVE_FLOAT);
    H5Tset_precision(tid, 32);
    return tid;
  }
  if (0 == strcmp(tp, ADFH_R8)) {
    hid_t tid = H5Tcopy(H5T_NATIVE_DOUBLE);
    H5Tset_precision(tid, 64);
    return tid;
  }
  return 0;
}

/* ----------------------------------------------------------------- */

static int check_data_type(const char *tp, int *err)
{
  if (strcmp(tp, ADFH_B1) &&
      strcmp(tp, ADFH_C1) &&
      strcmp(tp, ADFH_I4) &&
      strcmp(tp, ADFH_I8) &&
      strcmp(tp, ADFH_U4) &&
      strcmp(tp, ADFH_U8) &&
      strcmp(tp, ADFH_R4) &&
      strcmp(tp, ADFH_R8)) {
    set_error(INVALID_DATA_TYPE, err);
    return 1;
  }
  set_error(NO_ERROR, err);
  return 0;
}

/* =================================================================
 * callback routines for H5Giterate and H5Aiterate
 * ================================================================= */

static int i_start, i_len, n_length, n_names;
#ifdef ADFH_NO_ORDER
static int i_count;
#endif

/* ----------------------------------------------------------------- */

static herr_t find_by_name(hid_t id, const char *name, void *dsname)
{
    if (0 == strcmp (name, (char *)dsname)) return 1;
    return 0;
}

/* ----------------------------------------------------------------- */

static herr_t count_children(hid_t id, const char *name, void *number)
{
  if (*name != D_PREFIX)
    (*((int *)number))++;
  return 0;
}

/* ----------------------------------------------------------------- */

static herr_t children_names(hid_t id, const char *name, void *namelist)
{
  hid_t gid;
  int order, err;
  char *p;

  if (*name == D_PREFIX) return 0;
#ifdef ADFH_NO_ORDER
  order = ++i_count - i_start;
  if (order >= 0 && order < i_len) {
    p = (char *)namelist + order * n_length;
    strncpy(p, name, n_length-1);
    p[n_length-1] = 0;
    n_names++;
  }
#else
  if ((gid = H5Gopen(id, name)) < 0) return 1;
  if (get_int_att(gid, A_ORDER, &order, &err)) {
    H5Gclose(gid);
    return 1;
  }
  order -= i_start;
  if (order >= 0 && order < i_len) {
    p = (char *)namelist + order * n_length;
    strncpy(p, name, n_length-1);
    p[n_length-1] = 0;
    n_names++;
  }
  H5Gclose(gid);
#endif
  return 0;
}

/* ----------------------------------------------------------------- */

static herr_t children_ids(hid_t id, const char *name, void *idlist)
{
  hid_t gid;
  int order, err;

  if (*name == D_PREFIX) return 0;
  if ((gid = H5Gopen(id, name)) < 0) return 1;
#ifdef ADFH_NO_ORDER
  order = ++i_count - i_start;
  if (order >= 0 && order < i_len) {
      ((double *)idlist)[order] = to_ADF_ID(gid);
      n_names++;
  }
#else
  if (get_int_att(gid, A_ORDER, &order, &err)) {
    H5Gclose(gid);
    return 1;
  }
  order -= i_start;
  if (order >= 0 && order < i_len) {
      ((double *)idlist)[order] = to_ADF_ID(gid);
      n_names++;
  }
#endif
  else
      H5Gclose(gid);
  return 0;
}

/*called via H5Giterate in Move_Child & Delete functions.
  removes gaps in _order index attributes */

static herr_t fix_order(hid_t id, const char *name, void *data)
{
  int start, order, err, ret = 0;
  hid_t gid, aid;

  if (*name == D_PREFIX) return 0;
  if ((gid = H5Gopen(id, name)) < 0)
    return ADFH_ERR_GOPEN;
  if ((aid = get_att_id(gid, A_ORDER, &err)) < 0) {
    H5Gclose(gid);
    return err;
  }
  if (H5Aread(aid, H5T_NATIVE_INT, &order) < 0)
    ret = ADFH_ERR_AREAD;
  else {
    start = *((int *)data);
    if (order > start) {
      order--;
      if (H5Awrite(aid, H5T_NATIVE_INT, &order) < 0)
        ret = ADFH_ERR_AWRITE;
    }
  }
  H5Aclose(aid);
  H5Gclose(gid);
  return ret;
}

/* ----------------------------------------------------------------- */

static herr_t compare_children(hid_t id, const char *name, void *data)
{
  H5G_stat_t stat, *pstat;

  if (*name != D_PREFIX) {
    pstat = (H5G_stat_t *)data;
    if (H5Gget_objinfo(id, name, 0, &stat) >= 0)
      return CMP_OSTAT(&stat, pstat);
  }
  return 0;
}

/* ----------------------------------------------------------------- */

static herr_t print_children(hid_t id, const char *name, void *data)
{
  if (*name != D_PREFIX)
    printf(" %s", name);
  return 0;
}

/* -----------------------------------------------------------------
 * get file ID from node ID
 * ----------------------------------------------------------------- */

static hid_t get_file_id (hid_t id)
{
  int n, nobj;
  hid_t *objs, fid = -1;
  H5G_stat_t gstat, rstat;

  /* find the file ID from the root ID */

  if (H5Gget_objinfo(id, "/", 0, &gstat) >= 0) {
    nobj = H5Fget_obj_count(H5F_OBJ_ALL, H5F_OBJ_FILE);
    if (nobj > 0) {
      objs = (hid_t *) malloc (nobj * sizeof(hid_t));
      if (objs == NULL) return fid;
      H5Fget_obj_ids(H5F_OBJ_ALL, H5F_OBJ_FILE, -1, objs);
      for (n = 0; n < nobj; n++) {
        H5Gget_objinfo(objs[n], "/", 0, &rstat);
        if (CMP_OSTAT(&gstat, &rstat)) {
          fid = objs[n];
          break;
        }
      }
      free (objs);
    }
  }
  return fid;
}

static int get_file_number (hid_t id, int *err)
{
  int n;
  hid_t fid = get_file_id(id);

  for (n = 0; n < ADFH_MAXIMUM_FILES; n++) {
    if (fid == g_files[n].fid) {
      set_error(NO_ERROR, err);
      return n;
    }
  }
  set_error(ADFH_ERR_FILE_INDEX, err);
  return -1;
}

/* ================================================================
 * routines for dealing with links
 * ================================================================ */

static herr_t find_link_file(hid_t id, const char *child, void *fname)
{
  char linkfile[ADF_FILENAME_LENGTH+1];
  hid_t gid;
  int err, status;

  gid = H5Gopen(id, child);
  status = get_str_att(gid, A_FILE, linkfile, &err);
  H5Gclose(gid);
  if (!status && 0 == strcmp(linkfile, (char *)fname))
    return (atoi(child));
  return 0;
}

/* ----------------------------------------------------------------- */

static herr_t next_link_file(hid_t id, const char *child, void *fileno)
{
  int num = atoi(child);
  int *max_num = (int *)fileno;

  if (*max_num < num) *max_num = num;
  return 0;
}

/* ----------------------------------------------------------------- */

static ADFH_MOUNT *is_mounted (int fn, int mn) {
  ADFH_MOUNT *mnt = g_files[fn].mounted;

  while (mnt != NULL) {
    if (mnt->mntnum == mn) return mnt;
    mnt = mnt->next;
  }
  return NULL;
}

/* ----------------------------------------------------------------- */

static char *get_mount_point(int mntnum)
{
  static char mntpath[64];

  sprintf(mntpath, "/%s/%c%d", D_MOUNT, D_PREFIX, mntnum);
  return mntpath;
}

/* ----------------------------------------------------------------- */

static hid_t open_link(hid_t id, int *err)
{
  hid_t lid;

  /* open and mount file if not already done */

  if (has_att(id, A_MOUNT)) {
    int filenum, mntnum;
    if ((filenum = get_file_number(id, err)) < 0 ||
        get_int_att(id, A_MOUNT, &mntnum, err)) return -1;

    if (is_mounted(filenum, mntnum) == NULL) {
      hid_t mid, fid;
      char linkfile[ADF_FILENAME_LENGTH+1];
      ADFH_MOUNT *mnt;

      mid = H5Gopen(id, get_mount_point(mntnum));
      if (get_str_att(mid, A_FILE, linkfile, err)) {
        H5Gclose(mid);
        return -1;
      }
      if (access(linkfile, 0)) {
        set_error(LINKED_TO_FILE_NOT_THERE, err);
        H5Gclose(mid);
        return -1;
      }
      if (H5Fis_hdf5(linkfile) <= 0) {
        set_error(ADFH_ERR_NOT_HDF5_FILE, err);
        H5Gclose(mid);
        return -1;
      }

      /* try to open file with same mode as parent */

      if (g_files[filenum].mode == ADFH_MODE_RDO || access(linkfile, 2))
        fid = H5Fopen(linkfile, H5F_ACC_RDONLY, H5P_DEFAULT);
      else
        fid = H5Fopen(linkfile, H5F_ACC_RDWR, H5P_DEFAULT);
      if (fid < 0) {
        set_error(FILE_OPEN_ERROR, err);
        H5Gclose(mid);
        return -1;
      }

      /* mount the file */

      if (H5Fmount(mid, D_MOUNT, fid, H5P_DEFAULT) < 0) {
        H5Fclose(fid);
        H5Gclose(mid);
        set_error(ADFH_ERR_FMOUNT, err);
        return -1;
      }
      H5Gclose(mid);

      /* save mount info */

      mnt = (ADFH_MOUNT *) malloc (sizeof(ADFH_MOUNT));
      if (NULL == mnt) {
        set_error(MEMORY_ALLOCATION_FAILED, err);
        return -1;
      }
      mnt->mntnum = mntnum;
      mnt->fid = fid;
      mnt->next = g_files[filenum].mounted;
      g_files[filenum].mounted = mnt;
    }
  }

  /* open the link */

  if ((lid = H5Gopen(id, D_LINK)) < 0)
    set_error(LINK_TARGET_NOT_THERE, err);
  else
    set_error(NO_ERROR, err);
  return lid;
}

/* ----------------------------------------------------------------- */

static int is_link(hid_t id)
{
  char type[3];
  int err;

  if (!get_str_att(id, A_TYPE, type, &err) &&
    0 == strcmp(ADFH_LK, type)) return 1;
  return 0;
}

/* ----------------------------------------------------------------- */

static hid_t open_node(double id, int *err) {
  hid_t hid, gid;

  hid = to_HDF_ID(id);
  set_error(NO_ERROR, err);
  if (is_link(hid)) return open_link(hid, err);
  if ((gid = H5Gopen(hid, ".")) < 0)
    set_error(ADFH_ERR_GOPEN, err);
  return gid;
}

/* ----------------------------------------------------------------- */

static hid_t parse_path(hid_t pid, char *path, int *err)
{
  hid_t id, nid;
  char *p;

  if ((p = strchr(path, '/')) != NULL) *p++ = 0;
  if ((id = H5Gopen(pid, path)) < 0) {
    set_error(ADFH_ERR_GOPEN, err);
    return id;
  }
  if (p == NULL || !*p) return id;
  if (is_link(id)) {
    nid = open_link(id, err);
    H5Gclose(id);
    if (nid < 0) return nid;
    id = nid;
  }
  nid = parse_path(id, p, err);
  H5Gclose(id);
  return nid;
}

/* -----------------------------------------------------------------
 * deletion routines
 * ----------------------------------------------------------------- */

static void delete_node(hid_t pid, const char *name)
{
  int filenum, mntnum, err;
  hid_t mid, id = H5Gopen(pid, name);

  /* if this is a link, update mount point references */

  if (is_link(id) && has_att(id, A_MOUNT) &&
    (filenum = get_file_number(id, &err)) >= 0 &&
    get_int_att(id, A_MOUNT, &mntnum, &err) == 0 &&
    (mid = H5Gopen(id, get_mount_point(mntnum))) >= 0) {
    int refcnt = 1;
    hid_t aid = get_att_id(mid, A_REFCNT, &err);

    if (aid >= 0) {
      H5Aread(aid, H5T_NATIVE_INT, &refcnt);
      if (--refcnt > 0)
        H5Awrite(aid, H5T_NATIVE_INT, &refcnt);
      H5Aclose(aid);
    }

    /* if no more references, remove mount point */

    if (refcnt > 0)
      H5Gclose(mid);
    else {
      char mntnode[16];
      ADFH_MOUNT *mnt = is_mounted(filenum, mntnum);
      if (mnt != NULL) {
        H5Fclose(mnt->fid);
        H5Funmount(mid, D_MOUNT);
        if (g_files[filenum].mounted == mnt)
          g_files[filenum].mounted = mnt->next;
        else {
          ADFH_MOUNT *mlist = g_files[filenum].mounted;
          while (mlist != NULL) {
            if (mlist->next == mnt) {
              mlist->next = mnt->next;
              break;
            }
            mlist = mlist->next;
          }
        }
        free (mnt);
      }
      H5Gunlink(mid, D_MOUNT);
      H5Gclose(mid);
      H5Gunlink(id, get_mount_point(mntnum));

      /* if no more mount points, remove mount group */

      sprintf(mntnode, "/%s", D_MOUNT);
      refcnt = 0;
      H5Giterate(id, mntnode, NULL, count_children, (void *)&refcnt);
      if (!refcnt) H5Gunlink(id, mntnode);
    }
  }

  H5Gclose(id);
  H5Gunlink(pid, name);
}

/* ----------------------------------------------------------------- */

static herr_t delete_children(hid_t id, const char *name, void *data)
{
  if (*name == D_PREFIX)
    H5Gunlink(id, name);
  else {
    H5Giterate(id, name, NULL, delete_children, data);
    delete_node(id, name);
  }
  return 0;
}

/* -----------------------------------------------------------------
 * check for valid node name
 * The return points to static storage - make a copy if needed
 * ----------------------------------------------------------------- */

static char *check_name(const char *new_name, int *err)
{
  char *p;
  static char name[ADF_NAME_LENGTH+1];

  if (new_name == NULL) {
    set_error(NULL_STRING_POINTER, err);
    return NULL;
  }

  /* skip leading space */

  for (p = (char *)new_name; *p && isspace(*p); p++)
    ;
  if (!*p) {
    set_error(STRING_LENGTH_ZERO, err);
    return NULL;
  }

  if (strlen(p) > ADF_NAME_LENGTH) {
    set_error(STRING_LENGTH_TOO_BIG, err);
    return NULL;
  }
  strcpy(name, p);

  /* remove trailing space */

  for (p = name+strlen(name)-1; p >= name && isspace(*p); p--)
    ;
  *++p = 0;
  if (!*name) {
    set_error(STRING_LENGTH_ZERO, err);
    return NULL;
  }

  /* these may cause problems with HDF5 */

  if (NULL != strchr(name, '/') || 0 == strcmp (name, ".")) {
    set_error(INVALID_NODE_NAME, err);
    return NULL;
  }

  set_error(NO_ERROR, err);
  return name;
}

/* ================================================================= */
/* 1 to 1 mapping of ADF functions to HDF mimic functions            */
/* ================================================================= */

void ADFH_Search_Add(const char *path, int *error_return)
{
  *error_return = ADFH_ERR_NOT_IMPLEMENTED;
}

void ADFH_Search_Delete(int *error_return)
{
  *error_return = ADFH_ERR_NOT_IMPLEMENTED;
}

/* ----------------------------------------------------------------- */
/* move a node                                                       */

void ADFH_Move_Child(const double  pid,
                      const double  id,
                      const double  npid,
                      int          *err)
{
  hid_t hpid = to_HDF_ID(pid);
  hid_t hid = to_HDF_ID(id);
  hid_t hnpid = to_HDF_ID(npid);
  int len, namelen;
  int old_order, new_order;
  char buff[2];
  char nodename[ADF_NAME_LENGTH+1];
  char *newpath;
  herr_t status;
  H5G_stat_t stat;

  ADFH_DEBUG("ADFH_Move_Child");

  if (is_link(hpid) || is_link(hnpid)) {
    set_error(ADFH_ERR_LINK_MOVE, err);
    return;
  }

  /* check that node is actually child of the parent */

  if (H5Gget_objinfo(hid, ".", 0, &stat) < 0 ||
    !H5Giterate(hpid, ".", NULL, compare_children, (void *)&stat)) {
    set_error(CHILD_NOT_OF_GIVEN_PARENT, err);
    return;
  }

  /* get node name */

  if (get_str_att(hid, A_NAME, nodename, err)) return;
  namelen = strlen(nodename);

  /* get new node path */

  len = H5Iget_name(hnpid, buff, 2);
  if (len <= 0) {
    set_error(ADFH_ERR_IGET_NAME, err);
    return;
  }
  newpath = (char *) malloc (len+namelen+2);
  if (newpath == NULL) {
    set_error(MEMORY_ALLOCATION_FAILED, err);
    return;
  }
  H5Iget_name(hnpid, newpath, len+1);
  newpath[len++] = '/';
  strcpy(&newpath[len], nodename);

#ifdef ADFH_DEBUG_ON
  printf("%s move [%s]\n",ADFH_PREFIX,nodename);
  printf("%s to   [%s]\n",ADFH_PREFIX,newpath);
#endif

  status = H5Gmove(hpid, nodename, newpath);
  free(newpath);
  if (status < 0) {
    set_error(ADFH_ERR_GMOVE, err);
    return;
  }

#ifdef ADFH_NO_ORDER
  set_error(NO_ERROR, err);
#else
  /*update _order attribute for node we just moved*/
  ADFH_Number_of_Children(npid, &new_order, err);
  if (*err != NO_ERROR) return;

  /*read/write _order attr*/
  if (get_int_att(hid, A_ORDER, &old_order, err) ||
      set_int_att(hid, A_ORDER, new_order, err)) return;

  /*see if we need to decrement any node _orders under the old parent*/
  *err = H5Giterate(hpid, ".", NULL, fix_order, (void *)&old_order);
  if (!*err)
    set_error(NO_ERROR, err);
#endif
}

/* ----------------------------------------------------------------- */
/* Change the label attribute value                                  */

void ADFH_Set_Label(const double  id,
                    const char   *label,
                    int          *err)
{
  hid_t hid = to_HDF_ID(id);

  ADFH_DEBUG("ADFH_Set_Label");

  if (label == NULL) {
    set_error(NULL_STRING_POINTER, err);
    return;
  }
  if (strlen(label) > ADF_NAME_LENGTH) {
    set_error(STRING_LENGTH_TOO_BIG, err);
    return;
  }
  if (is_link(hid)) {
    set_error(ADFH_ERR_LINK_DATA, err);
    return;
  }
  set_str_att(hid, A_LABEL, label, err);
}

/* ----------------------------------------------------------------- */
/* Change attribute name and move the group name to new name         */

void ADFH_Put_Name(const double  pid,
                   const double  id,
                   const char   *name,
                   int          *err)
{
  hid_t hpid = to_HDF_ID(pid);
  hid_t hid = to_HDF_ID(id);
  char *nname, oname[ADF_NAME_LENGTH+1];

  ADFH_DEBUG("ADFH_Put_Name");

  if ((nname = check_name(name, err)) == NULL) return;
  if (is_link(hpid)) {
    set_error(ADFH_ERR_LINK_DATA, err);
    return;
  }
  if (has_child(hpid, nname)) {
    set_error(DUPLICATE_CHILD_NAME, err);
    return;
  }
  if (!get_str_att(hid, A_NAME, oname, err)) {
#ifdef ADFH_DEBUG_ON
    printf("%s change [%s] to [%s]\n",ADFH_PREFIX,oname,nname);
#endif
    if (H5Gmove(hpid, oname, nname) < 0)
      set_error(ADFH_ERR_GMOVE, err);
    else
      set_str_att(hid, A_NAME, nname, err);
  }
}

/* ----------------------------------------------------------------- */
/* Retrieve the name attribute value (same as group name)            */

void ADFH_Get_Name(const double  id,
                   char         *name,
                   int          *err)
{
  hid_t hid = to_HDF_ID(id);

  ADFH_DEBUG("ADFH_Get_Name");

  if (name == NULL) {
    set_error(NULL_STRING_POINTER, err);
    return;
  }
  get_str_att(hid, A_NAME, name, err);
}

/* ----------------------------------------------------------------- */
/* Retrieve the label attribute value                                */

void ADFH_Get_Label(const double  id,
                    char         *label,
                    int          *err)
{
  hid_t hid;

  ADFH_DEBUG("ADFH_Get_Label");

  if (label == NULL) {
    set_error(NULL_STRING_POINTER, err);
    return;
  }

  if ((hid = open_node(id, err)) >= 0) {
    get_str_att(hid, A_LABEL, label, err);
    H5Gclose(hid);
  }
}

/* ----------------------------------------------------------------- */
/* Create a new group, set its name in the name attribute
   - I may not need the name attribute, it's redundant and then
     dangerous. Anyway, now it's there, so let us use it
   - Update ref table                                                */

void ADFH_Create(const double  pid,
                 const char   *name,
                 double       *id,
                 int          *err)
{
  hid_t hpid = to_HDF_ID(pid);
  hid_t gid;
  int order;
  char *pname;

  ADFH_DEBUG("ADFH_Create");

  if ((pname = check_name(name, err)) == NULL) return;
  if (id == NULL) {
    set_error(NULL_NODEID_POINTER, err);
    return;
  }
  if (is_link(hpid)) {
    set_error(ADFH_ERR_LINK_NODE, err);
    return;
  }
  if (has_child(hpid, pname)) {
    set_error(DUPLICATE_CHILD_NAME, err);
    return;
  }

  *id = 0;
  gid = H5Gcreate(hpid, pname, 0);
  if (gid < 0)
    set_error(ADFH_ERR_GCREATE, err);
  else {
#ifdef ADFH_NO_ORDER
    if (new_str_att(gid, A_NAME, pname, ADF_NAME_LENGTH, err) ||
        new_str_att(gid, A_LABEL, "", ADF_NAME_LENGTH, err) ||
        new_str_att(gid, A_TYPE, ADFH_MT, 2, err)) return;
#else
    order = 0;
    H5Giterate(hpid, ".", NULL, count_children, (void *)&order);
    if (new_str_att(gid, A_NAME, pname, ADF_NAME_LENGTH, err) ||
        new_str_att(gid, A_LABEL, "", ADF_NAME_LENGTH, err) ||
        new_str_att(gid, A_TYPE, ADFH_MT, 2, err) ||
        new_int_att(gid, A_ORDER, order, err)) return;
#endif
    *id = to_ADF_ID(gid);
  }
}

/* ----------------------------------------------------------------- */
/* delete a node and all children recursively                        */

void ADFH_Delete(const double  pid,
                 const double  id,
                 int    *err)
{
  hid_t hpid = to_HDF_ID(pid);
  hid_t hid = to_HDF_ID(id);
  char old_name[ADF_NAME_LENGTH+1];
  int old_order;
  H5G_stat_t stat;

  ADFH_DEBUG("ADFH_Delete");

  if (is_link(hpid)) {
    set_error(ADFH_ERR_LINK_DELETE, err);
    return;
  }

  /* check that node is actually child of the parent */

  if (H5Gget_objinfo(hid, ".", 0, &stat) < 0 ||
    !H5Giterate(hpid, ".", NULL, compare_children, (void *)&stat)) {
    set_error(CHILD_NOT_OF_GIVEN_PARENT, err);
    return;
  }

  /* get name and order */

#ifdef ADFH_NO_ORDER
  if (get_str_att(hid, A_NAME, old_name, err)) return;
#else
  if (get_str_att(hid, A_NAME, old_name, err) ||
      get_int_att(hid, A_ORDER, &old_order, err)) return;
#endif

  /* delete children nodes recursively */

  H5Giterate(hid, ".", NULL, delete_children, NULL);

  /* delete current node */

  H5Gclose(hid);
  delete_node(hpid, old_name);

  /* decrement node orders */

#ifndef ADFH_NO_ORDER
  *err = H5Giterate(hpid, ".", NULL, fix_order, (void *)&old_order);
  if (!*err)
#endif
    set_error(NO_ERROR, err);
}

/* ----------------------------------------------------------------- */

void ADFH_Number_of_Children(const double  id,
                             int    *number,
                             int    *err)
{
  hid_t hid;

  ADFH_DEBUG("ADFH_Number_of_Children");

  if (number == NULL) {
    set_error(NULL_POINTER, err);
    return;
  }

  *number = 0;
  if ((hid = open_node(id, err)) >= 0) {
    H5Giterate(hid, ".", NULL, count_children, (void *)number);
    H5Gclose(hid);
  }
}

/* ----------------------------------------------------------------- */

void ADFH_Get_Node_ID(const double  pid,
                      const char   *name,
                      double       *id,
                      int          *err)
{
  hid_t sid, hpid = to_HDF_ID(pid);
  herr_t herr;

  ADFH_DEBUG("ADFH_Get_Node_ID");

  if (name == NULL) {
    set_error(NULL_STRING_POINTER, err);
    return;
  }
  if (id == NULL) {
    set_error(NULL_NODEID_POINTER, err);
    return;
  }

  *id = 0;
  set_error(NO_ERROR, err);
  if (*name == '/') {
    hid_t rid;
    char *path = (char *) malloc (strlen(name));
    if (path == NULL) {
      set_error(MEMORY_ALLOCATION_FAILED, err);
      return;
    }
    strcpy(path, &name[1]);
    rid = H5Gopen(hpid, "/");
    sid = parse_path(rid, path, err);
    H5Gclose(rid);
    free(path);
  }
  else if (is_link(hpid)) {
    hid_t lid = open_link(hpid, err);
    if (lid < 0) return;
    sid = H5Gopen(lid, name);
    H5Gclose(lid);
    if(sid < 0)
      set_error(ADFH_ERR_GOPEN, err);
  }
  else {
    sid = H5Gopen(hpid, name);
    if(sid < 0)
      set_error(ADFH_ERR_GOPEN, err);
  }
  *id = to_ADF_ID(sid);
}

/* ----------------------------------------------------------------- */

void ADFH_Children_Names(const double pid,
                         const int    istart,
                         const int    ilen,
                         const int    name_length,
                         int   *ilen_ret,
                         char  *names,
                         int   *err)
{
  int i, ret;
  hid_t hpid;

  ADFH_DEBUG("ADFH_Children_Names");

  if (ilen_ret == NULL) {
    set_error(NULL_POINTER, err);
    return;
  }
  if (names == NULL) {
    set_error(NULL_STRING_POINTER, err);
    return;
  }

  i_start = istart;
  i_len = ilen;
  n_length = name_length;
  n_names = 0;
#ifdef ADFH_NO_ORDER
  i_count = 0;
#endif

  /*initialize names to null*/
  memset(names, 0, ilen*name_length);

  if ((hpid = open_node(pid, err)) >= 0) {
    H5Giterate(hpid, ".", NULL, children_names, (void *)names);
    H5Gclose(hpid);
  }
  *ilen_ret = n_names;
}

/* ----------------------------------------------------------------- */

void ADFH_Children_IDs(const double pid,
                         const int    istart,
                         const int    icount,
                         int   *icount_ret,
                         double  *IDs,
                         int   *err)
{
  int ret;
  hid_t hpid;

  ADFH_DEBUG("ADFH_Children_IDs");

  if (icount_ret == NULL) {
    set_error(NULL_POINTER, err);
    return;
  }
  if (IDs == NULL) {
    set_error(NULL_NODEID_POINTER, err);
    return;
  }

  i_start = istart;
  i_len = icount;
  n_names = 0;
#ifdef ADFH_NO_ORDER
  i_count = 0;
#endif

  if ((hpid = open_node(pid, err)) >= 0) {
    H5Giterate(hpid, ".", NULL, children_ids, (void *)IDs);
    H5Gclose(hpid);
  }
  *icount_ret = n_names;
}

/* ----------------------------------------------------------------- */

void ADFH_Release_ID(const double ID)
{
  ADFH_DEBUG("ADFH_Release_ID");

  H5Gclose(to_HDF_ID(ID));
}

/* ----------------------------------------------------------------- */

void ADFH_Database_Open(const char   *name,
                        const char   *stat,
                        const char   *fmt,
                        double       *root,
                        int          *err)
{
  hid_t fid, gid, fapl;
  char *format, buff[ADF_VERSION_LENGTH+1];
  int i, pos, mode;

  ADFH_DEBUG("ADFH_Database_Open");

#ifndef ADF_DEBUG_ON
/*  H5Eset_auto(NULL, NULL); */
#endif
  if (!g_init) {
    H5Eset_auto(walk_H5_error, NULL);
    for (i = 0; i < ADFH_MAXIMUM_FILES; i++)
      g_files[i].fid = 0;
    g_init = 1;
  }

  if (name == NULL || stat == NULL || fmt == NULL) {
    set_error(NULL_STRING_POINTER, err);
    return;
  }

  /* get open mode */

  strncpy(buff, stat, 9);
  buff[9] = 0;
  for (i = 0; buff[i]; i++)
    buff[i] = TO_UPPER(buff[i]);

  if (0 == strcmp(buff, "UNKNOWN")) {
    if (access(name, 0))
      mode = ADFH_MODE_NEW;
    else if (access(name, 2))
      mode = ADFH_MODE_RDO;
    else
      mode = ADFH_MODE_OLD;
  }
  else if (0 == strcmp(buff, "NEW")) {
    if (!access(name, 0)) {
      set_error(REQUESTED_NEW_FILE_EXISTS, err);
      return;
    }
    mode = ADFH_MODE_NEW;
  }
  else if (0 == strcmp(buff, "READ_ONLY")) {
    if (access(name, 0)) {
      set_error(REQUESTED_OLD_FILE_NOT_FOUND, err);
      return;
    }
    mode = ADFH_MODE_RDO;
  }
  else if (0 == strcmp(buff, "OLD")) {
    if (access(name, 0)) {
      set_error(REQUESTED_OLD_FILE_NOT_FOUND, err);
      return;
    }
    mode = ADFH_MODE_OLD;
  }
  else {
    set_error(ADF_FILE_STATUS_NOT_RECOGNIZED, err);
    return;
  }

  /* get format */

#if 0
  if (mode == ADFH_MODE_NEW) {
    strncpy(buff, fmt, 11);
    buff[11] = 0;
    for (i = 0; buff[i]; i++)
      buff[i] = TO_UPPER(buff[i]);

    if (strcmp(buff, "NATIVE") &&
        strncmp(buff, "IEEE_BIG", 8) &&
        strcmp(buff, "IEEE_LITTLE", 11) &&
        strcmp(buff, "CRAY")) {
      set_error(ADF_FILE_FORMAT_NOT_RECOGNIZED, err);
      return;
    }
  }
#endif

  /* get unused slot */

  for (pos = 0; pos < ADFH_MAXIMUM_FILES; pos++) {
    if (g_files[pos].fid == 0) break;
  }
  if (pos == ADFH_MAXIMUM_FILES) {
    set_error(TOO_MANY_ADF_FILES_OPENED, err);
    return;
  }

  /* set acess property to close all open accesses when file closed */

  fapl = H5Pcreate(H5P_FILE_ACCESS);
  H5Pset_fclose_degree(fapl, H5F_CLOSE_STRONG);

  /* open the file */

  set_error(NO_ERROR, err);

  if (mode == ADFH_MODE_NEW) {
    fid = H5Fcreate(name, H5F_ACC_TRUNC, H5P_DEFAULT, fapl);
    H5Pclose(fapl);
    if (fid < 0) {
      set_error(FILE_OPEN_ERROR, err);
      return;
    }
    gid = H5Gopen(fid, "/");
    memset(buff, 0, ADF_VERSION_LENGTH+1);
    ADFH_Library_Version(buff, err);
    format = native_format();
    if (new_str_att(gid, A_NAME, "HDF5 MotherNode", ADF_NAME_LENGTH, err) ||
        new_str_att(gid, A_LABEL, "Root Node of HDF5 File", ADF_NAME_LENGTH, err) ||
        new_str_att(gid, A_TYPE, ADFH_MT, 2, err) ||
        new_str_data(gid, D_FORMAT, format, strlen(format), err) ||
        new_str_data(gid, D_VERSION, buff, ADF_VERSION_LENGTH, err)) {
      H5Gclose(gid);
      return;
    }
  }
  else {
    if (H5Fis_hdf5(name) <= 0) {
      H5Pclose(fapl);
      set_error(ADFH_ERR_NOT_HDF5_FILE, err);
      return;
    }
    if (mode == ADFH_MODE_RDO)
      fid = H5Fopen(name, H5F_ACC_RDONLY, fapl);
    else
      fid = H5Fopen(name, H5F_ACC_RDWR, fapl);
    H5Pclose(fapl);
    if (fid < 0) {
      set_error(FILE_OPEN_ERROR, err);
      return;
    }
    gid = H5Gopen(fid, "/");
  }

  g_files[pos].fid = fid;
  g_files[pos].mode = mode;
  g_files[pos].mounted = NULL;
  *root = to_ADF_ID(gid);
}

/* ----------------------------------------------------------------- */

void ADFH_Database_Valid(const char   *name,
                        int          *err)
{
    if (NULL == name || 0 == *name)
        *err = NULL_STRING_POINTER;
    else
        *err = H5Fis_hdf5(name);
}

/* ----------------------------------------------------------------- */

void ADFH_Database_Get_Format(const double  rootid,
                              char         *format,
                              int          *err)
{
  char node[ADF_NAME_LENGTH+1];
  hid_t did;
  herr_t status;

  ADFH_DEBUG("ADFH_Database_Get_Format");

  if (format == NULL) {
    set_error(NULL_STRING_POINTER, err);
    return;
  }
  *format = 0;
  set_error(NO_ERROR, err);

  sprintf(node, "/%s", D_FORMAT);
  if ((did = H5Dopen(to_HDF_ID(rootid), node)) < 0) {
    set_error(ADFH_ERR_DOPEN, err);
    return;
  }
  status = H5Dread(did, H5T_NATIVE_CHAR, H5S_ALL, H5S_ALL, H5P_DEFAULT, format);
  H5Dclose(did);

  if (status < 0)
    set_error(ADFH_ERR_DREAD, err);
}

/* ----------------------------------------------------------------- */

void ADFH_Database_Set_Format(const double  rootid,
                              const char   *format,
                              int          *err)
{
  ADFH_DEBUG("ADFH_Database_Set_Format");
  set_error(ADFH_ERR_NOT_IMPLEMENTED, err);
}

/* ----------------------------------------------------------------- */

void ADFH_Database_Delete(const char *name,
                          int        *err)
{
  ADFH_DEBUG("ADFH_Database_Delete");

  if (H5Fis_hdf5(name) <= 0)
    set_error(ADFH_ERR_NOT_HDF5_FILE, err);
  else if (unlink(name))
    set_error(ADFH_ERR_FILE_DELETE, err);
  else
    set_error(NO_ERROR, err);
}

/* ----------------------------------------------------------------- */

void ADFH_Database_Close(const double  root,
                         int          *status)
{
  int n;
  hid_t fid, mid;
  ADFH_MOUNT *mf, *next;
#if 0
  int nobj;
  hid_t *objs;
#endif

  ADFH_DEBUG("ADFH_Database_Close");
#ifdef ADFH_DEBUG_ON
  printf("%s close root [%d]\n",ADFH_PREFIX,(int)root);
#endif

  if ((n = get_file_number(to_HDF_ID(root), status)) < 0) return;
  fid = g_files[n].fid;
  g_files[n].fid = 0;

  /* unmount and close any mounted files */

  mf = g_files[n].mounted;
  while (mf != NULL) {
    next = mf->next;
    mid = H5Gopen(fid, get_mount_point(mf->mntnum));
    if (mid >= 0) {
      H5Funmount(mid, D_MOUNT);
      H5Gclose(mid);
    }
    H5Fclose(mf->fid);
    free(mf);
    mf = next;
  }

  /* free up all open accesses */

#if 0
  nobj = H5Fget_obj_count(fid, H5F_OBJ_ALL);
  objs = (hid_t *) malloc (nobj * sizeof(hid_t));

  /* close datatypes */

  nobj = H5Fget_obj_count(fid, H5F_OBJ_DATATYPE);
  if (nobj) {
    H5Fget_obj_ids(fid, H5F_OBJ_DATATYPE, -1, objs);
    for (n = 0; n < nobj; n++)
      H5Tclose(objs[n]);
  }

  /* close datasets */

  nobj = H5Fget_obj_count(fid, H5F_OBJ_DATASET);
  if (nobj) {
    H5Fget_obj_ids(fid, H5F_OBJ_DATASET, -1, objs);
    for (n = 0; n < nobj; n++)
      H5Dclose(objs[n]);
  }

  /* close attributes */

  nobj = H5Fget_obj_count(fid, H5F_OBJ_ATTR);
  if (nobj) {
    H5Fget_obj_ids(fid, H5F_OBJ_ATTR, -1, objs);
    for (n = 0; n < nobj; n++)
      H5Aclose(objs[n]);
  }

  /* close groups */

  nobj = H5Fget_obj_count(fid, H5F_OBJ_GROUP);
  if (nobj) {
    H5Fget_obj_ids(fid, H5F_OBJ_GROUP, -1, objs);
    for (n = 0; n < nobj; n++)
        H5Gclose(objs[n]);
  }

#if 0
  /* close file accesses except for current */

  nobj = H5Fget_obj_count(fid, H5F_OBJ_FILE);
  if (nobj) {
    H5Fget_obj_ids(fid, H5F_OBJ_FILE, -1, objs);
    for (n = 0; n < nobj; n++) {
        if (objs[n] != fid)
            H5Fclose(objs[n]);
    }
  }
#endif

  free (objs);
#endif

  /* close file */

  if (H5Fclose(fid) < 0)
    set_error(FILE_CLOSE_ERROR, status);
  else
    set_error(NO_ERROR, status);
}

/* ----------------------------------------------------------------- */

void ADFH_Is_Link(const double  id,
                  int          *link_path_length,
                  int          *err)
{
  hid_t hid = to_HDF_ID(id);

  ADFH_DEBUG("ADFH_Is_Link");

  if (is_link(hid)) {
    hid_t did, sid;
    hsize_t size;

    did = H5Dopen(hid, D_PATH);
    sid = H5Dget_space(did);
    size = H5Sget_simple_extent_npoints(sid);
    H5Sclose(sid);
    H5Dclose(did);
    *link_path_length = (int)size;

    if (has_child(hid, D_FILE)) {
      did = H5Dopen(hid, D_FILE);
      sid = H5Dget_space(did);
      size = H5Sget_simple_extent_npoints(sid);
      H5Sclose(sid);
      H5Dclose(did);
      *link_path_length += (int)size;
    }
  }
  else
    *link_path_length = 0;
  set_error(NO_ERROR, err);
}

/* ----------------------------------------------------------------- */

void ADFH_Get_Root_ID(const double  id,
                      double       *root_id,
                      int          *err)
{
  hid_t rid;

  ADFH_DEBUG("ADFH_Get_Root_ID");

  rid = H5Gopen(to_HDF_ID(id), "/");
  if (rid < 0)
    set_error(ADFH_ERR_GOPEN, err);
  else {
    *root_id = to_ADF_ID(rid);
    set_error(NO_ERROR, err);
  }
}

/* ----------------------------------------------------------------- */

void ADFH_Get_Data_Type(const double  id,
                        char         *data_type,
                        int          *err)
{
  hid_t hid;

  ADFH_DEBUG("ADFH_Get_Data_Type");

  if ((hid = open_node(id, err)) >= 0) {
    get_str_att(hid, A_TYPE, data_type, err);
    H5Gclose(hid);
  }
}

/* ----------------------------------------------------------------- */

void ADFH_Get_Number_of_Dimensions(const double  id,
                                   int          *num_dims,
                                   int          *err)
{
  hid_t hid, did, sid;
  char type[3];

  ADFH_DEBUG("ADFH_Get_Number_of_Dimensions");

  *num_dims = 0;
  if ((hid = open_node(id, err)) < 0) return;
  if (get_str_att(hid, A_TYPE, type, err) ||
    0 == strcmp(type, ADFH_MT) || 0 == strcmp(type, ADFH_LK)) {
    H5Gclose(hid);
    return;
  }

  if ((did = H5Dopen(hid, D_DATA)) < 0)
    set_error(NO_DATA, err);
  else {
    if ((sid = H5Dget_space(did)) < 0)
      set_error(ADFH_ERR_DGET_SPACE, err);
    else {
      *num_dims = H5Sget_simple_extent_ndims(sid);
      H5Sclose(sid);
    }
    H5Dclose(did);
  }
  H5Gclose(hid);
}

/* ----------------------------------------------------------------- */

void ADFH_Get_Dimension_Values(const double  id,
                               int           dim_vals[],
                               int           *err)
{
  int i, ndims;
  hid_t hid, did, sid;
  hsize_t temp_vals[ADF_MAX_DIMENSIONS];
  herr_t status;

  ADFH_DEBUG("ADFH_Get_Dimension_Values");

  ndims=0;
  dim_vals[0]=0;
  if ((hid = open_node(id, err)) < 0) return;
  if ((did = H5Dopen(hid, D_DATA)) < 0)
    set_error(NO_DATA, err);
  else {
    if ((sid = H5Dget_space(did)) < 0)
      set_error(ADFH_ERR_DGET_SPACE, err);
    else {
      ndims = H5Sget_simple_extent_ndims(sid);
      if (ndims > 0) {
        H5Sget_simple_extent_dims(sid, temp_vals, NULL);
        for (i = 0; i < ndims; i++)
          dim_vals[i] = (int)temp_vals[i];
      }
      H5Sclose(sid);
    }
    H5Dclose(did);
  }
  H5Gclose(hid);
}

/* ----------------------------------------------------------------- */

void ADFH_Put_Dimension_Information(const double  id,
                                    const char   *data_type,
                                    const int     dims,
                                    const int     dim_vals[],
                                    int          *err)
{
  hid_t hid = to_HDF_ID(id);
  hid_t did, tid, sid, mid;
  int i;
  hsize_t old_size;
  hsize_t old_dims[ADF_MAX_DIMENSIONS];
  hsize_t new_dims[ADF_MAX_DIMENSIONS];
  void *data = NULL;
  char old_type[3];
  char new_type[3];

  ADFH_DEBUG("ADFH_Put_Dimension_Information");

  if (is_link(hid)) {
    set_error(ADFH_ERR_LINK_DATA, err);
    return;
  }
  for (i = 0; i < 2; i++)
    new_type[i] = TO_UPPER(data_type[i]);
  new_type[2] = 0;

  if (0 == strcmp(new_type, ADFH_MT)) {
    if (has_data(hid))
      H5Gunlink(hid, D_DATA);
    set_str_att(hid, A_TYPE, new_type, err);
    return;
  }

  if (check_data_type(new_type, err)) return;
  if (dims < 1 || dims > ADF_MAX_DIMENSIONS) {
    set_error(BAD_NUMBER_OF_DIMENSIONS, err);
    return;
  }
  for (i = 0; i < dims; i++) {
    if (dim_vals[i] < 1) {
      set_error(BAD_DIMENSION_VALUE, err);
      return;
    }
  }

  /*
   * The ADF documentation allows the dimension values to be changed
   * without affecting the data, so long as the data type and number
   * of dimensions are the same. With HDF5, we could emulate that by
   * using extendable data spaces (with chunking). However this only
   * allows the data size to increase, not decrease, and coming up
   * with a good value for chunking is difficult. Since changing the
   * dimension values without rewiting the data is not a common
   * operation, I decided to use fixed sizes, then buffer the data
   * in these rare cases.
   */

  old_size = 0;
  if(has_data(hid)) {
#if 0  /* with CGNSlib, we never do this */
    if (get_str_att(hid, A_TYPE, old_type, err)) return;
    if ((did = H5Dopen(hid, D_DATA)) < 0) {
      set_error(ADFH_ERR_DOPEN, err);
      return;
    }

    /* check if the data type changed */

    if (0 == strcmp(new_type, old_type)) {
      if ((sid = H5Dget_space(did)) < 0) {
        set_error(ADFH_ERR_DGET_SPACE, err);
        H5Dclose(did);
        return;
      }

      /* check if the number of dimensions changed */

      if (dims == H5Sget_simple_extent_ndims(sid)) {
        H5Sget_simple_extent_dims(sid, old_dims, NULL);
        old_size = H5Dget_storage_size(did);
      }

      H5Sclose(sid);
    }

    /*
     * data type and number of dimensions are the same,
     * so read the data before deleting the dataset
     */

    if (old_size) {
        data = malloc ((unsigned)old_size);
        if (NULL == data) {
          H5Dclose(did);
          set_error(MEMORY_ALLOCATION_FAILED, err);
          return;
        }
        tid = H5Dget_type(did);
        mid = H5Tget_native_type(tid, H5T_DIR_ASCEND);
        H5Dread(did, mid, H5S_ALL, H5S_ALL, H5P_DEFAULT, data);
        H5Tclose(mid);
        H5Tclose(tid);
    }

    /* close and delete data */

    H5Dclose(did);
#endif
    H5Gunlink(hid, D_DATA);
  }

  if (set_str_att(hid, A_TYPE, new_type, err)) {
    if (data != NULL) free(data);
    return;
  }

  /* recreate the data space with the new values */

  for (i = 0; i < dims; i++)
      new_dims[i] = (hsize_t)dim_vals[i];

  tid = to_HDF_data_type(new_type);
  sid = H5Screate_simple(dims, new_dims, NULL);
  did = H5Dcreate(hid, D_DATA, tid, sid, H5P_DEFAULT);

  if (did < 0 && data != NULL) {
    free(data);
    data = NULL;
  }

  /* write the saved data back to the data space */

  if (data != NULL) {
    mid = H5Tget_native_type(tid, H5T_DIR_ASCEND);
    if (old_size < H5Dget_storage_size(did))
      H5Sset_extent_simple(sid, dims, old_dims, NULL);
    H5Dwrite(did, mid, H5S_ALL, sid, H5P_DEFAULT, data);
    H5Tclose(mid);
    free(data);
  }

  H5Sclose(sid);
  H5Tclose(tid);

  if (did < 0)
    set_error(ADFH_ERR_DCREATE, err);
  else {
    H5Dclose(did);
    set_error(NO_ERROR, err);
  }
}

/* ----------------------------------------------------------------- */

void ADFH_Get_Link_Path(const double  id,
                        char   *filename,
                        char   *link_path,
                        int    *err)
{
  hid_t hid, did;
  char *mntpath;

  ADFH_DEBUG("ADFH_Get_Link_Path");

  hid = to_HDF_ID(id);
  if (!is_link(hid)) {
    set_error(NODE_IS_NOT_A_LINK, err);
    return;
  }
  did = H5Dopen(hid, D_PATH);
  H5Dread(did, H5T_NATIVE_CHAR, H5S_ALL, H5S_ALL, H5P_DEFAULT, link_path);
  H5Dclose(did);

  if (has_child(hid, D_FILE)) {
    did = H5Dopen(hid, D_FILE);
    H5Dread(did, H5T_NATIVE_CHAR, H5S_ALL, H5S_ALL, H5P_DEFAULT, filename);
    H5Dclose(did);
  }
  else
    *filename = 0;
  set_error(NO_ERROR, err);
}

/* ----------------------------------------------------------------- */

void ADFH_Link(const double  pid,
               const char   *name,
               const char   *file,
               const char   *name_in_file,
               double       *id,
               int          *err)
{
  int refcnt, mntnum;
  char mntname[32], *target;
  herr_t status;
  hid_t rid, gid, mid, lid;

  ADFH_Create(pid, name, id, err);
  if (*err != NO_ERROR) return;
  lid = to_HDF_ID(*id);
  if (set_str_att(lid, A_TYPE, ADFH_LK, err)) return;

  /*
   * If this is a link to a file, then need to create a mount point.
   * The file will not be mounted until the link is traversed.
   * If there are multiple links to the same file, only mount it once.
   */

  if (*file) {
    rid = H5Gopen(lid, "/");

    /* create mount group, if it doesn't exist */

    if (has_child(rid, D_MOUNT)) {
      if ((gid = H5Gopen(rid, D_MOUNT)) < 0) {
        H5Gclose(rid);
        set_error(ADFH_ERR_GOPEN, err);
        return;
      }
    }
    else {
      if ((gid = H5Gcreate(rid, D_MOUNT, 0)) < 0) {
        H5Gclose(rid);
        set_error(ADFH_ERR_GCREATE, err);
        return;
      }
    }
    H5Gclose(rid);

    mntnum = H5Giterate(gid, ".", NULL, find_link_file, (void *)file);

    /* file mount point exists, so increment reference count */

    if (mntnum) {
      sprintf(mntname, "%c%d", D_PREFIX, mntnum);
      mid = H5Gopen(gid, mntname);
      H5Gclose(gid);
      if (mid < 0) {
        set_error(ADFH_ERR_GOPEN, err);
        return;
      }
      if (get_int_att(mid, A_REFCNT, &refcnt, err) ||
          set_int_att(mid, A_REFCNT, ++refcnt, err)) {
        H5Gclose(mid);
        return;
      }
      H5Gclose(mid);
    }

    /* file mount point doesn't exist, so create it */

    else {
      H5Giterate(gid, ".", NULL, next_link_file, (void *)&mntnum);
      sprintf(mntname, "%c%d", D_PREFIX, ++mntnum);
      mid = H5Gcreate(gid, mntname, 0);
      H5Gclose(gid);
      if (mid < 0) {
        set_error(ADFH_ERR_GCREATE, err);
        return;
      }
      refcnt = 1;
      if (new_str_att(mid, A_FILE, file, strlen(file), err) ||
          new_int_att(mid, A_REFCNT, refcnt, err)) {
        H5Gclose(mid);
        return;
      }
      gid = H5Gcreate(mid, D_MOUNT, 0);
      H5Gclose(mid);
      if (gid < 0) {
        set_error(ADFH_ERR_GCREATE, err);
        return;
      }
      H5Gclose(gid);
    }

    /* create mount point attribute */

    if (new_int_att(lid, A_MOUNT, mntnum, err)) return;

    /* set link path to include mount point */

    target = (char *) malloc (2*strlen(D_MOUNT)+strlen(mntname)+
      strlen(name_in_file)+5);
    if (target == NULL) {
      set_error(MEMORY_ALLOCATION_FAILED, err);
      return;
    }
    if (*name_in_file == '/')
      sprintf(target,"/%s/%s/%s%s", D_MOUNT, mntname, D_MOUNT, name_in_file);
    else
      sprintf(target,"/%s/%s/%s/%s", D_MOUNT, mntname, D_MOUNT, name_in_file);
  }

  /* link to node within current file */

  else {
    target = (char *) malloc (strlen(name_in_file)+2);
    if (target == NULL) {
      set_error(MEMORY_ALLOCATION_FAILED, err);
      return;
    }
    if (*name_in_file == '/')
      strcpy(target, name_in_file);
    else
      sprintf(target, "/%s", name_in_file);
  }

  /* create a soft link */

  status = H5Glink(lid, H5G_LINK_SOFT, target, D_LINK);
  free(target);
  if (status < 0) {
    set_error(ADFH_ERR_GLINK, err);
    return;
  }

  /* save link path and file */

  if (new_str_data(lid, D_PATH, name_in_file, strlen(name_in_file), err)) return;
  if (*file && new_str_data(lid, D_FILE, file, strlen(file), err)) return;

  set_error(NO_ERROR, err);
}

/* ----------------------------------------------------------------- */

void ADFH_Flush_to_Disk(const double  id,
                        int          *err)
{
  ADFH_DEBUG("ADFH_Flush_to_Disk");

  if(H5Fflush(to_HDF_ID(id), H5F_SCOPE_LOCAL) >=0 )
    set_error(NO_ERROR, err);
  else
    set_error(FFLUSH_ERROR, err);
}

/* ----------------------------------------------------------------- */

void ADFH_Database_Garbage_Collection(const double  id,
                                      int          *err)
{
  ADFH_DEBUG("ADFH_Database_Garbage_Collection");

  if(H5garbage_collect() >= 0)
    set_error(NO_ERROR, err);
  else
    set_error(NO_DATA, err);
}

/* ----------------------------------------------------------------- */

void ADFH_Database_Version(const double  root_id,
                           char         *version,
                           char         *creation_date,
                           char         *modification_date,
                           int          *err)
{
  char buff[ADF_VERSION_LENGTH+1];
  char node[ADF_NAME_LENGTH+1];
  hid_t did;
  herr_t status;

  ADFH_DEBUG("ADFH_Database_Version");

  if (version == NULL) {
    set_error(NULL_STRING_POINTER, err);
    return;
  }
  *version = 0;
  if (creation_date != NULL) *creation_date = 0;
  if (modification_date != NULL) *modification_date = 0;
  set_error(NO_ERROR, err);

  sprintf(node, "/%s", D_VERSION);
  if ((did = H5Dopen(to_HDF_ID(root_id), node)) < 0) {
    set_error(ADFH_ERR_DOPEN, err);
    return;
  }
  status = H5Dread(did, H5T_NATIVE_CHAR, H5S_ALL, H5S_ALL, H5P_DEFAULT, buff);
  H5Dclose(did);

  if (status < 0)
    set_error(ADFH_ERR_DREAD, err);
  else
    strcpy(version, buff);
}

/* ----------------------------------------------------------------- */

void ADFH_Library_Version(char *version,
                          int  *err)
{
  unsigned maj, min, rel;

  ADFH_DEBUG("ADFH_Library_Version");

  if (version == NULL) {
    set_error(NULL_STRING_POINTER, err);
    return;
  }
  H5get_libversion(&maj, &min, &rel);
  sprintf(version, "HDF5 Version %d.%d.%d", maj, min, rel);
  set_error(NO_ERROR, err);
}

/* ----------------------------------------------------------------- */

void ADFH_Set_Error_State(const int  error_state,
                          int       *error_return)
{
  ADFH_DEBUG("ADFH_Set_Error_State");

  g_error_state = error_state;
  set_error(NO_ERROR, error_return);
}

/* ----------------------------------------------------------------- */

void ADFH_Error_Message(const int  error_return_input,
                        char      *error_string )
{
  int i;

  ADFH_DEBUG("ADFH_Error_Message");

  if (error_string == NULL) return;

  for (i = 0; i < NUM_ERRORS; i++) {
    if (ErrorList[i].errcode == error_return_input) {
      strcpy(error_string, ErrorList[i].errmsg);
      return;
    }
  }
  sprintf(error_string, "error number %d", error_return_input);
}

/* ----------------------------------------------------------------- */

void ADFH_Get_Error_State(int *error_state,
                          int *error_return)
{
  ADFH_DEBUG("ADFH_Get_Error_State");
  *error_state = g_error_state;
  set_error(NO_ERROR, error_return);
}

/* ----------------------------------------------------------------- */

void ADFH_Read_Block_Data(const double ID,
                      const long b_start,
                      const long b_end,
                      char *data,
                      int *err )
{
  hid_t hid, did, mid, tid, dspace;
  size_t size, count, offset;
  char *buff;

  ADFH_DEBUG("ADFH_Read_Block_Data");

  if (data == NULL) {
    set_error(NULL_POINTER, err);
    return;
  }
  if (b_start > b_end) {
    set_error(MINIMUM_GT_MAXIMUM, err);
    return;
  }
  if (b_start < 1) {
    set_error(START_OUT_OF_DEFINED_RANGE, err);
    return;
  }
  if ((hid = open_node(ID, err)) < 0) return;

  if (!has_data(hid)) {
    H5Gclose(hid);
    set_error(NO_DATA, err);
    return;
  }
  if ((did = H5Dopen(hid, D_DATA)) < 0) {
    H5Gclose(hid);
    set_error(ADFH_ERR_DOPEN, err);
    return;
  }

  dspace = H5Dget_space(did);
  count = (size_t)H5Sget_simple_extent_npoints(dspace);
  H5Sclose(dspace);

  if ((size_t)b_end > count) {
    H5Dclose(did);
    H5Gclose(hid);
    set_error(END_OUT_OF_DEFINED_RANGE, err);
    return;
  }

  /* instead of trying to compute data space extents from
   * b_start and b_end, just read all the data into a
   * 1-d array and copy the range we want */

  tid = H5Dget_type(did);
  mid = H5Tget_native_type(tid, H5T_DIR_ASCEND);
  size = H5Tget_size(mid);

  if ((buff = (char *) malloc (size * count)) == NULL) {
    H5Tclose(mid);
    H5Tclose(tid);
    H5Dclose(did);
    H5Gclose(hid);
    set_error(MEMORY_ALLOCATION_FAILED, err);
    return;
  }

  if (H5Dread(did, mid, H5S_ALL, H5S_ALL, H5P_DEFAULT, buff) < 0)
    set_error(ADFH_ERR_DREAD, err);
  else {
    offset = size * (b_start - 1);
    count = size * (b_end - b_start + 1);
    memcpy(data, &buff[offset], count);
    set_error(NO_ERROR, err);
  }

  free (buff);
  H5Tclose(mid);
  H5Tclose(tid);
  H5Dclose(did);
  H5Gclose(hid);
}

/* ----------------------------------------------------------------- */

void ADFH_Read_Data(const double ID,
                     const int s_start[],
                     const int s_end[],
                     const int s_stride[],
                     const int m_num_dims,
                     const int m_dims[],
                     const int m_start[],
                     const int m_end[],
                     const int m_stride[],
                     char *data,
                     int *err )
{
  int n, ndim;
  hid_t hid, did, mid, tid, dspace, mspace;
  hsize_t dims[ADF_MAX_DIMENSIONS];
  hsize_t start[ADF_MAX_DIMENSIONS];
  hsize_t stride[ADF_MAX_DIMENSIONS];
  hsize_t count[ADF_MAX_DIMENSIONS];
  herr_t status;

  ADFH_DEBUG("ADFH_Read_Data");

  if ((hid = open_node(ID, err)) < 0) return;

  if (!has_data(hid)) {
    H5Gclose(hid);
    set_error(NO_DATA, err);
    return;
  }
  if ((did = H5Dopen(hid, D_DATA)) < 0) {
    H5Gclose(hid);
    set_error(ADFH_ERR_DOPEN, err);
    return;
  }

  /* get data space extents */

  dspace = H5Dget_space(did);
  ndim = H5Sget_simple_extent_ndims(dspace);
  H5Sget_simple_extent_dims(dspace, dims, NULL);

  /* create data hyperslab */

  for (n = 0; n < ndim; n++) {
    if (s_start[n] < 1)
      set_error(START_OUT_OF_DEFINED_RANGE, err);
    else if ((hsize_t)s_end[n] > dims[n])
      set_error(END_OUT_OF_DEFINED_RANGE, err);
    else if (s_start[n] > s_end[n])
      set_error(MINIMUM_GT_MAXIMUM, err);
    else if (s_stride[n] < 1 ||
      s_stride[n] > (s_end[n] - s_start[n] + 1))
      set_error(BAD_STRIDE_VALUE, err);
    else
      set_error(NO_ERROR, err);
    if (*err != NO_ERROR) {
      H5Sclose(dspace);
      H5Dclose(did);
      H5Gclose(hid);
      return;
    }
    start[n] = s_start[n] - 1;
    stride[n] = s_stride[n];
    count[n] = (s_end[n] - s_start[n] + 1) / s_stride[n];
  }

  H5Sselect_hyperslab(dspace, H5S_SELECT_SET, start, stride, count, NULL);

  /* create memory hyperslab */

  for (n = 0; n < m_num_dims; n++) {
    if (m_start[n] < 1)
      set_error(START_OUT_OF_DEFINED_RANGE, err);
    else if (m_end[n] > m_dims[n])
      set_error(END_OUT_OF_DEFINED_RANGE, err);
    else if (m_start[n] > m_end[n])
      set_error(MINIMUM_GT_MAXIMUM, err);
    else if (m_stride[n] < 1 ||
      m_stride[n] > (m_end[n] - m_start[n] + 1))
      set_error(BAD_STRIDE_VALUE, err);
    else
      set_error(NO_ERROR, err);
    if (*err != NO_ERROR) {
      H5Sclose(dspace);
      H5Dclose(did);
      H5Gclose(hid);
      return;
    }
    dims[n] = m_dims[n];
    start[n] = m_start[n] - 1;
    stride[n] = m_stride[n];
    count[n] = (m_end[n] - m_start[n] + 1) / m_stride[n];
  }

  mspace = H5Screate_simple(m_num_dims, dims, NULL);
  H5Sselect_hyperslab(mspace, H5S_SELECT_SET, start, stride, count, NULL);

  if (H5Sget_select_npoints(mspace) != H5Sget_select_npoints(dspace)) {
      H5Sclose(mspace);
      H5Sclose(dspace);
      H5Dclose(did);
      H5Gclose(hid);
      set_error(UNEQUAL_MEMORY_AND_DISK_DIMS, err);
      return;
  }

  /* read the data */

  tid = H5Dget_type(did);
  mid = H5Tget_native_type(tid, H5T_DIR_ASCEND);
  status = H5Dread(did, mid, mspace, dspace, H5P_DEFAULT, data);

  H5Sclose(mspace);
  H5Sclose(dspace);
  H5Tclose(mid);
  H5Tclose(tid);
  H5Dclose(did);
  H5Gclose(hid);

  if (status < 0)
    set_error(ADFH_ERR_DREAD, err);
  else
    set_error(NO_ERROR, err);
}

/* ----------------------------------------------------------------- */

void ADFH_Read_All_Data(const double  id,
                        char         *data,
                        int          *err)
{
  hid_t hid, did, tid, mid;
  herr_t status;

  ADFH_DEBUG("ADFH_Read_All_Data");

  if ((hid = open_node(id, err)) < 0) return;

  if (has_data(hid)) {
    did = H5Dopen(hid, D_DATA);
    tid = H5Dget_type(did);
    mid = H5Tget_native_type(tid, H5T_DIR_ASCEND);

    if (H5Dread(did, mid, H5S_ALL, H5S_ALL, H5P_DEFAULT, data) < 0)
      set_error(ADFH_ERR_DREAD, err);
    else
      set_error(NO_ERROR, err);

    H5Tclose(mid);
    H5Tclose(tid);
    H5Dclose(did);
  }
  else
    set_error(NO_DATA, err);
  H5Gclose(hid);
}

/* ----------------------------------------------------------------- */

void ADFH_Write_Block_Data(const double ID,
                            const long b_start,
                            const long b_end,
                            char *data,
                            int *err )
{
  hid_t hid, did, mid, tid, dspace;
  size_t size, count, offset;
  char *buff;

  ADFH_DEBUG("ADFH_Write_Block_Data");

  if (data == NULL) {
    set_error(NULL_POINTER, err);
    return;
  }
  if (b_start > b_end) {
    set_error(MINIMUM_GT_MAXIMUM, err);
    return;
  }
  if (b_start < 1) {
    set_error(START_OUT_OF_DEFINED_RANGE, err);
    return;
  }
  hid = to_HDF_ID(ID);
  if (is_link(hid)) {
    set_error(ADFH_ERR_LINK_DATA, err);
    return;
  }
  if (!has_data(hid)) {
    set_error(NO_DATA, err);
    return;
  }
  if ((did = H5Dopen(hid, D_DATA)) < 0) {
    set_error(ADFH_ERR_DOPEN, err);
    return;
  }

  dspace = H5Dget_space(did);
  count = (size_t)H5Sget_simple_extent_npoints(dspace);
  H5Sclose(dspace);

  if ((size_t)b_end > count) {
    H5Dclose(did);
    set_error(END_OUT_OF_DEFINED_RANGE, err);
    return;
  }

  /* instead of trying to compute data space extents from
   * b_start and b_end, just read all the data into a
   * 1-d array, copy the range we want and rewrite the data */

  tid = H5Dget_type(did);
  mid = H5Tget_native_type(tid, H5T_DIR_ASCEND);
  size = H5Tget_size(mid);

  if ((buff = (char *) malloc (size * count)) == NULL) {
    H5Tclose(mid);
    H5Tclose(tid);
    H5Dclose(did);
    set_error(MEMORY_ALLOCATION_FAILED, err);
    return;
  }

  if (H5Dread(did, mid, H5S_ALL, H5S_ALL, H5P_DEFAULT, buff) < 0)
    set_error(ADFH_ERR_DREAD, err);
  else {
    offset = size * (b_start - 1);
    count = size * (b_end - b_start + 1);
    memcpy(&buff[offset], data, count);
    if (H5Dwrite(did, mid, H5S_ALL, H5S_ALL, H5P_DEFAULT, buff) < 0)
      set_error(ADFH_ERR_DWRITE, err);
    else
      set_error(NO_ERROR, err);
  }

  free (buff);
  H5Tclose(mid);
  H5Tclose(tid);
  H5Dclose(did);
}

/* ----------------------------------------------------------------- */

void ADFH_Write_Data(const double ID,
                      const int s_start[],
                      const int s_end[],
                      const int s_stride[],
                      const int m_num_dims,
                      const int m_dims[],
                      const int m_start[],
                      const int m_end[],
                      const int m_stride[],
                      const char *data,
                      int *err )
{
  int n, ndim;
  hid_t hid, did, mid, tid, dspace, mspace;
  hsize_t dims[ADF_MAX_DIMENSIONS];
  hsize_t start[ADF_MAX_DIMENSIONS];
  hsize_t stride[ADF_MAX_DIMENSIONS];
  hsize_t count[ADF_MAX_DIMENSIONS];
  herr_t status;

  ADFH_DEBUG("ADFH_Write_Data");

  if (data == NULL) {
    set_error(NULL_POINTER, err);
    return;
  }
  hid = to_HDF_ID(ID);
  if (is_link(hid)) {
    set_error(ADFH_ERR_LINK_DATA, err);
    return;
  }
  if (!has_data(hid)) {
    set_error(NO_DATA, err);
    return;
  }
  if ((did = H5Dopen(hid, D_DATA)) < 0) {
    set_error(ADFH_ERR_DOPEN, err);
    return;
  }

  /* get data space extents */

  dspace = H5Dget_space(did);
  ndim = H5Sget_simple_extent_ndims(dspace);
  H5Sget_simple_extent_dims(dspace, dims, NULL);

  /* create data hyperslab */

  for (n = 0; n < ndim; n++) {
    if (s_start[n] < 1)
      set_error(START_OUT_OF_DEFINED_RANGE, err);
    else if ((hsize_t)s_end[n] > dims[n])
      set_error(END_OUT_OF_DEFINED_RANGE, err);
    else if (s_start[n] > s_end[n])
      set_error(MINIMUM_GT_MAXIMUM, err);
    else if (s_stride[n] < 1 ||
      s_stride[n] > (s_end[n] - s_start[n] + 1))
      set_error(BAD_STRIDE_VALUE, err);
    else
      set_error(NO_ERROR, err);
    if (*err != NO_ERROR) {
      H5Sclose(dspace);
      H5Dclose(did);
      return;
    }
    start[n] = s_start[n] - 1;
    stride[n] = s_stride[n];
    count[n] = (s_end[n] - s_start[n] + 1) / s_stride[n];
  }

  H5Sselect_hyperslab(dspace, H5S_SELECT_SET, start, stride, count, NULL);

  /* create memory hyperslab */

  for (n = 0; n < m_num_dims; n++) {
    if (m_start[n] < 1)
      set_error(START_OUT_OF_DEFINED_RANGE, err);
    else if (m_end[n] > m_dims[n])
      set_error(END_OUT_OF_DEFINED_RANGE, err);
    else if (m_start[n] > m_end[n])
      set_error(MINIMUM_GT_MAXIMUM, err);
    else if (m_stride[n] < 1 ||
      m_stride[n] > (m_end[n] - m_start[n] + 1))
      set_error(BAD_STRIDE_VALUE, err);
    else
      set_error(NO_ERROR, err);
    if (*err != NO_ERROR) {
      H5Sclose(dspace);
      H5Dclose(did);
      return;
    }
    dims[n] = m_dims[n];
    start[n] = m_start[n] - 1;
    stride[n] = m_stride[n];
    count[n] = (m_end[n] - m_start[n] + 1) / m_stride[n];
  }

  mspace = H5Screate_simple(m_num_dims, dims, NULL);
  H5Sselect_hyperslab(mspace, H5S_SELECT_SET, start, stride, count, NULL);

  if (H5Sget_select_npoints(mspace) != H5Sget_select_npoints(dspace)) {
      H5Sclose(mspace);
      H5Sclose(dspace);
      H5Dclose(did);
      set_error(UNEQUAL_MEMORY_AND_DISK_DIMS, err);
      return;
  }

  /* write the data */

  tid = H5Dget_type(did);
  mid = H5Tget_native_type(tid, H5T_DIR_ASCEND);
  status = H5Dwrite(did, mid, mspace, dspace, H5P_DEFAULT, data);

  H5Sclose(mspace);
  H5Sclose(dspace);
  H5Tclose(mid);
  H5Tclose(tid);
  H5Dclose(did);

  if (status < 0)
    set_error(ADFH_ERR_DWRITE, err);
  else
    set_error(NO_ERROR, err);
}

/* ----------------------------------------------------------------- */

void ADFH_Write_All_Data(const double  id,
                         const char   *data,
                         int          *err)
{
  hid_t hid = to_HDF_ID(id);
  hid_t did, tid, mid;

  ADFH_DEBUG("ADFH_Write_All_Data");

  if (data == NULL) {
    set_error(NULL_POINTER, err);
    return;
  }
  if (is_link(hid)) {
    set_error(ADFH_ERR_LINK_DATA, err);
    return;
  }
  if (has_data(hid)) {
    did = H5Dopen(hid, D_DATA);
    tid = H5Dget_type(did);
    mid = H5Tget_native_type(tid, H5T_DIR_ASCEND);

    if (H5Dwrite(did, mid, H5S_ALL, H5S_ALL, H5P_DEFAULT, data) < 0)
      set_error(ADFH_ERR_DWRITE, err);
    else
      set_error(NO_ERROR, err);

    H5Tclose(mid);
    H5Tclose(tid);
    H5Dclose(did);
  }
  else
    set_error(NO_DATA, err);
}

