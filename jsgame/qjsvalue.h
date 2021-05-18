#include <string>
extern "C" {
#include <quickjs.h>
#include <cutils.h>
}

namespace QJS {
  class Value {
  private:
    const char *cStr = nullptr;
    std::string str;
  public:
    JSContext *ctx;
    JSValueConst val;
    bool noFree = false;

    Value(JSContext *ctx, JSValueConst val, bool noFree = false) : ctx(ctx), val(val), noFree(noFree) {
        cStr = isString() ? JS_ToCString(ctx, val) : nullptr;
    }

    ~Value() {
      if (!noFree) JS_FreeValue(ctx, val);
      if (cStr != nullptr) JS_FreeCString(ctx, cStr);
    }

    operator bool() const {
      return JS_ToBool(ctx, val);
    }

    const Value operator[](std::string str) const {
      JSValueConst ret = JS_GetPropertyStr(ctx, val, str.c_str());
      return Value(ctx, ret, noFree);
    }

    const Value operator[](const char *str) const {
      JSValueConst ret = JS_GetPropertyStr(ctx, val, str);
      return Value(ctx, ret, noFree);
    }

    bool isString() const {
      return JS_IsString(val);
    }

    bool isObject() const {
      return JS_IsObject(val);
    }

    bool isNumber() const {
      return JS_IsNumber(val);
    }

    bool isUndefined() const {
      return JS_IsUndefined(val);
    }

    bool isException() const {
      return JS_IsException(val);
    }

    bool isFunction() const {
      return JS_IsFunction(ctx, val);
    }

    // note: returns string "undefined" if value is undefined
    // use isString or asCString != nullptr to check if value is a string
    const std::string asString() const {
        const char *tempStr = JS_ToCString(ctx, val);
        std::string str = tempStr;
        JS_FreeCString(ctx, tempStr);

        return str;
    }

    // note: returns null if not a string.
    // asString will return the string "undefined" like it does in JS
    const char * asCString() const {
      return cStr;
    }

    int32_t asInt32() const {
      int32_t ret;
      JS_ToInt32(ctx, &ret, val);
      return ret;
    }
  };
}