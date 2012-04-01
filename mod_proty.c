#include <httpd.h>
#include <http_config.h>
#include <http_log.h>
#include <proty/runtime/runtime.h>
#include <proty/compiler/compiler.h>
#include <proty/vm/module.h>
#include <proty/vm/eval.h>

#define VERSION "0.1"

module AP_MODULE_DECLARE_DATA proty_module;

static int proty_handler(request_rec* r) {
  if (!r->handler || strcasecmp(r->handler, "proty") != 0) {
    return DECLINED;
  }

  if (r->method_number != M_GET) {
    return HTTP_METHOD_NOT_ALLOWED;
  }

  Module* module = Module_new();
  Context* context = Compiler_newContext(module);

  FILE* f = fopen(r->canonical_filename, "r");
  int id = Compiler_compileFile(context, f);
  fclose(f);

  Compiler_deleteContext(context);

  State* state = State_new(module);
  State_setGlobalState(state);
  Object* res = eval(state, id);
  State_delete(state);

  Module_delete(module);

  ap_set_content_type(r, "text/html");
  ap_rputs(res->data.ptr , r);

  return OK;
}

static int proty_init(apr_pool_t *p, apr_pool_t *ptemp,
                      apr_pool_t *plog, server_rec *s) {

  const char* version = "mod_proty/" VERSION;
  ap_add_version_component(p, version);

  runtime_init();

  return OK;
}

static void proty_hooks(apr_pool_t* p) {
  ap_hook_handler(proty_handler, NULL, NULL, APR_HOOK_MIDDLE);
  ap_hook_post_config(proty_init, NULL, NULL, APR_HOOK_MIDDLE);
}

static const command_rec proty_cmds[] = {
    {NULL}
};

module proty_module = {
  STANDARD20_MODULE_STUFF,
  NULL,
  NULL,
  NULL,
  NULL,
  proty_cmds,
  proty_hooks,
};
