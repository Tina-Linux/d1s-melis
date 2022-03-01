#include <mod_charset.h>
const charset_api_entry_t charset_api __attribute__((section("CHARSET_API_TBL"))) =
{
    CHS_Uni2Char,
    CHS_Char2Uni,
    CHS_GetChUpperTbl,
    CHS_GetChLowerTbl,
};
