#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

#include "xml.hpp"

// the namespace prefix and URIs supported in calls to the XPath routines
#define RDF_PREFIX "rdf"
#define CELLML_1_0_PREFIX "cellml10"
#define CELLML_1_1_PREFIX "cellml11"
#define RDF_NS "http://www.w3.org/1999/02/22-rdf-syntax-ns#"
#define CELLML_1_0_NS "http://www.cellml.org/cellml/1.0#"
#define CELLML_1_1_NS "http://www.cellml.org/cellml/1.1#"

xmlNodeSetPtr executeXPath(xmlDocPtr doc, const xmlChar* xpathExpr)
{
    xmlXPathContextPtr xpathCtx;
    xmlXPathObjectPtr xpathObj;
    xmlNodeSetPtr results = NULL;
    /* Create xpath evaluation context */
    xpathCtx = xmlXPathNewContext(doc);
    if (xpathCtx == NULL)
    {
        fprintf(stderr, "Error: unable to create new XPath context\n");
        return NULL;
    }
    /* Register namespaces */
    if ((xmlXPathRegisterNs(xpathCtx, BAD_CAST RDF_PREFIX, BAD_CAST RDF_NS) != 0) &&
            (xmlXPathRegisterNs(xpathCtx, BAD_CAST CELLML_1_0_PREFIX, BAD_CAST CELLML_1_0_NS) != 0) &&
            (xmlXPathRegisterNs(xpathCtx, BAD_CAST CELLML_1_1_PREFIX, BAD_CAST CELLML_1_1_NS) != 0))
    {
        fprintf(stderr, "Error: unable to register namespaces\n");
        return NULL;
    }
    /* Evaluate xpath expression */
    xpathObj = xmlXPathEvalExpression(xpathExpr, xpathCtx);
    if (xpathObj == NULL)
    {
        fprintf(stderr, "Error: unable to evaluate xpath expression \"%s\"\n",
                xpathExpr);
        xmlXPathFreeContext(xpathCtx);
        return NULL;
    }

    if (xmlXPathNodeSetGetLength(xpathObj->nodesetval) > 0)
    {
        int i;
        results = xmlXPathNodeSetCreate(NULL);
        for (i=0; i< xmlXPathNodeSetGetLength(xpathObj->nodesetval); ++i)
            xmlXPathNodeSetAdd(results, xmlXPathNodeSetItem(xpathObj->nodesetval, i));
    }
    /* Cleanup */
    xmlXPathFreeObject(xpathObj);
    xmlXPathFreeContext(xpathCtx);
    return results;
}
