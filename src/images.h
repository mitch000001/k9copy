/**************************************************************************
*   Copyright (C) 2005 by Jean-Michel Petit                               *
*   jm_petit@laposte.net                                                  *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, write to the                         *
*   Free Software Foundation, Inc.,                                       *
*   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
***************************************************************************/

static const char* const img_text[] = {
"30 30 2 1",
". c None",
"# c #eee608",
"..............................",
"..............................",
"..............................",
"..............................",
"..............................",
"..............................",
"..............................",
"..............................",
"..............................",
"..............................",
"..............................",
"..............................",
"..............................",
"..............................",
"...........##.................",
"...........##.................",
"...........##.................",
"....####...##.###....####.....",
"...#...##..###.###..##..##....",
"...##..##..##...##..##..#.....",
"......###..##...##.###........",
"...##..##..##...##.###........",
"...##..##..##...##..##........",
"..###.###..###.##...###.......",
"...###.##............####.....",
"..............................",
"..............................",
"..............................",
"..............................",
".............................."};

static const char* const img_sound[] = {
"30 29 230 2",
".a c None",
".# c None",
"Qt c None",
"aA c #101820",
"aM c #181820",
"aL c #181829",
"#c c #181c29",
".V c #182029",
".K c #182031",
".6 c #182429",
"#q c #182431",
"#C c #202031",
"bq c #202420",
"#d c #202429",
".W c #202431",
"#N c #202831",
".7 c #202839",
".M c #202c41",
".8 c #20304a",
"bf c #292831",
".L c #292839",
"bg c #292c29",
"br c #292c31",
".X c #292c41",
"bt c #293029",
".Z c #293041",
"#k c #29304a",
".w c #29344a",
".Y c #293452",
"#1 c #29384a",
"#l c #293852",
"#m c #293c5a",
"bs c #313031",
"#A c #313431",
".U c #313441",
"#0 c #31344a",
".x c #31384a",
"#e c #31385a",
".J c #313c4a",
"#r c #313c52",
"#n c #31405a",
"#u c #314062",
"#v c #31446a",
"bh c #393439",
"a5 c #393839",
".3 c #393c31",
".5 c #393c4a",
"az c #393c52",
"aa c #39405a",
"#Z c #394452",
".N c #39445a",
"#S c #394462",
"#w c #394862",
"af c #39486a",
"bi c #413c41",
".l c #414039",
"aV c #41444a",
".Q c #414841",
"be c #414852",
"#p c #414c41",
"#G c #414c6a",
".E c #415041",
"ak c #41506a",
"#T c #415073",
"bj c #4a444a",
".j c #4a4841",
".f c #4a4c41",
"#h c #4a554a",
"#H c #4a597b",
"aW c #52505a",
"#b c #525062",
".t c #52554a",
".e c #52594a",
"bp c #525962",
".v c #525973",
"aq c #525d52",
"ao c #525d73",
"#5 c #525d7b",
"#W c #526152",
"al c #526183",
"#s c #526583",
"#6 c #52698b",
"#R c #526994",
"a6 c #5a555a",
"bu c #5a595a",
"a7 c #5a5d5a",
"aO c #5a5d7b",
"ap c #5a615a",
"aX c #5a6162",
"aN c #5a616a",
".y c #5a6173",
"as c #5a617b",
"#O c #5a657b",
"aD c #5a6583",
"ai c #5a6952",
"an c #5a698b",
"ac c #5a6994",
"#U c #5a6d94",
"#4 c #5a719c",
"bk c #625d62",
"bz c #626162",
".i c #62655a",
".F c #626562",
"#M c #626573",
".d c #62695a",
"#. c #626962",
"ar c #626983",
".g c #626d62",
"ba c #626d83",
"at c #626d8b",
".2 c #62715a",
".G c #627162",
"#7 c #62718b",
"#t c #627194",
"#D c #627594",
"#F c #62799c",
"by c #6a656a",
"a. c #6a696a",
".h c #6a6d62",
"aB c #6a717b",
"a2 c #6a7194",
"#z c #6a756a",
"aj c #6a758b",
"ay c #6a799c",
"ab c #6a7d9c",
"ae c #6a7da4",
"#Q c #6a81a4",
"bA c #736d73",
"#X c #73716a",
"#j c #737583",
".s c #737973",
"#V c #737994",
"#o c #737d94",
"aK c #737d9c",
"aE c #73819c",
"#E c #7389ac",
".S c #7b7973",
"bx c #7b797b",
".R c #7b7d73",
"bl c #7b7d7b",
"#f c #7b7d94",
"aC c #7b7d9c",
"#9 c #7b8173",
"a# c #7b8194",
".1 c #7b8573",
"ah c #7b857b",
"aU c #7b859c",
"#8 c #7b897b",
"#2 c #7b89a4",
"a8 c #838183",
".k c #83857b",
"bd c #838594",
".9 c #83899c",
"#x c #8389a4",
"au c #8389ac",
"#I c #838da4",
"ad c #838dac",
"#3 c #8395b4",
".H c #8b8583",
"#B c #8b899c",
".0 c #8b8d9c",
".T c #8b919c",
".I c #8b95a4",
".D c #949194",
"#i c #949594",
"bc c #9495a4",
"bb c #9495ac",
".P c #9499a4",
".u c #9499ac",
"aF c #9499b4",
".B c #949d94",
"aw c #949db4",
"am c #94a1bd",
"## c #9c959c",
"ag c #9c9994",
"bo c #9c999c",
".c c #9ca19c",
"a3 c #9ca1b4",
"#P c #9ca5c5",
"av c #9caabd",
"a9 c #a49da4",
"#L c #a4a194",
"#J c #a4a1a4",
"#Y c #a4a1ac",
"#g c #a4a59c",
".4 c #a4a5b4",
".n c #a4a5bd",
".C c #a4aaa4",
".A c #a4aabd",
"bF c #aca5ac",
"#a c #aca5b4",
".r c #acaaa4",
".b c #acaaac",
"aY c #acaab4",
"bG c #acaeac",
".o c #acaebd",
"aP c #acaec5",
"aG c #acb6c5",
"bB c #b4aeb4",
"bm c #b4b2b4",
"bH c #b4b6b4",
".O c #b4b6c5",
".m c #b4bac5",
"ax c #b4bad5",
"#K c #bdb6b4",
"bw c #bdb6bd",
"bE c #bdbabd",
"a1 c #bdbac5",
".p c #bdbacd",
".z c #bdbecd",
"aJ c #bdc2d5",
"bI c #c5bec5",
"#y c #c5c2c5",
"aQ c #c5c2d5",
".q c #c5c6cd",
"a4 c #c5c6d5",
"aT c #c5c6de",
"aZ c #cdcacd",
"aS c #cdd2de",
"aH c #cdd2e6",
"bD c #d5ced5",
"aR c #d5cede",
"bC c #d5d2d5",
"b# c #d5d2de",
"aI c #d5d6e6",
"bn c #ded6de",
"bv c #dedade",
"a0 c #e6dee6",
"b. c #e6e2e6",
"bJ c #eee2ee",
"Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#",
".aQt.aQt.aQt.aQt.aQt.aQt.aQt.aQt.aQt.aQt.aQt.aQt.aQt.aQt.aQt",
"Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#.b.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#",
".aQt.aQt.aQt.aQt.c.d.e.f.fQt.aQt.aQt.aQt.aQt.aQt.aQt.aQt.aQt",
"Qt.#Qt.#Qt.#Qt.#Qt.g.h.i.j.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#",
".aQt.aQt.aQt.aQt.a.k.aQt.lQt.aQt.aQt.m.n.o.p.qQt.aQt.aQt.aQt",
"Qt.#Qt.#Qt.#Qt.#.r.sQt.#.t.#Qt.#Qt.u.v.w.x.y.z.AQt.#Qt.#Qt.#",
".aQt.aQt.B.C.a.D.E.F.a.r.G.H.aQt.I.J.K.L.M.M.N.O.PQt.aQt.aQt",
"Qt.#Qt.#.s.Q.R.c.S.#Qt.k.F.#Qt.T.U.V.W.W.X.Y.Z.0.u.#Qt.#Qt.#",
".aQt.aQt.1.2.3.f.aQt.aQt.aQt.4.5.V.W.6.W.7.Y.8.y.9Qt.aQt.aQt",
"Qt.#Qt.#Qt.t#.##Qt.#Qt.#Qt#a#b#c.W#d.W.W.M.Y#e.N#f.#Qt.#Qt.#",
".aQt.aQt#g#h#iQt.aQt.aQt.a#j.V.K.V.W#d#k#l.Y#m#n#oQt.aQt.aQt",
"Qt.#Qt.#.c#pQt.#Qt.#Qt.#Qt.x#q.V.W#d#r#s#t#u#v#w#x.#Qt.#Qt.#",
".aQt.a#y#z#A.aQt.aQt.aQt#B#C.V.W.6.U#D#E#F#G#u#H#IQt.aQt.aQt",
"Qt.#Qt.##J.#Qt.##K#LQt.##M#q.7#N#N#O#P#Q#R#S#T#U#V.#Qt.#Qt.#",
".aQt.aQt.aQt.aQt#W#X.a#Y#Z#0#1#0#0#2#3#4#T#5#6#U#G#7.aQt.aQt",
"Qt.#Qt.#Qt.#Qt#8#9a.Qta##Saa#Saa#Gabac#Saaadae.Naf.vQt.#Qt.#",
".aQt.aQtagagahai.aQt.aaj#S#w#wakal#5#l.w#Vaman#N#lao.aQt.aQt",
"Qt.#Qt.#.DapaqahQt.#Qtarak#Tasatauavaw.AaxayazaA.ZaBQt.#Qt.#",
".aQt.aQt.aQt.aQt.aQt.aaCakaDaEaFaGaHaIaJaK#waLaMaNQt.aQt.aQt",
"Qt.#Qt.#Qt.#Qt.#Qt.#Qt.uaOaDaPaQaRaSaTaU.5aVaWaXaYaZa0.#Qt.#",
".aQt.aQt.aQt.aQt.aQt.aa1aja2a3a4a4aJaj.Ua5a6a7a.a8a9#yb..aQt",
"Qt.#Qt.#Qt.#Qt.#Qt.#Qtb#.9babbbcbdbebfbgbh#AbibjbkblbmbnQt.#",
".aQt.aQt.aQt.aQt.aQt.abnbobp.Ubfbqbrbsbsbt#Aa5bjbua8bmbv.aQt",
"Qt.#Qt.#Qt.#Qt.#Qt.#Qtbvbw.Dbxa.bybza7a6a7bzbAbx.DbBbC.#Qt.#",
".aQt.aQt.aQt.aQt.aQt.aQtbvbD#ybEbmbBbF.bbGbmbHbIaZbv.aQt.aQt",
"Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#bJ.#Qt.#Qt.#Qt.#Qt.#Qt.#",
".aQt.aQt.aQt.aQt.aQt.aQt.aQt.aQt.aQt.aQt.aQt.aQt.aQt.aQt.aQt",
"Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#"};


static const unsigned char img_preview[] = {
    0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d,
    0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x00, 0x00, 0x20,
    0x08, 0x06, 0x00, 0x00, 0x00, 0x05, 0x40, 0xc8, 0x7f, 0x00, 0x00, 0x00,
    0xd2, 0x49, 0x44, 0x41, 0x54, 0x48, 0x89, 0xe5, 0x96, 0x4b, 0x0e, 0xc3,
    0x20, 0x0c, 0x44, 0x4b, 0xd5, 0x53, 0x71, 0x7f, 0xf9, 0x5a, 0xcd, 0x0a,
    0xc9, 0x22, 0xb6, 0x33, 0xe3, 0x40, 0x1d, 0xa9, 0xb3, 0x49, 0x84, 0x2c,
    0x9e, 0x7f, 0x18, 0x9a, 0x88, 0xbc, 0x2a, 0xf4, 0x2e, 0xa1, 0x56, 0x82,
    0x3f, 0x88, 0x51, 0xef, 0xfd, 0x3b, 0xfe, 0x45, 0xa4, 0x65, 0x6d, 0xb4,
    0x5a, 0x54, 0x63, 0xbd, 0xd9, 0xac, 0xb1, 0x39, 0x62, 0x43, 0x81, 0xa3,
    0x0d, 0x19, 0x79, 0xf0, 0xff, 0x6b, 0xae, 0xe7, 0x80, 0x47, 0x6d, 0xaf,
    0x3a, 0x53, 0x44, 0x1a, 0x62, 0x03, 0x83, 0x11, 0xb8, 0x5e, 0x47, 0x6c,
    0x2c, 0xb9, 0xe7, 0x38, 0xea, 0xea, 0x15, 0x1d, 0xbf, 0xad, 0xc6, 0x96,
    0x73, 0x7a, 0xcd, 0x04, 0x23, 0xf5, 0xcb, 0x3a, 0x32, 0xbe, 0xd0, 0xc8,
    0xbc, 0x0b, 0xb3, 0x74, 0x8a, 0x78, 0x75, 0xa4, 0x30, 0xf8, 0x57, 0xda,
    0x9a, 0xea, 0x39, 0x7b, 0x3a, 0xf5, 0x5b, 0xc1, 0x91, 0x23, 0x65, 0xa9,
    0x0e, 0xef, 0x63, 0x4f, 0xec, 0xa5, 0x6f, 0x89, 0x8e, 0x78, 0x3e, 0x22,
    0xd9, 0x29, 0x46, 0x81, 0xbd, 0x48, 0x33, 0x70, 0x18, 0x6c, 0x41, 0xef,
    0xc0, 0x21, 0x70, 0x54, 0xd3, 0x2c, 0xfc, 0x12, 0x8c, 0x34, 0x52, 0x06,
    0x1e, 0x82, 0x99, 0xee, 0x65, 0xe1, 0x2e, 0x38, 0x73, 0x64, 0x18, 0x78,
    0xf8, 0x02, 0x61, 0xa0, 0x2c, 0xdc, 0x7d, 0x73, 0x65, 0xa0, 0x0c, 0x3c,
    0x35, 0xb9, 0x56, 0xa8, 0x6c, 0x56, 0x1f, 0x9a, 0x62, 0x93, 0xe0, 0x9f,
    0x36, 0x40, 0x7c, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4e, 0x44, 0xae,
    0x42, 0x60, 0x82
};


static const char* const img_video[] = {
"30 30 205 2",
".a c None",
".# c None",
"Qt c None",
".j c #292452",
".q c #292852",
".i c #39305a",
".p c #393c6a",
".r c #413c5a",
"#1 c #413c62",
".k c #414462",
"#0 c #4a4483",
"#Z c #524ca4",
"#O c #52506a",
".D c #5250a4",
".1 c #5250ac",
".d c #5a557b",
"## c #5a55ac",
".0 c #5a599c",
"al c #5a59a4",
"#a c #5a59ac",
"#M c #5a5db4",
"#R c #5a8de6",
".G c #625973",
"#Y c #6259ac",
".u c #625d7b",
"#N c #625d9c",
"#k c #625dac",
".e c #62619c",
".Y c #6261ac",
"#X c #6265b4",
"#T c #6295ee",
".Z c #6a61b4",
"#j c #6a659c",
".O c #6a65a4",
".2 c #6a65b4",
".T c #6a6d94",
"#E c #6a8dd5",
".s c #736973",
"a# c #736d8b",
".E c #736da4",
".C c #736db4",
"#l c #736dbd",
".H c #73719c",
"#P c #7371b4",
"a9 c #7371bd",
".P c #7375b4",
"#5 c #738dcd",
".l c #7b717b",
".t c #7b757b",
"ak c #7b759c",
".c c #7b75a4",
".f c #7b75ac",
"b# c #7b75b4",
".X c #7b75bd",
".N c #7b79b4",
"#b c #7b79bd",
"#i c #7b7da4",
"a1 c #7b7db4",
"b. c #7b7dbd",
"#6 c #7bbaff",
"ax c #8379bd",
"a3 c #837d8b",
"#2 c #837d94",
"a8 c #837dc5",
"#. c #8381bd",
"aZ c #8381c5",
"am c #8385bd",
"aO c #8b81c5",
"a. c #8b85a4",
"aU c #8b85bd",
"aN c #8b85c5",
".v c #8b89b4",
"#B c #8b89bd",
".A c #8b89c5",
".B c #8b89cd",
"#u c #8b8dac",
"a4 c #8b8db4",
"#C c #8b8dc5",
".m c #948da4",
".6 c #948dc5",
".o c #948dcd",
"aS c #94918b",
"aT c #94919c",
"aG c #9491bd",
".7 c #9491c5",
"#A c #9491cd",
"#v c #9495bd",
"bj c #9495cd",
"#H c #94a5d5",
"#8 c #94c2f6",
"aR c #9c919c",
"aE c #9c91c5",
".S c #9c95ac",
".F c #9c95b4",
"aD c #9c95bd",
".z c #9c95cd",
".M c #9c95d5",
"aK c #9c99ac",
"#e c #9c99cd",
"a0 c #9c99d5",
"bg c #9c9dc5",
"a5 c #9c9dcd",
"a2 c #a499a4",
"aw c #a49db4",
"#W c #a49dc5",
".h c #a49dcd",
".y c #a49dd5",
"ay c #a4a1cd",
"bf c #a4a1d5",
"aB c #a4a5cd",
"aQ c #a4a5d5",
"aC c #aca5c5",
"aL c #aca5cd",
"aF c #aca5d5",
"#z c #aca5de",
"aJ c #acaaac",
"aa c #acaab4",
"ba c #acaacd",
".L c #acaad5",
"af c #acaeb4",
"#s c #acbede",
"ag c #acd2f6",
"#V c #b4aebd",
"#h c #b4aecd",
"aV c #b4aed5",
"aY c #b4aede",
"at c #b4b2b4",
"be c #b4b2d5",
"aM c #b4b2de",
"bd c #b4b2e6",
"aI c #b4b6b4",
"aj c #b4b6bd",
"bi c #b4b6d5",
"#q c #b4bac5",
"av c #b4bed5",
"#r c #b4caf6",
"#7 c #b4daff",
".3 c #bdb2de",
"aH c #bdb6bd",
"as c #bdb6c5",
"ar c #bdb6d5",
"#L c #bdb6de",
"aP c #bdb6e6",
"ae c #bdbad5",
"aA c #bdbae6",
"au c #bdcede",
"#J c #c5bec5",
"#9 c #c5becd",
"#o c #c5bede",
".Q c #c5bee6",
".9 c #c5c2e6",
"aX c #c5c2ee",
"bb c #c5c6d5",
".b c #c5c6e6",
"#g c #cdc2de",
"#4 c #cdc6cd",
"#f c #cdcad5",
".U c #cdced5",
"ai c #cde6f6",
"#G c #cde6ff",
".K c #d5caee",
"az c #d5cecd",
"#U c #d5ced5",
"#y c #d5cee6",
"#m c #d5ceee",
"aq c #d5d2d5",
"aW c #d5d2de",
"#t c #d5d2e6",
".g c #d5d2ee",
".I c #d5d2f6",
"bh c #d5d6de",
"#Q c #ded2d5",
"a6 c #ded2de",
".w c #ded2ee",
".W c #ded2f6",
"#D c #ded6de",
".5 c #ded6ee",
".n c #ded6f6",
"a7 c #dedade",
".8 c #dedae6",
"#K c #dedaf6",
"#S c #def6ff",
".x c #e6d6f6",
"#I c #e6dae6",
"#d c #e6daf6",
"ab c #e6dee6",
"#w c #e6def6",
"#3 c #e6deff",
"bk c #e6e2e6",
"bc c #e6e2ee",
"#c c #e6e2f6",
".V c #eee2ee",
"#p c #eee6ee",
".J c #eee6f6",
"ad c #eee6ff",
"#n c #eeeaff",
"an c #eeeeee",
".R c #eeeef6",
"#x c #f6eaff",
".4 c #f6eeff",
"ap c #f6f2f6",
"ah c #f6ffff",
"ao c #fff6ff",
"ac c #fffaff",
"#F c #ffffff",
"Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#",
".aQt.aQt.aQt.aQt.aQt.aQt.aQt.aQt.aQt.aQt.aQt.aQt.aQt.aQt.aQt",
"Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#",
".aQt.aQt.aQt.aQt.aQt.aQt.aQt.aQt.aQt.aQt.aQt.aQt.aQt.aQt.aQt",
"Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.bQt.#Qt.#Qt.#Qt.#Qt.#",
".aQt.aQt.aQt.aQt.aQt.aQt.aQt.aQt.aQt.a.c.d.e.f.g.aQt.aQt.aQt",
"Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#.h.i.j.k.l.mQt.#Qt.#Qt.#",
".aQt.aQt.aQt.aQt.aQt.aQt.aQt.nQt.a.o.p.q.r.s.t.u.vQt.aQt.aQt",
"Qt.#Qt.#Qt.#Qt.#Qt.#Qt.w.x.y.z.A.B.C.D.E.F.t.G.H.A.#Qt.#Qt.#",
".aQt.aQt.aQt.aQt.a.I.J.K.L.M.A.N.O.D.P.Q.R.S.T.L.U.V.aQt.aQt",
"Qt.#Qt.#Qt.#Qt.#Qt.#.W.y.X.Y.Z.0.1.2.3.4.5.6.7.8Qt.#Qt.#Qt.#",
".aQt.aQt.aQt.aQt.9#..2##.D###a#b.z.z#c#d#e.h#fQt.aQt.aQt.aQt",
"Qt.#Qt.#Qt.##d#g#h#i#j#k#l.A.3#m#n.n.w.h.B#o#p.#Qt.#Qt.#Qt.#",
".aQt.aQt.a.g#q#r#s#t#u#v.Q#w.4#x#y#z#A#B#C.V.aQt.aQt.aQt.aQt",
"Qt.#Qt.#Qt#D#E#F#G#H#I#J#c#K#L.z.X#M#N#O#P.#Qt.#Qt.#Qt.#Qt.#",
".aQt.aQt.g#Q#R#S#S#T#U#V#W.A#X#Y#Z#0#1#2#h.B.aQt.aQt.aQt.aQt",
"Qt.#Qt.##3#4#5#6#7#8#9a..O.D###ka#aaabac#p.AQt.#Qt.#Qt.#Qt.#",
".aQt.aad#waeafagahaiajakal##am#9anaoapapaq.o.aQt.aQt.aQt.aQt",
"Qt.#Qt.##warasatauavaw.Eax#eay#4abaqaqaz#J#CQt.#Qt.#Qt.#Qt.#",
".aQt.aQt#taAaBaCawaDaE#zaF.zaGaHaIataJaJaKaL.aQt.aQt.aQt.aQt",
"Qt.#Qt.#aM.L.zaNaOaNaPaQ#e.6aNaJaRaSaTaUaVaWQt.#Qt.#Qt.#Qt.#",
".aQt.aQtaXaYaZaZ#baZaVa0.AaZa1a2a3a4a5a6a7Qt.aQt.aQt.aQt.aQt",
"Qt.#Qt.#Qt.ha8a9.Xb.aM.o.o.7aOb#babbbc.#Qt.#Qt.#Qt.#Qt.#Qt.#",
".aQt.aQt.abdaY#Cb.aNbebfbg#ebh#pabQt.aQt.aQt.aQt.aQt.aQt.aQt",
"Qt.#Qt.#Qt.#Qtbi.LbjaF.L#tbkQt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#",
".aQt.aQt.aQt.aQt.aQt.8Qt.aQt.aQt.aQt.aQt.aQt.aQt.aQt.aQt.aQt",
"Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#",
".aQt.aQt.aQt.aQt.aQt.aQt.aQt.aQt.aQt.aQt.aQt.aQt.aQt.aQt.aQt",
"Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#",
".aQt.aQt.aQt.aQt.aQt.aQt.aQt.aQt.aQt.aQt.aQt.aQt.aQt.aQt.aQt"};
