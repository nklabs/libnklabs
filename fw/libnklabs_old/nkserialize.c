// Simple flash-based schema-defined database

// Schema of database is defined by 'struct type' and can include
// primitives, strings, structures, variable and fixed-length arrays, and
// tables.

// Serialized version of database with CRC and version number is stored in
// flash.  There are two banks, we ping-pong between them whenever the
// database is saved.  On load, we pick the bank with the highest version
// number that has a valid CRC.

// Serialized form of database looks somewhat like JSON, but not compatible.

// On load, extra fields or missing fields do not interfere with the loading
// process, thereby database migration (changes to schema) is possible.

// Note that heap (malloc) is not used.

// Copyright 2020 NK Labs, LLC

// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to permit
// persons to whom the Software is furnished to do so, subject to the
// following conditions:

// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT
// OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
// THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include <stdio.h> // For sprintf
#include <string.h>
#include "nkscan.h"
#include "nkprintf.h"
#include "nkserialize.h"

// Standard primitive types

const struct type tyINT = {
	.what = tINT,
	.size = sizeof(int),
	.members = NULL,
	.subtype = NULL,
	.check = NULL
};

const struct type tyUINT = {
	.what = tUINT,
	.size = sizeof(unsigned int),
	.members = NULL,
	.subtype = NULL,
	.check = NULL
};

const struct type tyINT8 = {
	.what = tINT8,
	.size = sizeof(char),
	.members = NULL,
	.subtype = NULL,
	.check = NULL
};

const struct type tyUINT8 = {
	.what = tUINT8,
	.size = sizeof(unsigned char),
	.members = NULL,
	.subtype = NULL,
	.check = NULL
};

const struct type tyINT16 = {
	.what = tINT16,
	.size = sizeof(short),
	.members = NULL,
	.subtype = NULL,
	.check = NULL
};

const struct type tyUINT16 = {
	.what = tUINT16,
	.size = sizeof(unsigned short),
	.members = NULL,
	.subtype = NULL,
	.check = NULL
};

const struct type tyBOOL = {
	.what = tBOOL,
	.size = sizeof(int),
	.members = NULL,
	.subtype = NULL,
	.check = NULL
};

const struct type tyDOUBLE = {
	.what = tDOUBLE,
	.size = sizeof(double),
	.members = NULL,
	.subtype = NULL,
	.check = NULL
};

const struct type tyFLOAT = {
	.what = tFLOAT,
	.size = sizeof(float),
	.members = NULL,
	.subtype = NULL,
	.check = NULL
};

int nk_dbase_fprint(nkoutfile_t *f, const struct type *type, void *location, int ind, const char *ed)
{
    int status = 0;
    char sbuf[32];
    switch (type->what) {
        case tSTRUCT: {
            const struct member *m;
            const char *o_name = 0;
            const struct type *o_type = 0;
            size_t o_offset = 0;

            status |= nk_fprintf(f, "{\n");

            for (m = type->members; m->name; ++m) {
                if (o_name) {
                    if (o_type->what == tTABLE) {
                    	status |= nk_findent(f, ind + 2);
                    	status |= nk_fprintf(f, "%s:\n", o_name);
                    	status |= nk_findent(f, ind + 4);
                    	status |= nk_dbase_fprint(f, o_type, (char *)location + o_offset, ind + 4, ",\n");
                    } else {
                    	status |= nk_findent(f, ind + 2);
                        status |= nk_fprintf(f, "%s: ", o_name);
                        status |= nk_dbase_fprint(f, o_type, (char *)location + o_offset, ind + 2, ",\n");
                    }
                }
                o_name = m->name;
                o_type = m->type;
                o_offset = m->offset;
            }

            if (o_name) {
                if (o_type->what == tTABLE) {
		    status |= nk_findent(f, ind + 2);
                    status |= nk_fprintf(f, "%s:\n", o_name);
                    status |= nk_findent(f, ind + 4);
                    status |= nk_dbase_fprint(f, o_type, (char *)location + o_offset, ind + 4, "\n");
                } else {
                    status |= nk_findent(f, ind + 2);
                    status |= nk_fprintf(f, "%s: ", o_name);
                    status |= nk_dbase_fprint(f, o_type, (char *)location + o_offset, ind + 2, "\n");
                }
            }

            status |= nk_findent(f, ind);
            status |= nk_fprintf(f, "}%s", ed);
            break;
        } case tTABLE: {
            size_t y;
            const struct type *subtype = type->subtype;
            const struct member *m;
            size_t nrows = ((union len *)location)->len;
            location = (char *)location + sizeof(union len);

            status |= nk_fputc(f, '(');

            for (m = subtype->members; m->name; ++m) {
                status |= nk_fprintf(f, "\t%s", m->name);
            }

            status |= nk_fprintf(f, "\n");

            for (y = 0; y != nrows; ++y) {
            	status |= nk_findent(f, ind);
                status |= nk_fprintf(f, ":");
                for (m = subtype->members; m->name; ++m) {
		    status |= nk_fputc(f, '\t');
                    status |= nk_dbase_fprint(f, m->type, (char *)location + m->offset, 0, "");
                }
                status |= nk_fprintf(f, "\n");
                location = (char *)location + subtype->size;
            }

            status |= nk_findent(f, ind);
            status |= nk_fprintf(f, ")%s", ed);
            break;
        } case tVARRAY: {
            size_t x;
            size_t len = ((union len *)location)->len;
            location = (char *)location + sizeof(union len);
            status |= nk_fprintf(f, "[\n");
            for (x = 0; x != len; ++x) {
            	status |= nk_findent(f, ind + 2);
                if (x + 1 == len)
                    status |= nk_dbase_fprint(f, type->subtype, location, ind + 2, "\n");
                else
                    status |= nk_dbase_fprint(f, type->subtype, location, ind + 2, ",\n");
                location = (char *)location + type->subtype->size;
            }
            status |= nk_findent(f, ind);
            status |= nk_fprintf(f, "]%s", ed);
            break;
        } case tARRAY: {
            size_t x;
            size_t len = (type->size / type->subtype->size);
            status |= nk_fprintf(f, "[\n");
            for (x = 0; x != len; ++x) {
            	status |= nk_findent(f, ind + 2);
                if (x + 1 == len)
                    status |= nk_dbase_fprint(f, type->subtype, location, ind + 2, "\n");
                else
                    status |= nk_dbase_fprint(f, type->subtype, location, ind + 2, ",\n");
                location = (char *)location + type->subtype->size;
            }
            status |= nk_findent(f, ind);
            status |= nk_fprintf(f, "]%s", ed);
            break;
/*        } case tVOID: {
            status |= nk_fprintf(f, "null%s", ed);
            break; */
        } case tDOUBLE: {
            int x;
            // Print double as decimal with enough precision to
            // recover the original double on read in.
            // (should be %.17g, but uses too much space)
            nk_snprintf(sbuf, sizeof(sbuf), "%g", *(double *)location);
            // %g will omit the decimal point if it can.  But we
            // always want it.. so add it back.
            // FIXME: handle nan and inf
            for (x = 0; sbuf[x]; ++x)
                if (sbuf[x] == '.' || sbuf[x] == 'e')
                    break;
            if (!sbuf[x]) {
                sbuf[x++] = '.';
                sbuf[x++] = '0';
                sbuf[x] = 0;
            }
            status |= nk_fprintf(f, "%s%s", sbuf, ed);
            break;
        } case tFLOAT: {
            int x;
            // Print double as decimal with enough precision to
            // recover the original double on read in.
            // (should be %.17g, but uses too much space)
            nk_snprintf(sbuf, sizeof(sbuf), "%g", *(float *)location);
            // %g will omit the decimal point if it can.  But we
            // always want it.. so add it back.
            // FIXME: handle nan and inf
            for (x = 0; sbuf[x]; ++x)
                if (sbuf[x] == '.' || sbuf[x] == 'e')
                    break;
            if (!sbuf[x]) {
                sbuf[x++] = '.';
                sbuf[x++] = '0';
                sbuf[x] = 0;
            }
            status |= nk_fprintf(f, "%s%s", sbuf, ed);
            break;
        } case tINT: {
            status |= nk_fprintf(f, "%d%s", *(int *)location, ed);
            break;
        } case tUINT: {
            status |= nk_fprintf(f, "%u%s", *(unsigned int *)location, ed);
            break;
        } case tINT8: {
            status |= nk_fprintf(f, "%d%s", *(char *)location, ed);
            break;
        } case tUINT8: {
            status |= nk_fprintf(f, "%u%s", *(unsigned char *)location, ed);
            break;
        } case tINT16: {
            status |= nk_fprintf(f, "%d%s", *(short *)location, ed);
            break;
        } case tUINT16: {
            status |= nk_fprintf(f, "%u%s", *(unsigned short *)location, ed);
            break;
        } case tBOOL: {
            if (*(int *)location)
                status |= nk_fprintf(f, "true%s", ed);
            else
                status |= nk_fprintf(f, "false%s", ed);
            break;
        } case tSTRING: {
            char *s = (char *)location;
            int x;
            status |= nk_fputc(f, '"');
            for (x = 0; s[x]; ++x) {
                if (s[x] == '\"')
                    status |= nk_fprintf(f, "\\\"");
                else if (s[x] == '\\')
                    status |= nk_fprintf(f, "\\\\");
                else if (s[x] == '\n')
                    status |= nk_fprintf(f, "\\n");
                else if (s[x] == '\r')
                    status |= nk_fprintf(f, "\\r");
                else if (s[x] < 32 || s[x] > 126)
                    status |= nk_fprintf(f, "\\x%2.2x", (0xFF & s[x]));
                else
                    status |= nk_fputc(f, s[x]);
            }
            status |= nk_fprintf(f, "\"%s", ed);
            break;
        } default: {
            status |= nk_fprintf(f, "unknown%s", ed);
            break;
        }
    }
    return status;
}

// Serialize a database with a given schema

int nk_dbase_serialize(nkoutfile_t *f, const struct type *type, void *location)
{
	int status = 0;
	switch (type->what) {
		case tSTRUCT: {
			const struct member *m;
			status |= nk_fputc(f, '{');
			m = type->members;
			while (m->name) {
				status |= nk_fprintf(f, "%s:", m->name);
				status |= nk_dbase_serialize(f, m->type, (char *)location + m->offset);
				++m;
				if (m->name) {
					status |= nk_fprintf(f, ", ");
				}
			}
			status |= nk_fputc(f, '}');
			break;
		} case tVARRAY: {
			status |= nk_fputc(f, '[');
			size_t count = ((union len *)location)->len;
			location = (char *)location + sizeof(union len);
			while (count--) {
				status |= nk_dbase_serialize(f, type->subtype, location);
				location = (char *)location + type->subtype->size;
				if (count) {
					status |= nk_fprintf(f, ", ");
				}
			}
			status |= nk_fputc(f, ']');
			break;
		} case tARRAY: {
			status |= nk_fputc(f, '[');
			size_t count = (type->size / type->subtype->size);
			while (count--) {
				status |= nk_dbase_serialize(f, type->subtype, location);
				location = (char *)location + type->subtype->size;
				if (count) {
					status |= nk_fprintf(f, ", ");
				}
			}
			status |= nk_fputc(f, ']');
			break;
		} case tTABLE: {
			const struct member *m;
			status |= nk_fputc(f, '(');
			size_t count = ((union len *)location)->len;
			location = (char *)location + sizeof(union len);
			m = type->subtype->members;
			while (m->name) {
				status |= nk_fprintf(f, "%s ", m->name);
				++m;
			}
			while (count--) {
				status |= nk_fputc(f, ':');
				m = type->subtype->members;
				while (m->name) {
					status |= nk_dbase_serialize(f, m->type, (char *)location + m->offset);
					++m;
					if (m->name)
						status |= nk_fputc(f, ' ');
				}
				location = (char *)location + type->subtype->size;
			}
			status |= nk_fputc(f, ')');
			break;
		} case tBOOL: {
			if (*(int *)location)
				status |= nk_fprintf(f, "true");
			else
				status |= nk_fprintf(f, "false");
			break;
		} case tINT: {
			status |= nk_fprintf(f, "%d", *(int *)location);
			break;
		} case tUINT: {
			status |= nk_fprintf(f, "%u", *(unsigned *)location);
			break;
		} case tINT8: {
			status |= nk_fprintf(f, "%d", *(char *)location);
			break;
		} case tUINT8: {
			status |= nk_fprintf(f, "%u", *(unsigned char *)location);
			break;
		} case tINT16: {
			status |= nk_fprintf(f, "%d", *(short *)location);
			break;
		} case tUINT16: {
			status |= nk_fprintf(f, "%u", *(unsigned short *)location);
			break;
		} case tDOUBLE: {
			status |= nk_fprintf(f, "%g", *(double *)location);
			break;
		} case tFLOAT: {
			status |= nk_fprintf(f, "%g", *(float *)location);
			break;
		} case tSTRING: {
			char *s = (char *)location;
			status |= nk_fputc(f, '"');
			while (*s) {
				if (*s == '"') {
					status |= nk_fprintf(f, "\\\"");
				} else if (*s == '\\') {
					status |= nk_fprintf(f, "\\\\");
				} else if (*s == '\n') {
					status |= nk_fprintf(f, "\\n");
				} else if (*s == '\r') {
					status |= nk_fprintf(f, "\\r");
				} else if (*s < 32 || *s > 126) {
					status |= nk_fprintf(f, "\\x%2.2x", *(unsigned char *)s);
				} else {
					status |= nk_fputc(f, *s);
				}
				++s;
			}
			status |= nk_fputc(f, '"');
		}
	}
	return status;
}

// Skip over a value without saving it

static char keyval_buf[NKDBASE_MAXIDENTLEN];

static int skip_value(nkinfile_t *f)
{
	int sta = 0;
	size_t orgpos = nk_ftell(f);
	if (nk_fscan(f, "true %e")) {
		sta = 1;
	} else if (nk_fscan(f, "false %e")) {
		sta = 1;
	} else if (nk_fscan(f, "null %e")) {
		sta = 1;
	} else if (nk_fpeek(f) == '{') {
		nk_fnext_fast(f);
		while (nk_fscan(f, " %i : %e", keyval_buf, sizeof(keyval_buf))) {
			int rtn;
			rtn = skip_value(f);
			if (!rtn) {
				goto hash_fail;
			}
			while (nk_fscan(f, " ,%e"));
		}
		if (nk_fpeek(f) == '}') {
			nk_fnext_fast(f);
			sta = 1;
		} else {
			hash_fail:;
		}
	} else if (nk_fpeek(f) == '[') {
		nk_fnext_fast(f);
		while (nk_fscan_ws(f) && !nk_feof(f) && nk_fpeek_fast(f) != ']') {
			if (nk_fpeek(f) ==',')
				nk_fnext_fast(f);
			else {
				int rtn;
				rtn = skip_value(f);
				if (!rtn) {
					goto array_fail;
				}
			}
		}
		if (nk_fpeek(f) == ']') {
			nk_fnext(f);
			sta = 1;
		} else {
			array_fail:;
		}
	} else if (nk_fpeek(f) == '(') {
		nk_fnext(f);

		/* Column headers */
		while (nk_fscan(f, " %i %e", keyval_buf, sizeof(keyval_buf))) {
		}

		/* Rows */
		/* Load data */
		while (nk_fscan(f, " :%e" )) {
			while (nk_fscan_ws(f) && !nk_feof(f) && nk_fpeek_fast(f) != ':' && nk_fpeek_fast(f) != ')') {
				if (nk_fscan_ws(f) && skip_value(f)) {
				} else {
					goto table_fail;
				}
			}
		}

		/* Eat end delimiter */
		if (nk_fscan(f, " )%e")) {
			sta = 1;
		} else {
			table_fail:;
		}
	} else if (nk_fpeek(f) == '"') {
		int c = nk_fnext_fast(f);
		while (c != -1 && c != '"') {
			nk_fscan_escape(f);
			c = nk_fpeek(f);
		}
		if (c == '"') {
			sta = 1;
		}
	} else if (nk_fpeek(f) == '0' && nk_fpeek_rel(f, 1) == 'x') {
		uint64_t val = 0;
		nk_fseek_rel(f, 2);
		sta = nk_fscan_hex(f, &val, -1);
	} else if (nk_fpeek(f) == '-' && nk_fpeek_rel(f, 1) == '0' && nk_fpeek_rel(f, 2) == 'x') {
		uint64_t val = 0;
		nk_fseek_rel(f, 3);
		sta = nk_fscan_hex(f, &val, -1);
	} else if (nk_fpeek(f) >= '0' && nk_fpeek(f) <= '9') {
		uint64_t num;
		int c = nk_fpeek(f);
		size_t org = nk_ftell(f);
		num = 0;
		while (c >= '0' && c <= '9') {
			num = num * 10 + (unsigned)(c - '0');
			c = nk_fnext_fast(f);
		}
		if (c == '.' || c == 'e' || c == 'E') {
			nk_fseek(f, org);
			goto DOUBLE;
		}
		sta = 1;
	} else if (nk_fpeek(f) == '-' && nk_fpeek_rel(f, 1) >= '0' && nk_fpeek_rel(f, 1) <= '9') {
		uint64_t num;
		int c = nk_fnext_fast(f);
		size_t org = nk_ftell(f);
		num = 0;
		while (c >= '0' && c <= '9') {
			num = num * 10 + (unsigned)(c - '0');
			c = nk_fnext_fast(f);
		}
		if (c == '.' || c == 'e' || c == 'E') {
			nk_fseek(f, org);
			goto NEGDOUBLE;
		}
		sta = 1;
	} else if (nk_fpeek(f) == '-' && nk_fpeek_rel(f, 1) == '.') {
		double val;
		nk_fnext_fast(f);
		NEGDOUBLE:
		val = 0.0;
		sta = nk_fscan_double(f, &val);
	} else if (nk_fpeek(f) == '.') {
		double val;
		DOUBLE:
		val = 0.0;
		sta = nk_fscan_double(f, &val);
	}
	if (!sta) {
		nk_fseek(f, orgpos);
#if 0
		nk_fprintf(nkstderr, "Something wrong here: %s\n", *ptr);
#endif
	}
	return sta;
}

// Parse any kind of value

int nk_fscan_keyval(nkinfile_t *f, const struct type *type, size_t location)
{
	size_t org_loc = location;
	size_t orgpos = nk_ftell(f);
	int sta = 0;
	int rtn;
	int c = nk_fpeek(f);

	switch (type->what) {
		case tBOOL: {
			if (nk_fscan(f, "true %e")) {
				*(int *)location = 1;
				sta = 1;
			} else if (nk_fscan(f, "false %e")) {
				*(int *)location = 0;
				sta = 1;
			} else {
				nk_fprintf(nkstderr, "Error: invalid Boolean value\n");
			}
			break;
		} case tINT: case tUINT: case tINT8: case tUINT8: case tINT16: case tUINT16: {
			if (c == '0' && nk_fpeek_rel(f, 1) == 'x') {
				uint64_t val = 0;
				nk_fseek_rel(f, 2);
				sta = nk_fscan_hex(f, &val, -1);
				switch (type->what) {
					case tUINT:
					case tINT: *(int *)location = (int)val; break;
					case tUINT8:
					case tINT8: *(char *)location = (char)val; break;
					case tUINT16:
					case tINT16: *(short *)location = (short)val; break;
				}
			} else if (c == '-' && nk_fpeek_rel(f, 1) == '0' && nk_fpeek_rel(f, 2) == 'x') {
				uint64_t val = 0;
				nk_fseek_rel(f, 3);
				sta = nk_fscan_hex(f, &val, -1);
				switch (type->what) {
					case tUINT:
					case tINT: *(int *)location = (int)-(int64_t)val; break;
					case tUINT8:
					case tINT8: *(char *)location = (char)-(int64_t)val; break;
					case tUINT16:
					case tINT16: *(short *)location = (short)-(int64_t)val; break;
				}
			} else if (c >= '0' && c <= '9') {
				uint64_t num;
				size_t org = nk_ftell(f);
				num = 0;
				while (c >= '0' && c <= '9') {
					num = num * 10 + (unsigned)(c - '0');
					c = nk_fnext_fast(f);
				}
				if (c == '.' || c == 'e' || c == 'E') {
					nk_fseek(f, org);
					goto DOUBLE;
				}
				switch (type->what) {
					case tUINT:
					case tINT: *(int *)location = (int)num; break;
					case tUINT8:
					case tINT8: *(char *)location = (char)num; break;
					case tUINT16:
					case tINT16: *(short *)location = (short)num; break;
				}
				sta = 1;
			} else if (c == '-' && nk_fpeek_rel(f, 1) >= '0' && nk_fpeek_rel(f, 1) <= '9') {
				uint64_t num;
				size_t org;
				c = nk_fnext_fast(f);
				org = nk_ftell(f);
				num = 0;
				while (c >= '0' && c<= '9') {
					num = num * 10 + (unsigned)(c - '0');
					c = nk_fnext_fast(f);
				}
				if (c == '.' || c == 'e' || c == 'E') {
					nk_fseek(f, org);
					goto NEGDOUBLE;
				}
				switch (type->what) {
					case tUINT:
					case tINT: *(int *)location = (int)-(int64_t)num; break;
					case tUINT8:
					case tINT8: *(char *)location = (char)-(int64_t)num; break;
					case tUINT16:
					case tINT16: *(short *)location = (short)-(int64_t)num; break;
				}
				sta = 1;
			} else if (c == '-' && nk_fpeek_rel(f, 1) == '.') {
				double val;
				nk_fnext_fast(f);
				NEGDOUBLE:
				val = 0.0;
				sta = nk_fscan_double(f, &val);
				switch (type->what) {
					case tUINT:
					case tINT: *(int *)location = (int)-val; break;
					case tUINT8:
					case tINT8: *(char *)location = (char)-val; break;
					case tUINT16:
					case tINT16: *(short *)location = (short)-val; break;
				}
			} else if (c == '.') {
				double val;
				DOUBLE:
				val = 0.0;
				sta = nk_fscan_double(f, &val);
				switch (type->what) {
					case tUINT:
					case tINT: *(int *)location = (int)val; break;
					case tUINT8:
					case tINT8: *(char *)location = (char)val; break;
					case tUINT16:
					case tINT16: *(short *)location = (short)val; break;
				}
			}
			break;
		} case tDOUBLE: {
			if (c == '0' && nk_fpeek_rel(f, 1) == 'x') {
				uint64_t val = 0;
				nk_fseek_rel(f, 2);
				sta = nk_fscan_hex(f, &val, -1);
				*(double *)location = (double)val;
			} else if (c == '-' && nk_fpeek_rel(f, 1) == '0' && nk_fpeek_rel(f, 2) == 'x') {
				uint64_t val = 0;
				nk_fseek_rel(f, 3);
				sta = nk_fscan_hex(f, &val, -1);
				*(double *)location = (double)-(int64_t)val;
			} else if (c >= '0' && c <= '9') {
				uint64_t num;
				size_t org = nk_ftell(f);
				num = 0;
				while (c >= '0' && c <= '9') {
					num = num * 10 + (unsigned)(c - '0');
					c = nk_fnext_fast(f);
				}
				if (c == '.' || c == 'e' || c == 'E') {
					nk_fseek(f, org);
					goto DOUBLE1;
				}
				*(double *)location = (double)num;
				sta = 1;
			} else if (c == '-' && nk_fpeek_rel(f, 1) >= '0' && nk_fpeek_rel(f, 1) <= '9') {
				uint64_t num;
				size_t org;
				c = nk_fnext_fast(f);
				org = nk_ftell(f);
				num = 0;
				while (c >= '0' && c  <= '9') {
					num = num * 10 + (unsigned)(c - '0');
					c = nk_fnext_fast(f);
				}
				if (c == '.' || c == 'e' || c == 'E') {
					nk_fseek(f, org);
					goto NEGDOUBLE1;
				}
				*(double *)location = (double)-(int64_t)num;
				sta = 1;
			} else if (c == '-' && nk_fseek_rel(f, 1) == '.') {
				double val;
				nk_fnext_fast(f);
				NEGDOUBLE1:
				val = 0.0;
				sta = nk_fscan_double(f, &val);
				*(double *)location = (double)-val;
			} else if (c == '.') {
				double val;
				DOUBLE1:
				val = 0.0;
				sta = nk_fscan_double(f, &val);
				*(double *)location = (double)val;
			}
			break;
		} case tFLOAT: {
			if (c == '0' && nk_fpeek_rel(f, 1) == 'x') {
				uint64_t val = 0;
				nk_fseek_rel(f, 2);
				sta = nk_fscan_hex(f, &val, -1);
				*(float *)location = (float)val;
			} else if (c == '-' && nk_fpeek_rel(f, 1) == '0' && nk_fpeek_rel(f, 2) == 'x') {
				uint64_t val = 0;
				nk_fseek_rel(f, 3);
				sta = nk_fscan_hex(f, &val, -1);
				*(float *)location = (float)-(int64_t)val;
			} else if (c >= '0' && c <= '9') {
				uint64_t num;
				size_t org = nk_ftell(f);
				num = 0;
				while (c >= '0' && c <= '9') {
					num = num * 10 + (unsigned)(c - '0');
					c = nk_fnext_fast(f);
				}
				if (c == '.' || c == 'e' || c == 'E') {
					nk_fseek(f, org);
					goto DOUBLE2;
				}
				*(float *)location = (float)num;
				sta = 1;
			} else if (c == '-' && nk_fpeek_rel(f, 1) >= '0' && nk_fpeek_rel(f, 1) <= '9') {
				uint64_t num;
				size_t org;
				c = nk_fnext_fast(f);
				org = nk_ftell(f);
				num = 0;
				while (c >= '0' && c <= '0') {
					num = num * 10 + (unsigned)(c - '0');
					c = nk_fnext_fast(f);
				}
				if (c == '.' || c == 'e' || c == 'E') {
					nk_fseek(f, org);
					goto NEGDOUBLE2;
				}
				*(float *)location = (float)-(int64_t)num;
				sta = 1;
			} else if (c == '-' && nk_fpeek_rel(f, 1) == '.') {
				double val;
				nk_fnext_fast(f);
				NEGDOUBLE2:
				val = 0.0;
				sta = nk_fscan_double(f, &val);
				*(float *)location = (float)-val;
			} else if (c == '.') {
				double val;
				DOUBLE2:
				val = 0.0;
				sta = nk_fscan_double(f, &val);
				*(float *)location = (float)val;
			}
			break;
		} case tSTRING: {
			if (c == '"') {
				c  = nk_fnext_fast(f);
				int toolong = 0;
				size_t size = type->size - 1; // Space for NUL
				size_t len = 0;
				char *s = (char *)location;
				while (c != -1 && c != '"') {
					int ch = nk_fscan_escape(f);
					if (ch == -1) {
						break;
					} else if (len != size) {
						s[len++] = (char)ch;
					} else {
						toolong = 1;
					}
					c = nk_fpeek(f);
				}
				if (c == '"') {
					nk_fnext_fast(f);
					sta = 1;
				}
				s[len] = 0;
				if (toolong) {
					nk_fprintf(nkstderr, "Warning: string was truncated\n");
				}
			}
			break;
		} case tSTRUCT: {
			if (c == '{') {
				rtn = 1;
				nk_fnext_fast(f);
				while (nk_fscan_ws(f) && nk_fscan(f, "%i : %e", keyval_buf, sizeof(keyval_buf))) {
					// Find member
					const struct member *m;
					for (m = type->members; m->name; ++m)
						if (!strcmp(m->name, keyval_buf))
							break;
					if (m->name) { // We found the member
						rtn = nk_fscan_keyval(f, m->type, location + m->offset);
						if (!rtn) {
							// Maybe type is wrong? Try to parse over it..
							rtn = skip_value(f);
							if (!rtn)
								break;
						}
					} else {
						// Ignore unknown member
						skip_value(f);
						nk_fprintf(nkstderr, "Warning: ignoring unknown structure member %s\n", keyval_buf);
					}
					while (nk_fscan(f, " ,%e"));
				}
				if (nk_fpeek(f) == '}') {
					nk_fnext_fast(f);
					sta = rtn;
				}
			}
			break;
		} case tVARRAY: {
			if (c == '[') {
				size_t idx = 0;
				int toolong = 0;
				size_t size = type->size; // Maximum size of array in bytes
				nk_fnext_fast(f);
				rtn = 1;
				while (nk_fscan_ws(f) && nk_fpeek(f) != -1 && nk_fpeek(f) != ']') {
					if (nk_fpeek(f) ==',') {
						nk_fnext_fast(f);
					} else if (idx < size) {
						rtn = nk_fscan_keyval(f, type->subtype, location + sizeof(union len) + idx);
						if (!rtn)
							break;
						idx += type->subtype->size;
					} else {
						// Ignore extra item
						rtn = skip_value(f);
						++toolong;
						if (!rtn)
							break;
					}
				}
				if (nk_fpeek(f) == ']') {
					nk_fnext_fast(f);
					sta = rtn;
				}
				((union len *)location)->len = idx / type->subtype->size; // Save number of elements in array
				if (toolong)
					nk_fprintf(nkstderr, "Warning: array exceeded allocated size (ignored %d items)\n", toolong);
			}
			break;
		} case tARRAY: {
			if (c == '[') {
				size_t idx = 0;
				int toolong = 0;
				size_t size = type->size; // Size of array in bytes
				nk_fnext_fast(f);
				rtn = 1;
				while (nk_fscan_ws(f) && nk_fpeek(f) != -1 && nk_fpeek(f) != ']') {
					if (nk_fpeek(f) ==',') {
						nk_fnext_fast(f);
					} else if (idx < size) {
						rtn = nk_fscan_keyval(f, type->subtype, location + idx);
						if (!rtn)
							break;
						idx += type->subtype->size;
					} else {
						// Ignore extra item
						rtn = skip_value(f);
						++toolong;
						if (!rtn)
							break;
					}
				}
				if (nk_fpeek(f) == ']') {
					nk_fnext_fast(f);
					sta = rtn;
				}
				if (toolong)
					nk_fprintf(nkstderr, "Warning: array exceeded allocated size (ignored %d items)\n", toolong);
				if (idx < size)
					nk_fprintf(nkstderr, "Warning: not enough items supplied to array\n");
			}
			break;
		} case tTABLE: {
			if (c == '(') {
				int map[NKDBASE_MAXCOLS];
				int x;
				int col;
				int maxcol;
				size_t maxrows;
				size_t *nrows = &((union len *)location)->len;
				size_t row;
				int toolong = 0;
				location += sizeof(union len);

				rtn = 1;
				
				nk_fnext_fast(f);

				// How many rows can we save?
				maxrows = type->size / type->subtype->size;

				// Clear map
				for (x = 0; x != NKDBASE_MAXCOLS; ++x)
					map[x] = -1;

				// Create map
				col = 0;
				while (nk_fscan(f, " %i %e", keyval_buf, sizeof(keyval_buf))) {
					for (x = 0; type->subtype->members[x].name; ++x)
						if (!strcmp(type->subtype->members[x].name, keyval_buf))
							break;
					if (type->subtype->members[x].name) {
						map[col] = x;
					} else {
						nk_fprintf(nkstderr, "Warning: ignoring unknown column %s\n", keyval_buf);
					}
					++col;
				}

				maxcol = col;

				// Parse rows
				row = 0;
				while (nk_fscan(f, " : %e")) {
					for (col = 0; col != maxcol; ++col) {
						x = map[col];
						if (row == maxrows || x == -1) {
							// Skip this field
							rtn = skip_value(f);
							if (!rtn)
								break;
						} else {
							nk_fscan_ws(f);
							rtn = nk_fscan_keyval(f, type->subtype->members[x].type, location + type->subtype->members[x].offset);
							if (!rtn) {
								break;
							}
						}
					}
					location += type->subtype->size;
					if (row != maxrows)
						++row;
					else
						++toolong;
				}
				*nrows = row; // Save row count

				if (nk_fscan(f, " )%e")) {
					sta = rtn;
				}
				if (toolong) {
					nk_fprintf(nkstderr, "Warning: ignoring %d extra rows in table\n", toolong);
				}
			}
			break;
		} default: {
			// Unknown type? This is an internal error..
			nk_fprintf(nkstderr, "Internal error: unknown type\n");
			break;
		}
	}

	if (!sta || (type->check && !type->check(org_loc))) {
		sta = 0;
		nk_fprintf(nkstderr, "--Something wrong here: ");
		nk_fcopy(nkstderr, f);
		nk_fprintf(nkstderr, "\n");
		nk_fseek(f, orgpos);
	}

	return sta;
}

// Xpath traversal
// It would be nice if this worked for tables...

const struct type *nk_xpath(char *key, const struct type *type, void **location_loc, uint32_t *triggers)
{
    char buf[NKDBASE_MAXIDENTLEN];
    nkinfile_t f;
    void *location = *location_loc;
    size_t idx;
    nkinfile_open_string(&f, key);

    while (!nk_feof(&f)) {
        if (nk_fscan(&f, "[ %u ]%e", &idx)) {
            size_t count;
            if (type->what == tVARRAY || type->what == tTABLE) {
                count = ((union len *)location)->len;
                location = (char *)location + sizeof(union len);
                if (idx >= count) {
                    nk_fprintf(nkstderr, "Array index out of bounds in key %s\n", key);
                    return 0;
                }
                location = (char *)location + idx * type->subtype->size;
                type = type->subtype;
            } else if (type->what == tARRAY) {
                count = (type->size / type->subtype->size);
                if (idx >= count) {
                    nk_fprintf(nkstderr, "Array index out of bounds in key %s\n", key);
                    return 0;
                }
                location = (char *)location + idx * type->subtype->size;
                type = type->subtype;
            } else {
                nk_fprintf(nkstderr, "Attempt to index a non-array in key %s\n", key);
                return 0;
            }
            if (nk_fpeek(&f) == '.')
                nk_fnext_fast(&f);
            else if (nk_fpeek(&f) == '[')
                ;
            else
                break;
        } else if (nk_fscan(&f, "%i%e", buf, sizeof(buf))) {
            const struct member *m;
            if (type->what != tSTRUCT) {
                nk_fprintf(nkstderr, "Attempt to index a non-struct in key %s\n", key);
                return 0;
            }
            for (m = type->members; m->name; ++m)
                if (!strcmp(m->name, buf))
                    break;
            if (!m->name) {
                nk_fprintf(nkstderr, "Item does not exist %s\n", buf);
                return 0;
            }
            type = m->type;
            location = (char *)location + m->offset;
            *triggers |= m->triggers;
            if (nk_fpeek(&f) == '.')
                nk_fnext_fast(&f);
            else if (nk_fpeek(&f) == '[')
                ;
            else
                break;
        } else {
            nk_fprintf(nkstderr, "Syntax error in key %s\n", key);
            return 0;
        }
    }
    if (!nk_feof(&f)) {
        nk_fprintf(nkstderr, "Syntax error in key %s\n", key);
        return 0;
    }
    *location_loc = location;
    return type;
}
