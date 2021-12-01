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
#include "nkcrclib.h"
#include "nkdbase.h"

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
			num = num * 10 + c - '0';
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
			num = num * 10 + c - '0';
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
					num = num * 10 + c - '0';
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
					num = num * 10 + c - '0';
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
					num = num * 10 + c - '0';
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
					num = num * 10 + c - '0';
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
					num = num * 10 + c - '0';
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
					num = num * 10 + c - '0';
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

const struct type *xpath(char *key, const struct type *type, void **location_loc, uint32_t *triggers)
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

struct ofilt
{
    struct nk_dbase *dbase;
    uint32_t pos;
    uint32_t bank_addr;
    unsigned long crc;
};

// Save buffer to flash, calculate CRC along the way

static int dbase_save_block_write(void *ptr, unsigned char *buffer, size_t len)
{
    int sta;
    size_t x;
    struct ofilt *f = (struct ofilt *)ptr;
    unsigned long crc;
    crc = f->crc;
    if (f->pos >= f->dbase->bank_size)
        return -1;
    for (x = 0; x != len; ++x)
        crc = nk_crc32be_update(crc, buffer[x]);
    f->crc = crc;
    sta = f->dbase->flash_write(f->bank_addr + f->pos, buffer, (len + f->dbase->flash_granularity - 1) & ~(f->dbase->flash_granularity - 1));
    f->pos += len;
    return sta;
}

// Database management

int nk_dbase_save(
    const struct nk_dbase *dbase,
    char *rev,
    void *ram
) {
    int x;
    nkoutfile_t f[1];
    int sta = 0;
    struct ofilt ofilt;
    unsigned long crc;
    int bank = (1 & (*rev + 1));

    nk_printf("Saving to bank %d...\n", bank);

    ofilt.dbase = dbase;
    ofilt.pos = 0;
    ofilt.crc = 0;
    if (bank)
    {
        ofilt.bank_addr = dbase->bank1;
    }
    else
    {
        ofilt.bank_addr = dbase->bank0;
    }

    nk_printf("Erasing...\n");
    sta = dbase->flash_erase(ofilt.bank_addr, dbase->bank_size);
    if (sta) {
        nk_printf("  Erase error\n");
        return -1;
    }

    nkoutfile_open(f, dbase_save_block_write, &ofilt, dbase->bigbuf, dbase->bigbuf_size);

    nk_printf("Writing...\n");

    sta |= nk_dbase_serialize(f, dbase->ty, ram);

    sta |= nk_fputc(f, 0); // End of text marker
    sta |= nk_fputc(f, 1 + *rev); // Revision

    crc = ofilt.crc;
    for (x = 0; x != f->ptr - f->start; ++x)
        crc = nk_crc32be_update(crc, f->start[x]);

    sta |= nk_fputc(f, (crc >> 24));
    sta |= nk_fputc(f, (crc >> 16));
    sta |= nk_fputc(f, (crc >> 8));
    sta |= nk_fputc(f, (crc >> 0));

    if (sta) {
        nk_fprintf(nkstderr, "Database is too large! Not saved.\n");
        return -1;
    }

    nk_printf("  size = %u\n", ofilt.pos + f->ptr - f->start);
    nk_printf("  rev = %d\n", 1 + *rev);

    nk_fflush(f);

    // Only bump rev if we are successful
    ++*rev;
    nk_printf("done.\n");
    return 0;
}

// Read function for nkinfile: we just call the dbase flash read function, but we limit the length to either the bank size
// (for checking CRC), or to the actual data size (for parsing).

struct filt
{
	struct nk_dbase *dbase;
	uint32_t bank_addr;
	size_t size;
};

static size_t dbase_flash_read(void *ptr, unsigned char *buffer, size_t offset)
{
	struct filt *f = (struct filt *)ptr;
	if (offset >= f->size)
	{
		return 0;
	}
	else
	{
		f->dbase->flash_read(f->bank_addr + offset, (unsigned char *)f->dbase->bigbuf, f->dbase->bigbuf_size);
		if (f->size - offset >= f->dbase->bigbuf_size)
			return f->dbase->bigbuf_size;
		else
			return f->size - offset;
	}
}

// Check CRC of database of a particular bank.
// Return 0 if it's bad, or the actual data size if it's good

static int nk_dbase_check(int bank, char *rev, const struct nk_dbase *dbase)
{
    unsigned long crc = 0;
    int c;
    nkinfile_t f[1];
    struct filt filt;
    int sta = 0;
    int x;
    nk_printf("Check bank %d:\n", bank);
    filt.dbase = dbase;
    filt.size = dbase->bank_size;
    if (bank)
    {
    	filt.bank_addr = dbase->bank1;
    }
    else
    {
    	filt.bank_addr = dbase->bank0;
    }

    nkinfile_open(f, dbase_flash_read, &filt, dbase->bigbuf_size, dbase->bigbuf);
    // Fixme: how to handle read errors?
    if (sta) {
        nk_fprintf(nkstderr, "  Read error\n");
        return 0;
    }
    // Find end of text, calculate CRC
    while ((c = nk_fpeek(f)), c != -1 && c)
    {
	crc = nk_crc32be_update(crc, c);
    	nk_fnext_fast(f);
    }

    // NUL
    if (c == -1)  goto badsize;
    crc = nk_crc32be_update(crc, c);
    nk_fnext_fast(f);

    // rev
    c = nk_fpeek(f);
    if (c == -1) goto badsize;
    *rev = c;
    crc = nk_crc32be_update(crc, c);
    nk_fnext_fast(f);

    // CRC bytes
    for (x = 0; x != 4; ++x)
    {
    	c = nk_fpeek(f);
    	if (c == -1) goto badsize;
    	nk_fnext_fast(f);
	crc = nk_crc32be_update(crc, c);
    }

    sta = nk_ftell(f);
    nk_printf("  size = %d\n", sta);
    nk_printf("  rev = %d\n", *rev);
    if (crc)
    {
        nk_fprintf(nkstderr, "  crc is bad\n");
        return 0;
    }

    return sta - 6; // Return size of just the data part

    badsize:
    nk_fprintf(nkstderr, "  size is bad\n");
    return 0;
}

// Choose most recent good version of database and load it

int nk_dbase_load(const struct nk_dbase *dbase, char *rev, void *ram)
{
    nkinfile_t f[1];
    struct filt filt;
    int zero_good = nk_dbase_check(0, rev, dbase);
    char zero_rev = *rev;
    int one_good = nk_dbase_check(1, rev, dbase);
    char one_rev = *rev;
    int use_bank = -1;

    if (one_good && zero_good) {
        if ((signed char)(zero_rev - one_rev) >= 0) // Must be signed comparison equal to rev size!
            use_bank = 0;
        else {
            use_bank = 1;
        }
    } else if (one_good) {
        use_bank = 1;
    } else if (zero_good) {
        use_bank = 0;
    }

    filt.dbase = dbase;
    if (use_bank == 0) {
        nk_printf("Using bank 0\n");
        // Need to re-read it
        filt.bank_addr = dbase->bank0;
        filt.size = zero_good;
    	nkinfile_open(f, dbase_flash_read, &filt, dbase->bigbuf_size, dbase->bigbuf);
    } else if (use_bank == 1) { 
        nk_printf("Using bank 1\n");
        filt.bank_addr = dbase->bank1;
        filt.size = one_good;
    	nkinfile_open(f, dbase_flash_read, &filt, dbase->bigbuf_size, dbase->bigbuf);
    } else {
        nk_fprintf(nkstderr, "Neither bank is good!\n");
        return -1;
    }

    if (nk_fscan(f, " %v", dbase->ty, ram)) {
        nk_printf("Calibration store loaded OK\n");
        return 0;
    } else {
        nk_fprintf(nkstderr, "CRC good, but calibration store failed to parse on load?\n");
        return -1;
    }
}
