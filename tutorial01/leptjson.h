// 由于 leptjson 只有一个头文件，可以简单命名为 LEPTJSON_H__。
// 如果项目有多个文件或目录结构，可以用 项目名称_目录_文件名称_H__ 这种命名方式
#ifndef LEPTJSON_H__
#define LEPTJSON_H__//预编译指令，未编译就编译，防止头文件被多个源文件重复编译
//JSON中的六种数据类型（true和false是布尔值Boolean）,别名叫做lept_type
typedef enum { LEPT_NULL, LEPT_FALSE, LEPT_TRUE, LEPT_NUMBER, LEPT_STRING, LEPT_ARRAY, LEPT_OBJECT } lept_type;

typedef struct {
    lept_type type;
}lept_value;

// 根据JSON语法子集，配备的相关错误返回符：whitespace value whitespace
enum {
    LEPT_PARSE_OK = 0,
    LEPT_PARSE_EXPECT_VALUE,//传入只有whitespace时，返回该错误码
    LEPT_PARSE_INVALID_VALUE,//在ws value ws中，val不是合法的JSON格式，返回该错误码
    LEPT_PARSE_ROOT_NOT_SINGULAR//在ws value ws中，ws后面还有字符，返回该错误码
};
// 解析JSON的函数:输入的字符串是不能修改的，使用const
// lept_value v;
// const char json[]=...;存放解析JSON的结果
//int ret = lept_parse(&v, json);
int lept_parse(lept_value* v, const char* json);

// 生成JSON的函数
lept_type lept_get_type(const lept_value* v);

#endif /* LEPTJSON_H__ *///预编译指令，未编译就编译，防止头文件被多个源文件重复编译
