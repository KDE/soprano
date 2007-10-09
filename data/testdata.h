static QList<Soprano::Statement> testData( bool withContext )
{

    QList<Soprano::Statement> sl;

    Node mainContext( QUrl( "http://soprano.sf.net/testdata/main" ) );
    Node truegContext( QUrl( "http://soprano.sf.net/testdata/trueg" ) );
    Node galdiContext( QUrl( "http://soprano.sf.net/testdata/galdi" ) );

    sl.append( Soprano::Statement( QUrl("http://soprano.sf.net/testdata/Soprano"),
				   QUrl("http://www.w3.org/2000/01/rdf-schema#label"),
				   LiteralValue("Soprano"),
				   withContext ? mainContext : Node() ) );
    sl.append( Soprano::Statement( QUrl("http://soprano.sf.net/testdata/Soprano"), 
				   QUrl("http://www.w3.org/2000/01/rdf-schema#comment"), 
				   LiteralValue("Soprano (formally known as QRDF) is a QT4 wrapper library for RDF storage, parsing, and querying"),
				   withContext ? mainContext : Node() ) );
    sl.append( Soprano::Statement( QUrl("http://soprano.sf.net/testdata/Soprano"), 
				   QUrl("http://soprano.sf.net/test#author"), 
				   QUrl("http://soprano.sf.net/testdata/DanieleGaldi"),
				   withContext ? mainContext : Node() ) );
    sl.append( Soprano::Statement( QUrl("http://soprano.sf.net/testdata/Soprano"), 
				   QUrl("http://soprano.sf.net/test#author"), 
				   QUrl("http://soprano.sf.net/testdata/SebastianTrueg"),
				   withContext ? mainContext : Node() ) );

    sl.append( Soprano::Statement( QUrl("http://soprano.sf.net/testdata/DanieleGaldi"), 
				   QUrl("http://soprano.sf.net/test#name"), 
				   LiteralValue("Daniele Galdi"),
				   withContext ? galdiContext : Node() ) );
    sl.append( Soprano::Statement( QUrl("http://soprano.sf.net/testdata/DanieleGaldi"), 
				   QUrl("http://soprano.sf.net/test#email"), 
				   LiteralValue("daniele.galdi@gmail.com"),
				   withContext ? galdiContext : Node() ) );
    sl.append( Soprano::Statement( QUrl("http://soprano.sf.net/testdata/DanieleGaldi"), 
				   QUrl("http://soprano.sf.net/test#location"), 
				   LiteralValue("Rome, Italy"),
				   withContext ? galdiContext : Node() ) );

    sl.append( Soprano::Statement( QUrl("http://soprano.sf.net/testdata/SebastianTrueg"), 
				   QUrl("http://soprano.sf.net/test#name"), 
				   LiteralValue("Sebastian Trueg"),
				   withContext ? truegContext : Node() ) );
    sl.append( Soprano::Statement( QUrl("http://soprano.sf.net/testdata/SebastianTrueg"), 
				   QUrl("http://soprano.sf.net/test#email"), 
				   LiteralValue("trueg@kde.org"),
				   withContext ? truegContext : Node() ) );
    sl.append( Soprano::Statement( QUrl("http://soprano.sf.net/testdata/SebastianTrueg"), 
				   QUrl("http://soprano.sf.net/test#location"), 
				   LiteralValue("Freiburg, Germany"),
				   withContext ? truegContext : Node() ) );

    if( withContext ) {
	sl.append( Statement( mainContext, 
			      Soprano::Vocabulary::RDF::type(),
			      Soprano::Vocabulary::NRL::GraphMetadata() ) );
	sl.append( Statement( truegContext, 
			      Soprano::Vocabulary::RDF::type(),
			      Soprano::Vocabulary::NRL::GraphMetadata() ) );
	sl.append( Statement( galdiContext, 
			      Soprano::Vocabulary::RDF::type(),
			      Soprano::Vocabulary::NRL::GraphMetadata() ) );
    }

    return sl;
}
