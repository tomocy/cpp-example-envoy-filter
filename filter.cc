#include "proxy_wasm_intrinsics.h"

class ExampleRootContext : public RootContext {
 public:
  explicit ExampleRootContext(uint32_t id, StringView root_id)
      : RootContext(id, root_id) {}

  bool onStart(size_t) override;
};

class ExampleContext : public Context {
 public:
  explicit ExampleContext(uint32_t id, RootContext* root) : Context(id, root) {}

  FilterHeadersStatus onResponseHeaders(uint32_t) override;

  FilterStatus onDownstreamData(size_t, bool) override;
};

static RegisterContextFactory register_FilterContext(
    CONTEXT_FACTORY(ExampleContext), ROOT_FACTORY(ExampleRootContext),
    "example_root");

bool ExampleRootContext::onStart(size_t) {
  LOG_DEBUG("ready to process stream");

  return true;
}

FilterHeadersStatus ExampleContext::onResponseHeaders(uint32_t) {
  addResponseHeader("x-message", "added by example filter");

  return FilterHeadersStatus::Continue;
}

FilterStatus ExampleContext::onDownstreamData(size_t, bool) {
  auto resp = setBuffer(WasmBufferType::NetworkDownstreamData, 0, 0,
                        "prepended to downstream data by example filter");
  if (resp != WasmResult::Ok) {
    LOG_ERROR("failed to modify downstream data: " + toString(resp));
    return FilterStatus::StopIteration;
  }

  return FilterStatus::Continue;
}