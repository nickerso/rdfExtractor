
#include <string>
#include <iostream>
#include <fstream>

#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

#include "utils.hpp"
#include "xml.hpp"

static std::string usage("Usage: rdfExtractor <input document> [<overwrite input>]");

static std::string correctRdf(const std::string& src, const std::string& base);
static std::string cleanModel(const const char* src);

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cerr << usage.c_str() << std::endl;
        return 1;
    }
    bool overWrite = false;
    if (argc > 2) overWrite = true;
    // set up output
    std::string outputFile(argv[1]);
    outputFile += ".rdf";
    std::streambuf* buf;
    std::ofstream of;
    if (outputFile != ".rdf")
    {
        of.open(outputFile.c_str());
        buf = of.rdbuf();
    }
    else
    {
        buf = std::cout.rdbuf();
    }
    std::ostream output(buf);

    // get the RDF from the CellML model
    std::string rdf = getCellMLMetadataAsRDFXMLString(argv[1]);

    // and fix up the rdf:about references
    std::string correctedRdf = correctRdf(rdf, argv[1]);

    // and remove the RDF from the source document
    std::string cleanedModel = cleanModel(argv[1]);
    if (overWrite)
    {
        std::ofstream newModel;
        newModel.open(argv[1]);
        newModel << cleanedModel.c_str();
        newModel.close();
    }
    else std::cout << cleanedModel.c_str() << std::endl;

    output << correctedRdf.c_str() << std::endl;

    // make sure the output file is closed
    if (argc > 2) of.close();

    return 0;
}


static std::string correctRdf(const std::string& src, const std::string& base)
{
    std::string s;
    xmlDocPtr doc;
    xmlChar *xmlbuff;
    int buffersize;
    /* Init libxml */
    xmlInitParser();
    LIBXML_TEST_VERSION;

    doc = xmlParseMemory(src.c_str(), src.size());
    if (doc == NULL)
    {
        std::cerr << "correctRdf: Error: unable to parse source document: " << src.c_str() << std::endl;
        return("");
    }

    xmlNodeSetPtr abouts = executeXPath(doc, BAD_CAST "//@rdf:about");
    if (abouts)
    {
        int i, n = xmlXPathNodeSetGetLength(abouts);
        for (i = 0; i < n; ++i)
        {
            xmlNodePtr n = xmlXPathNodeSetItem(abouts, i);
            xmlChar* xs = xmlNodeGetContent(n);
            std::string xss((char*)xs);
            xmlFree(xs);
            // correct RDF about locators (?)
            // empty locators should now refer to the src document
            if (xss == "") xss = base;
            // locators starting with '#' need to the prefixed by the src document
            if (xss[0] == '#') xss = base + xss;
            // FIXME: any others?

            // assign the corrected content back into the xml node
            xmlNodeSetContent(n, BAD_CAST xss.c_str());
        }
        xmlXPathFreeNodeSet(abouts);
    }

    xmlDocDumpFormatMemory(doc, &xmlbuff, &buffersize, 1);
    s = std::string((char*)xmlbuff);
    xmlFree(xmlbuff);

    xmlFreeDoc(doc);
    /* Shutdown libxml */
    xmlCleanupParser();

    return s;
}

static std::string cleanModel(const char *src)
{
    std::string model;
    xmlDocPtr doc;
    xmlChar *xmlbuff;
    int buffersize;
    /* Init libxml */
    xmlInitParser();
    LIBXML_TEST_VERSION;

    doc = xmlParseFile(src);
    if (doc == NULL)
    {
        std::cerr << "cleanModel: Error: unable to parse source document: " << src << std::endl;
        return("");
    }

    xmlNodeSetPtr rdf = executeXPath(doc, BAD_CAST "//rdf:RDF");
    if (rdf)
    {
        int i, n = xmlXPathNodeSetGetLength(rdf);
        for (i = 0; i < n; ++i)
        {
            xmlNodePtr n = xmlXPathNodeSetItem(rdf, i);
            xmlUnlinkNode(n);
            xmlFreeNode(n);
        }
        xmlXPathFreeNodeSet(rdf);
    }

    xmlDocDumpFormatMemory(doc, &xmlbuff, &buffersize, 1);
    model = std::string((char*)xmlbuff);
    xmlFree(xmlbuff);

    xmlFreeDoc(doc);
    /* Shutdown libxml */
    xmlCleanupParser();

    return model;
}
