static QList<Soprano::Statement> testData()
{
    QList<Soprano::Statement> sl;

    sl.append( Soprano::Statement( QUrl("http://soprano.sf.net/testdata/Soprano"), QUrl("http://www.w3.org/2000/01/rdf-schema#label"), LiteralValue("Soprano") ) );
    sl.append( Soprano::Statement( QUrl("http://soprano.sf.net/testdata/Soprano"), QUrl("http://www.w3.org/2000/01/rdf-schema#comment"), LiteralValue("Soprano (formally known as QRDF) is a QT4 wrapper library for RDF storage, parsing, and querying") ) );
    sl.append( Soprano::Statement( QUrl("http://soprano.sf.net/testdata/Soprano"), QUrl("http://soprano.sf.net/test#author"), QUrl("http://soprano.sf.net/testdata/DanieleGaldi") ) );
    sl.append( Soprano::Statement( QUrl("http://soprano.sf.net/testdata/Soprano"), QUrl("http://soprano.sf.net/test#author"), QUrl("http://soprano.sf.net/testdata/SebastianTrueg") ) );

    sl.append( Soprano::Statement( QUrl("http://soprano.sf.net/testdata/DanieleGaldi"), QUrl("http://soprano.sf.net/test#name"), LiteralValue("Daniele Galdi") ) );
    sl.append( Soprano::Statement( QUrl("http://soprano.sf.net/testdata/DanieleGaldi"), QUrl("http://soprano.sf.net/test#email"), LiteralValue("daniele.galdi@gmail.com") ) );
    sl.append( Soprano::Statement( QUrl("http://soprano.sf.net/testdata/DanieleGaldi"), QUrl("http://soprano.sf.net/test#location"), LiteralValue("Rome, Italy") ) );

    sl.append( Soprano::Statement( QUrl("http://soprano.sf.net/testdata/SebastianTrueg"), QUrl("http://soprano.sf.net/test#name"), LiteralValue("Sebastian Trueg") ) );
    sl.append( Soprano::Statement( QUrl("http://soprano.sf.net/testdata/SebastianTrueg"), QUrl("http://soprano.sf.net/test#email"), LiteralValue("trueg@kde.org") ) );
    sl.append( Soprano::Statement( QUrl("http://soprano.sf.net/testdata/SebastianTrueg"), QUrl("http://soprano.sf.net/test#location"), LiteralValue("Freiburg, Germany") ) );

    return sl;
}
