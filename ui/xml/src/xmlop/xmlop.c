/* xmlop.c - simple API for XML output */

/* Copyright 2002-2003, 2005, 2009, 2013 Wind River Systems, Inc. */

/*
modification history
--------------------
01p,09aug13,f_f  fixed some coverity issues (WIND00429907)
01o,13jun13,f_f  fixed the BUFFER_SIZE_WARNING and NULL_RETURNS issues 
                 (WIND00421257)
01n,29mar13,f_f  fix the Coverity issue (WIND00410897)
01m,30mar09,f_f  upgrade to Expat 2.0.1
01l,23feb05,pas  added in-file comments from clearcase checkin comments
01k,03nov03,tky  added S_xmlop_BUF_CONCAT error code
01j,03nov03,tky  fix a null pointer bug in XML_NotationDeclWrite
01i,31mar03,tky  code review changes
01h,29jan03,tky  - moved all Canonicalization functionality into the output
		 module, this option to canonicalize can be toggled on/off
		 based on a switch
		 - second canonical form is also based on a switch
		 - the xmlCanon module now is completely dependent on the
		 output module to do all of its output
01g,15jan03,ssk  added errno generation
01f,09jan03,ssk  refactoring: built xmlCanon on top of xmlop
01e,08jan03,???  Added DTD output routines.
01d,08jan03,ssk  refactoring, cleanup
01c,13dec02,ssk  added pretty-printing
01b,13dec02,tky  changing xmlwf to xmlCanon
01a,11dec02,ssk  created
*/

/*
DESCRIPTION

This library provides a SAX-like API for outputting XML to a
file pointer. For example the routine for handling an XML start element in 
SAX is XML_StartElementHandler, and in this output module to write
an XML start element you call the routine XML_StartElementWrite.

INCLUDES: stdio.h, stdlib.h, errnoLib.h, vxworks.h, xmlop.h, expat.h,
xmltchar.h
*/

/* includes */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errnoLib.h>
#include <vxWorks.h>

#include "webservices/xml/xmlop/xmlop.h"
#include "webservices/xml/expat.h"
#include "webservices/xml/xmlCanon/xmltchar.h"

/* defines */

#define XMLOP_INC_INDENT(op) (op->doFormat ? (op->indent)++ : 0 )
#define XMLOP_DEC_INDENT(op)  (op->doFormat ? (op->indent)-- : 0 )
#define XMLOP_INDENT(op) (op->doFormat ? XML_Indent(op) : 0 )
#define XMLOP_LF(op) (op->doFormat ? puttc(T('\n'), op->fp) : 0 )

/* LOCAL void incNsAxis(NSAXIS_T * pAxis) */
#define incNsAxis(pAxis) \
    (pAxis->size >= XMLOP_AXIS_SIZE ? \
        (errno = S_xmlop_NSAXIS_OVERFLOW, ERROR) : \
        (pAxis->size++, OK))

/* LOCAL void incNsStack(NSSTACK_T * pStack); */
#define incNsStack(pStack) \
    (pStack->size >= XMLOP_STACK_SIZE ? \
        (errno = S_xmlop_NSSTACK_OVERFLOW, ERROR) : \
        (pStack->size++, OK))

/* forward declarations */

LOCAL STATUS initNamespaceOutput(XML_Output output);

LOCAL STATUS XML_Indent(XML_Output output);

LOCAL void XML_NameWrite(XML_Output output, const XML_Char *name);
LOCAL void XML_NamespaceDeclsWrite(XML_Output output);
LOCAL void XML_AttributesWrite(XML_Output output, const XML_Char **atts);
LOCAL void XML_AttValueWrite(XML_Output output, const XML_Char *s);

LOCAL void XML_OutputExternalID( XML_Output output, const XML_Char *systemId,
        const XML_Char *publicId );
LOCAL void XML_ParseXmlContent( XML_Output output, XML_Content *model );

LOCAL STATUS pushNsAxis(NSSTACK_T * pStack, NSAXIS_T * pAxis);  /* push axis onto stack */
LOCAL NSAXIS_T * popNsAxis(NSSTACK_T * pStack);       /* remove a copy from the axis stack */
LOCAL NSAXIS_T * peekNsAxis(NSSTACK_T * pStack);      /* find an axis */

LOCAL XML_Char * findNsPrefix(NSSTACK_T * pNsStack, const XML_Char *uri);
LOCAL XML_Char * findParentNsUri(NSSTACK_T * pNsStack, XML_Char *prefix);

/* qsort customized comparison functions */
LOCAL int nsentrycmp(const void *p1, const void *p2);
LOCAL int nsattcmp(const void *p1, const void *p2);
LOCAL int notationCmp(const void *notation1, const void *notation2);

/* local variables */
LOCAL XML_Char nsSeparator;

struct notationStorage
    {
	XML_Char notationName[XMLCANON_NOTATIONNAME_SIZE];	/* NOTATION attribute contents */
	XML_Char base[XMLOP_URI_LENGTH];			/* base path to entity set by XML_SetBase*/
	XML_Char systemId[XMLOP_URI_LENGTH];		/* SYSTEM attribute contents */
	XML_Char publicId[XMLOP_URI_LENGTH];		/* PUBLIC attribute contents */
    };

LOCAL BOOL notationDeclOccurred = FALSE;
LOCAL XML_Char doctypeDeclBuffer[XMLCANON_DOCTYPEBUF_SIZE];
LOCAL int notationIndex = 0;
LOCAL struct notationStorage *notationList[XMLCANON_NOTATIONLIST_SIZE];

/*******************************************************************************
*
* xmlOutputInit - dummy init routine
*
* This routine is a dummy init routine for Tornado component 
* inclusion/dependency checking. Currently does nothing.
*
* RETURNS: N/A
*
* ERRNO: N/A
*/
void xmlOutputInit(void)
    {
    }

/*******************************************************************************
*
* XML_OutputCreate - create an XML output object
*
* This routine constructs a new output object. <encoding> is the output stream 
* encoding; if <encoding> is "UTF-16", it is the responsibility of the user to 
* write the byte order mark prior to any other output. <nsSep> is the 
* namespace/name separator character; if the output object is being used in 
* conjunction with the expat parser, this must be set to the same value as the 
* parser's namespace separator. Avoid using ":" as the separator, as some output
* routines may misinterpret names given in the format "prefix:tagname". Output
* is written to <fp>.
*
* RETURNS: A new XML_Output object, or NULL if one cannot be created.
*
* ERRNO: S_memLib_NOT_ENOUGH_MEMORY 
*
* SEE ALSO: XML_StartElementWrite()
*/
XML_Output XML_OutputCreate
    (
    const XML_Char * encoding,  /* output char encoding, not presently used, exists only to match the SAX API */
    XML_Char nsSep,             /* namespace separator char */
    FILE * fp                   /* output file pointer */
    )
    {
    XML_Output output;

    output = (XML_Output) malloc(sizeof(struct XML_OutputStruct));
    if ( NULL == output )
        {
        errno = S_xmlop_MALLOC_FAILED;
        return NULL;
        }        

    if ( OK != initNamespaceOutput(output) )
        {
        free(output);
        return NULL;
        }

    output->fp = fp;
    output->nsSep = nsSep;
    nsSeparator = nsSep;
    output->cdataActive = XML_FALSE;
    output->indent = 0;
    output->doFormat = XML_FALSE;
    output->doCanonicalize = XML_FALSE;
    output->doSecondCanonForm = XML_FALSE;
    return output;
    }

/*******************************************************************************
*
* XML_OutputReset - resets an output object
*
* This routine prepares an output object to be re-used. This is particularly 
* valuable when memory allocation overhead is disproportionatley high, such as 
* when a large number of small documents need to be created. All namespace 
* declarations and tag stacks are cleared. <encoding>, <nsSep> and <fp> are as 
* described in XML_OutputCreate().
*
* RETURNS: OK
*
* ERRNO: N/A
*
* SEE ALSO: XML_OutputCreate()
*/
STATUS XML_OutputReset
    (
    XML_Output output,          /* XML output object */
    const XML_Char * encoding,  /* output char encoding */
    XML_Char nsSep,             /* namespace separator char */
    FILE * fp                   /* new output stream */
    )
    {
    output->fp = fp;
    output->nsSep = nsSep;
    nsSeparator = nsSep;

    output->pNsStack->size = 1; /* leave predefined namespaces in place */
    output->pNsAxis->size = 0;

    return OK;
    }

/*******************************************************************************
*
* XML_OutputDestroy - destroys an XML output object
*
* This routine destroys an existing output object. All memory used by <output> 
* is freed. The stream passed to XML_OutputCreate() is NOT closed; closing of 
* the stream is left to the user.
*
* RETURNS: OK
*
* ERRNO: N/A
*
* SEE ALSO: XML_OutputCreate()
*/
STATUS XML_OutputDestroy
    (
    XML_Output output  /* XML output object */
    )
    {
    free(output->pNsStack);
    free(output->pNsAxis);
    free(output);

    return OK;
    }

/*******************************************************************************
*
* XML_OutputFormatSet - turns output formatting on/off
*
* This routine turns on/off automatic formatting for <output>. Tags are
* indented based on depth, and a newline is placed before each start tag 
* and after each end tag.  This is not related to Canonical formatting.
*
* RETURNS: OK
*
* ERRNO: N/A
*
*/
STATUS XML_OutputFormatSet
    (
    XML_Output output,  /* XML output object */
    XML_Bool format     /* enable/disable formatting */
    )
    {
    output->doFormat = format;
    return OK;
    }

/*******************************************************************************
*
* XML_OutputCanonSet - turns output canonicalization formatting on/off
*
* This routine turns on/off automatic canonicalization formatting for <output>. 
* Canonicalization of output will take place according to Canonical XML Version
* 1.0 specs at http://www.w3.org/TR/xml-c14n. 
*
* RETURNS: OK
*
* ERRNO: N/A
*
*/
STATUS XML_OutputCanonSet
    (
    XML_Output output,  /* XML output object */
    XML_Bool canonicalize     /* enable/disable formatting */
    )
    {

    if(canonicalize)
        {
        output->doCanonicalize = TRUE;
        }
    else
        {
        output->doCanonicalize = FALSE;
        output->doSecondCanonForm = FALSE;
        }
    
    return OK;
    }

/*******************************************************************************
*
* XML_OutputSecondCanonFormSet - turns output 2nd canonicalization form formatting on/off
*
* This routine turns on/off automatic 2nd form canonicalization formatting for 
* <output>. Canonicalization of output will take place according to Canonical 
* XML Version 1.0 specs at http://www.w3.org/TR/xml-c14n, with the exeption that
* DOCTYPE and NOTATION declarations are printed.  The term 2nd Canonical form 
* was coined by James Clark, for the purpose of the XML conformance test suite. 
*
* RETURNS: OK
*
* ERRNO: N/A
*
*/
STATUS XML_OutputSecondCanonFormSet
    (
    XML_Output output,  /* XML output object */
    XML_Bool secondForm     /* enable/disable formatting */
    )
    {
    if(secondForm)
        {
        output->doSecondCanonForm = TRUE;
        output->doCanonicalize = TRUE;
        }
    else
        {
        output->doSecondCanonForm = FALSE;
        }
    
    return OK;
    }

/*******************************************************************************
*
* XML_XmlDeclWrite - write an XML (or text) declaration
*
* This routine is called for both XML declarations and text declarations. The 
* way to distinguish between the two is that the version parameter may be
* NULL for text declarations. The encoding parameter may be NULL for XML
* declarations. The standalone parameter may be -1, 0, or 1 indicating
* respectively that there is no standalone parameter in the declaration, that it
* is given as no, or that it is given as yes.
*
* RETURNS: OK
*
* ERRNO: N/A
*/
STATUS XML_XmlDeclWrite
    (
    XML_Output output,         /* XML output object */
    const XML_Char * version,  /* XML specification version */
    const XML_Char * encoding, /* document char. encoding */
    int standalone             /* standalone flag */
    )
    {
    XMLOP_INDENT(output);
    fputts(T("<?xml ") ,output->fp);

    if ( NULL != version)
        {
        fputts(T("version=\"") ,output->fp);
        fputts(version ,output->fp);
        fputts(T("\" ") ,output->fp);
        }

    if ( NULL != encoding )
        {
        fputts(T("encoding=\"") ,output->fp);
        fputts(encoding ,output->fp);
        fputts(T("\" ") ,output->fp);
        }

    if ( standalone >= 0 )
        {
        fputts(T("standalone=\"") ,output->fp);
        fputts(standalone != 0 ? T("yes") : T("no"), output->fp);
        puttc(T('"') ,output->fp);
        }

    fputts(T("?>") ,output->fp);

    XMLOP_LF(output);

    return OK;
    }

/*******************************************************************************
*
* XML_StartNamespaceDeclWrite - begins a namespace
*
* This routine is called once for each namespace declaration; the namespace 
* declaration will be generated as output in the subsequent start tag. Calls to
* XML_StartElementWrite() and XML_EndElementWrite() occur between the calls to
* the start and end namespace declaration routines. For an xmlns attribute,
* <prefix> is NULL. For an xmlns="" attribute, <uri> is NULL.
*
* RETURNS: ERROR in the event of a namespace stack or name buffer overflow,
* OK otherwise.
*
* ERRNO: S_xmlop_NSSTACK_OVERFLOW, S_xmlop_NAMEBUF_CONCAT
*
* SEE ALSO: XML_StartElementWrite(), XML_EndElementWrite()
*/
STATUS XML_StartNamespaceDeclWrite
    (
    XML_Output output,        /* XML output object */
    const XML_Char * prefix,  /* namespace prefix (local name) */
    const XML_Char * uri      /* namespace URI */
    )
    {
    XML_Char * nsName;
    XML_Char * nsUri;

    nsName = output->pNsAxis->namespaces[output->pNsAxis->size].localName;
    nsUri = output->pNsAxis->namespaces[output->pNsAxis->size].uri;

    if( NULL == prefix )
        {
        *nsName = T(EOS);
        }
    else
        {
        if( strlen(nsName) > XMLOP_NS_LENGTH )
            {
            errno = S_xmlop_NAMEBUF_OVERFLOW;
            return ERROR;
            }
        tcsncpy(nsName, prefix, XMLOP_NS_LENGTH);
        }

    if( NULL == uri )
        {
        *nsUri = T(EOS);
        }
    else
        {
        if( strlen(nsUri) > XMLOP_URI_LENGTH )
            {
            errno = S_xmlop_NAMEBUF_OVERFLOW;
            return ERROR;
            }
        tcsncpy(nsUri, uri, XMLOP_URI_LENGTH);
        }

    return incNsAxis(output->pNsAxis);
    }

/*******************************************************************************
*
* XML_EndNamespaceDeclWrite - ends a namespace
*
* This routine is called once to end each namespace declaration. The declaration
* to end a namespace does not affect the output and is not necessary. This
* routine is only present to match the SAX API.
*
* RETURNS: OK
*
* ERRNO: N/A
*/
STATUS XML_EndNamespaceDeclWrite
    (
    XML_Output output,       /* XML output object */
    const XML_Char * prefix  /* namespace prefix (local name) */
    )
    {
    /* function is nop */
    return OK;
    }

/*******************************************************************************
*
* XML_StartElementWrite - writes an element start tag
*
* This routine writes an element start tag with the given <name>, the list of 
* namespace declarations established in prior calls to 
* XML_StartNamespaceDeclWrite(), and the list of attributes given by <atts>.
*
* <name> is of the format "http://www.some.uri/nsuri<!-- -->&sect;tagname",
* where the URI is the fully qualified URI for the desired namespace, as passed
* to XML_StartNamespaceDeclWrite(), "&sect;" is the namespace separator char
* passed to XML_OutputCreate()b and "tagname" is the base name of the element.
*
* <atts> is an array of attribute name/value pairs, terminated with a NULL 
* sentinel. Attribute names and values are NULL terminated strings; attribute 
* names are in the format described above.
*
* If no namespaces are to be declared in this tag, names may be specified
* without the URI or &sect;. As a shortcut if the output object is not being
* used to process input from a parser, namespace delimited tag and attribute
* names may be passed in in the form "prefix:name".
*
* RETURNS: OK
*
* ERRNO: N/A
*
* SEE ALSO: XML_StartNamespaceDeclWrite()
*/
STATUS XML_StartElementWrite
    (
    XML_Output output,      /* XML output object */
    const XML_Char * name,  /* element tagname */
    const XML_Char ** atts  /* element attributes */
    )
    {
    FILE *fp = output->fp;

    if(output->doCanonicalize && NULL != atts)
        {
        NSAXIS_T * pAxis;
        const XML_Char **p;
        int nAtts;

        /* pNsAxis has been filled by the startCanonNSDecl callback, but has not
         * yet been pushed to pNsStack (done in XML_StartElementWrite)
         */
        pAxis = output->pNsAxis;
        
        /* sorts the list of namespaces, using qsort and nsentrycmp as comparison method */
        if( pAxis->size > 1 )
            qsort((void *)pAxis->namespaces, pAxis->size, sizeof(pAxis->namespaces[0]), nsentrycmp);
        
        /* sort attributes via nsattcmp */
        p = atts;
        while( *p )
            ++p;
        
        nAtts = (int)((p - atts) / 2);
        
        if( nAtts > 1 )
            qsort((void *)atts, nAtts, sizeof(XML_Char *) * 2, nsattcmp);

        }

    XMLOP_LF(output);
    XMLOP_INDENT(output);

    /* pNsAxis filled by XML_StartNamespaceDeclWrite */
    pushNsAxis(output->pNsStack, output->pNsAxis);
    output->pNsAxis->size = 0;

    puttc(T('<'), fp);

    XML_NameWrite(output, name);
    XML_NamespaceDeclsWrite(output);
    
    if(NULL != atts)
        {
    XML_AttributesWrite(output, atts);
        }

    puttc(T('>'), fp);

    XMLOP_INC_INDENT(output);

    return OK;
    }

/*******************************************************************************
*
* XML_EndElementWrite - writes an element end tag
*
* This routine writes an element end tag. <name> is in the format described in
* XML_StartElementWrite().
*
* Note that this routine does not check that <name> agrees with that of the
* corresponding element start tag.
*
* RETURNS: OK
*
* ERRNO: N/A
*
* SEE ALSO: XML_StartElementWrite()
*/
STATUS XML_EndElementWrite
    (
    XML_Output output,     /* XML output object */
    const XML_Char * name  /* element tagname */
    )
    {
    XMLOP_DEC_INDENT(output);
    XMLOP_INDENT(output);

    fputts(T("</"), output->fp);
    XML_NameWrite(output, name);
    puttc(T('>'), output->fp);

    (void)popNsAxis(output->pNsStack);

    XMLOP_LF(output);

    return OK;
    }

/*******************************************************************************
*
* XML_CharacterDataWrite - write arbitrary character data
*
* This routine writes <len> bytes of the string <s> to the output. Note that <s>
* is not NULL-terminated. The characters '\&', '\<', '\>', are expanded into 
* entity refernces unless a CDATA section is active. If a CDATA section is 
* active, the entire string is written literally.
*
* RETURNS: OK
*
* ERRNO: N/A
*/
STATUS XML_CharacterDataWrite
    (
    XML_Output output,   /* XML output object */
    const XML_Char * s,  /* char data (non NULL-terminated) */
    int len              /* length of char data */
    )
    {

    if ( output->cdataActive )
        {
        for ( ; len > 0; --len, ++s )
            puttc(*s, output->fp);
        return OK;
        }

    for( ; len > 0; --len, ++s )
        {
        switch( *s )
            {
            case T('&'):
                fputts(T("&amp;"), output->fp);
                break;
            case T('<'):
                fputts(T("&lt;"), output->fp);
                break;
            case T('>'):
                fputts(T("&gt;"), output->fp);
                break;
#ifdef W3C14N
            case 13:
                fputts(T("&#xD;"), output->fp);
                break;
#else
            /* XML conformance tests expect the following output */
            case 9:
                fputts(T("&#9;"), output->fp);
                break;
            case 10:
                fputts(T("&#10;"), output->fp);
                break;
            case 13:
                fputts(T("&#13;"), output->fp);
                break;
            case T('"'):
                fputts(T("&quot;"), output->fp);
                break;
#endif
            default:
                puttc(*s, output->fp);
                break;
            }
        }
    return OK;
    }

/*******************************************************************************
*
* XML_ProcessingInstructionWrite - writes an XML processing instruction
*
* This routine writes an XML processing instruction with the specified target 
* and data. <target> and <data> are both NULL-terminated, and either or both 
* may be NULL. If <data> is NULL, a PI with only a target is written; if 
* <target> is NULL, an empty PI is written, regardless of the value of <data>.
*
* RETURNS: OK
*
* ERRNO: N/A
*/
STATUS XML_ProcessingInstructionWrite
    (
    XML_Output output,        /* XML output object */
    const XML_Char * target,  /* PI target */
    const XML_Char * data     /* PI data */
    )
    {
    XMLOP_INDENT(output);
    fputts(T("<?"), output->fp);

    if(target == NULL)
        {
        fputts(T(" ?>"), output->fp);
        return OK;
        }

    fputts(target, output->fp);

    if( data != NULL && *data != EOS )
        {
        puttc(T(' '), output->fp);
        fputts(data, output->fp);
        }
    else
        {
        /* XML conformance tests expect the following */
        puttc(T(' '), output->fp);
        }

    fputts(T("?>"), output->fp);
    XMLOP_LF(output);

    return OK;
    }

/*******************************************************************************
*
* XML_CommentWrite - writes an XML comment
*
* This routine writes an XML comment to the output, where <data> is 
* NULL-terminated and provides the complete comment text.
*
* RETURNS: ERROR if <data> contains "--\>" or ends with "-", OK otherwise.
*
* ERRNO: S_xmlop_ILLEGAL_STRING
*/
STATUS XML_CommentWrite
    (
    XML_Output output,     /* XML output object */
    const XML_Char * data  /* comment text (NULL-terminated) */
    )
    {
    /* sanity checking */
    if ( tcsstr(data, T("-->")) != NULL )
        {
        errno = S_xmlop_ILLEGAL_STRING;
        return ERROR;
        }

    if ( *(data + tcslen(data) - 1) == T('-') )
        {
        errno = S_xmlop_ILLEGAL_STRING;
        return ERROR;
        }

    XMLOP_INDENT(output);

    fputts(T("<!--"), output->fp);
    fputts(data, output->fp);
    fputts(T("-->"), output->fp);

    XMLOP_LF(output);

    return OK;
    }

/*******************************************************************************
*
* XML_StartCdataSectionWrite - begins a CDATA section
*
* This routine begins a CDATA section.  CDATA character data is written with 
* XML_CharacterDataWrite(). CDATA sections may not be nested and cause 
* XML_CharacterDataWrite() to generate output with no entitity expansion.
*
* RETURNS: ERROR if a CDATA section is already active, OK otherwise.
*
* ERRNO: S_xmlop_CDATA_ACTIVE
*
* SEE ALSO: XML_CharacterDataWrite()
*/
STATUS XML_StartCdataSectionWrite
    (
    XML_Output output  /* XML output object */
    )
    {
    if ( output->cdataActive )
        {
        errno = S_xmlop_CDATA_ACTIVE;
        return ERROR;
        }

    XMLOP_INDENT(output);
    fputts(T("<![CDATA["), output->fp);
    output->cdataActive = XML_TRUE;

    return OK;
    }

/*******************************************************************************
*
* XML_EndCdataSectionWrite - ends a CDATA section
*
* This routine writes an end-of-CDATA marker. Outside the CDATA section,
* XML_CharacterDataWrite() will return to doing entity expansion as necessary
* on character data.
*
* RETURNS: ERROR if no CDATA section is active, OK otherwise.
*
* ERRNO: S_xmlop_NO_CDATA_ACTIVE
*
* SEE ALSO: XML_StartCdataSectionWrite()
*/
STATUS XML_EndCdataSectionWrite
    (
    XML_Output output  /* XML output object */
    )
    {
    if ( ! output->cdataActive )
        {
        errno = S_xmlop_NO_CDATA_ACTIVE;
        return ERROR;
        }
    
    fputts(T("]]>"), output->fp);
    XMLOP_LF(output);
    output->cdataActive = XML_FALSE;
    
    return OK;
    }

/*******************************************************************************
*
* XML_EntityRefWrite - writes an entity reference
*
* This routine writes an arbitrary entity reference to the output. Note that it
* is not necessary to use XML_EntityRefWrite() for the predefined entities '\&',
* '\<', '\>' as these are handled automatically by XML_CharacterDataWrite().
*
* RETURNS: OK
*
* ERRNO: N/A
*
* SEE ALSO: XML_CharacterDataWrite()
*/
STATUS XML_EntityRefWrite
    (
    XML_Output output,    /* XML output object */
    const XML_Char *name  /* entity name */
    )
    {
    puttc(T('&'), output->fp);
    fputts(name, output->fp);
    puttc(T(';'), output->fp);

    return OK;
    }


/*******************************************************************************
*
* XML_StartDoctypeDeclWrite - starts a doctype declaration, does not print immediately if the Canonicalization option is turned on
*
* This routine begins a DOCTYPE declaration. Either <sysid>, <pubid> or both 
* may be NULL. If the Canonicalization option is turned on then this function 
* does not print immediately but instead waits for the XML_EndDoctypeDeclWrite 
* function to be called in order to sort any NOTATION writes.
*
* RETURNS: OK
*
* ERRNO: N/A
*/
STATUS XML_StartDoctypeDeclWrite
    (
    XML_Output output,            /* XML output object */
    const XML_Char *doctypeName,  /* DOCTYPE name */
    const XML_Char *sysid,        /* SYSTEM attribute contents */
    const XML_Char *pubid,        /* PUBLIC attribute contents */
    int has_internal_subset       /* not used, present for SAX completness */
    )
    {
    if(output->doCanonicalize && output->doSecondCanonForm)
        {
        snprintf(doctypeDeclBuffer, XMLCANON_DOCTYPEBUF_SIZE, "<!DOCTYPE %s [\n", doctypeName);
        notationIndex = 0;
        }
    else
        {
        fputts(T("<!DOCTYPE "), output->fp);
        fputts(doctypeName, output->fp);
        puttc(T(' '), output->fp);
    
        XML_OutputExternalID( output, sysid, pubid );
    
        fputts(T(" [\n"), output->fp);
        }
    
    return OK;
    }

/*******************************************************************************
*
* XML_EndDoctypeDeclWrite - ends a doctype declaration
*
* This routine ends a DOCTYPE declaration. If the Canonicalization option is 
* turned on then the entire DOCTYPE element is printed here, including any 
* NOTATION declarations. Also all NOTATION declarations are sorted in 
* lexicographic order.
*
* RETURNS: OK
*
* ERRNO: N/A
*/
STATUS XML_EndDoctypeDeclWrite
    (
    XML_Output output  /* XML output object */
    )
    {
    if(output->doCanonicalize && output->doSecondCanonForm)
        {
        int i=0;
        FILE *fp = output->fp;

        if(notationDeclOccurred == TRUE)
            {
            /* print out stored NOTATION declarations */
            /* sort NOTATION declarations */
            qsort(notationList, notationIndex, sizeof(struct notationStorage *), notationCmp);

            if (strlen(doctypeDeclBuffer) >= XMLCANON_DOCTYPEBUF_SIZE)
                return ERROR;
            for(i=0; i<notationIndex; i++)
                {
                strncat(doctypeDeclBuffer, "<!NOTATION ", XMLCANON_DOCTYPEBUF_SIZE - strlen(doctypeDeclBuffer) - 1);
                strncat(doctypeDeclBuffer, notationList[i]->notationName, XMLCANON_DOCTYPEBUF_SIZE - strlen(doctypeDeclBuffer) - 1);

                if(notationList[i]->publicId[0] != 0)
                    {
                    strncat(doctypeDeclBuffer, " PUBLIC ", XMLCANON_DOCTYPEBUF_SIZE - strlen(doctypeDeclBuffer) - 1);
                    strncat(doctypeDeclBuffer, "\'", XMLCANON_DOCTYPEBUF_SIZE - strlen(doctypeDeclBuffer) - 1);
                    strncat(doctypeDeclBuffer, notationList[i]->publicId, XMLCANON_DOCTYPEBUF_SIZE - strlen(doctypeDeclBuffer) - 1);

                    if(notationList[i]->systemId[0] != 0)
                        {
                        strncat(doctypeDeclBuffer, "\' \'", XMLCANON_DOCTYPEBUF_SIZE - strlen(doctypeDeclBuffer) - 1);
                        strncat(doctypeDeclBuffer, notationList[i]->systemId, XMLCANON_DOCTYPEBUF_SIZE - strlen(doctypeDeclBuffer) - 1);
                        }

                    strncat(doctypeDeclBuffer, "\'>\n", XMLCANON_DOCTYPEBUF_SIZE - strlen(doctypeDeclBuffer) - 1);
                    }
                else if(notationList[i]->systemId[0] != 0)
                    {
                    strncat(doctypeDeclBuffer, " SYSTEM ", XMLCANON_DOCTYPEBUF_SIZE - strlen(doctypeDeclBuffer) - 1);
                    strncat(doctypeDeclBuffer, "\'", XMLCANON_DOCTYPEBUF_SIZE - strlen(doctypeDeclBuffer) - 1);
                    strncat(doctypeDeclBuffer, notationList[i]->systemId, XMLCANON_DOCTYPEBUF_SIZE - strlen(doctypeDeclBuffer) - 1);
                    strncat(doctypeDeclBuffer, "\'>\n", XMLCANON_DOCTYPEBUF_SIZE - strlen(doctypeDeclBuffer) - 1);
                    }
                }

            ftprintf(fp, "%s", doctypeDeclBuffer);
            fputts("]>", fp);    
            puttc(T('\n'), fp);
            notationDeclOccurred = FALSE;

            for(i=0; i<notationIndex; i++)
                {
                free(notationList[i]);
                notationList[i] = NULL;
                }
            
            notationIndex = 0;
            }                    
        }
    else
        fputts(T("]>\n"), output->fp);

    return OK;
    }


/*******************************************************************************
*
* XML_EntityDeclWrite - writes a complete entity declaration
*
* This routine is called for entity declarations. <is_parameter_entity> is 
* non-zero if the entity is a parameter entity, zero otherwise.
*
* For internal entities (\<!ENTITY foo "bar"\>), <value> is non-NULL and
* <systemId>, <publicID>, and <notationName> is NULL. <value> is NOT
* NULL-terminated; the length is provided in <value_length>. Since it is legal
* to have zero-length values, do not use this argument to test for internal
* entities.
*
* For external entities, <value> is NULL and <systemId> is non-NULL.
* <publicId> is NULL unless a public identifier was provided.
* <notationName> has a non-NULL value only for unparsed entity
* declarations.
*
* RETURNS: OK
*
* ERRNO: N/A
*
* INTERNAL:
* Note that <is_parameter_entity> cannot be changed to XML_Bool, since
* that would break binary compatibility.
*/ 
STATUS XML_EntityDeclWrite
    (
    XML_Output output,             /* XML output object */
    const XML_Char * entityName,   /* entity name */
    int is_parameter_entity,       /* indicates if this is a parameter entity */
    const XML_Char * value,        /* entity value */
    int value_length,              /* length of <value> */
    const XML_Char * base,         /* not used in this routine */
    const XML_Char * systemId,     /* system ID */
    const XML_Char * publicId,     /* public ID */
    const XML_Char * notationName  /* notation name */
    )
    {

    fputts(T("<!ENTITY "), output->fp);

    /* If it is a parameter entity add "%" percent sign */
    if (is_parameter_entity != 0)
        {
        fputts(T("% "), output->fp);
        }

    /*  output entity name  */
    fputts(entityName, output->fp);
    puttc(T(' '), output->fp);

    /* if it is an internal entity */
    if (systemId == NULL && publicId == NULL && notationName == NULL)
        {
        int i;

        puttc(T('\"'), output->fp);
        for (i=0; i < value_length; i++)
            puttc(value[i], output->fp);
        puttc(T('\"'), output->fp);
        }
    /* if it is an external entity */
    else
        {
        XML_OutputExternalID( output, systemId, publicId );

        /*  handle NDATA keyword notation name  */
        if ( notationName != NULL )
            {
            fputts(T(" NDATA "), output->fp);
            fputts(notationName, output->fp);
            }
        }


    fputts(T(">\n"), output->fp);
    return OK;
    }



/*******************************************************************************
*
* XML_NotationDeclWrite - write a complete notation declaration, this does not print immediately if the Canonicalization option is turned on
*
* This routine writes a notation declaration to <output>. <base> is a base path
* set by the user, in the equivalent SAX API, this is set by XML_SetBase. The 
* <notationName> will never be NULL; <systemId> and <publicId> may be NULL. If 
* the Canonicalization option is turned on then the NOTATION declaration is 
* stored and later sorted and printed when XML_EndDoctypeDeclWrite is called.
*
* RETURNS: OK
*
* ERRNO: N/A
*/
STATUS XML_NotationDeclWrite
    (
    XML_Output output,              /* XML output object */
    const XML_Char * notationName,  /* notation name */
    const XML_Char * base,          /* base path */
    const XML_Char * systemId,      /* system ID */
    const XML_Char * publicId       /* public ID */
    )
    {
    if(output->doCanonicalize)
        {
        if(output->doSecondCanonForm)
            {
            notationDeclOccurred = TRUE;

            /* store NOTATION for end of DOCTYPE declaration */
            notationList[notationIndex] =  calloc(1, sizeof(struct notationStorage));

            if(notationList[notationIndex] == NULL)
                {
                /*fprintf(stderr, "%s:%d malloc failed - %s\n", __FILE__, __LINE__, strerror(errnoGet()));*/
                errno = S_xmlop_MALLOC_FAILED;
                return ERROR;
                }

            if(notationName != NULL)
                {
                tcsncpy(notationList[notationIndex]->notationName, notationName, XMLCANON_NOTATIONNAME_SIZE-1);
                notationList[notationIndex]->notationName[XMLCANON_NOTATIONNAME_SIZE-1] = '\0';
                if(strlen(notationName) >= XMLCANON_NOTATIONNAME_SIZE)
                    {
                    /*fprintf(stderr, "%s:%d NOTATION related string is being concatenated\n", __FILE__, __LINE__);*/
                    errno = S_xmlop_BUF_CONCAT;
                    return ERROR;
                    }
                }
                
            if(base != NULL)
                {
                tcsncpy(notationList[notationIndex]->base, base, XMLOP_URI_LENGTH-1);
                notationList[notationIndex]->base[XMLOP_URI_LENGTH-1] = '\0';    
                if(strlen(base) >= XMLOP_URI_LENGTH)
                    {
                    /*fprintf(stderr, "%s:%d NOTATION related string is being concatenated\n", __FILE__, __LINE__);*/
                    errno = S_xmlop_BUF_CONCAT;
                    return ERROR;
                    }
                }
                
            if(systemId != NULL)
                {
                tcsncpy(notationList[notationIndex]->systemId, systemId, XMLOP_URI_LENGTH-1);
                notationList[notationIndex]->systemId[XMLOP_URI_LENGTH-1] = '\0';    
                if(strlen(systemId) >= XMLOP_URI_LENGTH)
                    {
                    /*fprintf(stderr, "%s:%d NOTATION related string is being concatenated\n", __FILE__, __LINE__);*/
                    errno = S_xmlop_BUF_CONCAT;
                    return ERROR;
                    }
                }
                
            if(publicId != NULL)
                {
                tcsncpy(notationList[notationIndex]->publicId, publicId, XMLOP_URI_LENGTH-1);
                notationList[notationIndex]->publicId[XMLOP_URI_LENGTH-1] = '\0';    
                if(strlen(publicId) >= XMLOP_URI_LENGTH )
                    {
                    /*fprintf(stderr, "%s:%d NOTATION related string is being concatenated\n", __FILE__, __LINE__);*/
                    errno = S_xmlop_BUF_CONCAT;
                    return ERROR;
                    }
                }

            notationIndex++;                
            }
        }
    else
        {
        fputts(T("<!NOTATION "), output->fp);

        /*  output notation name  */
        fputts(notationName, output->fp);
        puttc(T(' '), output->fp);
        
        XML_OutputExternalID( output, systemId, publicId );
        
        fputts(T(">\n"), output->fp);
        }

    return OK;
    }

/*******************************************************************************
*
* XML_AttlistDeclWrite - writes a complete attlist declaration
*
* This routine is called for *each* attribute. A single Attlist declaration with
* multiple attributes will need to call this multiple times. The "default" 
* parameter may be NULL in the case of the "#IMPLIED" or "#REQUIRED" keyword. 
* The "isrequired" parameter is true and the default value is NULL in the case 
* of "#REQUIRED". If "isrequired" is true and default is non-NULL, then this is 
* a "#FIXED" default.
*
* RETURNS: OK
*
* ERRNO: N/A
*/
STATUS XML_AttlistDeclWrite
    (
    XML_Output output,  /* XML output object */
    const XML_Char *elname,
    const XML_Char *attname,
    const XML_Char *att_type,
    const XML_Char *dflt,
    int isrequired
    )
    {

    fputts(T("<!ATTLIST "), output->fp);


    /*  output attribute list element name  */
    fputts(elname, output->fp);
    puttc(T(' '), output->fp);

    /*  output attribute list attribute name  */
    fputts(attname, output->fp);
    puttc(T(' '), output->fp);

    /*  output attribute list attribute type  */
    fputts(att_type, output->fp);
    puttc(T(' '), output->fp);


    /*         isrequired        dflt                output               */
    /*             0               0               "#IMPLIED>\n"          */
    /*             1               0               "#REQUIRED>\n"         */
    /*             0               1               "%s>\n" (dflt)         */
    /*             1               1               "#FIXED %s>\n" (dflt)  */

    if ( dflt == NULL )
        {
        if ( isrequired )
            fputts(T("#REQUIRED"), output->fp);
        else
            fputts(T("#IMPLIED"), output->fp);

        fputts(T(">\n"), output->fp);
        return OK;
        }

    if ( isrequired )
        {
        fputts(T("#FIXED "), output->fp);
        }

    puttc(T('\"'), output->fp);
    fputts(dflt, output->fp);
    puttc(T('\"'), output->fp);

    fputts(T(">\n"), output->fp);
    return OK;
    }

/*******************************************************************************
*
* XML_ElementDeclWrite - writes a complete element declaration
*
* This routine is called for an element declaration. See XML_ParseXmlContent() 
* for a description of the model argument. It is the caller's responsibility
* to free model when finished with it.
*
* RETURNS: OK
*
* ERRNO: N/A
*
* SEE ALSO: XML_ParseXmlContent()
*/
STATUS XML_ElementDeclWrite
    (
    XML_Output output,  /* XML output object */
    const XML_Char *name, /* ELEMENT name */
    XML_Content *model /* XML content model*/
    )
    {
    fputts(T("<!ELEMENT "), output->fp);

    /*  output element name  */
    fputts(name, output->fp);
    puttc(T(' '), output->fp);

    if ( model->numchildren == 0 )
        {
        /*  if element declaration is ANY or EMPTY or (#PCDATA)
            then output keyword, close tag, and return  */
        switch ( model->type )
            {
            case XML_CTYPE_ANY:
                fputts(T("ANY"), output->fp);
                break;

            case XML_CTYPE_EMPTY:
                fputts(T("EMPTY"), output->fp);
                break;

            case XML_CTYPE_MIXED:
                fputts(T("(#PCDATA)"), output->fp);
                break;

            default:
                return ERROR;
            }

        fputts(T(">\n"), output->fp);
        return OK;
        }


    /*  parse model structure  */
    XML_ParseXmlContent( output, model );

    fputts(T(">\n"), output->fp);
    return OK;
    }



/*******************************************************************************
*
* initNamespaceOutput - Initializes the NS stack and axis for an output object
*
* This routine is called during output object creation. It allocates memory for
* the namespace stack and current axis, adds the pre-defined namespaces
* (xmlns:xml="http://www.w3.org/XML/1998/namespace" and xmlns="") to the
* namespace axis and pushes the axis onto the stack.
*
* RETURNS: ERROR if memory could not be allocated, OK otherwise.
*
* ERRNO: S_memLib_NOT_ENOUGH_MEMORY 
*/
LOCAL STATUS initNamespaceOutput
    (
    XML_Output output  /* XML output object */
    )
    {
    NSSTACK_T * pNsStack;
    NSAXIS_T *pNsAxis;

    pNsStack = (NSSTACK_T *)malloc(sizeof(NSSTACK_T));
    if( NULL == pNsStack )
        return ERROR;

    pNsAxis = (NSAXIS_T *)malloc(sizeof(NSAXIS_T));
    if( NULL == pNsAxis )
        {
        free(pNsStack);
        return ERROR;
        }

    pNsStack->size = 0;
    pNsAxis->size = 0;

    /* add some pre-defined namespaces */
    /* W3C XML namespace */
    tcsncpy(pNsAxis->namespaces[pNsAxis->size].localName, XML_NAMESPACE_PREFIX,
            XMLOP_NS_LENGTH);
    tcsncpy(pNsAxis->namespaces[pNsAxis->size].uri, XML_NAMESPACE_URI,
            XMLOP_URI_LENGTH);
    incNsAxis(pNsAxis);

    /* local namespace */
    pNsAxis->namespaces[pNsAxis->size].localName[0] = T(EOS);
    pNsAxis->namespaces[pNsAxis->size].uri[0] = T(EOS);
    incNsAxis(pNsAxis);

    pushNsAxis(pNsStack, pNsAxis);
    pNsAxis->size = 0;

    output->pNsStack = pNsStack;
    output->pNsAxis = pNsAxis;
    return OK;
    }

/*******************************************************************************
*
* XML_Indent - indents the current line of output
*
* Performs syntax-based indentation on the current line of XML output. The line
* is indented by 2 * (current tag depth) spaces, where the root element is
* at a depth of 0.
*
* RETURNS: OK
*
* ERRNO: N/A
*/
LOCAL STATUS XML_Indent
    (
    XML_Output output  /* XML output object */
    )
    {
    int i;

    for ( i = output->indent * 2; i > 0; --i )
        puttc(T(' '), output->fp);

    return OK;
    }

/*******************************************************************************
*
* XML_NameWrite - write an XML tag or attribute name
*
* Writes an XML attribute or tag name to the output. If 
* 
* <name> is of the format "http://www.some.uri/nsuri<!-- -->&sect;tagname",
* where the URI is the fully qualified URI for the desired namespace, as passed
* to XML_StartNamespaceDeclWrite(), "&sect;" is the namespace separator char
* passed to XML_OutputCreate() and "tagname" is the base name of the element.
*
* RETURNS: void
*
* ERRNO: N/A
*
* SEE ALSO: XML_OutputCreate()
*/
LOCAL void XML_NameWrite
    (
    XML_Output output,     /* XML output object */
    const XML_Char * name  /* name string */
    )
    {
    XML_Char *prefix;
    XML_Char *sep;

    /* find namespace URI/name separator */
    sep = tcsrchr(name, output->nsSep);

    if( sep != NULL)
        {
        *sep = T(EOS);
        prefix = findNsPrefix(output->pNsStack, name);

        if( prefix != NULL && *prefix != T(EOS) )
            {
            /* NOT default namespace */
            fputts(prefix, output->fp);
            puttc(':', output->fp);
            }

        *sep = output->nsSep;
        /* write the localname (comes after the separator) */
        fputts(sep + 1, output->fp);
        }
    else
        {
        fputts(name, output->fp);
        }
    }

/*******************************************************************************
*
* XML_NamespaceDeclsWrite - writes current tag's namespace declarations 
*
* This routine sends to the output the namespaces in the topmost axis on the
* namespace stack. Redundant namespace declarations are eliminated prior to
* writing the axis.
*
* RETURNS: void
*
* ERRNO: N/A
*/
LOCAL void XML_NamespaceDeclsWrite
    (
    XML_Output output         /* XML output object */
    )
    {
    int i;
    XML_Char *localName;
    XML_Char *uri;
    XML_Char *parentUri;

    /* get current ns axis from top of stack */
    NSAXIS_T * nsAxis = peekNsAxis(output->pNsStack);

    for( i = 0; nsAxis != NULL && i < nsAxis->size; i++ )
        {
        localName = nsAxis->namespaces[i].localName;
        uri = nsAxis->namespaces[i].uri;

        /* TODO: A namespace node N is ignored if the nearest ancestor element
         * of the node's parent element that is in the node-set has a namespace
         * node in the node-set with the same local name and value as N. */
        parentUri = findParentNsUri(output->pNsStack, localName);

        if( parentUri != NULL && tcscmp(parentUri, uri) == 0 )
            {
            /* ignore this namespace declaration */
            continue;
            }

        /* sends to the output file all the sorted namespaces */
        fputts(T(" xmlns"), output->fp);

        if( *localName != T(EOS) )
            {
            puttc(T(':'), output->fp);
            fputts(localName, output->fp);
            }

        XML_AttValueWrite(output, uri);
        }
    }

/*******************************************************************************
*
* XML_AttributesWrite - writes current tag's attributes
*
* Wrties the list of attributes given by <atts>. <atts> is array of
* name/value pairs, terminated with a NULL sentinel; attribute names and
* values are NULL terminated strings. Names are in the format described in
* XML_NameWrite(), values can be arbitrary text. Entity expansion is
* performed on values as described in XML_AttValueWrite().
*
* RETURNS: void
*
* ERRNO: N/A
*
* SEE ALSO: XML_NameWrite(), XML_AttValueWrite()
*/
LOCAL void XML_AttributesWrite
    (
    XML_Output output,     /* XML output object */
    const XML_Char ** atts  /* attribute name/value pairs */
    )
    {
    const XML_Char * name;

    while( *atts )
        {
        name = *atts++; /* stores the name of attribute and skips to the value */

        puttc(T(' '), output->fp); /* separator  for attributes */
        XML_NameWrite(output, name);
        XML_AttValueWrite(output, *atts);
        atts++;/* goes to the next attribute */
        }
    }

/*******************************************************************************
*
* XML_AttValueWrite - writes a normalized attribute value
*
* This routine normalizes the attribute values and writes them to the output.
* This follows section 2.3 "Attribute Nodes" of the w3c canonicalization spec
* (http://www.w3.org/TR/xml-c14n):
*
* Attribute values are normalized, as if by a validating processor; attribute
* value delimiters are set to quotation marks (double quotes); special
* characters in attribute values are replaced by character references. The
* string value of the node is modified by replacing the characters '\&', '\<',
* '\>' and '&quot;' with \&amp;, \&lt;, \&gt; and \&quot; and the whitespace
* characters #x9, #xA and #xD with numeric character references, written in
* uppercase hexadecimal with no leading zeroes (for example, #xD is represented
* by the entity reference "\&#xD;"). An "=" sign is written immediately
* preceeding the quoted value.
*
* RETURNS: void
*
* ERRNO: N/A
*/
LOCAL void XML_AttValueWrite
    (
    XML_Output output,  /* XML output object */
    const XML_Char *s   /* attribute value */
    )
    {
    FILE * fp;

    fp = output->fp;

    puttc(T('='), fp);
    puttc(T('"'), fp);

    for( ;; )
        {
        switch( *s )
            {
            case 0:
                puttc(T('"'), fp);
                return;
            case T('&'):
                fputts(T("&amp;"), fp);
                break;
            case T('<'):
                fputts(T("&lt;"), fp);
                break;
            case T('"'):
                fputts(T("&quot;"), fp);
                break;
                /* Moved > == &gt; rule from within #else below. */
            case T('>'):
                fputts(T("&gt;"), fp);
                break;
            case 9:
                fputts(T("&#9;"), fp);
                break;
#ifdef W3C14N
            case 10:
                fputts(T("&#xA;"), fp);
                break;
            case 13:
                fputts(T("&#xD;"), fp);
                break;
#else /* W3C14N */
            /* XML conformance test suite expects non hex character references */
            case 10:
                fputts(T("&#10;"), fp);
                break;
            case 13:
                fputts(T("&#13;"), fp);
                break;
#endif /* W3C14N */
            default:
                if ( *s == output->nsSep )
                    {
                    puttc(T('"'), fp);
                    return;
                    }
                puttc(*s, fp);
                break;
            }
        s++;
        }
    }

/*******************************************************************************
*
* XML_OutputExternalID - output an external ID
*
* Writes a public ID and/or a system ID.
*
* RETURNS: void
*
* ERRNO: N/A
*/
LOCAL void XML_OutputExternalID
    (
    XML_Output output,  /* XML output object */
    const XML_Char *systemId,
    const XML_Char *publicId
    )
    {
    /*  SYSTEM "URI"  */
    if ( systemId != NULL && publicId == NULL )
        {
        fputts(T("SYSTEM "), output->fp);
        puttc(T('\"'), output->fp);
        fputts(systemId, output->fp);
        puttc(T('\"'), output->fp);
        return;
        }

    /*  PUBLIC "Public ID"  */
    if ( publicId != NULL )
        {
        fputts(T("PUBLIC "), output->fp);
        puttc(T('\"'), output->fp);
        fputts(publicId, output->fp);
        puttc(T('\"'), output->fp);
        }

    /*  "URI"  */
    if ( systemId != NULL )
        {
        puttc(T(' '), output->fp);
        puttc(T('\"'), output->fp);
        fputts(systemId, output->fp);
        puttc(T('\"'), output->fp);
        }
    return;
    }

/*******************************************************************************
*
* XML_ParseXmlContent - writes an XML DTD content type description
*
* PRECONDITIONS: model-\>numchildren \> 0
*
* This recursive function traverses the children of the XML_Content data 
* structure and outputs their names with quantifiers in the base case 
* (model-\>numchildren == 0).  The recursive calls (a for loop over all 
* children) is wrapped with formatting such as group braces and delimiter 
* choice.
*
* Special Case: For XML_CTYPE_MIXED models, each group is prefixed with
* "(#PCDATA|", delimited with "|", and postfixed with ")*".
*
* RETURNS: void
*
* ERRNO: N/A
*/
LOCAL void XML_ParseXmlContent
    (
    XML_Output output,   /* XML output object */
    XML_Content * model  /* XML content model*/
    )
    {
    int i;
    XML_Char delimiter=0;

    /*  base case (leaf node) */
    if ( model->numchildren == 0 )
        {
        /*  output node name  */
        fputts( model->name, output->fp);

        switch( model->quant )
            {
            case XML_CQUANT_OPT:
                puttc(T('?'), output->fp);
                break;

            case XML_CQUANT_REP:
                puttc(T('*'), output->fp);
                break;

            case XML_CQUANT_PLUS:
                puttc(T('+'), output->fp);
                break;

            default:
                break;
            }

        return;
        }


    /*  start content model or group  */
    puttc(T('('), output->fp);

    /*  choose delimiter for children  */
    if ( model->type == XML_CTYPE_SEQ )
        {
        delimiter = T(',');
        }
    if ( model->type == XML_CTYPE_CHOICE )
        {
        delimiter = T('|');
        }
    if ( model->type == XML_CTYPE_MIXED )
        {
        fputts(T("#PCDATA|"), output->fp);
        delimiter = T('|');
        }

    /*  recursively call for all children  */
    for ( i = 0; i <  model->numchildren; i++ )
        {
        if ( i )
            {
            puttc(delimiter, output->fp);
            }
        XML_ParseXmlContent( output, &((model->children)[i]) );
        }

    /*  end content model or group  */
    puttc(T(')'), output->fp);


    if ( model->type == XML_CTYPE_MIXED )
        {
        puttc(T('*'), output->fp);
        }

    return;
    }

/*******************************************************************************
*
* pushNsAxis - saves a namespace axis to the stack
*
* This routine pushes a namespace axis onto the namespace stack.
*
* RETURNS: ERROR in event of a stack overflow, OK otherwise.
*
* ERRNO: S_xmlop_NSSTACK_OVERFLOW
*/
LOCAL STATUS pushNsAxis
    (
    NSSTACK_T * pNsStack,  /* namespace axis stack */
    NSAXIS_T * pNsAxis     /* namespace axis */
    )
    {
    memcpy(&pNsStack->nsAxis[pNsStack->size], pNsAxis, sizeof(NSAXIS_T));
    return incNsStack(pNsStack);
    }

/*******************************************************************************
*
* peekNsAxis - finds a namespace axis in the stack
*
* This routine finds the last namespace axis saved into the stack, without
* popping it from the stack. If the stack is empty the routine returns NULL.
*
* RETURNS: NSAXIS_T - the pointer to the namespace axis.
*
* ERRNO: N/A
*/
LOCAL NSAXIS_T * peekNsAxis
    (
    NSSTACK_T * pNsStack  /* namespace axis stack */
    )
    {
    return ( pNsStack->size > 0 ?
             &pNsStack->nsAxis[pNsStack->size - 1] :
             NULL );
    }

/*******************************************************************************
*
* popNsAxis - removes a namespace axis from the stack
*
* This routine removes the last namespace axis from the stack and returns it. If
* the stack is empty the routine returns NULL.
*
* RETURNS: NSAXIS_T * - the pointer to the namespace axis.
*
* ERRNO: N/A
*/
LOCAL NSAXIS_T * popNsAxis
    (
    NSSTACK_T * pNsStack  /* namespace axis stack */
    )
    {
    NSAXIS_T * pAxis;

    if( pNsStack->size > 0 )
        {
        pAxis = &pNsStack->nsAxis[pNsStack->size - 1];
        pNsStack->size--;
        }
    else
        {
        pAxis = NULL;
        }

    return pAxis;
    }

/***************************************************************************
*
* findNsPrefix - finds a namespace by its URI
*
* This routine searches the namespace stack for a namespace with a given
* <uri> and returns its prefix.
*
* RETURNS: the documnet-local name (prefix) of the namespace.
*
* ERRNO: N/A
*/
LOCAL XML_Char * findNsPrefix
    (
    NSSTACK_T * pNsStack,  /* namespace axis stack */
    const XML_Char * uri   /* namespace URI */
    )
    {
    int sp;
    NSAXIS_T *pAxis;
    XML_Char *entryUri;
    int entry;

    if ( NULL == uri )
        return NULL;

    /* look at every entry in the current axis.  If not found
     * continue by looking at parent axis entries. */
    sp = pNsStack->size - 1;

    while( sp >= 0 )
        {
        pAxis = &pNsStack->nsAxis[sp];

        for( entry = 0; entry < pAxis->size; entry++ )
            {
            entryUri = pAxis->namespaces[entry].uri;
            if( NULL == entryUri ) continue;
            if( tcscmp(entryUri, uri) == 0 )
                return pAxis->namespaces[entry].localName;
            }
        /* visit parent's axis */
        sp--;
        }

    return NULL;
    }



/***************************************************************************
*
* findParentNsUri - finds the namespace with a given prefix
*
* This routine looks up for a namespace with a given <prefix> in the parent
* axis entries and returns the namespace's URI.
*
* RETURNS: the URI of the namespace.
*
* ERRNO: N/A
*/
LOCAL XML_Char * findParentNsUri
    (
    NSSTACK_T * pNsStack,  /* namespace axis stack */
    XML_Char  *prefix      /* namespace prefix (local name) */
    )
    {
    int sp;
    NSAXIS_T *pAxis;
    XML_Char *entryPrefix;
    int entry;
    int stack_offset = 1; /* start at parent */

    if ( NULL == prefix )
        return NULL;
    /* look at every entry in the parent axis.  If not found
     * continue by looking at ancestor axis entries. */
    sp = pNsStack->size - 1 - stack_offset;

    while( sp >= 0 )
        {
        pAxis = &pNsStack->nsAxis[sp];

        for( entry = 0; entry < pAxis->size; entry++ )
            {
            entryPrefix = pAxis->namespaces[entry].localName;
            if( NULL == entryPrefix ) continue;
            if( tcscmp(entryPrefix, prefix) == 0 )
                return pAxis->namespaces[entry].uri;
            }
        /* visit parent's axis */
        sp--;
        }

    return NULL;
    }

/***************************************************************************
*
* nsentrycmp - compares two namespaces
*
* This routine compares two namespace entries by localName and sorts them
* lexicographically.
* This follows section 2.2 "Document order" from canonicalization specifications:
* "An element's namespace nodes are sorted lexicographically by local name
* (the default namespace node, if one exists, has no local name and
* is therefore lexicographically least)."
*
* RETURNS: int.
*
* ERRNO: N/A
*/
static int nsentrycmp
    (
    const void *p1,
    const void *p2
    )
    {
    const NSENTRY_T *entry1 = (const NSENTRY_T *)p1;
    const NSENTRY_T *entry2 = (const NSENTRY_T *)p2;
    int default1 = (entry1->localName[0] == T('\0'));
    int default2 = (entry2->localName[0] == T('\0'));

    if( default1 != default2 )
        return default2 - default1;

    return tcscmp(entry1->localName, entry2->localName); 
    }


/***************************************************************************
*
* nsattcmp - compares two attributes
*
* This routine  compares two attributes to sort them lexicographically.
*
* RETURNS: int.
*
* ERRNO: N/A
*/
static int nsattcmp
    (
    const void *p1,
    const void *p2
    )
    {
    const XML_Char *att1 = *(const XML_Char **)p1;
    const XML_Char *att2 = *(const XML_Char **)p2;
    int sep1 = (tcsrchr(att1, nsSeparator) != 0);
    int sep2 = (tcsrchr(att2, nsSeparator) != 0);

    if( sep1 != sep2 ) /* one has separator, the other doesn't */
        return sep1 - sep2;
    
    return tcscmp(att1, att2);
    }

/***************************************************************************
*
* notationCmp - compares two NOTATION names
*
* This routine lexicographically compares UTF-8 encoded notation names.
* This is equivalent to lexicographically comparing based on the character number.
* Lexicographic order is imposed on the attributes of each element:see W3C14N 1.1.
*  
* RETURNS: 0 if names are equal, non-zero if names are not equal
*
* ERRNO: N/A
*/
static int notationCmp
    (
    const void *notation1, 
    const void *notation2
    )
    {
    return tcscmp((*(struct notationStorage **)notation1)->notationName,
                  (*(struct notationStorage **)notation2)->notationName);
    }
