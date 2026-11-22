#include "elementdata.h"
#include <QVector>

namespace {

const quint32 kCatCount = 10;
const char *const kCatNames[kCatCount] = {
    "碱金属", "碱土金属", "过渡金属", "后过渡金属",
    "类金属", "非金属", "卤素", "稀有气体", "镧系", "锕系"
};

// Z, symbol, en-name, cn-name, category, mass, period, group, block,
// config, electroneg, density, mp(K), bp(K), discovery, phase, valence, color
const ElementData kRaw[] = {
{1,"H","Hydrogen","氢","非金属",1.008,1,1,"s","1s1",2.20,0.00008988,14.01,20.28,"1766","Gas",1,QColor("#7ec8e3")},
{2,"He","Helium","氦","稀有气体",4.0026,1,18,"s","1s2",0.0,0.0001785,0.95,4.22,"1868","Gas",2,QColor("#b3e5fc")},
{3,"Li","Lithium","锂","碱金属",6.94,2,1,"s","[He] 2s1",0.98,0.534,453.69,1615,"1817","Solid",1,QColor("#ffab91")},
{4,"Be","Beryllium","铍","碱土金属",9.0122,2,2,"s","[He] 2s2",1.57,1.85,1560,2742,"1798","Solid",2,QColor("#ffcc80")},
{5,"B","Boron","硼","类金属",10.81,2,13,"p","[He] 2s2 2p1",2.04,2.34,2349,4200,"1808","Solid",3,QColor("#a5d6a7")},
{6,"C","Carbon","碳","非金属",12.011,2,14,"p","[He] 2s2 2p2",2.55,2.267,3823,4098,"Ancient","Solid",4,QColor("#90a4ae")},
{7,"N","Nitrogen","氮","非金属",14.007,2,15,"p","[He] 2s2 2p3",3.04,0.0012506,63.15,77.36,"1772","Gas",3,QColor("#81d4fa")},
{8,"O","Oxygen","氧","非金属",15.999,2,16,"p","[He] 2s2 2p4",3.44,0.001429,54.36,90.20,"1774","Gas",2,QColor("#ef9a9a")},
{9,"F","Fluorine","氟","卤素",18.998,2,17,"p","[He] 2s2 2p5",3.98,0.001696,53.48,85.03,"1886","Gas",1,QColor("#80cbc4")},
{10,"Ne","Neon","氖","稀有气体",20.180,2,18,"p","[He] 2s2 2p6",0.0,0.0008999,24.56,27.07,"1898","Gas",0,QColor("#b3e5fc")},
{11,"Na","Sodium","钠","碱金属",22.990,3,1,"s","[Ne] 3s1",0.93,0.97,370.94,1156,"1807","Solid",1,QColor("#ffab91")},
{12,"Mg","Magnesium","镁","碱土金属",24.305,3,2,"s","[Ne] 3s2",1.31,1.738,923,1363,"1755","Solid",2,QColor("#ffcc80")},
{13,"Al","Aluminium","铝","后过渡金属",26.982,3,13,"p","[Ne] 3s2 3p1",1.61,2.70,933.47,2792,"1825","Solid",3,QColor("#b0bec5")},
{14,"Si","Silicon","硅","类金属",28.085,3,14,"p","[Ne] 3s2 3p2",1.90,2.329,1687,3538,"1824","Solid",4,QColor("#a5d6a7")},
{15,"P","Phosphorus","磷","非金属",30.974,3,15,"p","[Ne] 3s2 3p3",2.19,1.823,317.30,553.7,"1669","Solid",3,QColor("#90a4ae")},
{16,"S","Sulfur","硫","非金属",32.06,3,16,"p","[Ne] 3s2 3p4",2.58,2.067,388.36,717.8,"Ancient","Solid",2,QColor("#fff176")},
{17,"Cl","Chlorine","氯","卤素",35.45,3,17,"p","[Ne] 3s2 3p5",3.16,0.003214,171.6,239.11,"1774","Gas",1,QColor("#80cbc4")},
{18,"Ar","Argon","氩","稀有气体",39.948,3,18,"p","[Ne] 3s2 3p6",0.0,0.0017837,83.81,87.30,"1894","Gas",0,QColor("#b3e5fc")},
{19,"K","Potassium","钾","碱金属",39.098,4,1,"s","[Ar] 4s1",0.82,0.89,336.7,1032,"1807","Solid",1,QColor("#ffab91")},
{20,"Ca","Calcium","钙","碱土金属",40.078,4,2,"s","[Ar] 4s2",1.00,1.526,1115,1757,"1808","Solid",2,QColor("#ffcc80")},
{21,"Sc","Scandium","钪","过渡金属",44.956,4,3,"d","[Ar] 3d1 4s2",1.36,2.985,1814,3109,"1879","Solid",3,QColor("#90caf9")},
{22,"Ti","Titanium","钛","过渡金属",47.867,4,4,"d","[Ar] 3d2 4s2",1.54,4.506,1941,3560,"1791","Solid",4,QColor("#90caf9")},
{23,"V","Vanadium","钒","过渡金属",50.942,4,5,"d","[Ar] 3d3 4s2",1.63,6.0,2183,3680,"1801","Solid",5,QColor("#90caf9")},
{24,"Cr","Chromium","铬","过渡金属",51.996,4,6,"d","[Ar] 3d5 4s1",1.66,7.19,2180,2944,"1797","Solid",6,QColor("#90caf9")},
{25,"Mn","Manganese","锰","过渡金属",54.938,4,7,"d","[Ar] 3d5 4s2",1.55,7.21,1519,2334,"1774","Solid",7,QColor("#90caf9")},
{26,"Fe","Iron","铁","过渡金属",55.845,4,8,"d","[Ar] 3d6 4s2",1.83,7.874,1811,3134,"Ancient","Solid",2,QColor("#90caf9")},
{27,"Co","Cobalt","钴","过渡金属",58.933,4,9,"d","[Ar] 3d7 4s2",1.88,8.86,1768,3200,"1735","Solid",2,QColor("#90caf9")},
{28,"Ni","Nickel","镍","过渡金属",58.693,4,10,"d","[Ar] 3d8 4s2",1.91,8.908,1728,3186,"1751","Solid",2,QColor("#90caf9")},
{29,"Cu","Copper","铜","过渡金属",63.546,4,11,"d","[Ar] 3d10 4s1",1.90,8.96,1357.77,2835,"Ancient","Solid",1,QColor("#90caf9")},
{30,"Zn","Zinc","锌","后过渡金属",65.38,4,12,"d","[Ar] 3d10 4s2",1.65,7.134,692.68,1180,"1746","Solid",2,QColor("#b0bec5")},
{31,"Ga","Gallium","镓","后过渡金属",69.723,4,13,"p","[Ar] 3d10 4s2 4p1",1.81,5.91,302.91,2477,"1875","Solid",3,QColor("#b0bec5")},
{32,"Ge","Germanium","锗","类金属",72.630,4,14,"p","[Ar] 3d10 4s2 4p2",2.01,5.323,1211.40,3106,"1886","Solid",4,QColor("#a5d6a7")},
{33,"As","Arsenic","砷","类金属",74.922,4,15,"p","[Ar] 3d10 4s2 4p3",2.18,5.776,1090,887,"1250","Solid",3,QColor("#a5d6a7")},
{34,"Se","Selenium","硒","非金属",78.971,4,16,"p","[Ar] 3d10 4s2 4p4",2.55,4.809,453,958,"1817","Solid",2,QColor("#90a4ae")},
{35,"Br","Bromine","溴","卤素",79.904,4,17,"p","[Ar] 3d10 4s2 4p5",2.96,3.1028,265.8,332.0,"1826","Liquid",1,QColor("#80cbc4")},
{36,"Kr","Krypton","氪","稀有气体",83.798,4,18,"p","[Ar] 3d10 4s2 4p6",3.00,0.003749,115.78,119.93,"1898","Gas",0,QColor("#b3e5fc")},
{37,"Rb","Rubidium","铷","碱金属",85.468,5,1,"s","[Kr] 5s1",0.82,1.532,312.45,961,"1861","Solid",1,QColor("#ffab91")},
{38,"Sr","Strontium","锶","碱土金属",87.62,5,2,"s","[Kr] 5s2",0.95,2.64,1050,1655,"1790","Solid",2,QColor("#ffcc80")},
{39,"Y","Yttrium","钇","过渡金属",88.906,5,3,"d","[Kr] 4d1 5s2",1.22,4.472,1799,3609,"1794","Solid",3,QColor("#90caf9")},
{40,"Zr","Zirconium","锆","过渡金属",91.224,5,4,"d","[Kr] 4d2 5s2",1.33,6.52,2128,4682,"1789","Solid",4,QColor("#90caf9")},
{41,"Nb","Niobium","铌","过渡金属",92.906,5,5,"d","[Kr] 4d4 5s1",1.6,8.57,2750,5017,"1801","Solid",5,QColor("#90caf9")},
{42,"Mo","Molybdenum","钼","过渡金属",95.95,5,6,"d","[Kr] 4d5 5s1",2.16,10.28,2896,4912,"1781","Solid",6,QColor("#90caf9")},
{43,"Tc","Technetium","锝","过渡金属",98.0,5,7,"d","[Kr] 4d5 5s2",1.9,11.0,2430,4538,"1937","Solid",7,QColor("#90caf9")},
{44,"Ru","Ruthenium","钌","过渡金属",101.07,5,8,"d","[Kr] 4d7 5s1",2.2,12.45,2607,4423,"1844","Solid",8,QColor("#90caf9")},
{45,"Rh","Rhodium","铑","过渡金属",102.91,5,9,"d","[Kr] 4d8 5s1",2.28,12.41,2237,3968,"1803","Solid",9,QColor("#90caf9")},
{46,"Pd","Palladium","钯","过渡金属",106.42,5,10,"d","[Kr] 4d10",2.20,12.023,1828.05,3236,"1803","Solid",0,QColor("#90caf9")},
{47,"Ag","Silver","银","过渡金属",107.87,5,11,"d","[Kr] 4d10 5s1",1.93,10.49,1234.93,2435,"Ancient","Solid",1,QColor("#90caf9")},
{48,"Cd","Cadmium","镉","后过渡金属",112.41,5,12,"d","[Kr] 4d10 5s2",1.69,8.69,594.22,1040,"1817","Solid",2,QColor("#b0bec5")},
{49,"In","Indium","铟","后过渡金属",114.82,5,13,"p","[Kr] 4d10 5s2 5p1",1.78,7.31,429.75,2345,"1863","Solid",3,QColor("#b0bec5")},
{50,"Sn","Tin","锡","后过渡金属",118.71,5,14,"p","[Kr] 4d10 5s2 5p2",1.96,7.287,505.08,2875,"Ancient","Solid",4,QColor("#b0bec5")},
{51,"Sb","Antimony","锑","类金属",121.76,5,15,"p","[Kr] 4d10 5s2 5p3",2.05,6.685,903.78,1860,"Ancient","Solid",3,QColor("#a5d6a7")},
{52,"Te","Tellurium","碲","类金属",127.60,5,16,"p","[Kr] 4d10 5s2 5p4",2.1,6.232,722.66,1261,"1782","Solid",2,QColor("#a5d6a7")},
{53,"I","Iodine","碘","卤素",126.90,5,17,"p","[Kr] 4d10 5s2 5p5",2.66,4.933,386.85,457.4,"1811","Solid",1,QColor("#80cbc4")},
{54,"Xe","Xenon","氙","稀有气体",131.29,5,18,"p","[Kr] 4d10 5s2 5p6",2.6,0.005894,161.4,165.05,"1898","Gas",0,QColor("#b3e5fc")},
{55,"Cs","Caesium","铯","碱金属",132.91,6,1,"s","[Xe] 6s1",0.79,1.93,301.7,944,"1860","Solid",1,QColor("#ffab91")},
{56,"Ba","Barium","钡","碱土金属",137.33,6,2,"s","[Xe] 6s2",0.89,3.51,1000,2170,"1808","Solid",2,QColor("#ffcc80")},
{57,"La","Lanthanum","镧","镧系",138.91,6,3,"f","[Xe] 5d1 6s2",1.10,6.162,1193,3737,"1839","Solid",3,QColor("#ce93d8")},
{58,"Ce","Cerium","铈","镧系",140.12,6,4,"f","[Xe] 4f1 5d1 6s2",1.12,6.77,1068,3716,"1803","Solid",4,QColor("#ce93d8")},
{59,"Pr","Praseodymium","镨","镧系",140.91,6,5,"f","[Xe] 4f3 6s2",1.13,6.77,1208,3793,"1885","Solid",3,QColor("#ce93d8")},
{60,"Nd","Neodymium","钕","镧系",144.24,6,6,"f","[Xe] 4f4 6s2",1.14,7.01,1297,3347,"1885","Solid",3,QColor("#ce93d8")},
{61,"Pm","Promethium","钷","镧系",145.0,6,7,"f","[Xe] 4f5 6s2",1.13,7.26,1315,3273,"1945","Solid",3,QColor("#ce93d8")},
{62,"Sm","Samarium","钐","镧系",150.36,6,8,"f","[Xe] 4f6 6s2",1.17,7.52,1345,2067,"1879","Solid",3,QColor("#ce93d8")},
{63,"Eu","Europium","铕","镧系",151.96,6,9,"f","[Xe] 4f7 6s2",1.20,5.244,1099,1802,"1901","Solid",3,QColor("#ce93d8")},
{64,"Gd","Gadolinium","钆","镧系",157.25,6,10,"f","[Xe] 4f7 5d1 6s2",1.20,7.90,1585,3546,"1880","Solid",3,QColor("#ce93d8")},
{65,"Tb","Terbium","铽","镧系",158.93,6,11,"f","[Xe] 4f9 6s2",1.20,8.23,1629,3503,"1843","Solid",3,QColor("#ce93d8")},
{66,"Dy","Dysprosium","镝","镧系",162.50,6,12,"f","[Xe] 4f10 6s2",1.22,8.54,1680,2840,"1886","Solid",3,QColor("#ce93d8")},
{67,"Ho","Holmium","钬","镧系",164.93,6,13,"f","[Xe] 4f11 6s2",1.23,8.79,1734,2993,"1878","Solid",3,QColor("#ce93d8")},
{68,"Er","Erbium","铒","镧系",167.26,6,14,"f","[Xe] 4f12 6s2",1.24,9.066,1802,3141,"1842","Solid",3,QColor("#ce93d8")},
{69,"Tm","Thulium","铥","镧系",168.93,6,15,"f","[Xe] 4f13 6s2",1.25,9.32,1818,2223,"1879","Solid",3,QColor("#ce93d8")},
{70,"Yb","Ytterbium","镱","镧系",173.05,6,16,"f","[Xe] 4f14 6s2",1.25,6.90,1097,1469,"1878","Solid",2,QColor("#ce93d8")},
{71,"Lu","Lutetium","镥","镧系",174.97,6,17,"f","[Xe] 4f14 5d1 6s2",1.27,9.841,1925,3675,"1907","Solid",3,QColor("#ce93d8")},
{72,"Hf","Hafnium","铪","过渡金属",178.49,6,4,"d","[Xe] 4f14 5d2 6s2",1.30,13.31,2506,4876,"1923","Solid",4,QColor("#90caf9")},
{73,"Ta","Tantalum","钽","过渡金属",180.95,6,5,"d","[Xe] 4f14 5d3 6s2",1.50,16.69,3290,5731,"1802","Solid",5,QColor("#90caf9")},
{74,"W","Tungsten","钨","过渡金属",183.84,6,6,"d","[Xe] 4f14 5d4 6s2",2.36,19.25,3695,6203,"1783","Solid",6,QColor("#90caf9")},
{75,"Re","Rhenium","铼","过渡金属",186.21,6,7,"d","[Xe] 4f14 5d5 6s2",1.90,21.02,3459,5869,"1925","Solid",7,QColor("#90caf9")},
{76,"Os","Osmium","锇","过渡金属",190.23,6,8,"d","[Xe] 4f14 5d6 6s2",2.20,22.59,3306,5285,"1803","Solid",8,QColor("#90caf9")},
{77,"Ir","Iridium","铱","过渡金属",192.22,6,9,"d","[Xe] 4f14 5d7 6s2",2.20,22.56,2719,4701,"1803","Solid",9,QColor("#90caf9")},
{78,"Pt","Platinum","铂","过渡金属",195.08,6,10,"d","[Xe] 4f14 5d9 6s1",2.28,21.46,2041.4,4098,"1735","Solid",10,QColor("#90caf9")},
{79,"Au","Gold","金","过渡金属",196.97,6,11,"d","[Xe] 4f14 5d10 6s1",2.54,19.282,1337.33,3129,"Ancient","Solid",1,QColor("#90caf9")},
{80,"Hg","Mercury","汞","后过渡金属",200.59,6,12,"d","[Xe] 4f14 5d10 6s2",2.00,13.5336,234.43,629.88,"Ancient","Liquid",2,QColor("#b0bec5")},
{81,"Tl","Thallium","铊","后过渡金属",204.38,6,13,"p","[Xe] 4f14 5d10 6s2 6p1",1.62,11.85,577,1746,"1861","Solid",3,QColor("#b0bec5")},
{82,"Pb","Lead","铅","后过渡金属",207.2,6,14,"p","[Xe] 4f14 5d10 6s2 6p2",1.87,11.342,600.61,2022,"Ancient","Solid",4,QColor("#b0bec5")},
{83,"Bi","Bismuth","铋","后过渡金属",208.98,6,15,"p","[Xe] 4f14 5d10 6s2 6p3",2.02,9.79,544.7,1837,"1753","Solid",3,QColor("#b0bec5")},
{84,"Po","Polonium","钋","后过渡金属",209.0,6,16,"p","[Xe] 4f14 5d10 6s2 6p4",2.00,9.196,527,1235,"1898","Solid",2,QColor("#b0bec5")},
{85,"At","Astatine","砹","卤素",210.0,6,17,"p","[Xe] 4f14 5d10 6s2 6p5",2.20,0.0,575,610,"1940","Solid",1,QColor("#80cbc4")},
{86,"Rn","Radon","氡","稀有气体",222.0,6,18,"p","[Xe] 4f14 5d10 6s2 6p6",0.0,0.00973,202,211.5,"1900","Gas",0,QColor("#b3e5fc")},
{87,"Fr","Francium","钫","碱金属",223.0,7,1,"s","[Rn] 7s1",0.70,1.87,300,950,"1939","Solid",1,QColor("#ffab91")},
{88,"Ra","Radium","镭","碱土金属",226.0,7,2,"s","[Rn] 7s2",0.90,5.5,973,2010,"1898","Solid",2,QColor("#ffcc80")},
{89,"Ac","Actinium","锕","锕系",227.0,7,3,"f","[Rn] 6d1 7s2",1.10,10.07,1323,3471,"1899","Solid",3,QColor("#f48fb1")},
{90,"Th","Thorium","钍","锕系",232.04,7,4,"f","[Rn] 6d2 7s2",1.30,11.7,2023,5061,"1829","Solid",4,QColor("#f48fb1")},
{91,"Pa","Protactinium","镤","锕系",231.04,7,5,"f","[Rn] 5f2 6d1 7s2",1.50,15.37,1841,4300,"1913","Solid",5,QColor("#f48fb1")},
{92,"U","Uranium","铀","锕系",238.03,7,6,"f","[Rn] 5f3 6d1 7s2",1.38,19.1,1405.3,4404,"1789","Solid",6,QColor("#f48fb1")},
{93,"Np","Neptunium","镎","锕系",237.0,7,7,"f","[Rn] 5f4 6d1 7s2",1.36,20.45,917,4175,"1940","Solid",5,QColor("#f48fb1")},
{94,"Pu","Plutonium","钚","锕系",244.0,7,8,"f","[Rn] 5f6 7s2",1.28,19.816,912.5,3501,"1940","Solid",4,QColor("#f48fb1")},
{95,"Am","Americium","镅","锕系",243.0,7,9,"f","[Rn] 5f7 7s2",1.30,12.0,1449,2880,"1944","Solid",3,QColor("#f48fb1")},
{96,"Cm","Curium","锔","锕系",247.0,7,10,"f","[Rn] 5f7 6d1 7s2",1.30,13.51,1613,3383,"1944","Solid",3,QColor("#f48fb1")},
{97,"Bk","Berkelium","锫","锕系",247.0,7,11,"f","[Rn] 5f9 7s2",1.30,14.0,1259,2900,"1949","Solid",3,QColor("#f48fb1")},
{98,"Cf","Californium","锎","锕系",251.0,7,12,"f","[Rn] 5f10 7s2",1.30,15.1,1173,1743,"1950","Solid",3,QColor("#f48fb1")},
{99,"Es","Einsteinium","锿","锕系",252.0,7,13,"f","[Rn] 5f11 7s2",1.30,0.0,1133,1269,"1952","Solid",3,QColor("#f48fb1")},
{100,"Fm","Fermium","镄","锕系",257.0,7,14,"f","[Rn] 5f12 7s2",1.30,0.0,1800,0.0,"1952","Solid",3,QColor("#f48fb1")},
{101,"Md","Mendelevium","钔","锕系",258.0,7,15,"f","[Rn] 5f13 7s2",1.30,0.0,1100,0.0,"1955","Solid",3,QColor("#f48fb1")},
{102,"No","Nobelium","锘","锕系",259.0,7,16,"f","[Rn] 5f14 7s2",1.30,0.0,1100,0.0,"1958","Solid",2,QColor("#f48fb1")},
{103,"Lr","Lawrencium","铹","锕系",266.0,7,17,"f","[Rn] 5f14 7s2 7p1",1.30,0.0,1900,0.0,"1961","Solid",3,QColor("#f48fb1")},
{104,"Rf","Rutherfordium","𬬻","过渡金属",267.0,7,4,"d","[Rn] 5f14 6d2 7s2",0.0,0.0,0.0,0.0,"1969","Unknown",4,QColor("#90caf9")},
{105,"Db","Dubnium","𬭊","过渡金属",268.0,7,5,"d","[Rn] 5f14 6d3 7s2",0.0,0.0,0.0,0.0,"1970","Unknown",5,QColor("#90caf9")},
{106,"Sg","Seaborgium","𬭳","过渡金属",269.0,7,6,"d","[Rn] 5f14 6d4 7s2",0.0,0.0,0.0,0.0,"1974","Unknown",6,QColor("#90caf9")},
{107,"Bh","Bohrium","𬭛","过渡金属",270.0,7,7,"d","[Rn] 5f14 6d5 7s2",0.0,0.0,0.0,0.0,"1981","Unknown",7,QColor("#90caf9")},
{108,"Hs","Hassium","𬭶","过渡金属",277.0,7,8,"d","[Rn] 5f14 6d6 7s2",0.0,0.0,0.0,0.0,"1984","Unknown",8,QColor("#90caf9")},
{109,"Mt","Meitnerium","鿏","过渡金属",278.0,7,9,"d","[Rn] 5f14 6d7 7s2",0.0,0.0,0.0,0.0,"1982","Unknown",9,QColor("#90caf9")},
{110,"Ds","Darmstadtium","𫟼","过渡金属",281.0,7,10,"d","[Rn] 5f14 6d8 7s2",0.0,0.0,0.0,0.0,"1994","Unknown",10,QColor("#90caf9")},
{111,"Rg","Roentgenium","𬬭","过渡金属",282.0,7,11,"d","[Rn] 5f14 6d9 7s2",0.0,0.0,0.0,0.0,"1994","Unknown",11,QColor("#90caf9")},
{112,"Cn","Copernicium","鿔","过渡金属",285.0,7,12,"d","[Rn] 5f14 6d10 7s2",0.0,0.0,0.0,0.0,"1996","Unknown",2,QColor("#90caf9")},
{113,"Nh","Nihonium","鿭","后过渡金属",286.0,7,13,"p","[Rn] 5f14 6d10 7s2 7p1",0.0,0.0,0.0,0.0,"2016","Unknown",3,QColor("#b0bec5")},
{114,"Fl","Flerovium","𫓧","后过渡金属",289.0,7,14,"p","[Rn] 5f14 6d10 7s2 7p2",0.0,0.0,0.0,0.0,"1999","Unknown",4,QColor("#b0bec5")},
{115,"Mc","Moscovium","镆","后过渡金属",290.0,7,15,"p","[Rn] 5f14 6d10 7s2 7p3",0.0,0.0,0.0,0.0,"2010","Unknown",5,QColor("#b0bec5")},
{116,"Lv","Livermorium","鉝","后过渡金属",293.0,7,16,"p","[Rn] 5f14 6d10 7s2 7p4",0.0,0.0,0.0,0.0,"2010","Unknown",6,QColor("#b0bec5")},
{117,"Ts","Tennessine","鿬","卤素",294.0,7,17,"p","[Rn] 5f14 6d10 7s2 7p5",0.0,0.0,0.0,0.0,"2010","Unknown",7,QColor("#80cbc4")},
{118,"Og","Oganesson","鿫","稀有气体",294.0,7,18,"p","[Rn] 5f14 6d10 7s2 7p6",0.0,0.0,0.0,0.0,"2016","Unknown",8,QColor("#b3e5fc")},
};

const quint32 kElementCount = quint32(sizeof(kRaw) / sizeof(kRaw[0]));

QVector<ElementData> buildElements() {
    QVector<ElementData> v;
    v.reserve(int(kElementCount));
    for (quint32 i = 0; i < kElementCount; ++i) {
        ElementData e = kRaw[i];
        e.categoryIndex = kCatCount;
        for (quint32 c = 0; c < kCatCount; ++c) {
            if (e.category == QString::fromUtf8(kCatNames[c])) { e.categoryIndex = c; break; }
        }
        v.append(e);
    }
    return v;
}

const QVector<ElementData> &allElements() {
    static const QVector<ElementData> v = buildElements();
    return v;
}

const ElementData &emptyElement() {
    static const ElementData e;
    return e;
}

} // namespace

quint32 ElementDatabase::categoryCount() { return kCatCount; }

const QString &ElementDatabase::categoryName(quint32 idx) {
    static const QString kEmpty;
    static QVector<QString> names;
    if (names.isEmpty()) {
        names.reserve(int(kCatCount));
        for (quint32 i = 0; i < kCatCount; ++i) names.append(QString::fromUtf8(kCatNames[i]));
    }
    if (idx >= kCatCount) return kEmpty;
    return names[int(idx)];
}

quint32 ElementDatabase::categoryIndex(const QString &cat) {
    for (quint32 c = 0; c < kCatCount; ++c)
        if (cat == QString::fromUtf8(kCatNames[c])) return c;
    return kCatCount;
}

quint32 ElementDatabase::elementCount() { return kElementCount; }

const ElementData &ElementDatabase::elementAt(quint32 idx) {
    const auto &v = allElements();
    if (idx >= quint32(v.size())) return emptyElement();
    return v[int(idx)];
}

const ElementData *ElementDatabase::byAtomicNumber(int z) {
    const auto &v = allElements();
    for (const auto &e : v) if (e.atomicNumber == z) return &e;
    return nullptr;
}

const ElementData *ElementDatabase::bySymbol(const QString &sym) {
    const auto &v = allElements();
    for (const auto &e : v) if (e.symbol.compare(sym, Qt::CaseInsensitive) == 0) return &e;
    return nullptr;
}

quint32 ElementDatabase::indexOf(int z) {
    const auto &v = allElements();
    for (int i = 0; i < v.size(); ++i) if (v[i].atomicNumber == z) return quint32(i);
    return kElementCount;
}

quint32 ElementDatabase::indexOfSymbol(const QString &sym) {
    const auto &v = allElements();
    for (int i = 0; i < v.size(); ++i)
        if (v[i].symbol.compare(sym, Qt::CaseInsensitive) == 0) return quint32(i);
    return kElementCount;
}

quint32 ElementDatabase::indexOfName(const QString &n) {
    const auto &v = allElements();
    for (int i = 0; i < v.size(); ++i)
        if (v[i].name.compare(n, Qt::CaseInsensitive) == 0) return quint32(i);
    return kElementCount;
}

quint32 ElementDatabase::indexOfNameCn(const QString &n) {
    const auto &v = allElements();
    for (int i = 0; i < v.size(); ++i) if (v[i].nameCn == n) return quint32(i);
    return kElementCount;
}

quint32 ElementDatabase::countInCategory(quint32 catIdx) {
    const auto &v = allElements();
    quint32 n = 0;
    for (const auto &e : v) if (e.categoryIndex == catIdx) ++n;
    return n;
}

quint32 ElementDatabase::elementAtInCategory(quint32 catIdx, quint32 i) {
    const auto &v = allElements();
    quint32 n = 0;
    for (int j = 0; j < v.size(); ++j) {
        if (v[j].categoryIndex == catIdx) {
            if (n == i) return quint32(j);
            ++n;
        }
    }
    return kElementCount;
}

quint32 ElementDatabase::searchFirst(const QString &q, quint32 catIdx) {
    return findNext(0, q, catIdx);
}

quint32 ElementDatabase::searchFirstMask(const QString &q, quint32 catMask) {
    const auto &v = allElements();
    const QString needle = q.trimmed().toLower();
    for (int i = 0; i < v.size(); ++i) {
        const ElementData &e = v[i];
        // mask 为 0 时不匹配任何分类；否则只匹配选中位
        if (catMask == 0 || !(catMask & (quint32(1) << e.categoryIndex))) continue;
        if (needle.isEmpty() ||
            e.symbol.toLower().contains(needle) ||
            e.name.toLower().contains(needle) ||
            e.nameCn.contains(needle) ||
            QString::number(e.atomicNumber).contains(needle))
            return quint32(i);
    }
    return kElementCount;
}

quint32 ElementDatabase::findNext(quint32 start, const QString &q, quint32 catIdx) {
    const auto &v = allElements();
    const QString needle = q.trimmed().toLower();
    for (int i = int(start); i < v.size(); ++i) {
        const ElementData &e = v[i];
        if (catIdx < kCatCount && e.categoryIndex != catIdx) continue;
        if (needle.isEmpty() ||
            e.symbol.toLower().contains(needle) ||
            e.name.toLower().contains(needle) ||
            e.nameCn.contains(needle) ||
            QString::number(e.atomicNumber).contains(needle))
            return quint32(i);
    }
    return kElementCount;
}