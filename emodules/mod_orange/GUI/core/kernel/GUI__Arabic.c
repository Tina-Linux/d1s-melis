/*
* Copyright (c) 2019-2025 Allwinner Technology Co., Ltd. ALL rights reserved.
*
* Allwinner is a trademark of Allwinner Technology Co.,Ltd., registered in
* the the People's Republic of China and other countries.
* All Allwinner Technology Co.,Ltd. trademarks are used with permission.
*
* DISCLAIMER
* THIRD PARTY LICENCES MAY BE REQUIRED TO IMPLEMENT THE SOLUTION/PRODUCT.
* IF YOU NEED TO INTEGRATE THIRD PARTY’S TECHNOLOGY (SONY, DTS, DOLBY, AVS OR MPEGLA, ETC.)
* IN ALLWINNERS’SDK OR PRODUCTS, YOU SHALL BE SOLELY RESPONSIBLE TO OBTAIN
* ALL APPROPRIATELY REQUIRED THIRD PARTY LICENCES.
* ALLWINNER SHALL HAVE NO WARRANTY, INDEMNITY OR OTHER OBLIGATIONS WITH RESPECT TO MATTERS
* COVERED UNDER ANY REQUIRED THIRD PARTY LICENSE.
* YOU ARE SOLELY RESPONSIBLE FOR YOUR USAGE OF THIRD PARTY’S TECHNOLOGY.
*
*
* THIS SOFTWARE IS PROVIDED BY ALLWINNER"AS IS" AND TO THE MAXIMUM EXTENT
* PERMITTED BY LAW, ALLWINNER EXPRESSLY DISCLAIMS ALL WARRANTIES OF ANY KIND,
* WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING WITHOUT LIMITATION REGARDING
* THE TITLE, NON-INFRINGEMENT, ACCURACY, CONDITION, COMPLETENESS, PERFORMANCE
* OR MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
* IN NO EVENT SHALL ALLWINNER BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
* NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS, OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
*/
//#include <stddef.h>
//#include "GUI_Protected.h"
#include "gui_core_kernel_private.h"


/*********************************************************************
*
*       Types
*
**********************************************************************
*/
typedef struct
{
    U16 Isolated;
    U16 Final;
    U16 Initial;
    U16 Medial;
} FORM_INFO_t;

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
static const FORM_INFO_t _aFormInfo[] =
{

    /*        Base      Isol.   Final   Initial Medial */

    { /* 0  0x0621 */ 0xFE80, 0x0000, 0x0000, 0x0000 /* Arabic letter Hamza                 */ },
    { /* 1  0x0622 */ 0xFE81, 0xFE82, 0x0000, 0x0000 /* Arabic letter Alef with Madda above */ },
    { /* 2  0x0623 */ 0xFE83, 0xFE84, 0x0000, 0x0000 /* Arabic letter Alef with Hamza above */ },
    { /* 3  0x0624 */ 0xFE85, 0xFE86, 0x0000, 0x0000 /* Arabic letter Waw with Hamza above  */ },
    { /* 4  0x0625 */ 0xFE87, 0xFE88, 0x0000, 0x0000 /* Arabic letter Alef with Hamza below */ },
    { /* 5  0x0626 */ 0xFE89, 0xFE8A, 0xFE8B, 0xFE8C /* Arabic letter Yeh with Hamza above  */ },
    { /* 6  0x0627 */ 0xFE8D, 0xFE8E, 0x0000, 0x0000 /* Arabic letter Alef                  */ },
    { /* 7  0x0628 */ 0xFE8F, 0xFE90, 0xFE91, 0xFE92 /* Arabic letter Beh                   */ },
    { /* 8  0x0629 */ 0xFE93, 0xFE94, 0x0000, 0x0000 /* Arabic letter Teh Marbuta           */ },
    { /* 9  0x062A */ 0xFE95, 0xFE96, 0xFE97, 0xFE98 /* Arabic letter Teh                   */ },
    { /* 10 0x062B */ 0xFE99, 0xFE9A, 0xFE9B, 0xFE9C /* Arabic letter Theh                  */ },
    { /* 11 0x062C */ 0xFE9D, 0xFE9E, 0xFE9F, 0xFEA0 /* Arabic letter Jeem                  */ },
    { /* 12 0x062D */ 0xFEA1, 0xFEA2, 0xFEA3, 0xFEA4 /* Arabic letter Hah                   */ },
    { /* 13 0x062E */ 0xFEA5, 0xFEA6, 0xFEA7, 0xFEA8 /* Arabic letter Khah                  */ },
    { /* 14 0x062F */ 0xFEA9, 0xFEAA, 0x0000, 0x0000 /* Arabic letter Dal                   */ },
    { /* 15 0x0630 */ 0xFEAB, 0xFEAC, 0x0000, 0x0000 /* Arabic letter Thal                  */ },
    { /* 16 0x0631 */ 0xFEAD, 0xFEAE, 0x0000, 0x0000 /* Arabic letter Reh                   */ },
    { /* 17 0x0632 */ 0xFEAF, 0xFEB0, 0x0000, 0x0000 /* Arabic letter Zain                  */ },
    { /* 18 0x0633 */ 0xFEB1, 0xFEB2, 0xFEB3, 0xFEB4 /* Arabic letter Seen                  */ },
    { /* 19 0x0634 */ 0xFEB5, 0xFEB6, 0xFEB7, 0xFEB8 /* Arabic letter Sheen                 */ },
    { /* 20 0x0635 */ 0xFEB9, 0xFEBA, 0xFEBB, 0xFEBC /* Arabic letter Sad                   */ },
    { /* 21 0x0636 */ 0xFEBD, 0xFEBE, 0xFEBF, 0xFEC0 /* Arabic letter Dad                   */ },
    { /* 22 0x0637 */ 0xFEC1, 0xFEC2, 0xFEC3, 0xFEC4 /* Arabic letter Tah                   */ },
    { /* 23 0x0638 */ 0xFEC5, 0xFEC6, 0xFEC7, 0xFEC8 /* Arabic letter Zah                   */ },
    { /* 24 0x0639 */ 0xFEC9, 0xFECA, 0xFECB, 0xFECC /* Arabic letter Ain                   */ },
    { /* 25 0x063A */ 0xFECD, 0xFECE, 0xFECF, 0xFED0 /* Arabic letter Ghain                 */ },
    { /* 26 0x0641 */ 0xFED1, 0xFED2, 0xFED3, 0xFED4 /* Arabic letter Feh                   */ },
    { /* 27 0x0642 */ 0xFED5, 0xFED6, 0xFED7, 0xFED8 /* Arabic letter Qaf                   */ },
    { /* 28 0x0643 */ 0xFED9, 0xFEDA, 0xFEDB, 0xFEDC /* Arabic letter Kaf                   */ },
    { /* 29 0x0644 */ 0xFEDD, 0xFEDE, 0xFEDF, 0xFEE0 /* Arabic letter Lam                   */ },
    { /* 30 0x0645 */ 0xFEE1, 0xFEE2, 0xFEE3, 0xFEE4 /* Arabic letter Meem                  */ },
    { /* 31 0x0646 */ 0xFEE5, 0xFEE6, 0xFEE7, 0xFEE8 /* Arabic letter Noon                  */ },
    { /* 32 0x0647 */ 0xFEE9, 0xFEEA, 0xFEEB, 0xFEEC /* Arabic letter Heh                   */ },
    { /* 33 0x0648 */ 0xFEED, 0xFEEE, 0x0000, 0x0000 /* Arabic letter Waw                   */ },
    { /* 34 0x0649 */ 0xFEEF, 0xFEF0, 0x0000, 0x0000 /* Arabic letter Alef Maksura          */ },
    { /* 35 0x064A */ 0xFEF1, 0xFEF2, 0xFEF3, 0xFEF4 /* Arabic letter Yeh                   */ },
    { /* 36 0x067E */ 0xFB56, 0xFB57, 0xFB58, 0xFB59 /* Eastern arabic letter Peh           */ },
    { /* 37 0x0686 */ 0xFB7A, 0xFB7B, 0xFB7C, 0xFB7D /* Eastern arabic letter Tcheh         */ },
    { /* 38 0x0698 */ 0xFB8A, 0xFB8B, 0x0000, 0x0000 /* Eastern arabic letter Jeh           */ },
    { /* 39 0x06A9 */ 0xFB8E, 0xFB8F, 0xFB90, 0xFB91 /* Eastern arabic letter Keheh         */ },
    { /* 40 0x06AF */ 0xFB92, 0xFB93, 0xFB94, 0xFB95 /* Eastern arabic letter Gaf           */ },
    { /* 41 0x06CC */ 0xFBFC, 0xFBFD, 0xFBFE, 0xFBFF /* Eastern arabic letter Farsi Yeh     */ },
};

static unsigned short NSMList[] =
{
    0x0610, /* ARABIC SIGN SALLALLAHOU ALAYHE WASSALLAM */
    0x0611,  /* ARABIC SIGN ALAYHE ASSALLAM */
    0x0612,  /* ARABIC SIGN RAHMATULLAH ALAYHE */
    0x0613,  /* ARABIC SIGN RADI ALLAHOU ANHU */
    0x0614,  /* ARABIC SIGN TAKHALLUS */
    0x064B,  /* ARABIC FATHATAN */
    0x064C,   /* ARABIC DAMMATAN */
    0x064D,   /* ARABIC KASRATAN */
    0x064E,   /* ARABIC FATHA */
    0x064F,   /* ARABIC DAMMA */
    0x0650,   /* ARABIC KASRA */
    0x0651,   /* ARABIC SHADDA */
    0x0652,   /* ARABIC SUKUN */
    0x0653,   /* ARABIC MADDAH ABOVE */
    0x0654,   /* ARABIC HAMZA ABOVE */
    0x0655,   /* ARABIC HAMZA BELOW */
    0x0656,   /* ARABIC SUBSCRIPT ALEF */
    0x0657,   /* ARABIC INVERTED DAMMA */
    0x0658,   /* ARABIC MARK NOON GHUNNA */
    0x0659,   /* ARABIC ZWARAKAY */
    0x065A,   /* ARABIC VOWEL SIGN SMALL V ABOVE */
    0x065B,   /* ARABIC VOWEL SIGN INVERTED SMALL V ABOVE */
    0x065C,   /* ARABIC VOWEL SIGN DOT BELOW */
    0x065D,   /* ARABIC REVERSED DAMMA */
    0x065E,   /* ARABIC FATHA WITH TWO DOTS */
    0x0615,   /* ARABIC SMALL HIGH TAH */
    0x0670,   /* ARABIC LETTER SUPERSCRIPT ALEF */
    0x06D6,   /* ARABIC SMALL HIGH LIGATURE SAD WITH LAM WITH ALEF MAKSURA */
    0x06D7,   /* ARABIC SMALL HIGH LIGATURE QAF WITH LAM WITH ALEF MAKSURA */
    0x06D8,   /* ARABIC SMALL HIGH MEEM INITIAL FORM */
    0x06D9,   /* ARABIC SMALL HIGH LAM ALEF */
    0x06DA,   /* ARABIC SMALL HIGH JEEM */
    0x06DB,   /* ARABIC SMALL HIGH THREE DOTS */
    0x06DC,   /* ARABIC SMALL HIGH SEEN */
    0x06DE,   /* ARABIC START OF RUB EL HIZB */
    0x06DF,   /* ARABIC SMALL HIGH ROUNDED ZERO */
    0x06E0,   /* ARABIC SMALL HIGH UPRIGHT RECTANGULAR ZERO */
    0x06E1,   /* ARABIC SMALL HIGH DOTLESS HEAD OF KHAH */
    0x06E2,   /* ARABIC SMALL HIGH MEEM ISOLATED FORM */
    0x06E3,   /* ARABIC SMALL LOW SEEN */
    0x06E4,   /* ARABIC SMALL HIGH MADDA */
    0x06E7,   /* ARABIC SMALL HIGH YEH */
    0x06E8,   /* ARABIC SMALL HIGH NOON */
    0x06EA,   /* ARABIC EMPTY CENTRE LOW STOP */
    0x06EB,   /* ARABIC EMPTY CENTRE HIGH STOP */
    0x06EC,   /* ARABIC ROUNDED HIGH STOP WITH FILLED CENTRE */
    0x06ED,   /* ARABIC SMALL LOW MEEM */
};


/*********************************************************************
*
*       _GetTableIndex
*/
static int _GetTableIndex(U16 c)
{
    if (c < 0x621)
    {
        return 0;
    }
    if (c > 0x6cc)
    {
        return 0;
    }
    if ((c >= 0x621) && (c <= 0x63a))
    {
        return c - 0x621;
    }
    if ((c >= 0x641) && (c <= 0x64a))
    {
        return c - 0x641 + 26;
    }
    if (c == 0x67e)
    {
        return 36;
    }
    if (c == 0x686)
    {
        return 37;
    }
    if (c == 0x698)
    {
        return 38;
    }
    if (c == 0x6a9)
    {
        return 39;
    }
    if (c == 0x6af)
    {
        return 40;
    }
    if (c == 0x6cc)
    {
        return 41;
    }
    return 0;
}


/*********************************************************************
*
*       _GetLigature
*/
static int _GetLigature(U16 Char, U16 Next, int PrevAffectsJoining)
{
    if (((Next != 0x622) && (Next != 0x623) && (Next != 0x625) && (Next != 0x627)) || (Char != 0x644))
    {
        return 0;
    }
    if (PrevAffectsJoining)
    {
        switch (Next)
        {
            case 0x622:
                return 0xfef6;
            case 0x623:
                return 0xfef8;
            case 0x625:
                return 0xfefa;
            case 0x627:
                return 0xfefc;
        }
    }
    else
    {
        switch (Next)
        {
            case 0x622:
                return 0xfef5;
            case 0x623:
                return 0xfef7;
            case 0x625:
                return 0xfef9;
            case 0x627:
                return 0xfefb;
        }
    }
    return 0;
}

int _IsNsmArabic(U16 c)
{
    int low = 0;
    int high = sizeof(NSMList) - 1;
    int mid;

    mid = (low + high) / 2;
    while (low <= high)
    {
        if (c > NSMList[mid])
        {
            low = mid + 1;
        }
        else if (c < NSMList[mid])
        {
            high = mid - 1;
        }
        else
        {
            return 1;
        }

        mid = (low + high) / 2;
    }

    return 0;
}



/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI__IsArabicCharacter
*/
int GUI__IsArabicCharacter(U16 c)
{
    return ((c >= 0x600) && (c <= 0x6ff)) ? 1 : 0;
}



/*********************************************************************
*
*       GUI__GetPresentationForm
*
* Purpose:
*   Returns the Arabic presentation form of the given character(s).
*   Depending on their position in the text the presentation form of
*   Arabic characters differr from their character codes. So the function
*   needs the previous and the next character code to calculate the right
*   representation form.
*   Some sequences of 2 characters are formed to one ligature. In this
*   case the routine uses the pointer pIgnoreNextCharacter to inform the
*   calling routine that the next character should be ignored.
*
* Parameters:
*   Char                 - Current character
*   Next                 - Previous character
*   Prev                 - Next character following the current character
*   pIgnoreNextCharacter - Pointer to inform calling routine that the
*                          next character should be ignored.
*   s                    - Pointer for decoding text data. Need if 'Next' is a Non Spacing Mark.
*/
U16 GUI__GetPresentationForm(U16 Char, U16 Next, U16 Prev, int *pIgnoreNext, const U16 *s, const U16 *sstart, int *vowelshift)
{
    int CharIndex, NextAffectsJoining, PrevAffectsJoining, Final, Initial, Medial, Ligature;
    static int PrevAffectsNextJoining;

    CharIndex = _GetTableIndex(Char);
    if (!CharIndex)
    {
        if (!_IsNsmArabic(Char))
        {
            PrevAffectsNextJoining = 0;
        }
        else
        {
            *vowelshift = 1;
        }

        return Char;
    }
    PrevAffectsJoining = PrevAffectsNextJoining;
    Ligature = _GetLigature(Char, Next, PrevAffectsJoining);
    if (!Ligature)
    {
        Ligature = _GetLigature(Prev, Char, PrevAffectsJoining);
    }
    if (Ligature)
    {
        if (pIgnoreNext)
        {
            *pIgnoreNext = 1;
        }
        PrevAffectsNextJoining = 0;
        return Ligature;
    }
    else
    {
        if (pIgnoreNext)
        {
            *pIgnoreNext = 0;
        }
    }

    if (s > sstart)
    {
        s--;
        while (_IsNsmArabic(Next))
        {
            if (Char)
            {
                if (s >= sstart)
                {
                    Next = *(s - 1);
                    s --;
                }
                else
                {
                    Next = 0;
                }
            }
        }
    }

    NextAffectsJoining = _GetTableIndex(Next) && (_aFormInfo[CharIndex].Medial);
    if ((!PrevAffectsJoining) && (!NextAffectsJoining))
    {
        Char = _aFormInfo[CharIndex].Isolated;
        PrevAffectsNextJoining = 0;
    }
    else if ((!PrevAffectsJoining) && (NextAffectsJoining))
    {
        Initial = _aFormInfo[CharIndex].Initial;
        PrevAffectsNextJoining = 1;
        if (Initial)
        {
            Char = Initial;
        }
        else
        {
            Char = _aFormInfo[CharIndex].Isolated;
        }
    }
    else if ((PrevAffectsJoining) && (NextAffectsJoining))
    {
        Medial = _aFormInfo[CharIndex].Medial;
        PrevAffectsNextJoining = 1;
        if (Medial)
        {
            Char = Medial;
        }
        else
        {
            Char = _aFormInfo[CharIndex].Isolated;
        }
    }
    else if ((PrevAffectsJoining) && (!NextAffectsJoining))
    {
        Final = _aFormInfo[CharIndex].Final;
        PrevAffectsNextJoining = 0;
        if (Final)
        {
            Char = Final;
        }
        else
        {
            Char = _aFormInfo[CharIndex].Isolated;
        }
    }
    return Char;
}
