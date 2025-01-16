// Hand-written Test database metadata

const struct type tyTESTSTRING = {
    .what = tSTRING,
    .size = nk_member_size(struct testfwd, tstring),
    .members = NULL,
    .subtype = NULL,
    .check = NULL
};

const struct member testfwd_members[] = {
    { "tbool", &tyBOOL, offsetof(struct testfwd, tbool) },
    { "tint", &tyINT, offsetof(struct testfwd, tint) },
    { "tuint", &tyUINT, offsetof(struct testfwd, tuint) },
    { "tint8", &tyINT8, offsetof(struct testfwd, tint8) },
    { "tuint8", &tyUINT8, offsetof(struct testfwd, tuint8) },
    { "tint16", &tyINT16, offsetof(struct testfwd, tint16) },
    { "tuint16", &tyUINT16, offsetof(struct testfwd, tuint16) },
    { "tdouble", &tyDOUBLE, offsetof(struct testfwd, tdouble) },
    { "tfloat", &tyFLOAT, offsetof(struct testfwd, tfloat) },
    { "tstring", &tyTESTSTRING, offsetof(struct testfwd, tstring) },
    { NULL, NULL, 0 }
};

const struct type tyTESTFWD = {
    .what = tSTRUCT,
    .size = sizeof(struct testfwd),
    .members = testfwd_members,
    .subtype = NULL,
    .check = NULL
};

const struct member testbkwd_members[] = {
    { "tstring", &tyTESTSTRING, offsetof(struct testbkwd, tstring) },
    { "tfloat", &tyFLOAT, offsetof(struct testbkwd, tfloat) },
    { "tdouble", &tyDOUBLE, offsetof(struct testbkwd, tdouble) },
    { "tuint16", &tyUINT16, offsetof(struct testbkwd, tuint16) },
    { "tint16", &tyINT16, offsetof(struct testbkwd, tint16) },
    { "tuint8", &tyUINT8, offsetof(struct testbkwd, tuint8) },
    { "tint8", &tyINT8, offsetof(struct testbkwd, tint8) },
    { "tuint", &tyUINT, offsetof(struct testbkwd, tuint) },
    { "tint", &tyINT, offsetof(struct testbkwd, tint) },
    { "tbool", &tyBOOL, offsetof(struct testbkwd, tbool) },
    { NULL, NULL, 0 }
};

const struct type tyTESTBKWD = {
    .what = tSTRUCT,
    .size = sizeof(struct testbkwd),
    .members = testbkwd_members,
    .subtype = NULL,
    .check = NULL
};

const struct type tyTESTARRAY = {
    .what = tARRAY,
    .size = nk_member_size(struct testtop, tarray),
    .members = NULL,
    .subtype = &tyTESTBKWD,
    .check = NULL
};

const struct type tyTESTVARARRAY = {
    .what = tVARRAY,
    .size = nk_member_size(struct testtop, tvararray),
    .members = NULL,
    .subtype = &tyTESTFWD,
    .check = NULL
};

const struct type tyTESTTABLE = {
    .what = tTABLE,
    .size = nk_member_size(struct testtop, ttable),
    .members = NULL,
    .subtype = &tyTESTBKWD,
    .check = NULL
};

const struct member testtop_members[] = {
    { "tstruct", &tyTESTFWD, offsetof(struct testtop, tstruct) },
    { "tarray", &tyTESTARRAY, offsetof(struct testtop, tarray) },
    { "tvararray", &tyTESTVARARRAY, offsetof(struct testtop, tvararray_len) },
    { "ttable", &tyTESTTABLE, offsetof(struct testtop, ttable_len) },
    { NULL, NULL, 0 }
};

const struct type ty_testtop = {
    .what = tSTRUCT,
    .size = sizeof(struct testtop),
    .members = testtop_members,
    .subtype = NULL,
    .check = NULL
};
