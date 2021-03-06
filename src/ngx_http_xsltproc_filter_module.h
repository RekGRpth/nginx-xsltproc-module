#ifndef _NGX_HTTP_XSLTPROC_FILTER_MODULE_H_INCLUDED_
#define _NGX_HTTP_XSLTPROC_FILTER_MODULE_H_INCLUDED_

#ifndef NGX_HTTP_XSLTPROC_REUSE_DTD
#define NGX_HTTP_XSLTPROC_REUSE_DTD  1
#endif

#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>

#include "ngx_http_xsltproc_core.h"
#include "ngx_http_xsltproc_xslt.h"

typedef struct {
    u_char              *name;
    void                *data;
} ngx_http_xsltproc_file_t;


typedef struct {
    ngx_array_t          dtd_files;    /* ngx_http_xsltproc_file_t */
    ngx_array_t          sheet_cache;  /* ngx_http_xsltproc_file_t */
    ngx_pool_t          *pool;
} ngx_http_xsltproc_filter_main_conf_t;


typedef struct {
    xsltStylesheetPtr    stylesheet;
    ngx_http_xsltproc_xslt_stylesheet_t *xslt_stylesheet;
    ngx_array_t          params;       /* ngx_http_complex_value_t */
} ngx_http_xsltproc_sheet_t;


typedef struct {
    xmlDocPtr            doc;
    xmlParserCtxtPtr     ctxt;
    ngx_http_request_t  *request;
    ngx_array_t         *sheets;       /* ngx_http_xsltproc_sheet_t */
#if (NGX_HAVE_LIBMEMCACHED)
    memcached_st        *memcached;
    ngx_str_t            memcached_key;
    ngx_str_t            memcached_key_prefix;
    ngx_str_t            memcached_key_real;
    ngx_uint_t           memcached_key_auto;
    time_t               memcached_expire;
    ngx_uint_t           memcached_done;
#endif
#if (NGX_HTTP_XSLTPROC_PROFILER)
    ngx_http_xsltproc_profiler_t *profiler;
#endif
    ngx_uint_t           done;         /* unsigned  done:1; */
} ngx_http_xsltproc_filter_ctx_t;


static ngx_int_t ngx_http_xsltproc_parse_stylesheet(ngx_http_request_t *r,
    u_char *name, ngx_http_xsltproc_xslt_stylesheet_t **xslt_stylesheet);
static ngx_int_t ngx_http_xsltproc_parse_params(ngx_http_request_t *r, ngx_array_t *params);
#if (NGX_HAVE_LIBMEMCACHED)
static ngx_int_t ngx_http_xsltproc_parse_header(ngx_http_request_t *r, ngx_str_t *root,
    ngx_array_t *sheets, ngx_str_t *memcached_key);
#else
static ngx_int_t ngx_http_xsltproc_parse_header(ngx_http_request_t *r, ngx_str_t *root,
    ngx_array_t *sheets);
#endif
static ngx_int_t ngx_http_xsltproc_header_filter(ngx_http_request_t *r);

static ngx_int_t ngx_http_xsltproc_body_filter(ngx_http_request_t *r, ngx_chain_t *in);
static ngx_int_t ngx_http_xsltproc_send(ngx_http_request_t *r, ngx_http_xsltproc_filter_ctx_t *ctx,
    ngx_buf_t *b);
static ngx_int_t ngx_http_xsltproc_add_chunk(ngx_http_request_t *r, ngx_http_xsltproc_filter_ctx_t *ctx,
    ngx_buf_t *b);
static void ngx_http_xsltproc_sax_external_subset(void *data, const xmlChar *name,
    const xmlChar *externalId, const xmlChar *systemId);
static void ngx_cdecl ngx_http_xsltproc_sax_error(void *data, const char *msg, ...);
static ngx_buf_t * ngx_http_xsltproc_apply_stylesheet(ngx_http_request_t *r,
    ngx_http_xsltproc_filter_ctx_t *ctx, int *doc_type);
static ngx_int_t ngx_http_xsltproc_apply_encoding(ngx_http_request_t *r,
    ngx_http_xsltproc_xslt_stylesheet_t *xslt_stylesheet, int doc_type);

static u_char * ngx_http_xsltproc_content_type(xsltStylesheetPtr s);

static u_char * ngx_http_xsltproc_encoding(xsltStylesheetPtr s);

static void ngx_http_xsltproc_cleanup(void *data);

static char * ngx_http_xsltproc_entities(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);

static void ngx_http_xsltproc_cleanup_dtd(void *data);

#if (NGX_HTTP_XSLTPROC_PROFILER)
static char * ngx_http_xsltproc_profiler_stylesheet(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
static void ngx_http_xsltproc_cleanup_profiler_stylesheet(void *data);
#endif

#if (NGX_HAVE_LIBMEMCACHED)
static char *
ngx_http_xsltproc_memcached_server(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
static ngx_buf_t *
ngx_http_xsltproc_memcached_get(ngx_http_request_t *r, ngx_chain_t *in,
                                ngx_http_xsltproc_filter_ctx_t *ctx);
static void
ngx_http_xsltproc_memcached_set(ngx_http_request_t *r, ngx_buf_t *b, int doc_type,
                                ngx_http_xsltproc_filter_ctx_t *ctx);
#endif

static void * ngx_http_xsltproc_filter_create_main_conf(ngx_conf_t *cf);
static void * ngx_http_xsltproc_filter_create_conf(ngx_conf_t *cf);
static char * ngx_http_xsltproc_filter_merge_conf(ngx_conf_t *cf, void *parent, void *child);
static ngx_int_t ngx_http_xsltproc_filter_init(ngx_conf_t *cf);
static ngx_int_t ngx_http_xsltproc_filter_init_process(ngx_cycle_t *cycle);
static void ngx_http_xsltproc_filter_exit(ngx_cycle_t *cycle);

static u_char ngx_http_xsltproc_empty_xpath_expression[] = "''\0";

ngx_str_t  ngx_http_xsltproc_default_types[] = {
    ngx_string("text/xml"),
    ngx_null_string
};


static ngx_command_t  ngx_http_xsltproc_filter_commands[] = {

    { ngx_string("xsltproc"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_HTTP_LIF_CONF
                        |NGX_CONF_FLAG,
      ngx_conf_set_flag_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_xsltproc_filter_loc_conf_t, enable),
      NULL },

    { ngx_string("xsltproc_stylesheet_caching"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_HTTP_LIF_CONF
                        |NGX_CONF_FLAG,
      ngx_conf_set_flag_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_xsltproc_filter_loc_conf_t, stylesheet_caching),
      NULL },

    { ngx_string("xsltproc_stylesheet_check_if_modify"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_HTTP_LIF_CONF
                        |NGX_CONF_FLAG,
      ngx_conf_set_flag_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_xsltproc_filter_loc_conf_t, stylesheet_check_if_modify),
      NULL },

    { ngx_string("xsltproc_stylesheet_root"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_HTTP_LIF_CONF
                        |NGX_CONF_TAKE1,
      ngx_conf_set_str_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_xsltproc_filter_loc_conf_t, stylesheet_root),
      NULL },

    { ngx_string("xml_entities"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_HTTP_LIF_CONF
                        |NGX_CONF_TAKE1,
      ngx_http_xsltproc_entities,
      NGX_HTTP_LOC_CONF_OFFSET,
      0,
      NULL },

#if (NGX_HTTP_XSLTPROC_PROFILER)
    { ngx_string("xsltproc_profiler"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_HTTP_LIF_CONF
                        |NGX_CONF_FLAG,
      ngx_conf_set_flag_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_xsltproc_filter_loc_conf_t, profiler),
      NULL },

    { ngx_string("xsltproc_profiler_stylesheet"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_HTTP_LIF_CONF
                        |NGX_CONF_TAKE1,
      ngx_http_xsltproc_profiler_stylesheet,
      NGX_HTTP_LOC_CONF_OFFSET,
      0,
      NULL },

    { ngx_string("xsltproc_profiler_repeat"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_HTTP_LIF_CONF
                        |NGX_CONF_FLAG,
      ngx_conf_set_flag_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_xsltproc_filter_loc_conf_t, profiler_repeat),
      NULL },

#endif

#if (NGX_HAVE_LIBMEMCACHED)
    { ngx_string("xsltproc_memcached"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_HTTP_LIF_CONF
                        |NGX_CONF_FLAG,
      ngx_conf_set_flag_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_xsltproc_filter_loc_conf_t, memcached_enable),
      NULL },

    { ngx_string("xsltproc_memcached_server"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_HTTP_LIF_CONF
                        |NGX_CONF_TAKE1,
      ngx_http_xsltproc_memcached_server,
      NGX_HTTP_LOC_CONF_OFFSET,
      0,
      NULL },

    { ngx_string("xsltproc_memcached_key_prefix"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_HTTP_LIF_CONF
                        |NGX_CONF_TAKE1,
      ngx_conf_set_str_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_xsltproc_filter_loc_conf_t, memcached_key_prefix),
      NULL },

    { ngx_string("xsltproc_memcached_key_auto"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_HTTP_LIF_CONF
                        |NGX_CONF_FLAG,
      ngx_conf_set_flag_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_xsltproc_filter_loc_conf_t, memcached_key_auto),
      NULL },

    { ngx_string("xsltproc_memcached_expire"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_HTTP_LIF_CONF
                        |NGX_CONF_TAKE1,
      ngx_conf_set_sec_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_xsltproc_filter_loc_conf_t, memcached_expire),
      NULL },
#endif

    { ngx_string("xsltproc_types"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_HTTP_LIF_CONF
                        |NGX_CONF_1MORE,
      ngx_http_types_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_xsltproc_filter_loc_conf_t, types_keys),
      &ngx_http_xsltproc_default_types[0] },

      ngx_null_command
};


static ngx_http_module_t  ngx_http_xsltproc_filter_module_ctx = {
    NULL,                                  /* preconfiguration */
    ngx_http_xsltproc_filter_init,         /* postconfiguration */

    ngx_http_xsltproc_filter_create_main_conf, /* create main configuration */
    NULL,                                  /* init main configuration */

    NULL,                                  /* create server configuration */
    NULL,                                  /* merge server configuration */

    ngx_http_xsltproc_filter_create_conf,  /* create location configuration */
    ngx_http_xsltproc_filter_merge_conf    /* merge location configuration */
};


ngx_module_t ngx_http_xsltproc_filter_module = {
    NGX_MODULE_V1,
    &ngx_http_xsltproc_filter_module_ctx,  /* module context */
    ngx_http_xsltproc_filter_commands,     /* module directives */
    NGX_HTTP_MODULE,                       /* module type */
    NULL,                                  /* init master */
    NULL,                                  /* init module */
    ngx_http_xsltproc_filter_init_process, /* init process */
    NULL,                                  /* init thread */
    NULL,                                  /* exit thread */
    ngx_http_xsltproc_filter_exit,         /* exit process */
    NULL,                                  /* exit master */
    NGX_MODULE_V1_PADDING
};

#endif
