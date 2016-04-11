/*
 * schemastypes.c : interface of the XML Schema Datatypes
 *             definition and validity checking
 *
 * See Copyright for the status of this software.
 *
 * Daniel Veillard "webservices/xml/libxml2/veillard@redhat.com>
 */


#ifndef __XML_SCHEMA_TYPES_H__
#define __XML_SCHEMA_TYPES_H__

#if defined(WIN32) && defined(_MSC_VER)
#include "webservices/xml/libxml2/libxml/xmlwin32version.h"
#else
#include "webservices/xml/libxml2/libxml/xmlversion.h"
#endif
#ifdef LIBXML_SCHEMAS_ENABLED

#include "webservices/xml/libxml2/libxml/schemasInternals.h"

#ifdef __cplusplus
extern "C" {
#endif

void		xmlSchemaInitTypes		(void);
void		xmlSchemaCleanupTypes		(void);
xmlSchemaTypePtr xmlSchemaGetPredefinedType	(const xmlChar *name,
						 const xmlChar *ns);
int		xmlSchemaValidatePredefinedType	(xmlSchemaTypePtr type,
						 const xmlChar *value,
						 xmlSchemaValPtr *val);
int		xmlSchemaValidateFacet		(xmlSchemaTypePtr base,
						 xmlSchemaFacetPtr facet,
						 const xmlChar *value,
						 xmlSchemaValPtr val);
void		xmlSchemaFreeValue		(xmlSchemaValPtr val);

#ifdef __cplusplus
}
#endif

#endif /* LIBXML_SCHEMAS_ENABLED */
#endif /* __XML_SCHEMA_TYPES_H__ */
