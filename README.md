rdfExtractor
============

Simple metadata extractor for CellML models.

Uses the CellML API to extract all the RDF from a CellML model and writes it out to a new file. All the rdf:about URIs which reference what was the current document are correctred to point to the original file. (i.e., rdf:about="" becomes rdf:about="bob.xml"; rdf:about="#cool_model_channel" becomes rdf:about="bob.xml#cool_model_channel".)

