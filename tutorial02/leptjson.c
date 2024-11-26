#include "leptjson.h"
#include <assert.h>  /* assert() */
#include <stdlib.h>  /* NULL, strtod() */
#include <errno.h> //erron
#include <math.h> //提供HUGE_VAL

#define EXPECT(c, ch)       do { assert(*c->json == (ch)); c->json++; } while(0)

typedef struct {
    const char* json;
}lept_context;

static void lept_parse_whitespace(lept_context* c) {
    const char *p = c->json;
    while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')
        p++;
    c->json = p;
}


static int lept_parse_true(lept_context* c, lept_value* v) {
    EXPECT(c, 't');
    if (c->json[0] != 'r' || c->json[1] != 'u' || c->json[2] != 'e')
        return LEPT_PARSE_INVALID_VALUE;
    c->json += 3;
    v->type = LEPT_TRUE;
    return LEPT_PARSE_OK;
}

static int lept_parse_false(lept_context* c, lept_value* v) {
    EXPECT(c, 'f');
    if (c->json[0] != 'a' || c->json[1] != 'l' || c->json[2] != 's' || c->json[3] != 'e')
        return LEPT_PARSE_INVALID_VALUE;
    c->json += 4;
    v->type = LEPT_FALSE;
    return LEPT_PARSE_OK;
}

static int lept_parse_null(lept_context* c, lept_value* v) {
    // EXPECT 宏或函数用于检查当前解析位置的字符是否与 literal[0] 匹配。如果不匹配，会返回错误码。
    EXPECT(c, 'n');
    if (c->json[0] != 'u' || c->json[1] != 'l' || c->json[2] != 'l')
        return LEPT_PARSE_INVALID_VALUE;
    c->json += 3;
    v->type = LEPT_NULL;
    return LEPT_PARSE_OK;
}
//重构合并 lept_parse_null()、lept_parse_false()、lept_parse_true() 为 lept_parse_literal()
// literal 是null、true、false 中的任意一个。
static int lept_parse_literal(lept_context* c, lept_value* v, const char* literal, lept_type type) {
    size_t i;//非负整数类型，用于循环计数
    EXPECT(c, literal[0]);
    // 检查 c->json 中的字符是否与 literal 中的对应字符匹配。如果不匹配，返回错误码 LEPT_PARSE_INVALID_VALUE。
    for (i = 0; literal[i + 1]; i++) {
        if (c->json[i]!= literal[i + 1])
            return LEPT_PARSE_INVALID_VALUE;
    }
    c->json += i + 1;
    v->type = type;
    return LEPT_PARSE_OK;
}


//json中存放的数字是十进制的字符，需要转为二进制
// c是解析之后的内容
static int lept_parse_number(lept_context* c, lept_value* v) {
    const char* p = c->json;
    v->n = strtod(c->json, NULL);//将json的内容转换为double类型，同时保存end的位置
    v->type = LEPT_NUMBER;
    c->json = p;
    return LEPT_PARSE_OK;
    /*处理各种数字的格式*/
    // number = [ "-" ] int [ frac ] [ exp ]
    // int = "0" / digit1-9 *digit
    // frac = "." 1*digit
    // exp = ("e" / "E") ["-" / "+"] 1*digit   
    // 1.遇到负数
    if(*p == '-') p++;
    // 2.遇到整数
    if(*p == '0') p++;
    else {
        // 判断是否是0-9，直到遇到非数字的情况
        if (!ISDIGIT1TO9(*p)) return LEPT_PARSE_INVALID_VALUE;
        for (p++; ISDIGIT(*p); p++);
    }
    // 3.遇到小数
    if (*p == '.') {
        p++;
        if (!ISDIGIT(*p)) return LEPT_PARSE_INVALID_VALUE;
        for (p++; ISDIGIT(*p); p++);

    }
    // 4.遇到指数
    if (*p == 'e' || *p == 'E') {
        p++;
        if (*p == '+' || *p == '-') p++;
        if (!ISDIGIT(*p)) return LEPT_PARSE_INVALID_VALUE;
        for (p++; ISDIGIT(*p); p++);
    }
    
    #if 0
    if (c->json == end)
        return LEPT_PARSE_INVALID_VALUE;//如果 c->json 和 end 相等，说明没有有效的数字被解析，返回错误码
    errno = 0;
    v->n = strtod(c->json, NULL);
    if(errno == ERANGE && v->n == HUGE_VAL)
        return LEPT_PARSE_NUMBER_TOO_BIG;
    c->json = end;
    v->type = LEPT_NUMBER;
    #endif
}

static int lept_parse_value(lept_context* c, lept_value* v) {
    switch (*c->json) {
        case 't':  return lept_parse_literal(c, v,"true", LEPT_TRUE);
        case 'f':  return lept_parse_literal(c, v,"false", LEPT_FALSE);
        case 'n':  return lept_parse_literal(c, v,"null", LEPT_NULL);
        default:   return lept_parse_number(c, v);
        case '\0': return LEPT_PARSE_EXPECT_VALUE;
    }
}

int lept_parse(lept_value* v, const char* json) {
    lept_context c;
    int ret;
    assert(v != NULL);
    c.json = json;
    v->type = LEPT_NULL;
    lept_parse_whitespace(&c);
    if ((ret = lept_parse_value(&c, v)) == LEPT_PARSE_OK) {
        lept_parse_whitespace(&c);
        if (*c.json != '\0') {
            v->type = LEPT_NULL;
            ret = LEPT_PARSE_ROOT_NOT_SINGULAR;
        }
    }
    return ret;
}

lept_type lept_get_type(const lept_value* v) {
    assert(v != NULL);
    return v->type;
}

double lept_get_number(const lept_value* v) {
    assert(v != NULL && v->type == LEPT_NUMBER);
    return v->n;
}
