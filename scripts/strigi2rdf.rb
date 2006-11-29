#!/usr/bin/ruby

require 'rexml/parsers/sax2parser'
require 'rexml/sax2listener'

class MyListener
  include REXML::SAX2Listener

  @out
  @collectCh
  @tmp

  def start_document
    @out = File.new('test.rdf', 'w')
    @out << "<rdf:RDF \n"
    @out << "   xmlns:nepomuk=\"http://nepomuk-kde.semanticdesktop.org#\"\n"
    @out << "   xmlns:rdf=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#\">\n"
  end

  def end_document
    @out << "\n</rdf:RDF>"
    @out.close
  end

  def start_element(uri, localname, qname, attributes)
    if localname == 'file'
      @out << "<rdf:Description rdf:about=\""
      @out << attributes['uri']
      @out << "\">\n"
      @out << "   <nepomuk:mtime rdf:resource=\""
      @out << attributes['mtime']
      @out << "\"/>\n"
      @out << "   <rdf:type rdf:resource=\"http://nepomuk-kde.semanticdesktop.org\"/>\n"
    end
    if localname == 'value'
      @collectCh = true
      attributes.each_key {
        |key| @tmp = "   <nepomuk:" + attributes[key] + " rdf:resource=\""   
      }
    end 
  end

  def end_element(uri, localname, qname)
    if localname == 'file'
      @out << "</rdf:Description>\n"
    end
    if localname == 'value'
      @collectCh = false
      @tmp += "\"/>\n"
      @out << @tmp
    end
  end

  def characters(text)
    if @collectCh
      @tmp += text
    end  
  end
end

parser = REXML::Parsers::SAX2Parser.new( File.new( 'output.xml' ) )
parser.listen( MyListener.new )
parser.parse

