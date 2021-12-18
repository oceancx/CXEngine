/* The MIT License (MIT)
 *
 * Copyright (c) 2016 Stefano Trettel
 *
 * Software repository: MoonGL, https://github.com/stetre/moongl
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef bitfieldDEFINED
#define bitfieldDEFINED

/*--------------------------------------------------------------------------*
 | GLbitfield <-> string mappings                                           |
 *--------------------------------------------------------------------------*/

#define bitfield_t moongl_bitfield_t
typedef struct {
    const char **strings;
    GLbitfield *codes;
    size_t n;
} bitfield_t;

#define bitfieldCheck moongl_bitfieldCheck
GLbitfield bitfieldCheck(lua_State *L, int arg, int mandatory, bitfield_t *b);
#define bitfieldPush moongl_bitfieldPush
int bitfieldPush(lua_State *L, GLbitfield code, bitfield_t *b);

#define BITFIELD_STRINGS(strings)   static const char* (strings) []
#define BITFIELD_CODES(codes)       static GLbitfield (codes) []
#define BITFIELD_T(name, strings, codes)    \
    static bitfield_t name = { (strings), (codes), sizeof((codes))/sizeof(GLbitfield) };

/* whole_framebuffer.c */
#if 0
#define  moongl_
#endif

#define bitfieldBuffer moongl_bitfieldBuffer
bitfield_t *bitfieldBuffer(void);



#if 0 /* 11yy */
BITFIELD_STRINGS(ZzzStrings) = {
    "",
    NULL
};
BITFIELD_CODES(ZzzCodes) = {
    GL_,
};
BITFIELD_T(ZzzBitfield, ZzzStrings, ZzzCodes)
#define CheckZzz(L, arg, mand) bitfieldCheck((L), (arg), (mand), &ZzzBitfield)
#define PushZzz(L, code) bitfieldPush((L), (code), &ZzzBitfield)

bitfield_t *bitfieldZzz(void)
    { return &ZzzBitfield; }

#define ZzzBitfield yyyZzz()
#define CheckZzz(L, arg, mand) bitfieldCheck((L), (arg), (mand), ZzzBitfield)
#define PushZzz(L, code) bitfieldPush((L), (code), ZzzBitfield)

#endif

#endif /* bitfieldDEFINED */
