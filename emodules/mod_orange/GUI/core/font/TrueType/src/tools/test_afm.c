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
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_INTERNAL_STREAM_H
#include FT_INTERNAL_POSTSCRIPT_AUX_H

void dump_fontinfo(AFM_FontInfo  fi)
{
    FT_Int  i;
    printf("This AFM is for %sCID font.\n\n",
           (fi->IsCIDFont) ? "" : "non-");
    printf("FontBBox: %.2f %.2f %.2f %.2f\n", fi->FontBBox.xMin / 65536.,
           fi->FontBBox.yMin / 65536.,
           fi->FontBBox.xMax / 65536.,
           fi->FontBBox.yMax / 65536.);
    printf("Ascender: %.2f\n", fi->Ascender / 65536.);
    printf("Descender: %.2f\n\n", fi->Descender / 65536.);

    if (fi->NumTrackKern)
        printf("There are %d sets of track kernings:\n",
               fi->NumTrackKern);
    else
    {
        printf("There is no track kerning.\n");
    }

    for (i = 0; i < fi->NumTrackKern; i++)
    {
        AFM_TrackKern  tk = fi->TrackKerns + i;
        printf("\t%2d: %5.2f %5.2f %5.2f %5.2f\n", tk->degree,
               tk->min_ptsize / 65536.,
               tk->min_kern / 65536.,
               tk->max_ptsize / 65536.,
               tk->max_kern / 65536.);
    }

    printf("\n");

    if (fi->NumKernPair)
        printf("There are %d kerning pairs:\n",
               fi->NumKernPair);
    else
    {
        printf("There is no kerning pair.\n");
    }

    for (i = 0; i < fi->NumKernPair; i++)
    {
        AFM_KernPair  kp = fi->KernPairs + i;
        printf("\t%3d + %3d => (%4d, %4d)\n", kp->index1,
               kp->index2,
               kp->x,
               kp->y);
    }
}

int dummy_get_index(const char  *name,
                    FT_UInt      len,
                    void        *user_data)
{
    if (len)
    {
        return name[0];
    }
    else
    {
        return 0;
    }
}

FT_Error parse_afm(FT_Library    library,
                   FT_Stream     stream,
                   AFM_FontInfo  fi)
{
    PSAux_Service  psaux;
    AFM_ParserRec  parser;
    FT_Error       error = FT_Err_Ok;
    psaux = (PSAux_Service)FT_Get_Module_Interface(library, "psaux");

    if (!psaux || !psaux->afm_parser_funcs)
    {
        return -1;
    }

    error = FT_Stream_EnterFrame(stream, stream->size);

    if (error)
    {
        return error;
    }

    error = psaux->afm_parser_funcs->init(&parser,
                                          library->memory,
                                          stream->cursor,
                                          stream->limit);

    if (error)
    {
        return error;
    }

    parser.FontInfo = fi;
    parser.get_index = dummy_get_index;
    error = psaux->afm_parser_funcs->parse(&parser);
    psaux->afm_parser_funcs->done(&parser);
    return error;
}


int main(int    argc,
         char **argv)
{
    FT_Library       library;
    FT_StreamRec     stream;
    FT_Error         error = FT_Err_Ok;
    AFM_FontInfoRec  fi;

    if (argc < 2)
    {
        return FT_Err_Invalid_Argument;
    }

    error = FT_Init_FreeType(&library);

    if (error)
    {
        return error;
    }

    FT_ZERO(&stream);
    error = FT_Stream_Open(&stream, argv[1]);

    if (error)
    {
        goto Exit;
    }

    stream.memory = library->memory;
    FT_ZERO(&fi);
    error = parse_afm(library, &stream, &fi);

    if (!error)
    {
        FT_Memory  memory = library->memory;
        dump_fontinfo(&fi);

        if (fi.KernPairs)
        {
            FT_FREE(fi.KernPairs);
        }

        if (fi.TrackKerns)
        {
            FT_FREE(fi.TrackKerns);
        }
    }
    else
    {
        printf("parse error\n");
    }

    FT_Stream_Close(&stream);
Exit:
    FT_Done_FreeType(library);
    return error;
}
