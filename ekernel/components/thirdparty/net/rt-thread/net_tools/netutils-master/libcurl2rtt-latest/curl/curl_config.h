/* Define to 1 if you have the timeval struct. */
#define   HAVE_STRUCT_TIMEVAL  1

/* Define to 1 if you have the socket function. */
#define   HAVE_SOCKET  1

/* If you have a fine poll */
//#define   HAVE_POLL_FINE  1

/* Define to 1 if you have the select function. */
#define   HAVE_SELECT  1

#define   BUILDING_LIBCURL  1
/* Define to 1 if you have the recv function. */
#define   HAVE_RECV  1

/* Define to 1 if you have the send function. */
#define   HAVE_SEND  1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define   HAVE_SYS_STAT_H  1

/* Define to 1 if you have the <fcntl.h> header file. */
#define   HAVE_FCNTL_H  1

/* to disable LDAP */
#define   CURL_DISABLE_LDAP  1

/* Define to 1 if you have a working fcntl O_NONBLOCK function. */
#define   HAVE_FCNTL_O_NONBLOCK  1

/* Define to 1 if you have the gethostbyname_r function. */
//#define   HAVE_GETHOSTBYNAME_R  1

/* Define to 1 if the compiler supports the 'long long' data type. */
#define   HAVE_LONGLONG  1

#define   HAVE_GETADDRINFO  1
/* Define to 1 if you have the freeaddrinfo function. */
#define   HAVE_FREEADDRINFO  1

/* Define to 1 if you have the <netdb.h> header file. */
#define   HAVE_NETDB_H  1

#define   USE_LWIPSOCK  1
/* if mbedTLS is enabled  select USE_SSL*/
#define   USE_MBEDTLS  1

/* to disable SMB/CIFS */
#define   CURL_DISABLE_SMB  1

/* The number of bytes in type curl_off_t */
#define   SIZEOF_CURL_OFF_T 8

/* The number of bytes in type size_t */
#define   SIZEOF_SIZE_T 8

/* The number of bytes in type long */
#define   SIZEOF_LONG 8

/* The size of `int', as computed by sizeof. */
#define SIZEOF_INT 4

/* The size of `short', as computed by sizeof. */
#define SIZEOF_SHORT 2

/* Define to the type of args for recv. */
#define   RECV_TYPE_ARG1  int
#define   RECV_TYPE_ARG2  void *
#define   RECV_TYPE_ARG3  size_t
#define   RECV_TYPE_ARG4  int
#define   RECV_TYPE_RETV  ssize_t

/* Define to the type of args for send. */
#define   SEND_TYPE_ARG1  int
#define   SEND_QUAL_ARG2  const
#define   SEND_TYPE_ARG2  void *
#define   SEND_TYPE_ARG3  size_t
#define   SEND_TYPE_ARG4  int
#define   SEND_TYPE_RETV  ssize_t
