--[[
/* Copyright (c) Mark J. Kilgard, 1994. */

/**
(c) Copyright 1993, Silicon Graphics, Inc.

ALL RIGHTS RESERVED

Permission to use, copy, modify, and distribute this software
for any purpose and without fee is hereby granted, provided
that the above copyright notice appear in all copies and that
both the copyright notice and this permission notice appear in
supporting documentation, and that the name of Silicon
Graphics, Inc. not be used in advertising or publicity
pertaining to distribution of the software without specific,
written prior permission.

THE MATERIAL EMBODIED ON THIS SOFTWARE IS PROVIDED TO YOU
"AS-IS" AND WITHOUT WARRANTY OF ANY KIND, EXPRESS, IMPLIED OR
OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  IN NO
EVENT SHALL SILICON GRAPHICS, INC.  BE LIABLE TO YOU OR ANYONE
ELSE FOR ANY DIRECT, SPECIAL, INCIDENTAL, INDIRECT OR
CONSEQUENTIAL DAMAGES OF ANY KIND, OR ANY DAMAGES WHATSOEVER,
INCLUDING WITHOUT LIMITATION, LOSS OF PROFIT, LOSS OF USE,
SAVINGS OR REVENUE, OR THE CLAIMS OF THIRD PARTIES, WHETHER OR
NOT SILICON GRAPHICS, INC.  HAS BEEN ADVISED OF THE POSSIBILITY
OF SUCH LOSS, HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
ARISING OUT OF OR IN CONNECTION WITH THE POSSESSION, USE OR
PERFORMANCE OF THIS SOFTWARE.

US Government Users Restricted Rights

Use, duplication, or disclosure by the Government is subject to
restrictions set forth in FAR 52.227.19(c)(2) or subparagraph
(c)(1)(ii) of the Rights in Technical Data and Computer
Software clause at DFARS 252.227-7013 and/or in similar or
successor clauses in the FAR or the DOD or NASA FAR
Supplement.  Unpublished-- rights reserved under the copyright
laws of the United States.  Contractor/manufacturer is Silicon
Graphics, Inc., 2011 N.  Shoreline Blvd., Mountain View, CA
94039-7311.

OpenGL(TM) is a trademark of Silicon Graphics, Inc.
*/
--]]

local glmath = require("moonglmath")
local vec3 = glmath.vec3

-- Rim, body, lid, and bottom data must be reflected in x and y;
-- handle and spout data across the y axis only. 

return {
   -- Patches -----------------------------------------------------------------
   -- Note: patches contain the **0-based** indices of vertices in the data array.
   -- (e.g. the position of first rim vertex is positions[102+1])
   patchdata = {
   -- 1: rim
      {102, 103, 104, 105, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
   -- 2,3: body
      {12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27},
      {24, 25, 26, 27, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40},
   -- 4, 5: lid
      {96, 96, 96, 96, 97, 98, 99, 100, 101, 101, 101, 101, 0, 1, 2, 3,},
      {0, 1, 2, 3, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117},
   -- 6, 7: bottom
      {118, 118, 118, 118, 124, 122, 119, 121, 123, 126, 125, 120, 40, 39, 38, 37},
   -- 8, 9: handle
      {41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56},
      {53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 28, 65, 66, 67},
   -- 10, 11: spout
      {68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83},
      {80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95},
   },
   -- Position data -----------------------------------------------------------
   positions = {
      vec3(0.2, 0, 2.7),
      vec3(0.2, -0.112, 2.7),
      vec3(0.112, -0.2, 2.7),
      vec3(0, -0.2, 2.7),
      vec3(1.3375, 0, 2.53125),
      vec3(1.3375, -0.749, 2.53125),
      vec3(0.749, -1.3375, 2.53125),
      vec3(0, -1.3375, 2.53125),
      vec3(1.4375, 0, 2.53125),
      vec3(1.4375, -0.805, 2.53125),
      vec3(0.805, -1.4375, 2.53125),
      vec3(0, -1.4375, 2.53125),
      vec3(1.5, 0, 2.4),
      vec3(1.5, -0.84, 2.4),
      vec3(0.84, -1.5, 2.4),
      vec3(0, -1.5, 2.4),
      vec3(1.75, 0, 1.875),
      vec3(1.75, -0.98, 1.875),
      vec3(0.98, -1.75, 1.875),
      vec3(0, -1.75, 1.875),
      vec3(2, 0, 1.35),
      vec3(2, -1.12, 1.35),
      vec3(1.12, -2, 1.35),
      vec3(0, -2, 1.35),
      vec3(2, 0, 0.9),
      vec3(2, -1.12, 0.9),
      vec3(1.12, -2, 0.9),
      vec3(0, -2, 0.9),
      vec3(-2, 0, 0.9),
      vec3(2, 0, 0.45),
      vec3(2, -1.12, 0.45),
      vec3(1.12, -2, 0.45),
      vec3(0, -2, 0.45),
      vec3(1.5, 0, 0.225),
      vec3(1.5, -0.84, 0.225),
      vec3(0.84, -1.5, 0.225),
      vec3(0, -1.5, 0.225),
      vec3(1.5, 0, 0.15),
      vec3(1.5, -0.84, 0.15),
      vec3(0.84, -1.5, 0.15),
      vec3(0, -1.5, 0.15),
      vec3(-1.6, 0, 2.025),
      vec3(-1.6, -0.3, 2.025),
      vec3(-1.5, -0.3, 2.25),
      vec3(-1.5, 0, 2.25),
      vec3(-2.3, 0, 2.025),
      vec3(-2.3, -0.3, 2.025),
      vec3(-2.5, -0.3, 2.25),
      vec3(-2.5, 0, 2.25),
      vec3(-2.7, 0, 2.025),
      vec3(-2.7, -0.3, 2.025),
      vec3(-3, -0.3, 2.25),
      vec3(-3, 0, 2.25),
      vec3(-2.7, 0, 1.8),
      vec3(-2.7, -0.3, 1.8),
      vec3(-3, -0.3, 1.8),
      vec3(-3, 0, 1.8),
      vec3(-2.7, 0, 1.575),
      vec3(-2.7, -0.3, 1.575),
      vec3(-3, -0.3, 1.35),
      vec3(-3, 0, 1.35),
      vec3(-2.5, 0, 1.125),
      vec3(-2.5, -0.3, 1.125),
      vec3(-2.65, -0.3, 0.9375),
      vec3(-2.65, 0, 0.9375),
      vec3(-2, -0.3, 0.9),
      vec3(-1.9, -0.3, 0.6),
      vec3(-1.9, 0, 0.6),
      vec3(1.7, 0, 1.425),
      vec3(1.7, -0.66, 1.425),
      vec3(1.7, -0.66, 0.6),
      vec3(1.7, 0, 0.6),
      vec3(2.6, 0, 1.425),
      vec3(2.6, -0.66, 1.425),
      vec3(3.1, -0.66, 0.825),
      vec3(3.1, 0, 0.825),
      vec3(2.3, 0, 2.1),
      vec3(2.3, -0.25, 2.1),
      vec3(2.4, -0.25, 2.025),
      vec3(2.4, 0, 2.025),
      vec3(2.7, 0, 2.4),
      vec3(2.7, -0.25, 2.4),
      vec3(3.3, -0.25, 2.4),
      vec3(3.3, 0, 2.4),
      vec3(2.8, 0, 2.475),
      vec3(2.8, -0.25, 2.475),
      vec3(3.525, -0.25, 2.49375),
      vec3(3.525, 0, 2.49375),
      vec3(2.9, 0, 2.475),
      vec3(2.9, -0.15, 2.475),
      vec3(3.45, -0.15, 2.5125),
      vec3(3.45, 0, 2.5125),
      vec3(2.8, 0, 2.4),
      vec3(2.8, -0.15, 2.4),
      vec3(3.2, -0.15, 2.4),
      vec3(3.2, 0, 2.4),
      vec3(0, 0, 3.15),
      vec3(0.8, 0, 3.15),
      vec3(0.8, -0.45, 3.15),
      vec3(0.45, -0.8, 3.15),
      vec3(0, -0.8, 3.15),
      vec3(0, 0, 2.85),
      vec3(1.4, 0, 2.4),
      vec3(1.4, -0.784, 2.4),
      vec3(0.784, -1.4, 2.4),
      vec3(0, -1.4, 2.4),
      vec3(0.4, 0, 2.55),
      vec3(0.4, -0.224, 2.55),
      vec3(0.224, -0.4, 2.55),
      vec3(0, -0.4, 2.55),
      vec3(1.3, 0, 2.55),
      vec3(1.3, -0.728, 2.55),
      vec3(0.728, -1.3, 2.55),
      vec3(0, -1.3, 2.55),
      vec3(1.3, 0, 2.4),
      vec3(1.3, -0.728, 2.4),
      vec3(0.728, -1.3, 2.4),
      vec3(0, -1.3, 2.4),
      vec3(0, 0, 0),
      vec3(1.425, -0.798, 0),
      vec3(1.5, 0, 0.075),
      vec3(1.425, 0, 0),
      vec3(0.798, -1.425, 0),
      vec3(0, -1.5, 0.075),
      vec3(0, -1.425, 0),
      vec3(1.5, -0.84, 0.075),
      vec3(0.84, -1.5, 0.075),
   },
}


