/// generates an an optional type for the provided type

/// generates the declarations of the type, and functions,
///
/// `Type` is in CamelCase, used for generating type name, `type` is in
/// snake_case, used for generating associated functions
#define GEN_OPTION_H(Type, type)                                               \
    typedef struct {                                                           \
            Type content;                                                      \
            bool is_valid;                                                     \
    } Type##Option;                                                            \
                                                                               \
    Type##Option type##_option_some(Type value);                               \
    Type##Option type##_option_none(void);

/// generates the implementation of associated functions
#define GEN_OPTION(Type, type)                                                 \
    Type##Option type##_option_some(Type value) {                              \
        return (Type##Option){.content = value, .is_valid = true};             \
    }                                                                          \
    Type##Option type##_option_none(void) {                                    \
        return (Type##Option){.is_valid = false};                              \
    }
